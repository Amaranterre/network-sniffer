#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8081
#define BUFFER_SIZE 1048576


void* server_handler(void *arg) {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    /**
     * Socket initialization
     */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; 
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        int read_size = read(client_fd, buffer, sizeof(buffer) - 1);
        if (read_size > 0) {
            buffer[read_size] = '\0';
            printf("Request:\n%s\n", buffer);

            /**
             * Construct repsonse
             */
            
            // Call `parsing_msgs` to get newest msgs
            char* response = "Hi";
            int length =  3;

            // Response client
            ssize_t bytes_sent = send(client_fd, response, length, 0);
            if (bytes_sent < 0) {
                perror("send failed");
            } else if (bytes_sent < length) {
                fprintf(stderr, "Partial send: only %zd of %d bytes sent\n", bytes_sent, length);
            }
        }

    }

}

int main() {
    pthread_t server_tid;

    pthread_create(&server_tid, NULL, server_handler, NULL);

    pthread_join(server_tid, NULL);

}
