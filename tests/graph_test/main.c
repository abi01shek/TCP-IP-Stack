#include "../../graph.h"
#include <stdio.h>
#include <string.h>

extern graph_t * build_first_topo();

int main(){

    graph_t *first_topo = build_first_topo();
    dump_graph(first_topo);
    return 0; // success
    node_t *node = get_node_by_node_name(first_topo, "R0_re");
    if(node != NULL){
        dump_node(node);
    }
}
