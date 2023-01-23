/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file flow.cpp
 * @author Dominik Vagner, xvagne10
 * @brief CLI pcap file to NetFlow exporter
 * @date 2022-10-27
 */

/**
 * List of referenced materials:
 * 
 * [1] https://leimao.github.io/blog/Argument-Parser-Getopt-C/#GNU%E2%80%99s-Getopt-Long
 * ^^ (Parsing of program arguments.)
 * ^^ License: https://creativecommons.org/licenses/by/4.0/
 * ^^ Author: Lei Mao
 * 
 * [2] https://www.mutekh.org/doc/Network_library_module_reference.html
 * ^^ (For list of structures and predefined constants/macros.)
 * ^^ License: https://www.mutekh.org/doc/GNU_Free_Documentation_License.html 
 * 
 * [3] https://en.cppreference.com/w/
 * ^^ (For most of information about C++ structs and classes, like map, tuple, etc.)
 * ^^ License: https://en.cppreference.com/w/Cppreference:Copyright/CC-BY-SA
 *             https://en.cppreference.com/w/Cppreference:Copyright/GDFL
 * 
 * [4] ISA Lectures for working with UDP client.
 */

#include "flow.h"

using namespace std;

map<tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t>, netflow_record> flow_cache;
vector<netflow_record> flow_vec;
timeval boot_time;
timeval last_packet;
arg_values args;
int sock;
int control = 0;


struct comp_oldest {
    template<typename T>
    bool operator()(const T &l, const T &r) const {
        if (l.second.First != r.second.First) {
            return l.second.First < r.second.First;
        }
        return l.first < r.first;
    }
};

bool comp_oldest_vec(netflow_record nfr1, netflow_record nfr2) {
    return (nfr1.First < nfr2.First);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_catcher);
    char errbuf[PCAP_ERRBUF_SIZE];

    // Parse arguments and store results into ARG_VALUES struct
    args = parse_arguments(argc, argv);
    
    // Opening the the file with captured packets
    pcap_t *handle = pcap_open_offline(args.file_name.c_str(), errbuf);
    if (handle == nullptr) {
        cout << errbuf << endl;
        return 1;
    }

    // Create, compile and set a filter for TCP/UDP/ICMP 
    const char *const_f_exp = "icmp or tcp or udp";
    bpf_program compiled_filter;
    if (pcap_compile(handle, &compiled_filter, const_f_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
        cout << pcap_geterr(handle) << endl;
        return 1;
    }
    if (pcap_setfilter(handle, &compiled_filter) == PCAP_ERROR) {
        cout << pcap_geterr(handle) << endl;
        return 1;
    }

    // Initialize UDP connection
    init_udp();

    // Loop through all packets in the PCAP file.
    pcap_loop(handle, 0, callback_handler, nullptr);

    // Export any remaining flows after end of reading packets from PCAP file.
    while (!flow_cache.empty() || !flow_vec.empty()){
        auto local = flow_cache;
        set<pair<tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t>, netflow_record>, comp_oldest> set(local.begin(), local.end());
        for (auto itr = set.begin(); itr != set.end(); itr++) {
            netflow_record oldest = itr->second;
            flow_vec.push_back(oldest);
            flow_cache.erase(itr->first);
            if (flow_vec.size() >= 30)
                break;
        }
        send_udp();
    }

    close(sock);
    return 0;
}

void signal_catcher(__sig_atomic_t s) {
    cout << "Program stopped by interrupt number: " << s << "." << endl;
    exit(s);
}

