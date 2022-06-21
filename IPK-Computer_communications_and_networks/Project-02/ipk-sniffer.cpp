/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file ipk-sniffer.cpp
 * @author Dominik Vagner, xvagne10
 * @brief CLI packet sniffer source file
 * @date 2022-04-19
 */

/**
 * List of referenced materials:
 * 
 * [1] https://www.tcpdump.org/pcap.html 
 * ^^ (Most of my inspiration was taken from this offical tutorial.)
 * ^^ License: https://github.com/the-tcpdump-group/tcpdump/blob/master/LICENSE
 * ^^ Author: Tim Carstens
 * 
 * [2] https://www.tcpdump.org/manpages/pcap_findalldevs.3pcap.html 
 * ^^ (and other manpages from https://www.tcpdump.org/manpages/)
 * ^^ License: https://github.com/the-tcpdump-group/tcpdump/blob/master/LICENSE
 * 
 * [3] https://leimao.github.io/blog/Argument-Parser-Getopt-C/#GNU%E2%80%99s-Getopt-Long
 * ^^ (Parsing of program arguments.)
 * ^^ License: https://creativecommons.org/licenses/by/4.0/
 * ^^ Author: Lei Mao
 * 
 * [4] https://www.mutekh.org/doc/Network_library_module_reference.html
 * ^^ (For list of structures and predefined constants/macros.)
 * ^^ License: https://www.mutekh.org/doc/GNU_Free_Documentation_License.html
 * 
 * [5] https://en.cppreference.com/w/cpp/language/reinterpret_cast
 * ^^ License: https://en.cppreference.com/w/Cppreference:Copyright/CC-BY-SA
 *             https://en.cppreference.com/w/Cppreference:Copyright/GDFL
 * 
 * [6] https://stackoverflow.com/questions/53589535/c-format-cout-with-right-and-setw-for-a-string-and-float
 * [7] https://stackoverflow.com/questions/15255692/grabbing-n-bits-from-a-byte
 * ^^ License: https://creativecommons.org/licenses/by-sa/4.0/
 */

#include "ipk-sniffer.h"

using namespace std;

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_catcher);
    ARG_VALUES args;
    char *interface, errbuf[PCAP_ERRBUF_SIZE];

    // Parse arguments and store results into ARG_VALUES struct
    args = parse_arguments(argc, argv);

    // If no intefrace specified print all available devices for capture.
    if (!args.interface_flag or (args.interface_flag && args.interface_name == "")) {
        print_alldevs(errbuf);
        return 0;
    }
    interface = (char *) args.interface_name.c_str();

    //// Referenced material num.: [1]
    // Get mask and net for interface/device
    bpf_u_int32 net, mask;
    if (pcap_lookupnet(interface, &net, &mask, errbuf) == PCAP_ERROR) {
        cout << errbuf << endl;
        return 1;
    }
    
    // Opening the inteface/device for sniffing and verifying that it's ethernet.
    pcap_t *handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        cout << errbuf << endl;
        return 1;
    }
    if (pcap_datalink(handle) != DLT_EN10MB) {
        cout << errbuf << endl;
        return 1;
    }
    //// End of reference

    // Create filter from arguments, compile it and set it.
    string filter_exp = fill_filter(args);
    const char *const_f_exp = filter_exp.c_str();

    //// Referenced material num.: [1]
    bpf_program compiled_filter;
    if (pcap_compile(handle, &compiled_filter, const_f_exp, 0, net) == PCAP_ERROR) {
        cout << pcap_geterr(handle) << endl;
        exit(1);
    }
    if (pcap_setfilter(handle, &compiled_filter) == PCAP_ERROR) {
        cout << pcap_geterr(handle) << endl;
        exit(1);
    }

    // Loop until amount of caught packages equals the specifed amount.
    // On catch call the function named callback_handler that processes the packet.
    pcap_loop(handle, args.num_of_packets, callback_handler, nullptr);
    //// End of reference
}

void signal_catcher(__sig_atomic_t s) {
    cout << "Program stopped by interrupt number: " << s << "." << endl;
    exit(s);
}

