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
	
	// Opening savedata
	printf("\nOpening savegame ...");
	fflush(stdout);
	FILE* input = fopen("persistentgamedata1.dat","r");
	if (input < 0){
		printf("\nFile not found.");
		fflush(stdout);
		close(my_socket->sock);
		return -1;
	}
	fseek(input, 0, SEEK_END);
	int file_size = ftell(input);
	fseek(input, 0, SEEK_SET);
	printf("Done! (%i KBs)",(file_size/1024));
	char* buffer = (char*)malloc(file_size);
	fread(buffer, file_size, 1, input);
	fclose(input);
	
	
	char header[256];
	
	// Building payload
	int payload_size = file_size + strlen(id) + strlen(pass) + 289;
	uint8_t* payload = malloc(payload_size);	
	char block1[128];
	char block2[128];
	char block3[128];
	char block4[128];
	int idx_payload = 0;
	int block_len;
	char* block;
	
	// First block
	sprintf(block1, "------startContent\r\n" \
					"Content-Disposition: form-data; name=\"l\"\r\n" \
					"\r\n");	
	block = block1;
	block_len = strlen(block1);
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	// Username
	block = id;
	block_len = strlen(id);
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	// Second block
	sprintf(block2, "\r\n" \
					"------startContent\r\n" \
					"Content-Disposition: form-data; name=\"p\"\r\n" \
					"\r\n");	
	block = block2;
	block_len = strlen(block2);
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	// Password
	block = pass;
	block_len = strlen(pass);
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	// Third block
	sprintf(block3, "\r\n" \
					"------startContent\r\n" \
					"Content-Disposition: form-data; name=\"save\"; filename=\"gamedata1.dat\"\r\n" \
					"Content-Type: application/octet-stream\r\n"
					"\r\n");	
	block = block3;
	block_len = strlen(block3);
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	// Savegame
	block = buffer;
	block_len = file_size;
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	
	// End block
	sprintf(block4, "\r\n------startContent--\r\n");
	block = block4;
	block_len = strlen(block4);
	memcpy(&payload[idx_payload], block, block_len);
	idx_payload = idx_payload + block_len;
	
	// Building HTTP header
	sprintf(header,"POST /TSOI/uploader.php HTTP/1.1\r\n" \
				 "Host: rinnegatamante.it\r\n" \
				 "Content-Length: %i\r\n" \
				 "Content-Type: multipart/form-data; " \
				 "boundary=----startContent\r\n\r\n", payload_size);
	
	// Creating TCP packet
	uint8_t* packet = malloc(payload_size + strlen(header));
	memcpy(packet, header, strlen(header));
	memcpy(&packet[strlen(header)], payload, payload_size);
	free(payload);
	
	// Uploading savegame on the server
	printf("\nUploading savegame ...");
	fflush(stdout);	
	send(my_socket->sock, packet, payload_size + strlen(header), 0);
	
	// Receiving response
	int bytesReceived = 0;
	char response[1024];
	while (bytesReceived == 0){
		bytesReceived = recv(my_socket->sock, &response, 1024, 0);
	}	
	if (response[strlen(response)-1] == '0') printf(" ERROR!\nAn error occurred!");
	else printf(" Done!");
	printf("\n\nPress ENTER to exit!");
	fflush(stdout);
	
	// Closing connection
	close(my_socket->sock);
	free(packet);
	
	// Waiting input
	while (getchar() == 0){}
	return 1;
	
}