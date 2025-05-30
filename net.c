#include "net.h"
#include "graph.h"
#include "string.h"
#include <stdio.h>
#include "utils.h"

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


/**
 * @brief Given an end-point IP addr return interface in node that lies in the same subnet.
 *
 * @param  node: pointer to node whose interfaces are to be checked
 * @param  ip_addr: IP address whose subnet is to be matched
 * @return interface_t* : pointer to interface in same subnet
 *
 * @details
 * Check if network address of configured interface matches
 * input network address. If matches interface is found.
 */
interface_t *
node_get_matching_subnet_interface(node_t *node, char *ip_addr){
    interface_t *curr_if;
    for(int i=0; i<MAX_INTERFACES_PER_NODE; i++){
        curr_if = node->interfaces[i];
        if(IF_IP_CONFIG(curr_if) == 1){
            // IP is configured
            char* curr_if_ip = IF_IP(curr_if).ip_addr;
            int curr_mask = IF_IP(curr_if).mask;
            char curr_if_subnet[17];
            char ip_addr_subnet[17];
            apply_mask(curr_if_ip, curr_mask, curr_if_subnet);
            apply_mask(ip_addr, curr_mask, ip_addr_subnet);
            if(strncmp(curr_if_subnet, ip_addr_subnet, 16) == 0){
                return curr_if;
            }
        }
    }
    return NULL;
}