ARG_VALUES parse_arguments(int argc, char *argv[]) {
    ARG_VALUES args;
    int interface_flag;
    string opcode = "";
    int opt, option_index;

    if (!strcmp(argv[argc-1], "-i")) {
        args.interface_flag = true;
        return args;
    }

    //// Referenced material num.: [3]
    static struct option long_options[] = {
            {"interface", no_argument, &interface_flag, 1},
            {"tcp", no_argument, 0, 't'},
            {"udp", no_argument, 0, 'u'},
            {"arp", no_argument, 0, 0},
            {"icmp", no_argument, 0, 0},
            {0, 0, 0, 0}
    };
        
    while ((opt = getopt_long(argc, argv, "i:p:tuhn:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 0: {
                opcode = long_options[option_index].name;
                if (opcode == "arp") {
                    args.arp = true;
                } else if (opcode == "icmp") {
                    args.icmp = true;
                }
                break;
            }
            case 'i': {
                args.interface_flag = true;
                if (optarg) {
                    args.interface_name = (string) optarg;
                }
                break;
            }
            case 'p': {
                args.port_flag = true;
                if (optarg) {
                    opcode = optarg;
                    try {
                        args.port_num = (short) stoi(opcode.c_str(), nullptr, 0);
                    }
                    catch(const exception& e) {
                        cerr << "Invalid value for port (-p) argument." << '\n';
                        exit(1);
                    }
                }
                break;
            }
            case 't': {
                args.tcp = true;
                break;
            }
            case 'u': {
                args.udp = true;
                break;
            }
            case 'n': {
                if (optarg) {
                    opcode = optarg;
                    try {
                        args.num_of_packets = stoi(opcode.c_str(), nullptr, 0);
                    }
                    catch(const exception& e) {
                        cerr << "Invalid value for number (-n) argument." << '\n';
                        exit(1);
                    }
                }
                break;
            }
            case 'h': {
                cout << "ipk-sniffer - Simple CLI packet sniffer." << endl
                     << "./ipk-sniffer [-i intrfc | --interface intrfc] {-p port}"
                     << "{[--tcp|-t] [--udp|-u] [--arp] [--icmp] } {-n num}" << endl
                     << endl << "Options:" << endl
                     << "   '-i|--interface intrfc': intrfc = Interface on which are the packets going to captured on. " << endl
                     << "   '-p num': num = Which port number we want to filter" << endl
                     << "   '-t|--tcp': TCP Packets are going to captured." << endl
                     << "   '-u|--udp': UDP Packets are going to captured." << endl
                     << "   '--icmp': ICMPv4 or ICMPv6 Packets are going to captured." << endl
                     << "   '--arp': ARP Packets are going to captured." << endl
                     << "   '-n num': num = Amount of packets we want to sniff." << endl
                     << endl << "   If none of the protocol flags are specified, then all are set to be captured." << endl;
            }
            case '?': {exit(1);}
            default: {continue;}
        }
    }
    
    if (interface_flag == 1 && optind < argc && args.interface_name == "") {
        args.interface_name = argv[optind];
        args.interface_flag = true;
    }
    //// End of reference.

    if (!(args.arp or args.icmp or args.tcp or args.udp)) {
        args.arp = true;
        args.icmp = true;
        args.tcp = true; 
        args.udp = true;
    }
    return args;
}

void print_alldevs(char *errbuf) {
    pcap_if_t *list_of_devs;
    if (pcap_findalldevs(&list_of_devs, errbuf) != PCAP_ERROR) {
        cout << "List of all interfaces:" << endl;
        pcap_if_t *temp = list_of_devs;
        while (temp != nullptr) {
            cout << temp->name << endl;
            temp = temp->next;
        }
        pcap_freealldevs(list_of_devs);
    } else {
        cerr << errbuf << '\n';
        pcap_freealldevs(list_of_devs);
        exit(1);
    }
}

string fill_filter(ARG_VALUES args) {
    bool first = true;
    string filter_exp = "";

    if (args.arp) {
        first = false;
        filter_exp += "arp ";
    }
    if (args.icmp) {
        if (!first)
            filter_exp += "or ";
        first = false;
        filter_exp += "icmp or icmp6 ";
    }
    if (!(args.tcp || args.udp || args.port_flag)) {
        return filter_exp;
    }
    if (!(args.icmp || args.arp)) {
        filter_exp += "((";
    } else {
        filter_exp += "or ((";        
    }
    first = true;
    if (args.tcp) {
        first = false;
        filter_exp += "tcp ";
    }
    if (args.udp) {
        if (!first)
            filter_exp += "or ";
        first = false;
        filter_exp += "udp ";
    }
    if (!args.tcp && !args.udp && args.port_flag) {
        filter_exp += "tcp or udp";
    }
    filter_exp += ") ";
    if (args.port_flag) {
        filter_exp += "and port ";
        filter_exp += to_string(args.port_num);
    }
    filter_exp += ")";
    return filter_exp;
}

