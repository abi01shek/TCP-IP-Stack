/**
 * @file topologies.c
 * @author Abishek Ramdas
 * @brief Functions to build different topologies.
 */

#include "graph.h"
#include "net.h"
#include "comm.h"

// My network
/*********************************************************************************/
// #if 0                                                                         */
/*                                                                               */
/*                           +----------+                                        */
/*                       0/4 |          |0/0                                     */
/*          +----------------+   R0_re  +---------------------------+            */
/*          |     40.1.1.1/24| 122.1.1.0|20.1.1.1/24                |            */
/*          |                +----------+                           |            */
/*          |                                                       |            */
/*          |                                                       |            */
/*          |                                                       |            */
/*          |40.1.1.2/24                                            |20.1.1.2/24 */
/*          |0/5                                                    |0/1         */
/*      +---+---+                                              +----+-----+      */
/*      |       |0/3                                        0/2|          |      */
/*      | R2_re +----------------------------------------------+    R1_re |      */
/*      |       |30.1.1.2/24                        30.1.1.1/24|          |      */
/*      +-------+                                              +----------+      */
/*                                                                               */
/* #endif                                                                        */
/*********************************************************************************/

/**
 * @brief Create a graph of nodes.
 *
 * Creates nodes, configures and assigns connections between them
 * Also starts a thread that monitors comm sockets of these nodes
 *
 * @param  None
 * @return pointer to graph
 *
 */
graph_t * build_first_topo() {
    graph_t *topo = create_new_graph("first_topo");
    node_t *R0_re = create_graph_node(topo, "R0_re");
    node_t *R1_re = create_graph_node(topo, "R1_re");
    node_t *R2_re = create_graph_node(topo, "R2_re");

    insert_link_between_two_nodes(R0_re, R1_re, "eth0", "eth1", 5);
    insert_link_between_two_nodes(R1_re, R2_re, "eth2", "eth3", 4);
    insert_link_between_two_nodes(R0_re, R2_re, "eth4", "eth5", 9);

    // Configure the loop back IPs of each node
    if(node_set_loopback_address(R0_re, "122.1.1.0") < 0){
        printf("Unable to assign loopback address to node %s\n", R0_re->node_name);
        return NULL;
    }

    if(node_set_loopback_address(R1_re, "122.1.1.1") < 0){
        printf("Unable to assign loopback address to node %s\n", R1_re->node_name);
        return NULL;
    }

    if(node_set_loopback_address(R2_re, "122.1.1.2") < 0){
        printf("Unable to assign loopback address to node %s\n", R2_re->node_name);
        return NULL;
    }

    // Assign IP address to each interface
    if(node_set_intf_ip_address(R0_re, "eth0", "20.1.1.1", 24) < 0){
        printf("Unale to assign IP address to interface eth0 of node %s\n", R0_re->node_name);
        return NULL;
    }
    if(node_set_intf_ip_address(R0_re, "eth4", "40.1.1.1", 24) < 0){
        printf("Unale to assign IP address to interface eth4 of node %s\n", R0_re->node_name);
        return NULL;
    }
    if(node_set_intf_ip_address(R1_re, "eth1", "20.1.1.2", 24) < 0){
        printf("Unale to assign IP address to interface eth1 of node %s\n", R1_re->node_name);
        return NULL;
    }
    if(node_set_intf_ip_address(R1_re, "eth2", "30.1.1.1", 24) < 0){
        printf("Unale to assign IP address to interface eth2 of node %s\n", R1_re->node_name);
        return NULL;
    }

    if(node_set_intf_ip_address(R2_re, "eth3", "30.1.1.2", 24) < 0){
        printf("Unale to assign IP address to interface eth3 of node %s\n", R2_re->node_name);
        return NULL;
    }
    if(node_set_intf_ip_address(R2_re, "eth5", "40.1.1.2", 24) < 0){
        printf("Unale to assign IP address to interface eth5 of node %s\n", R2_re->node_name);
        return NULL;
    }

    network_start_pkt_receiver_thread(topo);


    char *message = "This is a test message\n";
    send_pkt_flood(R0_re, R0_re->interfaces[0], message, strlen(message));
    //send_pkt_out(message, strlen(message), R0_re->interfaces[0]);

    return topo;
}
