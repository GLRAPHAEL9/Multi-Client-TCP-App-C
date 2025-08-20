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
char clients_names[MAX_CLIENTS][50];
FILE *log_file;

void *handle_client(void *arg);

// Current time stamp as [HH:MM:SS]
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%H:%M:%S]", t);
}

void send_message(char *message, int sender) {
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

for (int i = 0; i <MAX_CLIENTS; i++) {
    if (clients[i] != 0 && clients[i] !=sender) {
        char colored_message[BUFFER_SIZE + 100];

        // Applied Colours for Gabriel and Monique
        if (strstr(message, "Gabriel:") == message || strstr(message, "Gabriel has") == message) {
            snprintf(colored_message, sizeof(colored_message), "%s \033[1;34m%s\033[0m", timestamp, message); //Blue
        } else if (strstr(message, "Monique:") == message || strstr(message, "Monique has") == message) {
            snprint(colored_message, sizeof(colored_message), "%s \033[1,35m%s\033[0m", timestamp, message); //Purple
        } else {
            snprintf(colored_message, sizeof(colored_message), "%s %s", timestamp, message); //Default colour
        }

        send(clients[i], colored_message, strlen(colored_message), 0);
    }
}

 // Log to file (plain text, no colors)
    fprintf(log_file, "%s %s", timestamp, message);
    fflush(log_file);
}