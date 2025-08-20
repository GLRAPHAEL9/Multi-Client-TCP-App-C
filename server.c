// Standard library and networking headers for server functionality
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>
#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/in.h>
#endif
# include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
char client_names[MAX_CLIENTS][50];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *log_file;

void send_message(char *message, int sender);
void *handle_client(void *arg);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Open log file
    log_file = fopen("chat_log.txt", "a");
    if (!log_file) {
        perror("Could not open log file");
        exit(EXIT_FAILURE);
    }

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                clients[i] = new_socket;

                int *new_client_index = malloc(sizeof(int));
                *new_client_index = i;

                pthread_t tid;
                pthread_create(&tid, NULL, handle_client, new_client_index);
                pthread_detach(tid);
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    fclose(log_file);
    return 0;
}

void *handle_client(void *arg) {
    int index = *((int *)arg);
    free(arg);

    int sock = clients[index];
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Receive username
    bytes_read = recv(sock, buffer, sizeof(buffer)-1, 0);
    if (bytes_read <= 0) {
        close(sock);
        clients[index] = 0;
        return NULL;
    }
    buffer[bytes_read] = '\0';
    strcpy(client_names[index], buffer);

    // Join message
    char join_msg[BUFFER_SIZE];
    snprintf(join_msg, sizeof(join_msg), "%s has joined the chat.\n", client_names[index]);
    printf("%s", join_msg);
    fprintf(log_file, "%s", join_msg);
    fflush(log_file);
    send_message(join_msg, sock);

    // Chat loop
    while ((bytes_read = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[bytes_read] = '\0';

        char msg[BUFFER_SIZE + 50];
        snprintf(msg, sizeof(msg), "%s: %s", client_names[index], buffer);
        printf("%s", msg);

        fprintf(log_file, "%s", msg);   // log clean text
        fflush(log_file);

        send_message(msg, sock);
    }

    // Leave message
    char leave_msg[BUFFER_SIZE];
    snprintf(leave_msg, sizeof(leave_msg), "%s has left the chat.\n", client_names[index]);
    printf("%s", leave_msg);
    fprintf(log_file, "%s", leave_msg);
    fflush(log_file);
    send_message(leave_msg, sock);

    close(sock);
    clients[index] = 0;   // free slot
    return NULL;
}

void send_message(char *message, int sender) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != 0 && clients[i] != sender) {
            char colored_message[BUFFER_SIZE + 100];

            // Color Gabriel blue, Monique purple
            if (strstr(message, "Gabriel:") == message) {
                snprintf(colored_message, sizeof(colored_message), "\033[1;34m%s\033[0m", message);
            } else if (strstr(message, "Monique:") == message) {
                snprintf(colored_message, sizeof(colored_message), "\033[1;35m%s\033[0m", message);
            } else {
                snprintf(colored_message, sizeof(colored_message), "%s", message);
            }

            send(clients[i], colored_message, strlen(colored_message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}