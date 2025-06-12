/**
 * @file utils.c
 * @author abishek
 * @brief utilities to manipulate IP and MAC addresses.
 */


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
    char copy_prefix[16];
    strncpy(copy_prefix, prefix, 16);

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
    char *token = strtok(copy_prefix, ".");
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
    uint8_t ff = 0xFF;
    mac_array[0] = ff;
    mac_array[1] = ff;
    mac_array[2] = ff;
    mac_array[3] = ff;
    mac_array[4] = ff;
    mac_array[5] = ff;
}

/**
 * @brief Convert IP Addr to an unsigned integer.
 *
 * A.B.C.D 32 bit value (MSByte A, B, C, LSByte D)
 *
 * @param  ip_addr: Ip address string
 * @return ip_addr_uint: unsigned int of IP addr conversion
 */
unsigned int convert_ip_from_str_to_int(char *ip_addr){
    uint32_t ip_num = 0;
    uint32_t ip_addr_f[4];
    char copy_ip_addr[16];
    strncpy(copy_ip_addr, ip_addr, 16); // strtok modifies the original array

    // if IP address is A.B.C.D
    // Ip_addr_f[0] = A, [1] = B, [2] = C, [3] = D
    // A, B, C, D are bytes guaranteed to be lower than 255
    char *token = strtok(copy_ip_addr, ".");
    for(int i=0; token != NULL; i++){
        ip_addr_f[i] = atoi(token);
        token = strtok(NULL, ".");
    }

    ip_num += ip_addr_f[0] << 24;
    ip_num += ip_addr_f[1] << 16;
    ip_num += ip_addr_f[2] << 8;
    ip_num += ip_addr_f[3] << 0;
    return ip_num;
}

/**
 * @brief Convert an unsigned 32 bit int to IP address string.
 *
 *
 * @param  ip_addr: ip addrss in number format
 * @param  output_buffer: ip address in string format
 * @return none
 */
void
convert_ip_from_int_to_str(unsigned int ip_addr, char *output_buffer){
    // The best way is to use inet_ntop, only learnt it later
    // for now this will do.
    uint8_t ip_addr_f[4];
    uint32_t byte_mask = ((1U << 8) - 1); // run of 8 1s
    ip_addr_f[0] = (ip_addr & (byte_mask << 24)) >> 24;
    ip_addr_f[1] = (ip_addr & (byte_mask << 16)) >> 16;
    ip_addr_f[2] = (ip_addr & (byte_mask << 8)) >> 8;
    ip_addr_f[3] = (ip_addr & (byte_mask << 0)) >> 0;

    snprintf(output_buffer, 16, "%u.%u.%u.%u",
             ip_addr_f[0], ip_addr_f[1], ip_addr_f[2], ip_addr_f[3]);
    output_buffer[16] = '\0';
}
