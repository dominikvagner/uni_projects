/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file flow.h
 * @author Dominik Vagner, xvagne10
 * @brief Header file for a CLI pcap to NetFlow exporter
 * @date 2022-10-27 
 */

#ifndef FLOW_H
#define FLOW_H

#define __FAVOR_BSD
#define BUFFER 2048

#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <map>
#include <tuple>
#include <set>
#include <vector>
#include <err.h>
#include <unistd.h>
#include <netdb.h>
#include <iterator>
#include <algorithm>

/**
 * @brief Custom structure that keeps information about all the supported flags and their values.
 * 
 */
struct arg_values {
    bool file_flag = false;
    std::string file_name = "-";
    std::string collector_address = "127.0.0.1";
    uint16_t collector_port = 2055;
    uint32_t active_timer = 60;
    uint32_t interval = 10;
    uint32_t max_cache = 1024;
};

/**
 * @brief NetFlow v5 header structure.
 * 
 */
struct netflow_header {
    uint16_t version;           // NetFlow export format version number.
    uint16_t count;             // Number of flows exported in this packet (1-30).
    uint32_t SysUptime;         // Current time in milliseconds since the export device booted
    uint32_t unix_secs;         // Current count of seconds since 0000 UTC 1970
    uint32_t unix_nsecs;        // Residual nanoseconds since 0000 UTC 1970.
    uint32_t flow_sequence;     // Sequence counter of total flows seen.
    uint8_t engine_type;        // Type of flow-switching engine.
    uint8_t engine_id;          // Slot number of the flow-switching engine.
    uint16_t sampling_interval; // First two bits hold the sampling mode; remaining 14 bits hold value of sampling interval.
};

/**
 * @brief NetFlow v5 flow record structure.
 * 
 */
struct netflow_record {
    uint32_t srcaddr;   // Source IP address.
    uint32_t dstaddr;   // Destination IP address.
    uint32_t nexthop;   // IP address of next hop router.
    uint16_t input;     // SNMP index of input interface.
    uint16_t output;    // SNMP index of output interface.
    uint32_t dPkts;     // Packets in the flow.
    uint32_t dOctets;   // Total number of Layer 3 bytes in the packets of the flow.
    uint32_t First;     // SysUptime at start of flow.
    uint32_t Last;      // SysUptime at the time the last packet of the flow was received.
    uint16_t srcport;   // TCP/UDP source port number or equivalent.
    uint16_t dstport;   // TCP/UDP destination port number or equivalent.
    uint8_t pad1;       // Unused (zero) bytes.
    uint8_t tcp_flags;  // Cumulative OR of TCP flags.
    uint8_t prot;       // IP protocol type.
    uint8_t tos;        // IP type of service (ToS).
    uint16_t src_as;    // Autonomous system number of the source, either origin or peer.
    uint16_t dst_as;    // Autonomous system number of the destination, either origin or peer.
    uint8_t src_mask;   // Source address prefix mask bits.
    uint8_t dst_mask;   // Destination address prefix mask bits.
    uint16_t pad2;      // Unused (zero) bytes.
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
 * @return arg_values Filled structure with argument flags and values.
 */
arg_values parse_arguments(int argc, char *argv[]);

/**
 * @brief Funtcion that is called on every read packet, that handles the information contained in it.
 * 
 * @param args Arguments from pcap_loop().
 * @param header Packet header containing a timestamp and the lenght of the packet.
 * @param packet Pointer to the first u_char of the whole packet.
 */
void callback_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/**
 * @brief Updates an existing flow in the flow cache.
 * 
 * @param key 5-tuple key for flow cache, consinst of ScrIP, DstIP, SrcPort, DstPort, IPProtocol.
 * @param flags TCP Flags.
 * @param len Number of Layer 3 bytes in the captured packet.
 */
void update_flow(std::tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t> key, uint8_t flags, uint32_t len, timeval ts);

/**
 * @brief Inserts a new filled netflow_record (Flow) into flow cache.
 * 
 * @param key 5-tuple key for flow cache, consinst of ScrIP, DstIP, SrcPort, DstPort, IPProtocol.
 * @param tos Type of Service.
 * @param flags TCP Flags.
 * @param len Number of Layer 3 bytes in the captured packet.
 */
void insert_flow(std::tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t> key, uint8_t tos, uint8_t flags, uint32_t len, timeval ts);

/**
 * @brief Checks for expired flows, makes checks for active and inactive timers and TCP flags.
 * 
 * @param ts Time of last processed PCAP packet.
 */
void expire_checks(timeval ts);

/**
 * @brief Setup/Initialize UDP connection to specified collector address. 
 * 
 */
void init_udp();

/**
 * @brief Send NetFlow packet to collector. Filled with flows from export vector.
 * 
 */
void send_udp();

/**
 * @brief Calculate the difference between two timevals in milliseconds.
 * 
 * @param t1 Time of last processed PCAP packet.
 * @param t2 Time of first processed PCAP packet.
 * @return uint32_t Difference in milliseconds
 */
uint32_t timeval_sub_ms (const timeval *t1, const timeval *t2);

#endif //FLOW_H