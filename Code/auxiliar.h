#ifndef _AUXILIAR
#define _AUXILIAR

#define BUF_STRINGSIZE   256
#define BUF_LARGERSIZE   1024
#define IP_STRINGSIZE	16

#define FTP_PORT 21


struct url_components{
	char user[BUF_STRINGSIZE];
	char password[BUF_STRINGSIZE];
	char host[BUF_STRINGSIZE];
	char path[BUF_STRINGSIZE];
	char IP[IP_STRINGSIZE];
	
	int socket_fd;
	int data_socket_fd;
	
	int mode;
};

void clearscreen();

int getIP(char* address, char * result);

int checkURL(char* url, struct url_components* result);
int parseURL(char * url, struct url_components * data);
int parseURL_aux(char * buffer, char * result, char escape);

int connectSocket(char * IP, int port);
void disconnectSocket(int sockfd);

int ftpLogin(char * user,char * password, int socket_fd);
int ftpPasv(int socket_fd);

void ftpAbort(int socket_fd, int socket_data);


//Auxiliars
int ftpSendMessage(int socket_fd, char * message, int size);
int ftpReadMessage(int socket_fd, char * message, int size);
int parsePasvResponse(char * message, char * IP);



#endif