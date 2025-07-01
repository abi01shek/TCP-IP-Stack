#ifndef __MY_NET__H
#define __MY_NET__H

#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// Forward declaration will be defined later
typedef struct node_ node_t;
typedef struct interface_ interface_t;

typedef struct ip_addr_ {
    char ip_addr[16];
    int mask;
} ip_addr_t;

typedef struct mac_addr_{
    uint8_t mac[7]; // 48 bits (6 bytes) plus null character
}mac_addr_t;

/** @struct node_nw_props_t
 *  @brief Network properties of a node
 */
typedef struct node_nw_props_ {
    ip_addr_t loopback_ip; //< loopback IP address, mask is 32 always
    int loopback_ip_flag; //< Inidcates whether loopback IP is configured or not
} node_nw_props_t;

/** @struct intf_nw_props_
 *  @brief Interface network properties
 */
typedef struct intf_nw_props_{
    // L2 network properties
    mac_addr_t mac_address; //< MAC address burnt into the NIC
    // L3 network properties
    ip_addr_t ip_address;
    int ip_address_flag; //< indicates whether IP addr is configured
} intf_nw_props_t;

// Short hand macros
#define LOOPBACK_IP(node_p) ((node_p)->node_nw_props.loopback_ip)
#define IF_IP(intfp) ((intfp)->intf_nw_props.ip_address)
#define IF_MAC(intfp) ((intfp->intf_nw_props.mac_address))
#define IF_IP_CONFIG(intfp) ((intfp)->intf_nw_props.ip_address_flag)
#define IS_INTF_L3_MODE(intfp) ((IF_IP_CONFIG(intfp) == 1) ? 1 : 0)
/**
 * @brief Initialize network properties of a node.
 * @return node_nw_props: pointer to an node_nw_props_t with default values
 */
__attribute__((used)) static void init_node_nw_prop(node_nw_props_t* node_nw_props){
    memset(&node_nw_props->loopback_ip, 0, sizeof(node_nw_props->loopback_ip));
    node_nw_props->loopback_ip_flag = 0; // not configured yet
}


/**
 * @brief Initialize network properties of an interface.
 * @return intf_nw_props: pointer to an intf_nw_props_t with default values
 */
__attribute__((used)) static void init_intf_nw_prop(intf_nw_props_t *intf_nw_props) {
    memset(&intf_nw_props->mac_address, 0, sizeof(intf_nw_props->mac_address));
    memset(&intf_nw_props->ip_address, 0, sizeof(intf_nw_props->ip_address));
    intf_nw_props->ip_address_flag = 0;
}

/**
 * @brief Generate a random hash value from value in a pointer
 *
 * @param  ptr : pointer to data that serves as a seed for the hash code
 * @param  size: size of data in the pointer
 * @return hash_val: unique hash value for that pointer
 *
 */
 __attribute__((used)) static unsigned int hash_code(void *ptr, unsigned int size)
{
    unsigned int hash_val = 0;
    char *str = (char *)ptr;
    for(unsigned int i = 0; i < size; i++){
        hash_val += *str;
        hash_val *= 97;
        str++;
    }
    return hash_val;
}

extern int node_set_loopback_address(node_t *node, const char *ip_addr);
extern int intf_assign_mac_addr(interface_t *intf);
extern int node_set_intf_ip_address(node_t *node, char *local_if, const char *ip_addr, const int mask);
extern int node_unset_intf_ip_address(node_t *node, char *local_if);
extern  interface_t * node_get_matching_subnet_interface(node_t *node, char *ip_addr);

/**
 * @brief Convert IPv4 from text to binary form.
 *
 * @param  IP address in text form
 * @param  IP address in number form or 0 in case of failure
 */
inline uint32_t ip_addr_p_to_n(char *ip_addr){
    uint32_t ip_num;
    if(inet_pton(AF_INET, ip_addr, &ip_num) < 0){
        perror("inet_pton");
        return 0;
    }
    return ip_num;
}

/**
 * @brief Convert IPv4 from binary to text form.
 *
 * @param  src IP address in number form
 * @param  dest pointer to IP address in text form
 */
inline void ip_addr_n_to_p(uint32_t ip_addr, char *ip_add_str){
    if(ip_add_str == NULL){
        printf("IP address string destination cannot be NULL\n");
        return;
    }
    if(inet_ntop(AF_INET, &ip_addr, ip_add_str, 16) == NULL){
        perror("inet_ntop");
        return;
    }
    return;
}
#endif
