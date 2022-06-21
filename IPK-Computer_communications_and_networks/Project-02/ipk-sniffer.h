/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file ipk-sniffer.h
 * @author Dominik Vagner, xvagne10
 * @brief CLI packet sniffer header file
 * @date 2022-04-19 
 */

#ifndef IPK_SNIFFER_H
#define IPK_SNIFFER_H

#define BYTES_PER_ROW	16

#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <iostream>
#include <iomanip>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctime>
#include <locale>

/**
 * Custom structure that keeps information about all the supported flags and their values.
 */
struct ARG_VALUES {
    bool interface_flag = false;
    std::string interface_name = "";
    bool port_flag = false;
    unsigned short port_num = 0;
    bool tcp = false;
    bool udp = false;
    bool arp = false;
    bool icmp = false;
    unsigned int num_of_packets = 1;
};

/**
 * @brief Function for proper exit on interrupt signal.
 * 
 * @param s Interrupt code.
 */
void signal_catcher(__sig_atomic_t s);

/**
 * @brief Parsing of program aguments with getopt library.
 * 
 * @param argc Count of program arguments.
 * @param argv Array of program arguments.
 * @return ARG_VALUES Filled structure with argument flags and values.
 */
ARG_VALUES parse_arguments(int argc, char *argv[]);

/**
 * @brief Print all interfaces available for capture.
 * 
 * @param errbuf Char array for saving error message form pcap library.
 */
void print_alldevs(char *errbuf);

/**
 * @brief Fill the string according program arguments which will be turned into packet filter. 
 * 
 * @param args Struct with all the info about program arguments.
 * @return std::string Final filled string.
 */
std::string fill_filter(ARG_VALUES args);

/**
 * @brief Funtcion that is called on every captured packet, that checks type of information contained
 *        in the packet and calls print functions for the according type.
 * 
 * @param args Arguments from pcap_loop().
 * @param header Packet header containing a timestamp and the lenght of the packet.
 * @param packet Pointer to the first u_char of the whole packet.
 */
void callback_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/**
 * @brief Print the timestamp of packet, ethernet source and destination MAC addresses and frame length.
 * 
 * @param ethernet Ethernet header.
 * @param header Pcap packet header.
 */
void print_base(ether_header *ethernet, const struct pcap_pkthdr *header);

/**
 * @brief Print ARP specifics and source and destination IP addresses.
 * 
 * @param arp ARP header.
 */
void print_arp(ether_arp *arp);

/**
 * @brief Print IPv4 source and destination addresses.
 * 
 * @param ip IPv4 header.
 */
void print_ip4_addr(iphdr *ip);

/**
 * @brief Print IPv6 source and destination addresses.
 * 
 * @param ip IPv6 header.
 */
void print_ip6_addr(ip6_hdr *ip);

/**
 * @brief Print ICMPv4 specifics.
 * 
 * @param icmp ICMPv4 header.
 */
void print_icmp(icmphdr *icmp);

/**
 * @brief Print ICMPv6 specifics.
 * 
 * @param icmp ICMPv6 header.
 */
void print_icmpv6(icmp6_hdr *icmp);

/**
 * @brief Print source and destination ports and TCP specifics.
 * 
 * @param tcp TCP header.
 */
void print_tcp(tcphdr *tcp);

/**
 * @brief Print source and destination ports and UDP protocol info.
 * 
 * @param udp UDP header.
 */
void print_udp(udphdr *udp);

/**
 * @brief Print all of packet data in hex form, and printable chars.
 * 
 * @param packet All of packet data as unsigned chars.
 * @param caplen Lenght of frame.
 */
void print_payload(const u_char *packet, bpf_u_int32 caplen);

#endif //IPK_SNIFFER_H