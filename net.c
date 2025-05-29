#include "net.h"
#include "graph.h"
#include "string.h"
#include <stdio.h>

/**
 * @brief Set the loopback address of a node.
 * mask of loopback IP is always 32.
 *
 * @param  node: pointer to node whose loopback IP is to be set
 * @param  ip_addr: pointer to loopback IP address string
 * @return 0 upon success
 *         <0 upon failure
 */
int node_set_loopback_address(node_t *node, const char *ip_addr){
    if(ip_addr == NULL){
        return -1;
    }
    strncpy(LOOPBACK_IP(node).ip_addr, ip_addr, sizeof(LOOPBACK_IP(node).ip_addr));
    LOOPBACK_IP(node).ip_addr[sizeof(LOOPBACK_IP(node).ip_addr)-1] = '\0';
    LOOPBACK_IP(node).mask = 32; // loopback mask is always 32
    return 0; // success
}

/**
 * @brief Assign a random MAC address for an interface
 *
 * @param  intf: pointer to the interface
 * @return 0 : success
 *        <0 : failure
 *
 * @details
 * MAC addresses are burnt inside the NIC so we assign a random value
 * to emulate it.
 */
int intf_assign_mac_addr(interface_t *intf){
    unsigned int hash_val = 0;
    node_t* node = intf->attached_node;
    hash_val = hash_code((void*)node->node_name, sizeof(node->node_name));
    hash_val *= hash_code((void*)intf->interface_name, sizeof(intf->interface_name));
    memset(IF_MAC(intf).mac, 0, sizeof(IF_MAC(intf).mac));
    snprintf(IF_MAC(intf).mac, sizeof(IF_MAC(intf).mac), "%u", hash_val);
    return 0;
}

/**
 * @brief Set the interface IP address on a node.
 *
 * @param  node: pointer to node containing the interface
 * @param  local_if: pointer to interface name string
 * @param  ip_addr: pointer to ip address string to assign to interface
 * @param  mask: mask of ip_addr
 * @return 0: success
 *         <0: failure
 */
int node_set_intf_ip_address(node_t *node, char *local_if, const char *ip_addr, const int mask){
    interface_t *intf;
    intf = get_node_if_by_name(node, local_if);
    if(intf == NULL){
        printf("Unable to find interface %s on node %s\n", local_if, node->node_name);
        return -1;
    }
    strncpy(IF_IP(intf).ip_addr, ip_addr, sizeof(IF_IP(intf).ip_addr));
    IF_IP(intf).ip_addr[sizeof(IF_IP(intf).ip_addr)-1] = '\0';
    IF_IP(intf).mask = mask;
    IF_IP_CONFIG(intf) = 1;
    return 0;
}

/**
 * @brief Clear the interface IP address on a node.
 *
 * @param  node: pointer to node containing the interface
 * @param  local_if: pointer to interface name string
 * @return 0: success
 *         <0: failure
 */
int node_unset_intf_ip_address(node_t *node, char *local_if){
    interface_t *intf;
    intf = get_node_if_by_name(node, local_if);
    if(intf == NULL){
        printf("Unable to find interface %s on node %s\n", local_if, node->node_name);
        return -1;
    }
    memset(&IF_IP(intf), 0, sizeof(IF_IP(intf))); // set both ip addr and mask to 0s
    IF_IP_CONFIG(intf) = 0;
    return 0;
}
