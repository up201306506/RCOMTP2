#include <stdio.h>  
#include <stdlib.h>


#include <sys/types.h>
#include <regex.h>

#include "auxiliar.h"

//./Download ftp://[user:pass@]example.com/file.jpg
//./Download ftp://example.com/file.jpg


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
	
	
	puts("\n --- Info --- ");
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
		ftpAbort(data.socket_fd, data.data_socket_fd);
		return -1;
	}
	
	/* Login */
	puts("\n --- Logging in --- ");
	if (ftpLogin(data.user, data.password, data.socket_fd)) {
		printf("WARNING: Could not login");
		ftpAbort(data.socket_fd, data.data_socket_fd);
		return -1;
	}
	
	/* Passive Mode */
	puts("\n --- Sending Passive Mode Request --- ");
	
	int temp= ftpPasv(data.socket_fd);
	if (temp < 0) {
		printf("WARNING: Problem during passive mode request");
		ftpAbort(data.socket_fd, data.data_socket_fd);
		return -1;
	}
	data.data_socket_fd = temp;
	
	/* Send Retrieve Command */
	puts("\n --- Sending Retrieve File Request --- ");
	
	temp = ftpRetr(data.socket_fd, data.path);
	if (temp < 0) {
		printf("WARNING: Problem during retr request");
		ftpAbort(data.socket_fd, data.data_socket_fd);
		return -1;
	}
	data.filesize = temp;
	printf("File Size: %d \n", data.filesize);
	
	/* Download File */
	puts("\n --- Downloading File --- ");
	temp = ftpDownload(data.data_socket_fd, data.filesize, data.path);
	if (temp < 0) {
		printf("WARNING: Problem during download");
		ftpAbort(data.socket_fd, data.data_socket_fd);
		return -1;
	}
	puts("\n --- File Transfer Complete! --- ");
	
	
	
	/* Disconnect Socket */
	puts("\n --- Disconnecting --- ");
	temp = ftpDisconnect(data.socket_fd, data.data_socket_fd);
	if (temp < 0) {
		printf("WARNING: Problem during disconnection");
		ftpAbort(data.socket_fd, data.data_socket_fd);
		return -1;
	}
	
	puts("Done");
	return 0;
}
