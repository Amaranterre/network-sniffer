#ifndef SERVER_H
#define SERVER_H

#include "main.h"
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 65536

void* server_handler(void *arg) ;

extern message_control_t mcb;

#endif
