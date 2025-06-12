#include "CommandParser/clistd.h"
#include "graph.h"
#include <stdio.h>
#include <string.h>
#include "CommandParser/cmdtlv.h"
#include "CommandParser/libcli.h"

extern graph_t * build_first_topo();
graph_t *topo = NULL;

#define CMDCODE_SHOW_TOPOLOGY 1 ///< Show the topology of the network
static int
show_topo_callback(param_t *param,
                   ser_buff_t *tlv_buf,
                   op_mode enable_or_disable){
    int CMDCODE = -1;
    CMDCODE = EXTRACT_CMD_CODE(tlv_buf);
    switch(CMDCODE){
        case CMDCODE_SHOW_TOPOLOGY:
            dump_graph(topo);
            break;
        default:
            ;
    }
    return 0;
}

int main(){

    topo = build_first_topo();
    /* node_t *node = get_node_by_node_name(topo, "R0_re"); */
    /* if(node != NULL){ */
    /*     dump_node(node); */
    /* } */

    init_libcli();
    param_t *show   = libcli_get_show_hook();
    param_t *debug  = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *clear  = libcli_get_clear_hook();
    param_t *run    = libcli_get_run_hook();

    //CMD1: show topology
    {
        static param_t topology;
        init_param(&topology, CMD, "topology", show_topo_callback, 0, INVALID, 0,
                   "Show the topology of the net network");
        set_param_cmd_code(&topology, CMDCODE_SHOW_TOPOLOGY);
        libcli_register_param(show, &topology);
    }

    support_cmd_negation(config);
    /*Do not add any param in config command tree after above line*/
    start_shell();
    return 0;
}
