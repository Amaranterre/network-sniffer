#include "sniffer.h"

traffic_stat_t* find_or_create_stat(const struct ip* iphdr, time_t timestamp) {
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(iphdr->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(iphdr->ip_dst), dst_ip, INET_ADDRSTRLEN);


    pthread_mutex_lock(&mcb.lock);

    traffic_stat_t* curr = mcb.msgs_head;
    
    traffic_stat_t* new_stat = (traffic_stat_t*)malloc(sizeof(traffic_stat_t));
    if(!new_stat) {
        perror("Failed to allocate memory for traffic_stat_t");
        pthread_mutex_unlock(&mcb.lock);
        return NULL;
    }

    strncpy(new_stat->src_ip, src_ip, sizeof(new_stat->src_ip) - 1);
    strncpy(new_stat->dst_ip, dst_ip, sizeof(new_stat->dst_ip) - 1);
    new_stat->src_ip[sizeof(new_stat->src_ip) - 1] = '\0';
    new_stat->dst_ip[sizeof(new_stat->dst_ip) - 1] = '\0';

    new_stat->timestamp = timestamp;
    new_stat->bytes = 0;
    new_stat->ip_p = iphdr->ip_p;
    new_stat->ip_off = iphdr->ip_off;
    new_stat->ip_id = iphdr->ip_id;

    new_stat->next = mcb.msgs_head;
    mcb.msgs_head = new_stat;
    pthread_mutex_unlock(&mcb.lock);

    return new_stat;
}

void packet_handler(u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {
    const struct ether_header* ethernet = (struct ether_header*)(packet);
    if (ntohs(ethernet->ether_type) != ETHERTYPE_IP) {
        return;
    }

    const struct ip* iphdr = (struct ip*)(packet + sizeof(struct ether_header));

    time_t timestamp = header->ts.tv_sec;
    traffic_stat_t* stat = find_or_create_stat(iphdr, timestamp);
    stat->bytes += header->len;
}

const char* get_protocol_name(uint8_t ip_p) {
    switch (ip_p) {
        case IPPROTO_ICMP: return "ICMP";
        case IPPROTO_TCP: return "TCP";
        case IPPROTO_UDP: return "UDP";
        case IPPROTO_IGMP: return "IGMP";
        // case IPPROTO_OSPF: return "OSPF";
        default: return "UNKNOWN";
    }
}

void print_stats() {
    int cnt = 0;

    pthread_mutex_lock(&mcb.lock);

    int total_size = 0;
    int size_sec_60 = 0;
    int size_sec_30 = 0;
    time_t now = time(NULL);

    traffic_stat_t* curr = mcb.msgs_head;
    while (curr) {
        if(cnt < 10) {
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%F %T", localtime(&(curr->timestamp)));
            printf(
                "[%s]\t\t%s\t%s\t%lu bytes\t%s\t%d\n", 
                time_str, 
                curr->src_ip, 
                curr->dst_ip, 
                curr->bytes,
                get_protocol_name(curr->ip_p),
                curr->ip_id
            );
        } else if (cnt == 10) {
            printf("......\n\n");
        }
        total_size += curr->bytes;
        if(now - curr->timestamp <= 60) {
            size_sec_60 += curr->bytes;
        }
        if(now - curr->timestamp <= 30) {
            size_sec_30 += curr->bytes;
        }
        curr = curr->next;            

        ++cnt;
    }
    
    printf("-- Total size: %d\n", total_size);
    printf("-- Past 60 seconds: %d\n", size_sec_60);
    printf("-- Past 30 seconds: %d\n", size_sec_30);
    
    printf("-------------------------------------------------\n");
    pthread_mutex_unlock(&mcb.lock);

}



void* sniffing(void *arg) {
    char *dev = arg;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle;

    handle = pcap_create(dev, errbuf);
    if (pcap_set_timeout(handle, 1000) != 0) {
        fprintf(stderr, "Failed to set time out : %s\n", pcap_geterr(handle));
    }
    if (pcap_set_promisc(handle, 1) != 0) {
        fprintf(stderr, "Failed to set promisc: %s\n", pcap_geterr(handle));
    }
    if (pcap_set_snaplen(handle, SNAP_LEN) != 0) {
        fprintf(stderr, "Failed to set snap len %s\n", pcap_geterr(handle));
    }
    if (pcap_set_buffer_size(handle, 1024 * 1024 * 10) != 0) {
        fprintf(stderr, "Failed to set buffer size: %s\n", pcap_geterr(handle));
    }
    if (pcap_activate(handle) != 0 ) {
        fprintf(stderr, "Failed to activate handle: %s\n", pcap_geterr(handle));
        return NULL;
    }



    printf("Listening on %s...\n", dev);

    // Start capture packet
    while (1) {
        pcap_dispatch(handle, 0, packet_handler, NULL);
        print_stats();
        sleep(0.5);
    }

    pcap_close(handle);
    return NULL;
}
