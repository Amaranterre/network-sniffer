#include "server.h"
#include "sniffer.h"
#include "cJSON.h"

cJSON *msgs_bus = NULL;

void print_msgs(cJSON* msgs) {
    char *out = cJSON_Print(msgs);
    printf("JSON array of objects:\n%s\n", out);    
    free(out);
}

/**
 * @brief We use cJSON to parse messages to json.
 */
void parsing_msgs() {

    pthread_mutex_lock(&mcb.lock);

    traffic_stat_t* curr = mcb.msgs_head;

    if(msgs_bus) {
        cJSON_Delete(msgs_bus);

    }

    msgs_bus = cJSON_CreateArray();
    
    while (curr) {
        cJSON *msg = cJSON_CreateObject();

        // strftime(time_str, sizeof(time_str), "%F %T", localtime(&(curr->timestamp)));        
        
        cJSON_AddNumberToObject(msg, "time_stamp", curr->timestamp);
        cJSON_AddStringToObject(msg, "src_ip", curr->src_ip);
        cJSON_AddStringToObject(msg, "dst_ip", curr->dst_ip);
        cJSON_AddNumberToObject(msg, "size", curr->bytes);
        cJSON_AddStringToObject(msg, "protocol", get_protocol_name(curr->ip_p));
        cJSON_AddNumberToObject(msg, "transaction_id", curr->ip_id);

        cJSON_AddItemToArray(msgs_bus, msg);  // 加入数组

        curr = curr->next;

    }

    pthread_mutex_unlock(&mcb.lock);
}

/**
 * @brief We implement a simple socket server to communicate to client.
 */
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

    // I choose it randomly -_-
    int socket_buffer_size = 3 * BUFFER_SIZE;
    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        // Increase buffer size
        setsockopt(client_fd, SOL_SOCKET, SO_SNDBUF, &socket_buffer_size, sizeof(socket_buffer_size));
        setsockopt(client_fd, SOL_SOCKET, SO_RCVBUF, &socket_buffer_size, sizeof(socket_buffer_size));

        int read_size = read(client_fd, buffer, sizeof(buffer) - 1);
        if (read_size > 0) {
            buffer[read_size] = '\0';
            // printf("Request:\n%s\n", buffer);

            /**
             * Construct repsonse
             */
            
            // Call `parsing_msgs` to get newest msgs
            parsing_msgs();
            char *str_json = cJSON_Print(msgs_bus);
            if (str_json == NULL) {
                const char *error_response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
                send(client_fd, error_response, strlen(error_response), 0);
                close(client_fd);
                continue;
            }

            // 计算所需缓冲区大小
            size_t header_len = snprintf(NULL, 0,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: %zu\r\n"
                "Connection: close\r\n"
                "\r\n",
                strlen(str_json));
            size_t total_len = header_len + strlen(str_json) + 1;
            char *response = malloc(total_len);
            if (!response) {
                free(str_json);
                close(client_fd);
                continue;
            }
            // 构造响应
            snprintf(response, total_len,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: %zu\r\n"
                "Connection: close\r\n"
                "\r\n"
                "%s",
                strlen(str_json), str_json);

            // Response client
            // Big packet may not be sent in on function call
            size_t total_sent = 0;
            while(total_sent < total_len) {
                ssize_t bytes_sent = send(client_fd, response + total_sent, total_len - total_sent, 0);
                if (bytes_sent < 0) {
                    perror("send failed");
                } 
                total_sent += bytes_sent;
            }


            // Recycle memory
            free(str_json);
            free(response);
        }
        close(client_fd);
    }

}