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
    interface_t *if1; ///< interfaces in this link
    interface_t *if2;
    unsigned int cost; ///< cost of this link
} link_t;


// Functions
extern graph_t* create_new_graph(const char *topology_name);
extern node_t* create_graph_node(graph_t *graph, const char *node_name);
#endif
