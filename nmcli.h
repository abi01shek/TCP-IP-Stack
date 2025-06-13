#ifndef __MY_CMDCODES__H
#define __MY_CMDCODES__H

/**
 * The parameter code for different CLI commands
 *
 */
#define CMDCODE_SHOW_TOPOLOGY 1 ///< Show the topology of the network
#define CMDCODE_RUN_NODE_RESOLVE_ARP 2 ///< ARP resolution (IP to MAC address) on a node

extern void nw_init_cli();

#endif
