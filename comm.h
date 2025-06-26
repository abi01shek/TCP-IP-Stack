#ifndef __MY_COMM_H__
#define __MY_COMM_H__
#include "graph.h"
#include <stdint.h>

#define MAX_EVENTS 512
#define MAX_PACKET_BUFFER_SIZE 1024

int init_comm_server_socket(node_t *node);
int network_start_pkt_receiver_thread(graph_t *topo);
int data_link_pkt_receive(node_t *node, interface_t *rx_if,
                          char *pkt, size_t pkt_size);
int send_pkt_out(char *pkt, size_t pkt_size, interface_t* out_interface);
int send_pkt_flood(node_t *node, interface_t *exempted_intf,
                   char *pkt, unsigned int pkt_size);

#endif