void callback_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    ether_header *ethernet = (ether_header *)(packet);
    print_base(ethernet, header);
    if (ntohs(ethernet->ether_type) == ETHERTYPE_ARP) {
        ether_arp *arp = (ether_arp *)(packet + ETHER_HDR_LEN);
        print_arp(arp);
    } else if (ntohs(ethernet->ether_type) == ETHERTYPE_IP) {
        iphdr *ip = (iphdr *)(packet + ETHER_HDR_LEN);
        uint iphdr_len = ip->ihl * 4;
        print_ip4_addr(ip);
        switch (ip->protocol) {
            case 1: {
                icmphdr *icmp = (icmphdr *)(packet + ETHER_HDR_LEN + iphdr_len);
                print_icmp(icmp);
                break;
            }
            case 6: {
                tcphdr *tcp = (tcphdr *)(packet + ETHER_HDR_LEN + iphdr_len);
                print_tcp(tcp);
                break;
            }
            case 17: {
                udphdr *udp = (udphdr *)(packet + ETHER_HDR_LEN + iphdr_len);
                print_udp(udp);
                break;
            }    
            default: {
                break;
            }
        }
    } else if (ntohs(ethernet->ether_type) == ETHERTYPE_IPV6) {
        ip6_hdr *ip = (ip6_hdr *)(packet + ETHER_HDR_LEN);
        print_ip6_addr(ip);
        uint ip6hdr_len = sizeof(ip6_hdr);
        switch (ip->ip6_ctlun.ip6_un1.ip6_un1_nxt) {
            case 6: {
                tcphdr *tcp = (tcphdr *)(packet + ETHER_HDR_LEN + ip6hdr_len);
                print_tcp(tcp);
                break;
            }
            case 17: {
                udphdr *udp = (udphdr *)(packet + ETHER_HDR_LEN + ip6hdr_len);
                print_udp(udp);
                break;
            }    
            case 58: {
                icmp6_hdr *icmp = (icmp6_hdr *)(packet + ETHER_HDR_LEN + ip6hdr_len);
                print_icmpv6(icmp);
                break;
            }
            default: {
                break;
            }
        }
    }
    print_payload(packet, header->caplen);
}

void print_base(ether_header *ethernet, const struct pcap_pkthdr *header) {
    char str_time[100];
    strftime(str_time, sizeof(str_time), "%FT%T", localtime(&header->ts.tv_sec));
    char tmzone[12];
    strftime(tmzone, sizeof(tmzone), "%z", localtime(&header->ts.tv_sec));
    string correct_tmzone = "";
    correct_tmzone.append(tmzone);
    correct_tmzone.insert(3, ":");
    char final_str_time[128];
    sprintf(final_str_time, "%s.%03d%s",
        str_time, (int) (header->ts.tv_usec / 1000), correct_tmzone.c_str()
    );
    cout << "timestamp: " << final_str_time << endl;
    printf("src MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", 
        ethernet->ether_shost[0], ethernet->ether_shost[1], ethernet->ether_shost[2], 
        ethernet->ether_shost[3], ethernet->ether_shost[4], ethernet->ether_shost[5]
    );
    printf("dst MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
        ethernet->ether_dhost[0], ethernet->ether_dhost[1], ethernet->ether_dhost[2], 
        ethernet->ether_dhost[3], ethernet->ether_dhost[4], ethernet->ether_dhost[5]
    );
    cout << "frame length: " << header->caplen << " bytes" << endl;
}

void print_arp(ether_arp *arp) {
    printf("src IP: %d.%d.%d.%d\n", 
        arp->arp_spa[0], arp->arp_spa[1], arp->arp_spa[2], arp->arp_spa[3]
    );
    printf("dst IP: %d.%d.%d.%d\n", 
        arp->arp_tpa[0], arp->arp_tpa[1], arp->arp_tpa[2], arp->arp_tpa[3]
    );
    cout << "protocol: ARP" << endl;
    cout << endl << "ARP Specifics: " << endl;
    printf("sender MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", 
        arp->arp_sha[0], arp->arp_sha[1], arp->arp_sha[2], 
        arp->arp_sha[3], arp->arp_sha[4], arp->arp_sha[5]
    );
    printf("target MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
        arp->arp_tha[0], arp->arp_tha[1], arp->arp_tha[2], 
        arp->arp_tha[3], arp->arp_tha[4], arp->arp_tha[5]
    );
    if (ntohs(arp->ea_hdr.ar_op) == 1) {
        cout << "operation: 1 (request)" << endl; 
    } else if (ntohs(arp->ea_hdr.ar_op) == 2) {
        cout << "operation: 2 (reply)" << endl; 
    };
}

