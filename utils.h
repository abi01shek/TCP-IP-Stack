#ifndef _MY_UTILS__
#define _MY_UTILS__

#define IS_MAC_BROADCAST_ADDR(mac) \
    ((mac[0] == 0xFF) && (mac[1] == 0xFF) && (mac[2] == 0xFF) && \
     (mac[3] == 0xFF) && (mac[4] == 0xFF) && (mac[5] == 0xFF))

extern int apply_mask(char *prefix, int mask, char *str_prefix);
extern void layer2_fill_with_broadcast_mac(char *mac_array);
extern unsigned int convert_ip_from_str_to_int(char *ip_addr);
extern void convert_ip_from_int_to_str(unsigned int ip_addr, char *output_buffer);
extern int is_valid_ipv4(const char *ip);
#endif
