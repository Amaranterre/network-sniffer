#include "sniffer.h"
#include "server.h"

/**
 * @brief Server and sniffer use mcb to communicate with each other.
 * We add mutex-lock to avoid race condition between writing of sniffer and reading of server.
 */
message_control_t mcb = {
    .msgs_head = NULL,
    .lock = PTHREAD_MUTEX_INITIALIZER,
};


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        return 1;
    }

    char* dev = argv[1];
    // char* dev = "eth0";
    pthread_t sniffer_tid;
    pthread_t server_tid;

    pthread_attr_t st_attr;
    pthread_attr_init(&st_attr);
    pthread_attr_setstacksize(&st_attr, 1048576 * 2);

    pthread_create(&sniffer_tid, NULL, sniffing, (char*)dev);
    pthread_create(&server_tid, &st_attr, server_handler, NULL);

    pthread_join(sniffer_tid, NULL);
    pthread_join(server_tid, NULL);

    pthread_attr_destroy(&st_attr);
    return 0;
}
