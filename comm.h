#ifndef __MY_COMM_H__
#define __MY_COMM_H__
#include "graph.h"
#include <stdint.h>

#define MAX_EVENTS 512
#define MAX_PACKET_BUFFER_SIZE 1024

int init_comm_server_socket(node_t *node);
int network_start_pkt_receiver_thread(graph_t *topo);

#endif
