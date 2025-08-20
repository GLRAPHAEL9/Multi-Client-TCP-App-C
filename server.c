// Standard library and networking headers for server functionality
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>
# include <arpa/inet.h>
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