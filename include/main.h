#ifndef MAIN_H
#define MAIN_H

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

typedef struct traffic_stat {
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    unsigned short ip_id;		/* identification */
    unsigned short ip_off;	
    uint8_t ip_p;			/* protocol */
    time_t timestamp;
    unsigned long bytes;
    struct traffic_stat* next;
} traffic_stat_t;

typedef struct message_control {
    // Use link list to store messages
    traffic_stat_t* msgs_head;

    // Use mutex to avoid race condition
    pthread_mutex_t lock;
} message_control_t;

extern message_control_t mcb;

#endif