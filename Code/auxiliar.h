#ifndef _AUXILIAR
#define _AUXILIAR

#define BUF_STRINGSIZE   256
#define IP_STRINGSIZE	16

#define FTP_PORT = 21;

struct url_components{
	char user[BUF_STRINGSIZE];
	char password[BUF_STRINGSIZE];
	char host[BUF_STRINGSIZE];
	char path[BUF_STRINGSIZE];
	char IP[IP_STRINGSIZE];
	
	int mode;
};

void clearscreen();

int getIP(char* address, char * result);

int checkURL(char* url, struct url_components* result);
int parseURL(char * url, struct url_components * data);
int parseURL_aux(char * buffer, char * result, char escape);


#endif