// Moniques chat client in C
// Connects to the server and allows sending/receiving messages
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h> 

#pragma comment(lib, "ws2_32.lib") // This is the Link Winsock library

#define PORT 8080 
#define BUFFER_SIZE 1024 

SOCKET sock;
//This is a purple colored username
char username[] = "\033[35mMonique\033[0m"; 

