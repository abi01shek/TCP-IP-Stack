#include "graph.h"
#include "gluethread/glthread.h"
#include <stdlib.h>
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

void insert_link_between_two_nodes(node_t *node1,
                                   node_t *node2,
                                   char *from_if_name,
                                   char *to_if_name,
                                   unsigned int cost){
    
}
