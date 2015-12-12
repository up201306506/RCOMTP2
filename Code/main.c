#include <stdio.h>  
#include <stdlib.h>

#include "auxiliar.h"

#define IPSIZE	16


int main(int argc, char** argv){

	
	/* Verify ARGS	*/
	if (argc != 2) { 
		printf("WARNING: wrong number of arguments \n");
		printf("usage: %s ftp://<user>:<password>@<host>/<url-path> \n", argv[0]);
		exit(1);
	}
	
	/* Test */
	char buf[IPSIZE];
	if(getIP(argv[1], buf) < 0)
		return -1;

	
	/* Retrieve Arguments */
	
	return 0;
}
