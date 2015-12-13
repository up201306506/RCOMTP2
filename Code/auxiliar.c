#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>

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