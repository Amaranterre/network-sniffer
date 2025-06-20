#ifndef SNIFFER_H
#define SNIFFER_H

#include "main.h"

#define SNAP_LEN 1518

void* sniffing(void *arg) ;

const char* get_protocol_name(uint8_t ip_p);

extern message_control_t mcb;

#endif
