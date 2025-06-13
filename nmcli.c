#include <stdio.h>
#include "CommandParser/libcliid.h"
#include "graph.h"
#include "CommandParser/cmdtlv.h"
#include "CommandParser/libcli.h"
#include "CommandParser/clistd.h"
#include "nmcli.h" ///< Parameter codes for diff CLI commands.
#include "utils.h"

extern graph_t *topo;

/**
 * Callback functions
 *
 */
// A number of input parameters in callback function is not
// used. Ignore these warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// show topo
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

// run node <node-name> resolve-arp <ip-address>
static int
run_node_arp_resolve_callback(param_t *param,
      ser_buff_t *tlv_buf,
      op_mode enable_or_disable){
    int CMDCODE = -1;
    tlv_struct_t *tlv = NULL;
    char *node_name = NULL;
    char *ip_address = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv){
        if(strncmp(tlv->leaf_id, "node_name", strlen("node_name")) == 0){
            node_name = tlv->value;
        }
        if(strncmp(tlv->leaf_id, "ip_address", strlen("ip_address")) == 0){
            ip_address = tlv->value;
        }
    } TLV_LOOP_END;

    CMDCODE = EXTRACT_CMD_CODE(tlv_buf);
    switch(CMDCODE){
    case CMDCODE_RUN_NODE_RESOLVE_ARP:
            printf("Here, nodename %s\n", node_name);
            printf("Here, IP address %s\n", ip_address);
        break;
    default:
        ;
    }
    return 0;
}

#pragma GCC diagnostic pop
/**
 * Validation functions
 *
 */
static int
validate_node_name_callback(char *node_name){
    if(get_node_by_node_name(topo, node_name) == NULL){
        // Canont find node given node name
        printf("Node name %s not found in topology\n", node_name);
        printf("Use \"show topo\" to list all node names\n");
        return VALIDATION_FAILED;
    }
    return VALIDATION_SUCCESS; // VALIDATION_FAILED
}

static int
validate_ip_callback(char *ip_address){
     if(is_valid_ipv4(ip_address) < 0){
        printf("%s is not a valid IP address\n", ip_address);
        return VALIDATION_FAILED;
    }
    return VALIDATION_SUCCESS;
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
        init_param(&topology, CMD, "topology", show_topo_callback, 0, INVALID, 0, "Show the topology of the net network");
        set_param_cmd_code(&topology, CMDCODE_SHOW_TOPOLOGY);
        libcli_register_param(show, &topology);
    }

    //CMD: run node <node-name> resolve-arp <ip-address>
    {
        // Add node param as suboption of run param
        static param_t node;
        init_param(&node, CMD, "node", 0, 0, INVALID, 0, "Help: node");
        libcli_register_param(run, &node);
        {
            // run node <node-name> (node-name as sub option of node)
            static param_t node_name;
            init_param(&node_name, LEAF, 0, 0, validate_node_name_callback, STRING, "node_name", "Help: node name");
            libcli_register_param(&node, &node_name);

            // run node <node-name> resolve-arp
            {
                static param_t resolve_arp;
                init_param(&resolve_arp, CMD, "resolve-arp", 0, 0, INVALID, 0, "resolve-arp <IP-address>");
                libcli_register_param(&node_name, &resolve_arp);

                // run node <node-name> resolve-arp <ip-address> (and done)
                {
                    static param_t ip_address;
                    // validate_ip_callback
                    init_param(&ip_address, LEAF, 0, run_node_arp_resolve_callback, validate_ip_callback , IPV4, "ip_address", "Help: IP-address");
                    libcli_register_param(&resolve_arp, &ip_address);
                    set_param_cmd_code(&ip_address, CMDCODE_RUN_NODE_RESOLVE_ARP); // Completed constructing the entire command
                }
            }
        }
    }


    /**
     * Do not add any param in command config tree after here
     *
     */
    support_cmd_negation(config);
}
