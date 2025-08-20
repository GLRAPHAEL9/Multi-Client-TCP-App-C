// Windows Winsock2 version. Updated the libaries for windows compatibility.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

SOCKET clients[MAX_CLIENTS];
char client_names[MAX_CLIENTS][50];

DWORD WINAPI client_handler(void *client_socket) {
    SOCKET sock = *(SOCKET *)client_socket;
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int index = -1;

    // find index
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == sock) {
            index = i;
            break;
        }
    }

    // receive username
    bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        closesocket(sock);
        return 0;
    }
    buffer[bytes_read] = '\0';
    strcpy(client_names[index], buffer);

    FILE *log_file = fopen("chat_log.txt", "a");
    fprintf(log_file, "%s joined the chat.\n", client_names[index]);
    fclose(log_file);

    while ((bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';

        char msg[BUFFER_SIZE + 50];
        snprintf(msg, sizeof(msg), "%s: %s", client_names[index], buffer);

        // log
        log_file = fopen("chat_log.txt", "a");
        fprintf(log_file, "%s", msg);
        fclose(log_file);

        // broadcast
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != INVALID_SOCKET && clients[i] != sock) {
                send(clients[i], msg, strlen(msg), 0);
            }
        }
    }

    closesocket(sock);
    clients[index] = INVALID_SOCKET;
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket failed\n");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed\n");
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = INVALID_SOCKET;

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            continue;
        }

        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == INVALID_SOCKET) {
                clients[i] = new_socket;
                CreateThread(NULL, 0, client_handler, &clients[i], 0, NULL);
                break;
            }
        }
        if (i == MAX_CLIENTS) {
            char *msg = "Server full\n";
            send(new_socket, msg, strlen(msg), 0);
            closesocket(new_socket);
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