arg_values parse_arguments(int argc, char *argv[]) {
    arg_values args;
    string opcode = "";
    int opt;
        
    while ((opt = getopt(argc, argv, "f:c:a:i:hm:")) != -1) {
        switch (opt) {
            case 'f': {
                args.file_flag = true;
                if (optarg) {
                    args.file_name = (string) optarg;
                }
                break;
            }
            case 'c': {
                if (optarg) {
                    string address = (string) optarg;
                    char sep = ':';
                    size_t pos;
                    if ((pos = address.find(sep)) != string::npos) {
                        try {
                            args.collector_address = address.substr(0, pos);
                            args.collector_port = (uint16_t) stoul(address.substr(pos + 1, address.length()));
                        } catch(const exception& e) {
                            cerr << e.what() << endl;
                            cerr << "Invalid argument for \"-c\" flag!" << endl;
                            exit(1);
                        }
                    } else {
                        args.collector_address = address;
                    }
                    
                }
                break;
            }
            case 'a': {
                if (optarg) {
                    try {
                        args.active_timer = stoi(optarg);
                    } catch(exception& e) {
                        cerr << e.what() << endl;
                        cerr << "Invalid argument for \"-a\" flag!" << endl;
                        exit(1);
                    }
                }
                break;
            }
            case 'i': {
                if (optarg) {
                    try {
                        args.interval = stoi(optarg);
                    } catch(exception& e) {
                        cerr << e.what() << endl;
                        cerr << "Invalid argument for \"-i\" flag!" << endl;
                        exit(1);
                    }
                }
                break;
            }
            case 'm': {
                if (optarg) {
                    try {
                        args.max_cache = stoi(optarg);
                    } catch(exception& e) {
                        cerr << e.what() << endl;
                        cerr << "Invalid argument for \"-m\" flag!" << endl;
                        exit(1);
                    }
                }
                break;
            }
            case 'h': {
                cout << "flow - CLI NetFlow v5 exporter for already captured communication." << endl
                     << "./flow [-f <file>] [-c <netflow_collector>[:<port>]] [-a <active_timer>] [-i <inactive_timer>] [-m <count>]" << endl
                     << endl << "Options:" << endl
                     << "   '-f file': file = name of the analyzed file with captured packets, default of STDIN. " << endl
                     << "   '-c netflow_collector:[port]': netflow_collector = address of a collector to which we want to export, default of 127.0.0.1" << endl
                     << "                                  port = port on which the collector is listening, default 2055." << endl
                     << "   '-a active_timer': active_timer = interval for which want we to keep active flows in our flow cache, default of 60." << endl
                     << "   '-i inactive_timer': inactive_timer = interval between last received packet of each flow, before marking them as expired, defualt of 10." << endl
                     << "   '-m count': count = maximum number of flow records in our flow cache, default of 1024." << endl
                     << "   '-h': This output you are seeing right now." << endl
                     << endl;
                exit(0);
            }
            case '?': {
                cerr << "Unknown option detected!" << endl;
                exit(1);
            }
            default: {continue;}
        }
    }
    
    return args;
}

void callback_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    uint32_t src_ip, dst_ip;
    uint16_t src_port, dst_port;
    uint8_t prot, tos;
    uint8_t flags = 0;

    if (control == 0)
        boot_time = header->ts;

    ether_header *ethernet = (ether_header *)(packet);
    if (ntohs(ethernet->ether_type) == ETHERTYPE_IP) {
        iphdr *ip = (iphdr *)(packet + ETHER_HDR_LEN);
        int iphdr_len = ip->ihl * 4;
        src_ip = ip->saddr;
        dst_ip = ip->daddr;
        prot = ip->protocol;
        tos = ip->tos;
        if (iphdr_len >= 20) {
            switch (prot) {
                case 1: {
                    icmphdr *icmp = (icmphdr *)(packet + ETHER_HDR_LEN + iphdr_len);
                    src_port = 0;
                    dst_port = (icmp->type << 8) + icmp->code;
                    break;
                }
                case 6: {
                    tcphdr *tcp = (tcphdr *)(packet + ETHER_HDR_LEN + iphdr_len);
                    src_port = tcp->source;
                    dst_port = tcp->dest;
                    flags = (((tcp->urg >= 1) << 5) | ((tcp->ack >= 1) << 4) | ((tcp->psh >= 1) << 3) | 
                            ((tcp->rst >= 1) << 2) | ((tcp->syn >= 1) << 1) | ((tcp->fin >= 1)));
                    break;
                }
                case 17: {
                    udphdr *udp = (udphdr *)(packet + ETHER_HDR_LEN + iphdr_len);
                    src_port = udp->source;
                    dst_port = udp->dest;
                    break;
                }    
                default: {
                    break;
                }
            }

            last_packet = header->ts;
            expire_checks(header->ts);
            auto key = make_tuple(src_ip, dst_ip, src_port, dst_port, prot);
            auto search = flow_cache.find(key);
            if (search != flow_cache.end() && !flow_cache.empty()) {
                update_flow(key, flags, header->caplen - ETHER_HDR_LEN, header->ts);
            } else {
                insert_flow(key, tos, flags, header->caplen - ETHER_HDR_LEN, header->ts);
            }
            expire_checks(header->ts);
            while(!flow_vec.empty()) {
                send_udp();
            }
            control++;
        }
    }
}

void update_flow(tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t> key, uint8_t flags, uint32_t len, timeval ts) {
    auto &search = flow_cache.find(key)->second;
    search.Last = timeval_sub_ms(&ts, &boot_time);
    search.dPkts++;
    search.dOctets += len;
    search.tcp_flags = search.tcp_flags | flags;
    return;
}

