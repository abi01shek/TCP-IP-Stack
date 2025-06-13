#include "graph.h"
#include "CommandParser/cmdtlv.h"
#include "CommandParser/libcli.h"
#include "CommandParser/clistd.h"
#include "nmcli.h" ///< Parameter codes for diff CLI commands.

extern graph_t *topo;

/**
 * Callback functions
 *
 */

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

/**
 * @brief Initialie the Command line interface.
 *
 * Register different commands with the CLI library
 *
 * @param  void
 * @return void
 */
void nw_init_cli(){
    init_libcli();
    param_t *show   = libcli_get_show_hook();
    param_t *debug  = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *clear  = libcli_get_clear_hook();
    param_t *run    = libcli_get_run_hook();

    /**
     * All the CLI commands go here
     *
     */
    //CMD1: show topology
    {
        static param_t topology;
        init_param(&topology, CMD, "topology", show_topo_callback, 0, INVALID, 0,
                   "Show the topology of the net network");
        set_param_cmd_code(&topology, CMDCODE_SHOW_TOPOLOGY);
        libcli_register_param(show, &topology);
    }

    /**
     * Do not add any param in command config tree after here
     *
     */
    support_cmd_negation(config);
}
