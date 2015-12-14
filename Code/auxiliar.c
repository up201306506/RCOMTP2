#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>


#include "auxiliar.h"

void clearscreen(){
	printf("\033c");
}

int getIP(char * address, char * result){
	
	struct hostent *h;

	if ((h=gethostbyname(address)) == NULL) {  
		herror("gethostbyname");
		return 1;
	}
	
	strcpy(result, inet_ntoa(*((struct in_addr *)h->h_addr)));
	
	printf("Server Info:\n");
	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n", result);


	return 0;
}


int checkURL(char * url, struct url_components * data){

	regex_t regex;
    int reti = 1;
    char msgbuf[100];
	
	if (url[6] == '[') {
		data->mode = 1;
		puts("Working on User Mode");
		char* exp = "ftp://([([A-Za-z0-9])*:([A-Za-z0-9])*@])*([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+";
		reti = regcomp(&regex, exp,REG_EXTENDED);
	} else {
		data->mode = 0;
		puts("Working on Anonymous Mode");
		char* exp =  "ftp://([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+";
		reti = regcomp(&regex, exp,REG_EXTENDED);
	}

	if( reti )
	{
		fprintf(stderr, "Could not compile regex\n"); 
		return 1;
	}
	
	reti = regexec(&regex, url, 0, NULL, REG_EXTENDED);
	if( !reti ){
		//puts("Match");
	}
	else if( reti == REG_NOMATCH ){
		puts("No match");
		return 1;
	}
	else{
		regerror(reti, &regex, msgbuf, sizeof(msgbuf));
		fprintf(stderr, "Regex match failed: %s\n", msgbuf);
		return 1;
	}
	
	
	
	
	regfree(&regex);
	return 0;
}

int parseURL(char * url, struct url_components * data){
	
	char * buffer;
	char temp[256];
	
	buffer = url + 6;
	int offset;
	
	if (data->mode == 1)
	{
		buffer = buffer + 1;
		
		/* username */
		offset = parseURL_aux(buffer, temp, ':');
		if( offset < 0 )
		{
			return 1;
		}

		buffer = buffer + 1 + offset;
		strcpy(data->user, temp);
	
		/* password */
		offset = parseURL_aux(buffer, temp, '@');
		if( offset < 0 )
		{
			return 1;
		}
		buffer = buffer + 2 + offset;
		strcpy(data->password, temp);
	}
	
	/* host */
	offset = parseURL_aux(buffer, temp, '/');
	if( offset < 0 )
	{
		return 1;
	}
	buffer = buffer + 1 + offset;
	strcpy(data->host, temp);
	
	/* path */
	strcpy(data->path, buffer);
	
	return 0;
}
int parseURL_aux(char * buffer, char * result, char escape){
	size_t len;
	int i;
	
	len = strlen (buffer);
	for(i=0; i < len; i++){
		if(buffer[i] == escape)
		{
			memcpy(result, buffer, i);
			result[i] = '\0';
			return i;
		}
	}
	
	return -1;
}

int connectSocket(char * IP, int port) {
	int sockfd;
	struct sockaddr_in server_addr;

	/* server address handling */
	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */

	/* open an TCP socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return -1;
	}

	/* connect to the server */
	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			< 0) {
		perror("connect()");
		return -1;
	}
	
	/* Test responsiveness on FTP console port*/
	if (port == 21) 
	{
		char temp[BUF_LARGERSIZE];
		if (ftpReadMessage(sockfd, temp, sizeof(temp)) < 0 ) {
			printf("WARNING: Couldn't Read Response to socketing.\n");
			return -1;
		}
	}

	return sockfd;
}
void disconnectSocket(int sockfd) {
	close(sockfd);
}

int ftpLogin(char * user,char * password, int socket_fd){
	char buffer[BUF_STRINGSIZE];
	
	/* username */
	sprintf(buffer, "user %s\n", user);
	if (ftpSendMessage(socket_fd, buffer, strlen(buffer))< 0) {
		printf("WARNING: Error sending username\n");
		return -1;
	}
	if (ftpReadMessage(socket_fd, buffer, sizeof(buffer)) <0) {
		printf(	"WARNING: Error receiving response to username\n");
		return -1;
	}

	/* password */
	sprintf(buffer, "pass %s\n", password);
	if (ftpSendMessage(socket_fd, buffer, strlen(buffer))< 0) {
		printf("WARNING: Error sending password\n");
		return 1;
	}
	if (ftpReadMessage(socket_fd, buffer, sizeof(buffer))<0) 
	{
		printf(	"WARNING: Error receiving response to password\n");
		return 1;
	}
	
	return 0;
}
int ftpPasv(int socket_fd){
	
	char buffer[BUF_STRINGSIZE] = "pasv\n";
	/* pasv */
	if (ftpSendMessage(socket_fd, buffer, strlen(buffer))< 0) {
		printf("WARNING: Error sending pasv\n");
		return -1;
	}
	if (ftpReadMessage(socket_fd, buffer, sizeof(buffer)) <0) {
		printf(	"WARNING: Error receiving response to pasv\n");
		return -1;
	}	
	char IP[IP_STRINGSIZE];
	int port = parsePasvResponse(buffer, IP);
	
	if (port < 0)
	{
		printf(	"WARNING: Error processing pasv's response\n");
		return -1;
	}
	
	
	int socket_data = connectSocket(IP, port);
	if (socket_data < 0)
	{
		printf("WARNING: Could not connect to data socket.\n");
		return -1;
	}
	
	return socket_data;
	
}
int parsePasvResponse(char * message, char * IP){
		
	int IPvalues[4];
	int Portvalues[2];
	int temp = sscanf(message, "%*[^(](%d,%d,%d,%d,%d,%d)",&IPvalues[0],&IPvalues[1], &IPvalues[2], &IPvalues[3], &Portvalues[0], &Portvalues[1]);
	if ( temp !=6 ) 
	{
		printf("WARNING: Could not parse message received from pasv\n");
		printf("Message was: %s", message);
		return -1;
	}
	
	temp = sprintf(IP, "%d.%d.%d.%d",IPvalues[0],IPvalues[1],IPvalues[2],IPvalues[3]);
	if (temp < 7) 
	{
		printf("WARNING: Failed to concatenate IP from parsed elements\n");
		return -1;
	}
	
	int port = Portvalues[0]*256 + Portvalues[1];
	if(port < 0)
	{
		printf("WARNING: Something is wrong with the pasv returned port number\n");
		return -1;
	}
	
	printf("Dataport Info:\n");
	printf("IP: %s\n", IP);
	printf("port: %d\n", port);
	
	return port;
}


int ftpSendMessage(int socket_fd, char * message, int size){
	int sent = write(socket_fd, message, size);
	if(sent != size){
		printf("WARNING: Error while sending information to FTP server\n");
		printf("message was: %s\n", message);
		printf("Bytes sent: %d\n", sent);
		return -1;
	}
	
	return 0;
}
int ftpReadMessage(int socket_fd, char * message, int size){
	int res;
	res = read(socket_fd, message, size);
	return res;
}
void ftpAbort(int socket_fd, int socket_data){
	puts("\n !!! ABORTED !!! ");
	if(socket_fd) close(socket_fd);
	if(socket_data) close(socket_data);
}