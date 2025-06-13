#include "comm.h"
#include "graph.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>

// static variable global to this file indicating next available port
static uint32_t next_free_port = 40000;

static uint32_t get_next_free_port(){
    uint32_t ret = next_free_port;
    next_free_port++;
    return ret;
}

int init_comm_server_socket(node_t *node){
    int sockfd;
    struct sockaddr_in server_addr;
    uint32_t server_listen_port = get_next_free_port();

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Fill server information
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
    server_addr.sin_port = htons(server_listen_port); // Port number

    // Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return -1;
    }

    // store the socket fd and port number into the node data structure
    node->comm_server_listen_port = server_listen_port;
    node->comm_udp_server_sock_fd = sockfd;

    return 0; //success
}
