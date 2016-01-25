#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define u32 uint32_t

typedef struct
{
	u32 sock;
	struct sockaddr_in addrTo;
} Socket;

int sendData(int socket, int sendsize, char *buffer) {
   while(sendsize) {
      int len = send(socket, buffer, sendsize, 0);
      if (len <= 0) break;
      sendsize -= len;
      buffer += len;
   }
   return sendsize <= 0;
}

int main(int argc,char** argv){

	// Getting args
	char* host = (char*)(argv[1]);
	char* id = (char*)(argv[2]);
	char* pass = (char*)(argv[3]);
	
	// Creating client socket
	Socket* my_socket = (Socket*) malloc(sizeof(Socket));
	memset(&my_socket->addrTo, '0', sizeof(my_socket->addrTo)); 
	my_socket->addrTo.sin_family = AF_INET;
	my_socket->addrTo.sin_port = htons(80);
	my_socket->addrTo.sin_addr.s_addr = inet_addr(host);
	my_socket->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_socket->sock < 0){
		printf("\nFailed creating socket.");	
		return -1;
	}else printf("\nSocket created on port 80");
	fflush(stdout);
	
	// Connecting to the server
	int err = connect(my_socket->sock, (struct sockaddr*)&my_socket->addrTo, sizeof(my_socket->addrTo));
	if (err < 0 ){ 
		printf("\nFailed connecting server.");
		close(my_socket->sock);
		return -1;
	}else printf("\nConnection estabilished...");
	
	// Building HTTP header
	char header[256];
	sprintf(header,"GET /TSOI/%s_gamedata1.dat HTTP/1.1\r\n" \
				 "Host: rinnegatamante.it\r\n\r\n", id);
	
	// Sending request
	printf("\nDownloading savegame ...");
	fflush(stdout);	
	send(my_socket->sock, header, strlen(header), 0);
	
	// Receiving response
	int bytesReceived = 0;
	char response[1024*1024];
	while (bytesReceived == 0){
		bytesReceived = recv(my_socket->sock, &response, 1024*1024, 0);
	}	
	
	// Getting savegame size
	int c1 = response[179] - '0';
	int c2 = response[180] - '0';
	int c3 = response[181] - '0';
	int c4 = response[182] - '0';
	int length = c4 + c3 * 10 + c2 * 100 + c1 * 1000;
	printf(" Done!\n");
	
	// Updating local savegame
	int file_offset = bytesReceived - length;
	printf("Updating local savegame ...");
	fflush(stdout);
	FILE* output = fopen("persistentgamedata1.dat","wb");
	if (output < 0){
		printf("\nFile not found.");
		fflush(stdout);
		close(my_socket->sock);
		return -1;
	}
	fwrite(&response[file_offset], length, 1, output);
	printf(" Done! (%i KBs)\n\n",(length/1024));
	fclose(output);
	printf("Press ENTER to exit!");
	fflush(stdout);
	
	// Closing connection
	close(my_socket->sock);
	
	// Waiting input
	while (getchar() == 0){}
	return 1;
	
}