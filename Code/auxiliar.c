#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>

/*struct url_components{
	char user[256];
	char password[256];
	char host[256];
	char path[256];
}*/

int getIP(char * address, char * result){
	
	struct hostent *h;

	if ((h=gethostbyname(address)) == NULL) {  
		herror("gethostbyname");
		return -1;
	}
	
	strcpy(result, inet_ntoa(*((struct in_addr *)h->h_addr)));
	
	printf("Server Info:\n");
	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n", result);
	
	

	return 0;
}