void print_ip4_addr(iphdr *ip) {
    uint32_t source = ip->saddr;
    uint32_t dest = ip->daddr;
    //// Referenced material num.: [5]
    uint8_t *bytepointer_source = reinterpret_cast<uint8_t *>(&source);
    uint8_t *bytepointer_dest = reinterpret_cast<uint8_t *>(&dest);
    //// End of reference.
    printf("src IP: %d.%d.%d.%d\n", 
        bytepointer_source[0], bytepointer_source[1], bytepointer_source[2], bytepointer_source[3]
    );
    printf("dst IP: %d.%d.%d.%d\n", 
        bytepointer_dest[0], bytepointer_dest[1], bytepointer_dest[2], bytepointer_dest[3]
    );
}

void print_ip6_addr(ip6_hdr *ip) {
    uint16_t *source = ip->ip6_src.__in6_u.__u6_addr16;
    uint16_t *dest = ip->ip6_dst.__in6_u.__u6_addr16;
    printf("src IP: %.4x.%.4x.%.4x.%.4x.%.4x.%.4x.%.4x.%.4x\n", 
        ntohs(source[0]), ntohs(source[1]), ntohs(source[2]), ntohs(source[3]),
        ntohs(source[4]), ntohs(source[5]), ntohs(source[6]), ntohs(source[7])
    );
    printf("dst IP: %.4x.%.4x.%.4x.%.4x.%.4x.%.4x.%.4x.%.4x\n", 
        ntohs(dest[0]), ntohs(dest[1]), ntohs(dest[2]), ntohs(dest[3]),
        ntohs(dest[4]), ntohs(dest[5]), ntohs(dest[6]), ntohs(dest[7])
    );
}

void print_icmp(icmphdr *icmp) {
    cout << "protocol: ICMP" << endl
         << endl << "ICMP Specifics:" << endl
         << "Type: " << (int) icmp->type << endl
         << "Code: " << (int) icmp->code << endl;
}

void print_icmpv6(icmp6_hdr *icmp) {
    cout << "protocol: ICMPv6" << endl
         << endl << "ICMPv6 Specifics:" << endl
         << "Type: " << (int) icmp->icmp6_type << endl
         << "Code: " << (int) icmp->icmp6_code << endl;
}

void print_tcp(tcphdr *tcp) {
    cout << "src port: " << ntohs(tcp->th_sport) << endl
         << "dst port: " << ntohs(tcp->th_dport) << endl
         << "protocol: TCP" << endl
         << endl << "TCP Specifics:" << endl
         << "Sequence number: " << ntohl(tcp->th_seq) << endl
         << "Acknowledgment number: " << ntohl(tcp->th_ack) << endl;
    //// Referenced material num.: [7]
    uint8_t offset = tcp->th_off;
    cout << "NS: " << (1 & (offset)) << endl; 
    uint8_t flags = tcp->th_flags;
    cout << "CWR: " << (1 & (flags >> (7))) << endl
         << "ECE: " << (1 & (flags >> (6))) << endl
         << "URG: " << (1 & (flags >> (5))) << endl 
         << "ACK: " << (1 & (flags >> (4))) << endl 
         << "PSH: " << (1 & (flags >> (3))) << endl 
         << "RST: " << (1 & (flags >> (2))) << endl 
         << "SYN: " << (1 & (flags >> (1))) << endl 
         << "FIN: " << (1 & (flags)) << endl; 
    //// End of reference.
}

void print_udp(udphdr *udp) {
    cout << "src port: " << ntohs(udp->source) << endl
         << "dst port: " << ntohs(udp->dest) << endl
         << "protocol: UDP" << endl;
}

void print_payload(const u_char *packet, bpf_u_int32 caplen) {
    int num_of_rows = (caplen + BYTES_PER_ROW - 1) / BYTES_PER_ROW;
    int position = 0;
    cout << endl;
    for (int i = 0; i < num_of_rows; i++) {
        string bytes = "";
        string chars = "";
        char *tmp = nullptr;
        uint bytes_count = 0;

        printf("0x%.4x:  ", i * BYTES_PER_ROW);
        if (i != num_of_rows - 1) {
            bytes_count = BYTES_PER_ROW;
        } else {
            bytes_count = caplen % BYTES_PER_ROW;
            if (!bytes_count)
                bytes_count = BYTES_PER_ROW;
        }

        for (uint j = 0; j < bytes_count; j++) {
            if (j == 8) { 
                bytes += " ";
                chars += " ";
            }
            asprintf(&tmp, "%.2x ", packet[position]);
            bytes.append(tmp);
            if (packet[position] > 31 && packet[position] < 128) {
                chars += packet[position++];
            } else {
                chars += ".";
                position++;
            }
        }

        //// Referenced material num.: [6]
        cout << setw(50) << left << bytes;
        cout << setw(17) << left << chars << endl;
        //// End of reference.
    }
    cout << endl;
}
