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

// Thread to continuously receive messages from server
void receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);  // Print incoming message
        fflush(stdout);
    }
    _endthread();
}

int main() {
    WSADATA wsa;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket error\n");
        return 1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    // Send username to server
    send(sock, username, strlen(username), 0);

    // Start receiving messages in background thread
    _beginthread(receive_messages, 0, NULL);

    // Main loop: read user input and send to server
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        send(sock, buffer, strlen(buffer), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}