void insert_flow(tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t> key, uint8_t tos, uint8_t flags, uint32_t len, timeval ts) {
    uint32_t uptime = timeval_sub_ms(&ts, &boot_time);
    struct netflow_record new_flow = {
        .srcaddr = get<0>(key),
        .dstaddr = get<1>(key),
        .nexthop = 0,
        .input = 0, 
        .output = 0,
        .dPkts = 1, 
        .dOctets = len,
        .First = uptime,
        .Last = uptime,
        .srcport = get<2>(key),
        .dstport = get<3>(key),
        .pad1 = 0,
        .tcp_flags = (uint8_t)(0 | flags),
        .prot = get<4>(key),
        .tos = tos,    
        .src_as = 0,
        .dst_as = 0,
        .src_mask = 0,
        .dst_mask = 0,
        .pad2 = 0, 
    };

    if (flow_cache.size() == args.max_cache) {
        auto local = flow_cache;
        set<pair<tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t>, netflow_record>, comp_oldest> set(local.begin(), local.end());
        auto pair = set.begin();
        netflow_record oldest = pair->second;
        flow_vec.push_back(oldest);
        flow_cache.erase(pair->first);
    }
    flow_cache.insert({key, new_flow});
    return;
}

void expire_checks(timeval ts) {
    uint32_t inactive_interval = args.interval;
    uint32_t active_interval = args.active_timer;
    uint32_t uptime = timeval_sub_ms(&ts, &boot_time);
    auto local = flow_cache;
    if (!local.empty()) {
        for (auto i : local) {
            if (((uptime - i.second.Last) > inactive_interval * 1000) || ((i.second.Last - i.second.First) > active_interval * 1000)) {
                struct netflow_record extracted = i.second;
                flow_vec.push_back(extracted);
                flow_cache.erase(i.first);
                continue;
            }
            bool rst = (1 & (i.second.tcp_flags >> (2)));
            bool fin = (1 & (i.second.tcp_flags));
            if (rst || fin) {
                struct netflow_record extracted = i.second;
                flow_vec.push_back(extracted);
                flow_cache.erase(i.first);
            }
        }
    }
    return;
}

void init_udp() {
    struct sockaddr_in server;
    struct hostent *servent;         

    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;                   

    if ((servent = gethostbyname(args.collector_address.c_str())) == NULL)
        errx(1,"Invalid collector address.\n");

    memcpy(&server.sin_addr,servent->h_addr,servent->h_length); 

    server.sin_port = htons(args.collector_port);

    if ((sock = socket(AF_INET , SOCK_DGRAM , 0)) == -1)
        errx(1,"Creating a client socket has failed.\n");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server))  == -1)
        errx(1, "Creating a connected UDP socket has failed.");
    
    return;
}

void send_udp() {
    static int total_flows = 0;
    char buffer[BUFFER];
    int msg_size, i;
    
    if (flow_vec.empty())
        return;
    
    sort(flow_vec.begin(), flow_vec.end(), comp_oldest_vec);

    short count = 30;
    if (flow_vec.size() < 30)
        count = flow_vec.size();

    for (short i = 0; i < count; i++) {
        struct netflow_record flow = flow_vec[i];
        flow.First = htonl(flow.First);
        flow.Last = htonl(flow.Last);
        flow.dPkts = htonl(flow.dPkts);
        flow.dOctets = htonl(flow.dOctets);
        memcpy(buffer + sizeof(struct netflow_header) + (i * sizeof(struct netflow_record)), &flow, sizeof(struct netflow_record));
    }
    flow_vec.erase(flow_vec.begin(), flow_vec.begin() + count);

    uint32_t uptime = timeval_sub_ms(&last_packet, &boot_time);
    struct netflow_header header = {
        .version = htons(5),
        .count = htons(count),
        .SysUptime = htonl(uptime),
        .unix_secs = htonl(last_packet.tv_sec),
        .unix_nsecs = htonl(last_packet.tv_usec * 1000),
        .flow_sequence = htonl(total_flows),
        .engine_type = 0,
        .engine_id = 0,
        .sampling_interval = 0,
    };
    memcpy(buffer, &header, sizeof(struct netflow_header));

    msg_size = sizeof(struct netflow_header) + (count * sizeof(struct netflow_record));
    i = send(sock,buffer,msg_size,0);
    if (i == -1)
        err(1,"send() failed");
    else if (i != msg_size)
        err(1,"send(): buffer written partially");

    total_flows += count;
}

uint32_t timeval_sub_ms (const timeval *t1, const timeval *t2) {
  struct timeval res;
  res.tv_sec = t1->tv_sec - t2->tv_sec;
  res.tv_usec = t1->tv_usec - t2->tv_usec;
  if (res.tv_usec < 0) {
    res.tv_usec += 1000000L;
    res.tv_sec--;
  }

  return ((u_int32_t) res.tv_sec * 1000 + (u_int32_t) res.tv_usec / 1000);
}
