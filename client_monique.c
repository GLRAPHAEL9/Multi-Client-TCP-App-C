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

// Thread to continuously recieve messages from server
void recieve_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = recv(sock, buffer, sizeof(buffer ) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);  // Prints incoming messages
        fflush(stdout);
    }
}

int main() {
    WSADATA wsa;
    struct sockaddr_in server_addr; 

    // Initialise Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);

    // Created socket for client and check for errors.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { // Checking for errors
        printf("socket error\n");
        return 1;
    }

//Setup server address structure for connection to the server
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Localhost

// Connect to the server
if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("Connection to the server failed\n");
    return 1;
}
