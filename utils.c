#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/**
 * @brief Apply the mask on interface IP to get the network IP address
 *
 * Ex: 192.168.111.12/24 has network address 192.168.111.0
 *
 * @param  prefix: interface IP input
 * @param  mask: subnet mask
 * @param  str_prefix: network IP stored here
 * @return 0 success
 *         <0 failure
 *
 */
int apply_mask(char *prefix, int mask, char *str_prefix){
    uint32_t bit_mask;
    uint32_t byte_mask = (1U << 8) - 1; //< 00 00 ff ff
    uint32_t bit_mask_f[4];   //< bit mask fields
    uint32_t prefix_f[4];     //< 4 fields of input IP addr
    uint32_t str_prefix_f[4]; //< 4 fields of masked output IP addr

    // if mask is 24, bit_mask is ff ff ff 00
    // using unsigned long to account for mask 32
    bit_mask = (((1UL << mask) - 1) << (32-mask));

    // select individual bit mask fields.
    for(int i=0; i<4; i++){
        // if bit_mask is ff ff ff 00
        // bit_mask_f[0] = ff, bit_mask_f[1] = ff,
        // bit_mask_f[2] = ff, bit_mask_f[3] = 00
        bit_mask_f[i] =  (bit_mask & (byte_mask << ((3-i)*8))) >> ((3-i)*8);
    }

    // extract each token from prefix
    // if prefix is 192.168.200.2
    // prefix_f[0] = 192, prefix_f[1] = 168,
    // prefix_f[2] = 200, prefix_f[3] = 2
    char *token = strtok(prefix, ".");
    for(int i=0; token != NULL; i++){
        prefix_f[i] = atoi(token);
        token = strtok(NULL, ".");
    }

    // Apply the bit mask fields to the IP fields
    for(int i=0; i<4; i++){
        str_prefix_f[i] = prefix_f[i] & bit_mask_f[i];
    }

    // generate the output string
    snprintf(str_prefix, 16, "%d.%d.%d.%d",
             str_prefix_f[0], str_prefix_f[1],
             str_prefix_f[2], str_prefix_f[3]);
    str_prefix[16] = '\0';
    return 0;
}

/**
 * @brief Generate broadcast MAC adddress and write to array.
 *
 * @param  mac_array: array of 6 characters to write bcast MAC addr to
 * @return void
 */
void layer2_fill_with_broadcast_mac(char *mac_array){
    // MAC address is 48 bits or 6 bytes
    // In L2 Broadcast address each byte is 255 (FF)
    mac_array[0] = 0xFF;
    mac_array[1] = 0xFF;
    mac_array[2] = 0xFF;
    mac_array[3] = 0xFF;
    mac_array[4] = 0xFF;
    mac_array[5] = 0xFF;
}

