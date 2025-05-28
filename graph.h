/**
 * @file graph.h
 * @author Abishek Ramdas
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

// map function to extract node information from gl linked list node
GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue)

// Functions
extern graph_t* create_new_graph(const char *topology_name);
extern node_t* create_graph_node(graph_t *graph, const char *node_name);
extern link_t* insert_link_between_two_nodes(node_t *node1,
                                         node_t *node2,
                                         char *from_if_name,
                                         char *to_if_name,
                                         unsigned int cost);

// Print functions
extern void dump_graph(graph_t *graph);
extern void dump_node(node_t *node);
extern void dump_interface(interface_t *if1);


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

/**
 * @brief Search for interface matching interface name in a node
 *
 * @param  node: pointer to node to search in
 * @param  if_name: interface name to search for
 * @return interface: pointer to interface struct matching if_name in node upon success
 *         NULL: upon failure
 */
static inline interface_t *
get_node_if_by_name(node_t *node, char *if_name){
    interface_t *if1 = NULL;
    for(int i=0; i<MAX_INTERFACES_PER_NODE; i++){
        if(node->interfaces[i] == NULL){
            // No more interfaces found,
            break;
        }
        if(strncmp(node->interfaces[i]->interface_name, if_name, IF_NAME_SIZE) == 0){
            if1 = node->interfaces[i];
            break;
        }
        // else if i does not mach if_name so continue
    }
    return if1;
}


/**
 * @brief Given node name get the node from a graph.
 *
 * @param  topo: pointer to graph
 * @param  node_name: pointer to name of node string
 * @return node: pointer to node when able to find
 *         NULL: failure
 */
static inline node_t *
get_node_by_node_name(graph_t *topo, char *node_name){
    node_t *node = NULL;
    glthread_t *curr;
    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
        node = graph_glue_to_node(curr);
        if(strncmp(node->node_name, node_name, NODE_NAME_SIZE) == 0){
            break;
        }
    } ITERATE_GLTHREAD_END(&topo->node_list, curr);
    return node;
}

#endif
