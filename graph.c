/**
 * @file graph.c
 * @author Abishek Ramdas
 * @brief  Function definitions for graph library
 */

#include "graph.h"
#include "gluethread/glthread.h"
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * @brief Create a new graph data structure and initialize name.
 *
 * @param  topology_name  pointer to topology name
 * @return graph          pointer of graph_t init with topology name
 */
graph_t* create_new_graph(const char *topology_name){
    graph_t *new_graph  = calloc(1, sizeof(graph_t));
    if(new_graph == NULL){
        perror("calloc:");
        return NULL;
    }
    memset(new_graph->topology_name, '\0', sizeof(new_graph->topology_name));
    strcpy(new_graph->topology_name, topology_name);
    init_glthread(&new_graph->node_list);
    return new_graph;
}


/**
 * @brief Create a node and add it to the graph
 *
 * intitialize the node name and list of interfaces
 * add it to the graph linked list
 *
 * @param  graph      pointer to graph to add to
 * @param  node_name  pointer to name of node
 * @return pointer to node node_t*
 */
node_t* create_graph_node(graph_t *graph, const char *node_name){
    node_t* nodep = calloc(1, sizeof(node_t));
    if(nodep == NULL){
        perror("calloc:");
        return NULL;
    }
    memset(nodep->node_name, '\0', sizeof(nodep->node_name));
    strcpy(nodep->node_name, node_name);
    for(int i=0; i<MAX_INTERFACES_PER_NODE; i++){
        nodep->interfaces[i] = NULL;
    }
    init_node_nw_prop(&nodep->node_nw_props);
    glthread_add_next(&graph->node_list, &nodep->graph_glue);
    return nodep;
}

/**
 * @brief Create a link and connect it between nodes.
 *
 * @param  node1: pointer to first node
 * @param  node2: pointer to second node
 * @param  from_if_name: interface name on first node
 * @param  to_if_name: interface name on second node
 * @param  cost: cost of the link
 * @return link: pointer to link on success
 *         NULL: fail
 *
 */
link_t* insert_link_between_two_nodes(node_t *node1,
                                   node_t *node2,
                                   char *from_if_name,
                                   char *to_if_name,
                                   unsigned int cost){

    link_t *new_link = calloc(1, sizeof(link_t));
    if(new_link == NULL){
        perror("calloc:");
        return NULL;
    }

    int node1_free_if = get_free_if_idx_from_node(node1);
    if(node1_free_if < 0){
        printf("Unable to find free interface on node1\n");
        return NULL;
    }

    int node2_free_if = get_free_if_idx_from_node(node2);
    if(node2_free_if < 0){
        printf("Unable to find free interface on node2\n");
        return NULL;
    }

    new_link->if1.link = new_link;
    new_link->if2.link = new_link;
    new_link->cost = cost;

    interface_t *if1 = &new_link->if1;
    strncpy(if1->interface_name, from_if_name, IF_NAME_SIZE);
    if1->interface_name[IF_NAME_SIZE-1] = '\0';
    if1->link = new_link;
    if1->attached_node = node1;
    init_intf_nw_prop(&if1->intf_nw_props); // IP address is not configured yet
    intf_assign_mac_addr(if1); // assign random MAC address

    interface_t *if2 = &new_link->if2;
    strncpy(if2->interface_name, to_if_name, IF_NAME_SIZE);
    if2->interface_name[IF_NAME_SIZE-1] = '\0';
    if2->link = new_link;
    if2->attached_node = node2;
    init_intf_nw_prop(&if2->intf_nw_props); // IP address is not configured yet
    intf_assign_mac_addr(if2); // assign random MAC address

    node1->interfaces[node1_free_if] = if1;
    node2->interfaces[node2_free_if] = if2;

    return new_link; // success
}


void dump_graph(graph_t *graph){
    node_t *node;
    glthread_t *curr;
    printf("Topology Name = %s\n\n", graph->topology_name);
    ITERATE_GLTHREAD_BEGIN(&graph->node_list, curr){
        node = graph_glue_to_node(curr);
        dump_node(node);
    } ITERATE_GLTHREAD_END(&graph->node_list, curr);
}

void dump_node(node_t *node){
    printf("Node name: %s\n", node->node_name);
    printf("loopback IP: %s/%d\n", LOOPBACK_IP(node).ip_addr, LOOPBACK_IP(node).mask);
    for(int i=0; i<MAX_INTERFACES_PER_NODE; i++){
        if(node->interfaces[i] != NULL){
            dump_interface(node->interfaces[i]);
        } else break;
    }
    printf("\n");
}

void dump_interface(interface_t *if1){
    printf("Interface name: %s\n", if1->interface_name);
    printf("\tLocal node: %s\n", if1->attached_node->node_name);
    node_t *remote_node = get_nbr_node(if1);
    if(remote_node != NULL){
        printf("\tRemote node: %s\n", remote_node->node_name);
    } else {
        printf("\tRemote node: None\n");
    }
    printf("\tCost of link: %u\n", if1->link->cost);
    printf("\tMAC: %02x:%02x:%02x:%02x:%02x:%02x\n", IF_MAC(if1).mac[0],
           IF_MAC(if1).mac[1],IF_MAC(if1).mac[2],IF_MAC(if1).mac[3],
           IF_MAC(if1).mac[4],IF_MAC(if1).mac[5]);
    if(IF_IP_CONFIG(if1)){
        printf("\tIP address: %s/%d\n", IF_IP(if1).ip_addr, IF_IP(if1).mask);
    } else {
        printf("\tIP address not configured\n");
    }
}
