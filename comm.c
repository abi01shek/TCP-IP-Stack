#include "comm.h"
#include "gluethread/glthread.h"
#include "graph.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include "gluethread/glthread.h"
#include "net.h"

// packet format is 32 bytes of header with interface name
// rest 2016 bytes of payload
#define MAX_COMM_PKT_SIZE 2048

// static variable global to this file indicating next available port
static uint32_t next_free_port = 40000;

/**
 * @brief Returns a new unused port number.
 *
 * @param  None
 * @return port unsigned 32 bit integer port number
 */
static uint32_t get_next_free_port(){
    uint32_t ret = next_free_port;
    next_free_port++;
    return ret;
}

/**
 * @brief Initialize a UDP server socket on a node to recieve messages for that node.
 *
 * Each node is assigned a free port number and a UDP socket
 * is bound to that port number to receive data for that node.
 *
 * @param  node: pointer to node whose data structures are filled
 * @return  0: Success
 *         <0: Fail
 */
int init_comm_server_socket(node_t *node){
    int sockfd;
    struct sockaddr_in server_addr;
    uint32_t server_listen_port = get_next_free_port();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Fill server information
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
    server_addr.sin_port = htons(server_listen_port); // Port number

    // Bind the socket to its source port and source IPs
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

int _comm_pkt_recv(node_t *node, char *comm_pkt, size_t comm_pkt_size){
    // extract the rx interface of the packet
    char *rx_if_name = comm_pkt;
    printf("Rx node name: %s\n", node->node_name);
    printf("Rx if name: %s\n", rx_if_name);
    printf("Data: %s\n", comm_pkt+IF_NAME_SIZE);
    printf("Data size: %lu\n", comm_pkt_size);
    return 0;
}

/**
 * @brief Thread function that monitors each node's comm socket for data reception.
 *
 * Server thread running on local host that monitors per-node UDP sockets for data
 * reception.
 *
 * @param  arg: graph topology
 * @return NULL
 *
 */
void* __network_start_pkt_receiver_thread(void* arg) {
    graph_t *topo = (graph_t*) arg;
    glthread_t *curr = NULL;
    node_t *node = NULL;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Add comm socket of each node to an epoll for monitoring
    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
        node = graph_glue_to_node(curr);
        int node_sock_fd = node->comm_udp_server_sock_fd;

        // Add the node's socket to epoll
        struct epoll_event ev = {
            .events = EPOLLIN,
            .data.fd = node_sock_fd
        };
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, node_sock_fd, &ev) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
    } ITERATE_GLTHREAD_END(topo->node_list, curr);

    struct epoll_event events[MAX_EVENTS];

    // thread polls forever on the sockets waiting for any readable data
    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; ++i) {
            int sockfd = events[i].data.fd;
            node_t *rx_node = NULL;
            glthread_t *curr;

            // Identify the receiving node
            ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
                node_t *curr_node = graph_glue_to_node(curr);
                if(curr_node->comm_udp_server_sock_fd == sockfd){
                    rx_node = curr_node;
                    break;
                }
            } ITERATE_GLTHREAD_END(&topo->node_list, curr);

            if(rx_node == NULL){
                printf("Unable to identify rx_node for sock_fd %d\n", sockfd);
                break;
            }

            char buffer[MAX_PACKET_BUFFER_SIZE];
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr *)&client_addr, &addr_len);
            if (len > 0) {
                buffer[len] = '\0';
                /* printf("[Socket FD %d] Received from %s:%d: %s, %s\n", */
                /*        sockfd, */
                /*        inet_ntoa(client_addr.sin_addr), */
                /*        ntohs(client_addr.sin_port), */
                /*        buffer, buffer+IF_NAME_SIZE); */

                // recv comm packet by the node.
                _comm_pkt_recv(rx_node, buffer, len);
            }
        }
    }
    close(epoll_fd);
    return NULL;
}

/**
 * @brief Launch a thread that monitors data reception on each node's socket
 *
 * Once the topology is created a separate thread is launched that handles
 * data reception of each node. This function launches that thread
 *
 * @param  topo: pointer to the graph topology
 * @return 0: Success
 *
 */
int network_start_pkt_receiver_thread(graph_t *topo){
    // Create a detached pthread that will monitor UDP recv sockets
    // of all nodes in the topology. Detached thread does not have to
    // be joined and will remove its resources when it is completed.
    pthread_t node_rcv_thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&node_rcv_thread, &attr, __network_start_pkt_receiver_thread, (void *)topo);
    return 0;
}


/**
 * @brief Creates a UDP socket to send a message to a given port.
 *
 * The port here identifies the node on the graph to which data
 * is sent.
 *
 * @param  dst_node_port: port number of destination node
 * @param  pkt: pointer to packet to send
 * @param  pkt_size: size in bytes of packet to send
 * @return 0: Success
 *        -1: Fail
 *
 */
int _send_pkt_out(int dst_node_port, char *pkt, size_t pkt_size){
    int sockfd;
    struct sockaddr_in server_addr;
    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Configure server address
    // In our case the server is running on a thread on the local host
    // the destination port identifies the node to which the packet is sent
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(dst_node_port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

     // Send message to server using sendto
    if (sendto(sockfd, pkt, pkt_size, 0,
               (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Sendto failed");
        close(sockfd);
        return -1;
    }

    // Close socket
    close(sockfd);
    return 0;
}

/**
 * @brief Send a packet out of an interface
 *
 * Gets the port number of the destination node attached to
 * the link connected to the interface. Then send the packet
 * to the port number after encapsulating the packet with
 * a header containing the destination node's interface name
 *
 * @param  pkt: pointer of data to be sent.
 * @param  pkt_size: length of data in bytes
 * @param out_interface: interface through which packet is to be sent.
 * @return 0: Success
 *        -1: Fail
 *
 */
int send_pkt_out(char *pkt, size_t pkt_size, interface_t* out_interface){
    // Get link connected to the interface
    interface_t *from_if = out_interface;
    link_t *if_link = from_if->link;
    if(if_link == NULL){
        printf("Link connected to interface %s not found\n", from_if->interface_name);
        return -1;
    }

    // Get the interface at the other end of the link
    interface_t *to_if;
    if(&if_link->if1 == from_if){
        to_if = &if_link->if2;
    } else {
        to_if = &if_link->if1;
    }

    // Get the destination node attached to the to interface
    node_t *to_node = to_if->attached_node;
    if(to_node == NULL){
        printf("Node connected to interface %s not found\n", to_if->interface_name);
        return -1;
    }

    // Allocate buffer to send in heap
    char *sndbuf = (char *)calloc(MAX_COMM_PKT_SIZE, 1);
    if(sndbuf == NULL){
        perror("Calloc");
        return -1;
    }

    // Create COMM packet: 2048 bytes.
    // First IF_NAME_SIZE bytes is interface name string
    // Remaining is data payload
    strncpy(sndbuf, to_if->interface_name, IF_NAME_SIZE);
    memcpy(sndbuf+IF_NAME_SIZE, pkt, pkt_size);

    // Get the port number of destination port and send
    int to_node_port = to_node->comm_server_listen_port;
    int ret = _send_pkt_out(to_node_port, sndbuf, IF_NAME_SIZE+pkt_size);
    free(sndbuf);
    return ret;
}
