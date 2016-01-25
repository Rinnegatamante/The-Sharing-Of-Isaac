#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <3ds.h>
#include <fcntl.h>
#include "tinyxml2.h"

#define SERVER_ADDR "rinnegatamante.it/TSOI/"
using namespace tinyxml2;

struct config{
	char id[13];
	char psw[13];
	char host[32];
};
typedef struct
{
	int sock;
	struct sockaddr_in addrTo;
} Socket;

FS_Archive saveArchive;
Handle fsHandle;
u32 saveSize;

void loadConfig(config* cfg, char* path)
{
	
	// Loading XML file
	XMLDocument doc;
	doc.LoadFile(path);
	
	// Getting elements
	XMLElement* id = doc.FirstChildElement("id");
	const char* tmp = id->GetText();
	strcpy(cfg->id, tmp);
	XMLElement* psw = doc.FirstChildElement("password");
	const char* tmp2 = psw->GetText();
	strcpy(cfg->psw, tmp2);
	XMLElement* host = doc.FirstChildElement("host");
	const char* tmp3 = host->GetText();
	strcpy(cfg->host, tmp3);
	
}

int downloadSavegame(char* login, char* host){
	printf("Initializing httpc service... ");
	gfxFlushBuffers();
	Result res = httpcInit();
	if (res != 0){
		printf("ERROR (%i)\n",res);
		printf("Press B to continue.\n");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return -1;
	}else printf("Done!\n");
	gfxFlushBuffers();
	char url[256];
	sprintf(url, "http://%s%s_gamedata1.dat", host, login);
	httpcContext context;
	u32 statuscode=0;
	printf("Downloading file... ");
	gfxFlushBuffers();
	Result ret = httpcOpenContext(&context, (char*)url , 0);
	httpcBeginRequest(&context);
	u32 contentsize=0;
	httpcGetResponseStatusCode(&context, &statuscode, 0);
	if (statuscode == 200){
		httpcGetDownloadSizeState(&context, NULL, &contentsize);
		u8* buf = (u8*)malloc(contentsize);
		memset(buf, 0, contentsize);
		httpcDownloadData(&context, buf, contentsize, NULL);
		Handle fileHandle;
		u32 bytesWritten;
		FILE* output = fopen("sdmc://gamedata1.dat","wb");
		fwrite(buf, contentsize, 1, output);
		fclose(output);
		free(buf);
	}else{
		printf("ERROR (%i)\n\n",statuscode);
		printf("Press B to continue.\n");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return -1;
	}
	printf("Done!\n");
	gfxFlushBuffers();
	httpcCloseContext(&context);
	httpcExit();
	return 0;
}

