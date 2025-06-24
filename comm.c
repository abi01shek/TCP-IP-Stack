#include "comm.h"
#include "gluethread/glthread.h"
#include "graph.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include "gluethread/glthread.h"
#include "net.h"


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

/**
 * @brief Thread function that monitors each node's comm socket for data reception.
 *
 * <what fn does>
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
            char buffer[MAX_PACKET_BUFFER_SIZE];
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr *)&client_addr, &addr_len);
            if (len > 0) {
                buffer[len] = '\0';
                printf("[Socket FD %d] Received from %s:%d: %s\n",
                       sockfd,
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       buffer);
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
