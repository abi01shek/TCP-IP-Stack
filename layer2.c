#include "layer2.h"
#include "gluethread/glthread.h"
#include "graph.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char *alloc_eth_frame(char *dl_pkt, size_t dk_pkt_size){

    size_t eth_frame_size = sizeof(ethernet_hdr_t) + dk_pkt_size + sizeof(fcs_t);
    if(eth_frame_size >  ETH_FRAME_MTU){
        printf("Data link packet cannot be more then MTU size\n");
        return NULL;
    }

    char *ethfp = (char *)calloc(1, eth_frame_size);
    if(ethfp == NULL){
        perror("Calloc failed\n");
        return NULL;
    }

    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *) ethfp;
    for(int i=0; i<6; i++){
        eth_hdr->dst_mac[i] = 0;
        eth_hdr->src_mac[i] = 0;
    }
    eth_hdr->ethertype = dk_pkt_size;

    char *payload = ethfp + sizeof(ethernet_hdr_t);
    memcpy(payload, dl_pkt, dk_pkt_size);

    fcs_t *fcs = (fcs_t*)ETH_FCS(ethfp, dk_pkt_size);
    fcs = 0;

    return ethfp;
}


// ARP Table CRUD

/**
 * @brief Create an empty ARP table
 *
 * @param  None
 * @return pointer to heap allocated ARP table
 */
arp_tbl_t* create_arp_tbl(){
    arp_tbl_t* arp_tbl = (arp_tbl_t*) calloc(1, sizeof(arp_tbl_t));
    if(arp_tbl == NULL){
        perror("calloc");
        return NULL;
    }
    init_glthread(&arp_tbl->arp_tbl_list);
    return arp_tbl;
}

/**
 * @brief Lookup an IP address into the ARP table to get the ARP entry
 *
 * Iterate through the ARP table list and compare IP number (key)
 * return pointer to the ARP entry if a match is found. 
 *
 * @param  apr_tbl: pointer to the ARP table
 * @param  ip_num: IP address as uint32_t
 * @return pointer to the ARP table entry in the arp table if found
 *         NULL if not found
 */
arp_tbl_entry_t* lookup_arp_tbl_entry(arp_tbl_t* arp_tbl, uint32_t ip_num){
    if(arp_tbl != NULL){
        arp_tbl_entry_t *curr_arp_entry;
        glthread_t *curr;
        ITERATE_GLTHREAD_BEGIN(&arp_tbl->arp_tbl_list, curr){
            curr_arp_entry = arp_glue_to_arp_entry(curr);
            if(curr_arp_entry->ip_n == ip_num){
                return curr_arp_entry;
            }
        } ITERATE_GLTHREAD_END(&arp_tbl->arp_tbl_list, curr);
    }
    return NULL;
}


arp_tbl_entry_t* add_arp_tbl_entry(arp_tbl_t* arp_tbl, uint32_t ip_num, mac_addr_t mac, char *if_name){
    if(arp_tbl == NULL) return NULL;
    
}
