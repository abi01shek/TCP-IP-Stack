#include "layer2.h"
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
