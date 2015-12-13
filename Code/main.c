#include <stdio.h>  
#include <stdlib.h>


#include <sys/types.h>
#include <regex.h>

#include "auxiliar.h"

//./Download ftp://[user:pass@]example.com/wat
//./Download ftp://example.com/wat


int main(int argc, char** argv){

	clearscreen();

	/* Verify ARGS	*/
	if (argc != 2) { 
		printf("WARNING: wrong number of arguments \n");
		printf("usage: %s ftp://[<user>:<password>@]<host>/<url-path> \n", argv[0]);
		exit(1);
	}
	
	
	/* Retrieve Arguments */
	struct url_components data;
	if(checkURL(argv[1], &data))
		return -1;
	
	if(parseURL(argv[1], &data))
		return -1;
	
	if (data.mode  == 1){
	printf("User: %s \n", data.user);
	printf("Pass: %s \n",data.password);	
	}
	
	/* Retrieve IP from DNS*/
	if(getIP(data.host, data.IP) < 0)
		return -1;
	
	/* Connect a Socket */
	data.socket_fd = connectSocket(data.IP, FTP_PORT);
	if (data.socket_fd  < 0) {
		printf("WARNING: Could not connect socket.\n");
		return 1;
	}
	
	/* Disconnect Socket */
	disconnectSocket(data.socket_fd);
	
	
	puts("Done");
	return 0;
}
