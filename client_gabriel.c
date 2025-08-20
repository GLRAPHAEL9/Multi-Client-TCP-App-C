// Gabriel's Chat Client
// Windows Winsock2 Version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

SOCKET sock;
// Blue colored username (works in VS Code + Windows Terminal)
char username[] = "\033[34mGabriel\033[0m";