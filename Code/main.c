#include <stdio.h>  
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>



struct url_components{
	char user[256];
	char password[256];
	char host[256];
	char path[256];
}



int main(int argc, char** argv){

	//////////////////////////
	//	Verify ARGS	//
	//////////////////////////
	if (argc != 2) { 
		printf("WARNING: wrong number of arguments \n");
		printf("usage: %s ftp://<user>:<password>@<host>/<url-path> \n", argv[0]);
		exit(1);
	}
	
	return 0;

	//////////////////////////////////
	//	Retrieving arguments	//
	//////////////////////////////////
	
	
	
}
