#ifndef __MY_LAYER2_H
#define __MY_LAYER2_H

#include "gluethread/glthread.h"
#include "net.h"
#include "graph.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define ETH_FRAME_MTU 1500
#define ARP_ETHERTYPE 0x806

/**
 * Linked list of ARP entries denoting an ARP table
 *
 */
typedef struct arp_tbl_ {
    glthread_t arp_tbl_list; ///< linked list of arp entries
} arp_tbl_t;

typedef struct arp_tbl_entry_ {
    uint32_t ip_n; ///< IP address numerical (key)
    mac_addr_t mac; ///< MAC addr corresponding to IP address
    char if_name[IF_NAME_SIZE]; ///< Interface name
    glthread_t arp_glue;
} arp_tbl_entry_t;

// map function to extract arp information from gl linked list node
GLTHREAD_TO_STRUCT(arp_glue_to_arp_entry, arp_tbl_entry_t, arp_glue);


typedef struct arp_pkt_{
    uint16_t arp_hardware_type;    // Hardware type (e.g., 1 for Ethernet)
    uint16_t arp_protocol_type;    // Protocol type (e.g., 0x0800 for IPv4)
    uint8_t  arp_hw_addr_len;      // Length of hardware address (6 for Ethernet)
    uint8_t  arp_proto_addr_len;   // Length of protocol address (4 for IPv4)
    uint16_t arp_operation;        // Operation: 1 for request, 2 for reply

    mac_addr_t arp_sender_hw_addr; // Sender hardware address (MAC)
    uint32_t   arp_sender_proto_addr; // Sender protocol address (IP)

    mac_addr_t arp_target_hw_addr; // Target hardware address (MAC)
    uint32_t   arp_target_proto_addr; // Target protocol address (IP)
} __attribute__((packed)) apr_pkt_t;


typedef struct ethernet_hdr_{
    mac_addr_t dst_mac[6];       // Destination MAC address
    mac_addr_t src_mac[6];       // Source MAC address
    uint16_t ethertype;          // also length of payload
} __attribute__((packed)) ethernet_hdr_t;

typedef uint32_t fcs_t;

/**
 * Macro to get the ethernet header size till payload and excluding it and FCS
 */
#define ETH_HDR_SIZE_WO_PAYLOAD (sizeof(((ethernet_hdr_t *)NULL)->dst_mac) + \
                                 sizeof(((ethernet_hdr_t *)NULL)->src_mac) + \
                                 sizeof(((ethernet_hdr_t *)NULL)->ethertype))
/**
 * Get a pointer to location of FCS within the ethernet header
 *
 */
#define ETH_FCS(eth_hdr_p, payload_size) ((char *)eth_hdr_p + ETH_HDR_SIZE_WO_PAYLOAD + payload_size)


/**
 * Identify if a MAC address is broadcast or not
 *
 */
#define IS_MAC_BROADCAST(mac) ((mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF && mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF) ? 1 : 0)


char *alloc_eth_frame(char *dl_pkt, size_t dk_pkt_size);

inline void layer2_fill_broadcast_mac(uint8_t *mac_array){
    // mac array has 6 bytes, each byte should be filled with 1s
    // 1 byte filled with 1 is 255 (0xFF)
    for(int i=0; i<6; i++){
        mac_array[i] = 0xFF;
    }
    return;
}

static inline int l2_recv_qualify_at_if(interface_t *intfp, ethernet_hdr_t * eth){
    // if interface is not configured, drop the eth frame
    // if dst MAC is not broadcast or
    // if dst MAC does not match MAC of interface, drop
    if(IF_IP_CONFIG(intfp) == 0){
        return 0; // Drop
    }
    if(IS_MAC_BROADCAST(eth->dst_mac) == 0){
        // Not broadcast
        for(int i=0; i<6; i++){
            if((int)IF_MAC(intfp).mac[i] !=  eth->dst_mac[i]){
                return 0;
            }
        }
    }
    return 1; // Continue to next layer
}



/**
 * ARP Table CRUD
 *
 */
arp_tbl_t* create_arp_tbl();
arp_tbl_entry_t* lookup_arp_tbl_entry(arp_tbl_t* arp_tbl, uint32_t ip_num);
arp_tbl_entry_t* add_arp_tbl_entry(arp_tbl_t* arp_tbl, uint32_t ip_num, mac_addr_t mac, char *if_name);
void delete_arp_tbl_entry(arp_tbl_t* arp_tbl, uint32_t ip_num);



#endif
