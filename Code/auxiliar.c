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

	// server address handling
	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */

	// open an TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return -1;
	}

	// connect to the server
	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			< 0) {
		perror("connect()");
		return -1;
	}
	
	char temp[BUF_LARGERSIZE];
	if (ftpReadMessage(sockfd, temp, sizeof(temp)) < 0 ) {
		printf("WARNING: Couldn't Read Response to socketing.\n");
		return -1;
	}

	return sockfd;
}
void disconnectSocket(int sockfd) {
	close(sockfd);
}

int loginFTP(char * user,char * password){
	//char buffer[BUF_LARGERSIZE];
	
	return 0;
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
	memset(message, 0, size);
	res = read(socket_fd, message, size);
	puts(message);
	return res;
}
void ftpAbort(int socket_fd, int socket_data){
	puts("Aborting sockets");
	if(socket_fd) close(socket_fd);
	if(socket_data) close(socket_data);
}