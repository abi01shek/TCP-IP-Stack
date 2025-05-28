/**
 * @file graph.h
 * @author abishek
 * @brief Graph data structure to represent network topology
 */

#ifndef __MY_GRAPH__H
#define __MY_GRAPH__H

#include "gluethread/glthread.h"
#include <string.h>

#define TOPOLOGY_NAME_SIZE 32
#define NODE_NAME_SIZE 32
#define MAX_INTERFACES_PER_NODE 16
#define IF_NAME_SIZE 32

typedef struct graph_ graph_t;
typedef struct node_ node_t;
typedef struct interface_ interface_t;
typedef struct link_ link_t;

// Graph indicating the network of nodes.
typedef struct graph_ {
    char topology_name[TOPOLOGY_NAME_SIZE];
    glthread_t node_list; ///< linked list of nodes in this graph
} graph_t;

// each node has a number of interfaces
typedef struct node_{
    char node_name[NODE_NAME_SIZE]; ///< name of node
    interface_t* interfaces[MAX_INTERFACES_PER_NODE]; ///< pointer to interfaces in this node
    glthread_t graph_glue;
} node_t;

// An interface is attached to a node and has a link
// each interface is also given a name
typedef struct interface_ {
    char interface_name[IF_NAME_SIZE]; ///< name of interface
    link_t *link; ///< which interface is this connected to
    node_t *attached_node; ///< node to which this attached to
} interface_t;

// Link connects two interfaces
typedef struct link_ {
    interface_t if1; ///< interfaces in this link
    interface_t if2;
    unsigned int cost; ///< cost of this link, not used
} link_t;


// Functions
extern graph_t* create_new_graph(const char *topology_name);
extern node_t* create_graph_node(graph_t *graph, const char *node_name);


// Helper functions
/**
 * @brief Get the neighbour node connected across the link on a given interface.
 *
 * @param  interface* : pointer to interface to get nbr on
 * @return node * : pointer to neighbour node across link
 *         NULL   : no neighbour found
 *
 */
static inline node_t * get_nbr_node(interface_t *interface){
    link_t *if_link = interface->link;
    interface_t *nbr_link_if;
    if(if_link == NULL){
        return NULL;
    }

    if(&(if_link->if1) == interface){
        nbr_link_if = &(if_link->if2);
    } else if (&(if_link->if2) == interface) {
        nbr_link_if = &(if_link->if1);
    } else {
        return NULL;
    }

    return nbr_link_if->attached_node;
}

/**
 * @brief Get the index of a free interface in input node
 *
 * @param  *node  : pointer to node
 * @return int >=0: index of free interface in node
 *              <0: no free interfaces
 *
 */
static inline int get_free_if_idx_from_node(node_t *node){
    for(int i=0; i<MAX_INTERFACES_PER_NODE; i++){
        if(node->interfaces[i] == NULL){
            return i;
        }
    }
    return -1; // failed to find empty interface in current node
}


#endif
