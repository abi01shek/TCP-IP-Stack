#include "graph.h"
#include "gluethread/glthread.h"
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
    memset(nodep->node_name, '\0', sizeof(nodep->node_name));
    strcpy(nodep->node_name, node_name);
    for(int i=0; i<MAX_INTERFACES_PER_NODE; i++){
        nodep->interfaces[i] = NULL;
    }
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
 * @return  0: on success
 *         <0: fail
 *
 */
int insert_link_between_two_nodes(node_t *node1,
                                   node_t *node2,
                                   char *from_if_name,
                                   char *to_if_name,
                                   unsigned int cost){

    link_t *new_link = calloc(1, sizeof(link_t));
    if(new_link == NULL){
        perror("calloc:");
        return -1;
    }

    int node1_free_if = get_free_if_idx_from_node(node1);
    if(node1_free_if < 0){
        printf("Unable to find free interface on node1\n");
        return -1;
    }

    int node2_free_if = get_free_if_idx_from_node(node2);
    if(node2_free_if < 0){
        printf("Unable to find free interface on node2\n");
        return -1;
    }

    new_link->if1.link = new_link;
    new_link->if2.link = new_link;
    new_link->cost = cost;

    interface_t *if1 = &new_link->if1;
    strncpy(if1->interface_name, from_if_name, IF_NAME_SIZE);
    if1->link = new_link;
    if1->attached_node = node1;

    interface_t *if2 = &new_link->if2;
    strncpy(if2->interface_name, to_if_name, IF_NAME_SIZE);
    if2->link = new_link;
    if2->attached_node = node2;

    node1->interfaces[node1_free_if] = if1;
    node2->interfaces[node2_free_if] = if2;

    return 0; // success
}