void restoreSavegame(char* filename){
	FS_Path filePath=fsMakePath(PATH_ASCII, filename);
	Handle output;
	int fileSize;
	printf("Opening downloaded savegame... ");
	FILE* input = fopen("sdmc://gamedata1.dat","rb");
	if (input == NULL){
		printf("ERROR\n\n");
		printf("Press B to continue.");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return;
	}else printf("Done!\nOpening local savegame... ");
	gfxFlushBuffers();
	fseek(input, 0, SEEK_END);
	fileSize = ftell(input);
	fseek(input, 0, SEEK_SET);
	u8* buffer = (u8*)malloc(fileSize);
	fread(buffer, fileSize, 1, input);
	fclose(input);
	Result ret = FSUSER_OpenFile(&output, saveArchive, filePath, FS_OPEN_READ, 0x00000000);
	if (ret == 0){
		FSFILE_Close(output);
		svcCloseHandle(output);
		FSUSER_DeleteFile(saveArchive, filePath);
	}
	ret = FSUSER_OpenFile( &output, saveArchive, filePath, FS_OPEN_WRITE | FS_OPEN_CREATE, 0x00000000);
	if (ret != 0){
		printf("ERROR (%i)\n\n", ret);
		printf("Press B to continue.");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return;
	}else printf("Done!\nWriting savegame... ");
	gfxFlushBuffers();
	FSFILE_Write(output, NULL, 0, buffer, fileSize, 0x10001);
	FSFILE_Close(output);
	FSUSER_ControlArchive(saveArchive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
	svcCloseHandle(output);
	printf("Done!\n");
	gfxFlushBuffers();
}

u8* dumpSavegame(char* filename){
	FS_Path filePath=fsMakePath(PATH_ASCII, filename);
	Handle input;
	u64 fileSize;
	printf("Opening local savegame... ");
	Result ret = FSUSER_OpenFile(&input, saveArchive, filePath, FS_OPEN_READ, 0x00000000);
	if (ret != 0){
		printf("ERROR (%i)\n\n", ret);
		printf("Press B to continue.");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return NULL;
	}else printf("Done!\nReading data... ");
	FSFILE_GetSize(input, &fileSize);
	u8* buffer = (u8*)malloc(fileSize);
	FSFILE_Read(input, NULL, 0, buffer, fileSize);
	FSFILE_Close(input);
	svcCloseHandle(input);
	printf("Done!\n");
	saveSize = fileSize;
	gfxFlushBuffers();
	return buffer;
}

void updateSavegame(char* id, char* pass, char* host, u8* save){
	
	// Initializing socket service
	printf("Initializing soc:U service... ");
	u32* sharedMem = (u32*)memalign(0x1000, 0x100000);
	Result ret = socInit(sharedMem, 0x100000);
	if (ret != 0){
		printf("ERROR (%i)\n\n", ret);	
		printf("Press B to continue.");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return;
	}else printf("Done!\n");
	gfxFlushBuffers();
	
	// Creating client socket
	Socket* my_socket = (Socket*) malloc(sizeof(Socket));
	memset(&my_socket->addrTo, '0', sizeof(my_socket->addrTo)); 
	my_socket->addrTo.sin_family = AF_INET;
	my_socket->addrTo.sin_port = htons(80);
	my_socket->addrTo.sin_addr.s_addr = inet_addr(host);
	my_socket->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_socket->sock < 0){
		printf("ERROR: Failed creating socket.\n\n");	
		printf("Press B to continue.");
		gfxFlushBuffers();
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return;
	}else printf("Socket created on port 80.\n");
	gfxFlushBuffers();
	
	// Connecting to the server
	int err = connect(my_socket->sock, (struct sockaddr*)&my_socket->addrTo, sizeof(my_socket->addrTo));
	if (err < 0 ){ 
		printf("ERROR: Failed connecting server.\n\n");
		printf("Press B to continue.");
		gfxFlushBuffers();
		close(my_socket->sock);
		while(!(hidKeysDown() & KEY_B)) hidScanInput();
		return;
	}else printf("Connection estabilished.\nBuilding payload... ");
	gfxFlushBuffers();
	
	char header[256];
	
	// Building payload
	int payload_size = saveSize + strlen(id) + strlen(pass) + 289;
	u8* payload = (u8*)malloc(payload_size);	
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
	block = (char*)save;
	block_len = saveSize;
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
	u8* packet = (u8*)malloc(payload_size + strlen(header));
	memcpy(packet, header, strlen(header));
	memcpy(&packet[strlen(header)], payload, payload_size);
	free(payload);
	
	// Uploading savegame on the server
	printf("Done!\nUploading savegame... ");
	gfxFlushBuffers();	
	send(my_socket->sock, packet, payload_size + strlen(header), 0);
	
	// Receiving response
	int bytesReceived = 0;
	char response[1024];
	while (bytesReceived == 0){
		bytesReceived = recv(my_socket->sock, &response, 1024, 0);
	}	
	if (response[strlen(response)-1] == '0') printf(" ERROR!");
	else printf(" Done!");
	printf("\n\nPress B to continue.");
	gfxFlushBuffers();
	
	// Closing connection
	close(my_socket->sock);
	free(packet);
	socExit();
	free(sharedMem);
	
	while(!(hidKeysDown() & KEY_B)) hidScanInput();
	
}

void deleteSavegame(char* filename){
	printf("Deleting temporary files... ");
	remove(filename);
	printf("Done!\n\nPress B to continue.");
	gfxFlushBuffers();
	while(!(hidKeysDown() & KEY_B)) hidScanInput();
}

void printMenu(config settings){
	printf("* The Sharing of Isaac v.0.1 *\n\n");
	printf("Login ID: %s\n", settings.id);
	printf("Password: %s\n", settings.psw);
	printf("Host Address: %s\n\n\n", settings.host);
	printf("Press A to update LOCAL savegame\n", settings.host);
	printf("Press Y to update ONLINE savegame\n", settings.host);
	printf("Press START to exit\n", settings.host);
}

int main(){

	// Start basic stuffs
	gfxInit(GSP_RGB565_OES,GSP_RGB565_OES,false);
	gfxSetDoubleBuffering(GFX_TOP, false);
	gfxSetDoubleBuffering(GFX_BOTTOM, false);
	gfxSet3D(false);
	consoleInit(GFX_TOP, NULL);
	srvGetServiceHandleDirect(&fsHandle, "fs:USER");
	FSUSER_Initialize(fsHandle);
	fsUseSession(fsHandle, false);
	sdmcInit();
	hidInit();
	saveArchive = (FS_Archive){ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, NULL), fsHandle};
	FSUSER_OpenArchive(&saveArchive);
	
	// Loading config file
	config settings;
	loadConfig(&settings, "sdmc://TSOI.cfg");
	printMenu(settings);
	
	// Main Loop
	for (;;){
		hidScanInput();
		if (hidKeysDown() & KEY_A){
			consoleClear();
			int ret = downloadSavegame(settings.id, SERVER_ADDR);
			if (ret == 0){
				restoreSavegame("/gamedata1.dat");
				deleteSavegame("sdmc://gamedata1.dat");
			}
			consoleClear();
			printMenu(settings);
		}else if (hidKeysDown() & KEY_Y){
			consoleClear();
			u8* content = dumpSavegame("/gamedata1.dat");
			if (content != NULL){
				updateSavegame(settings.id, settings.psw, settings.host, content);
				free(content);
			}
			consoleClear();
			printMenu(settings);
		}else if (hidKeysDown() & KEY_START) break;
		gfxFlushBuffers();
	}
	
	consoleClear();
	printf("Exiting...");
	gfxFlushBuffers();
	
	// Closing Services
	FSUSER_CloseArchive(&saveArchive);
	sdmcExit();
	svcCloseHandle(fsHandle);
	hidExit();
	gfxExit();
	
}