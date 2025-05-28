/**
 * @file topologies.c
 * @author Abishek Ramdas
 * @brief Functions to build different topologies.
 */

#include "graph.h"

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


graph_t * build_first_topo() {
    graph_t *topo = create_new_graph("first_topo");
    node_t *R0_re = create_graph_node(topo, "R0_re");
    node_t *R1_re = create_graph_node(topo, "R1_re");
    node_t *R2_re = create_graph_node(topo, "R2_re");

    insert_link_between_two_nodes(R0_re, R1_re, "eth0", "eth1", 5);
    insert_link_between_two_nodes(R1_re, R2_re, "eth2", "eth3", 4);
    insert_link_between_two_nodes(R0_re, R2_re, "eth4", "eth5", 9);
    return topo;
}
