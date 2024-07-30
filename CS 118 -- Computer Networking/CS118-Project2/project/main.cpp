#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

const int WAN_PORT = 5152;
const std::string WAN_ADDR = "0.0.0.0";
const int MAX_PACKET_SIZE = 65535;
const int IP_ADDR_LEN = 4;
const int IP_TTL_OFFSET = 8;
const int IP_CHECKSUM_OFFSET = 10;
const int IP_DEST_OFFSET = 16; 
// const int UDP_DEST_PORT_OFFSET = 22;  
std::string WAN_IP;
int allocate_new_ports = 0;

struct NAPT_entry {
    std::string lan_ip;
    int lan_port;
    int wan_port;
};

struct ACL_entry {
    std::string src_ip;
    int src_sub;
    int src_start;
    int src_end;
    std::string dest_ip;
    int dest_sub;
    int dest_start;
    int dest_end;
};

enum Protocol {
    TCP = 6,
    UDP = 17,
};

int generateNewPort() {
    int max_port = 65535;
    if (allocate_new_ports < max_port){
        allocate_new_ports += 1;
    } else {
        allocate_new_ports = 49152;
    }
    return allocate_new_ports;
}

enum Protocol getProtocol(unsigned char* iphdr) {
    unsigned char protocol = iphdr[9];
    switch (protocol) {
        case TCP:
            return TCP;
        case UDP:
            return UDP;
    }
}

std::vector<std::string> split(const std::string &input) {
    std::istringstream iss(input);
    std::vector<std::string> parts;

    std::string part;
    while (iss >> part) {
        parts.push_back(part);
    }
    return parts;
}

std::unordered_map<std::string, int> setupSocket(const std::vector<std::string> addrs) {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(WAN_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Awaiting connections..." << std::endl;

    std::unordered_map<std::string, int> conns;
    for (std::string addr : addrs) {
        int conn_fd;
        if ((conn_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        std::cout << "Connected to: " << addr << std::endl;
        conns[addr] = conn_fd;
    }

    return conns;
}

std::string readDest(const unsigned char* packet) {
    std::string dest;
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        int dest_part = (int) packet[i + IP_DEST_OFFSET];
        if (!dest.empty()) dest += '.';
        dest += std::to_string(dest_part);
    }
    return dest;
}


std::string readSourceIP(const unsigned char* packet) {
    const int IP_SRC_OFFSET = 12;  // Source IP starts at byte 12 of the IP header
    std::string sourceIP;
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        int sourcePart = (int) packet[i + IP_SRC_OFFSET];
        if (!sourceIP.empty()) sourceIP += '.';
        sourceIP += std::to_string(sourcePart);
    }
    return sourceIP;
}

uint16_t readSourcePort(unsigned char* buffer) {
    int ipHeaderLen = (buffer[0] & 0x0F) * 4;
    uint16_t sourcePort = (buffer[ipHeaderLen] << 8) + buffer[ipHeaderLen + 1];
    return sourcePort; 
}

void updateSourceIP(unsigned char* buffer, const char* newIP) {
    const int IP_OFFSET = 12;
    std::memcpy(buffer + IP_OFFSET, newIP, IP_ADDR_LEN);
}

void updateDestIPAndPort(unsigned char* buffer, std::string new_ip, int new_port) {
    int headerLen = (buffer[0] & 0x0F) * 4;
    const int IP_DEST_OFFSET = 16;  // Destination IP starts at byte 16 of the IP header
    const int UDP_DEST_PORT_OFFSET = headerLen + 2;  // Destination port starts at byte 22 of the UDP header

    // Update destination IP
    std::stringstream sstream(new_ip);
    std::string part;
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        std::getline(sstream, part, '.');
        buffer[i + IP_DEST_OFFSET] = std::stoi(part);
    }

    // Update destination port
    buffer[UDP_DEST_PORT_OFFSET] = new_port >> 8;   // High byte of the new port number
    buffer[UDP_DEST_PORT_OFFSET + 1] = new_port & 0xFF;    // Low byte of the new port number
}


void updateSourceIPAndPort(unsigned char* buffer, std::string new_ip, int new_port) {
    int headerLen = (buffer[0] & 0x0F) * 4;
    const int IP_SRC_OFFSET = 12;  // Source IP starts at byte 12 of the IP header
    const int UDP_SRC_PORT_OFFSET = headerLen;  // Source port starts at byte 20 of the UDP header

    // Update source IP
    std::stringstream sstream(new_ip);
    std::string part;
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        std::getline(sstream, part, '.');
        buffer[i + IP_SRC_OFFSET] = std::stoi(part);
    }

    // Update source port
    buffer[UDP_SRC_PORT_OFFSET] = (unsigned char) ((new_port >> 8) & 0xFF);   // High byte of the new port number
    buffer[UDP_SRC_PORT_OFFSET+1] = (unsigned char) (new_port & 0xFF);    // Low byte of the new port number
}

uint16_t readDestPort(const unsigned char* packet) {
  int headerLen = (packet[0] & 0x0F) * 4;
  const int UDP_DEST_PORT_OFFSET = headerLen + 2;
  uint16_t src_port =(packet[UDP_DEST_PORT_OFFSET] << 8) | packet[UDP_DEST_PORT_OFFSET + 1];
  return src_port;
}
/*
 * Code to compute checksum from:
 * https://gist.github.com/david-hoze/0c7021434796997a4ca42d7731a7073a
 */
unsigned short computeChecksum(unsigned char* packet) {
    // Clear out existing UDP checksum
    packet[IP_CHECKSUM_OFFSET] = 0;
    packet[IP_CHECKSUM_OFFSET + 1] = 0;
    unsigned long checksum = 0;
    int headerLen = (packet[0] & 0x0F) * 4;

    for (int i = 0; i < headerLen; i += 2) {
        checksum += (packet[i] << 8) + packet[i + 1];
    }

    while (checksum >> 16) {
        checksum = (checksum & 0xFFFF) + (checksum >> 16);
    }
    checksum = ~checksum;

    return (unsigned short) checksum;
}

/*
 * Used code for efficient checksum update from:
 * https://www.packetmania.net/en/2021/12/26/IPv4-IPv6-checksum/
 */
void updateChecksum(unsigned char* packet, unsigned short old_checksum) {
    unsigned long sum = old_checksum + 0x100; 
    unsigned short checksum = (sum + (sum >> 16));
    packet[IP_CHECKSUM_OFFSET] = (unsigned char) (checksum >> 8);
    packet[IP_CHECKSUM_OFFSET+1] = (unsigned char) checksum;
}


// Assume packet is a vector of unsigned chars
unsigned short read_checksum(unsigned char* packet) {
    unsigned short checksum = (packet[10] << 8) | packet[11];
    return checksum;
}


unsigned short computeAndUpdateChecksum(unsigned char* packet) {
    packet[IP_CHECKSUM_OFFSET] = 0;
    packet[IP_CHECKSUM_OFFSET + 1] = 0;
    unsigned long checksum = 0;
    int headerLen = (packet[0] & 0x0F) * 4;

    for (int i = 0; i < headerLen; i += 2) {
        checksum += (packet[i] << 8) + packet[i + 1];
    }

    while (checksum >> 16) {
        checksum = (checksum & 0xFFFF) + (checksum >> 16);
    }
    checksum = ~checksum;

    unsigned short newChecksum = (unsigned short) checksum;

    packet[IP_CHECKSUM_OFFSET] = (unsigned char) (newChecksum >> 8);
    packet[IP_CHECKSUM_OFFSET+1] = (unsigned char) newChecksum;
    return newChecksum;
}

unsigned char* getIPHeader(unsigned char* buffer) {
    // IP header starts at the beginning of the buffer
    return buffer;
}

unsigned char* getUDPHeader(unsigned char* buffer) {
    // UDP header starts after the IP header
    // Assuming no IP options, IP header is 20 bytes
    int headerLen = (buffer[0] & 0x0F) * 4;
    return buffer + headerLen;
}

size_t getUDPLength(unsigned char* buffer) {
    // UDP length is in the 5th and 6th bytes of the UDP header (big endian)
    // As per RFC, it includes header length
    unsigned char* udpHeader = getUDPHeader(buffer);
    size_t udpLen = (udpHeader[4] << 8) | udpHeader[5];
    return udpLen;
}

unsigned char* getTCPHeader(unsigned char* buffer) {
    // TCP header starts after the IP header
    // Assuming no IP options, IP header is 20 bytes
    int headerLen = (buffer[0] & 0x0F) * 4;
    return buffer + headerLen;
}

size_t getTCPLength(unsigned char* buffer, int len) {
    int headerLen = (buffer[0] & 0x0F) * 4;
    return len - headerLen;
}

unsigned short computeTCPChecksum(unsigned char* iphdr, unsigned char* tcphdr, size_t tcpLen) {
    unsigned char check1 = tcphdr[16];
    unsigned char check2 = tcphdr[17];
    tcphdr[16] = 0;
    tcphdr[17] = 0;
 
    unsigned long sum = 0;
 
    // Calculate the sum of the pseudo-header
    // Source IP address
    // sum += (iphdr[12] << 8) + iphdr[13];
    // sum += (iphdr[14] << 8) + iphdr[15];
 
    // // Destination IP address
    // sum += (iphdr[16] << 8) + iphdr[17];
    // sum += (iphdr[18] << 8) + iphdr[19];
 
    for (int i = 0; i < 4; i += 2) {
      sum += (iphdr[12 + i] << 8) + iphdr[12 + i + 1];
    }
 
    // Destination IP address
    for (int i = 0; i < 4; i += 2) {
      sum += (iphdr[16 + i] << 8) + iphdr[16 + i + 1];
    }
 
    // Protocol (TCP is 6)
    sum += (unsigned short)iphdr[9];
 
    // TCP length
    sum += tcpLen;
 
 
    // TCP source port, destination port and length
    // for (int i = 0; i < 6; i += 2) {
    //   sum += (tcphdr[i] << 8) + tcphdr[i + 1];
    // }
 
    // TCP data
    for (size_t i = 0; i < tcpLen; i += 2) {
        sum += (tcphdr[i] << 8) + tcphdr[i + 1];
    }
    // if (tcpLen % 2 != 0) {
    //     sum += *((unsigned char*) (tcphdr + tcpLen - 1));
    // }
 
    // carry
    while (sum >> 16) {
      sum = (sum & 0xFFFF) + (sum >> 16);
    }
 
    sum = ~sum;

    tcphdr[16] = check1;
    tcphdr[17] = check2;
 
    std::cout << "New Checksum: " << (unsigned short)sum << std::endl;
 
    return ((unsigned short) sum);
}

void updateTCPChecksum(unsigned char* tcphdr, unsigned short sum) {
    tcphdr[16] = (unsigned char) (sum >> 8);
    tcphdr[17] = (unsigned char) sum;
}

unsigned short computeUDPChecksum(unsigned char* iphdr, unsigned char* udphdr, size_t udpLen)
{
    unsigned char check1 = udphdr[6];
    unsigned char check2 = udphdr[7];
    udphdr[6] = 0;
    udphdr[7] = 0;
    unsigned long sum = 0;
    
    // Add pseudo-header:
    // Source IP address
    for (int i = 0; i < 4; i+=2) {
        sum += (iphdr[12+i]<<8) + iphdr[12+i+1];
    }
    
    // Destination IP address
    for (int i = 0; i < 4; i+=2) {
        sum += (iphdr[16+i]<<8) + iphdr[16+i+1];
    }
    
    // Protocol and reserved: 17
    sum += (unsigned short)iphdr[9];

    // UDP length
    sum += (udphdr[4]<<8) + udphdr[5];
    
    // UDP source port, destination port and length
    for(int i=0; i < 6; i+=2) {
        sum += (udphdr[i]<<8) + udphdr[i+1];
    }
    
    // UDP data
    for (size_t i=8; i<udpLen; i+=2) {
        sum += (udphdr[i]<<8) + udphdr[i+1];
    }
    // carry
    while (sum>>16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    sum = ~sum;
    udphdr[6] = check1;
    udphdr[7] = check2;
    return ((unsigned short) sum);
}

void updateUDPChecksum(unsigned char* udphdr, unsigned short sum)
{
    udphdr[6] = (unsigned char) (sum >> 8);
    udphdr[7] = (unsigned char) sum;
}

/////// DELETE
std::string unorderedMapToString(std::unordered_map<std::string, int>& map) {
    std::ostringstream oss;
    oss << "{ ";
    
    for (const auto& pair : map) {
        oss << pair.first << ": " << pair.second << ", ";
    }
    
    oss << "}";
    
    return oss.str();
}

///////

unsigned int ipToInteger(const std::string &ipAddress) {
    std::stringstream ss(ipAddress);
    std::string octet;
    std::vector<int> octets;

    while (std::getline(ss, octet, '.')) {
        octets.push_back(std::stoi(octet));
    }

    if (octets.size() != 4) {
        std::cerr << "Invalid IP address format!" << std::endl;
        return 0;
    }

    unsigned int result =
        (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
    return result;
}

unsigned int createSubnetMask(unsigned int subnetMaskInteger) {
    unsigned int networkBits = 0;
    unsigned int subnetMask = subnetMaskInteger;

    // Determine the number of network bits in the subnet mask
    while (subnetMask > 0) {
        subnetMask = subnetMask >> 1;
        networkBits++;
    }

    unsigned int hostBits = 32 - networkBits;
    return (0xFFFFFFFF << hostBits) & 0xFFFFFFFF;
}

bool matchIP(std::string base, int sub, std::string ip) {
    unsigned int base_i = ipToInteger(base);
    unsigned int ip_i = ipToInteger(ip);
    unsigned int mask = createSubnetMask(sub);

    return (base_i & mask) == (ip_i & mask);
}

bool checkACL(unsigned char* packet, std::vector<ACL_entry> acl_table) {
    std::string src_ip = readSourceIP(packet);
    std::string dest_ip = readDest(packet);
    int src_port = readSourcePort(packet);
    int dest_port = readDestPort(packet);

    for (ACL_entry entry : acl_table) {
        if ((matchIP(entry.src_ip, entry.src_sub, src_ip) && src_port >= entry.src_start && src_port <= entry.src_end) ||
            (matchIP(entry.dest_ip, entry.dest_sub, dest_ip) && dest_port >= entry.dest_start && dest_port <= entry.dest_end)) {
                return false;
        }
    }
    return true;
}

void route(std::unordered_map<std::string, int> conns, std::vector<NAPT_entry> NAPT_table, std::vector<ACL_entry> acl_table) {
    unsigned char buffer[MAX_PACKET_SIZE + 1];
    fd_set readfds;
    while (conns.size() > 0) {
        FD_ZERO(&readfds);
        int max = 0;
        for (auto const &conn : conns) {
            FD_SET(conn.second, &readfds);
            if (conn.second > max) max = conn.second;
        }

        int activity = select(max + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        for (auto const &conn : conns) {
            if (FD_ISSET(conn.second, &readfds)) {
                ssize_t num_bytes;
                if ((num_bytes = read(conns[conn.first], buffer, MAX_PACKET_SIZE)) < 0) {
                    close(conn.second);
                    conns.erase(conn.first);
                    break;
                } else if (num_bytes > 0) {
                    buffer[num_bytes] = '\0';
                    std::cout << "Bytes Read: " << num_bytes << std::endl;
                    
                    std::string dest = readDest(buffer);
                    
                    unsigned short checksum = (((short) buffer[IP_CHECKSUM_OFFSET]) << 8) | buffer[IP_CHECKSUM_OFFSET + 1];
                    buffer[IP_CHECKSUM_OFFSET] = 0;
                    buffer[IP_CHECKSUM_OFFSET+1] = 0;
                    
                    if (computeChecksum(buffer) != checksum || buffer[IP_TTL_OFFSET] <= 1)
                        break;
                    buffer[IP_TTL_OFFSET]--;
                    //updateChecksum(buffer, checksum)
                    int destPort = -1;
                    computeAndUpdateChecksum(buffer);
                    // If we can't find the port, assume that it's going WAN
                    
                    unsigned char* ipHeader = getIPHeader(buffer);
                    unsigned char* hdr = NULL;
                    size_t len = 0;
                    if (getProtocol(ipHeader) == TCP) {
                        hdr = getTCPHeader(buffer);
                        len = getTCPLength(buffer, num_bytes);
                        unsigned short tcpChecksum = (((short) hdr[16]) << 8) | hdr[17];
                        if (computeTCPChecksum(ipHeader, hdr, len) != tcpChecksum) {
                            break;
                        }
                    } else if (getProtocol(ipHeader) == UDP) {
                        hdr = getUDPHeader(buffer);
                        len = getUDPLength(buffer);
                        unsigned short udpChecksum = (((short) hdr[6]) << 8) | hdr[7];
                        if(computeUDPChecksum(ipHeader, hdr, len) != udpChecksum) {
                            break;
                        }
                    }  
                    if(conns.find(dest) == conns.end() && dest != WAN_IP) {
                        std::cout << "Destination not found: " << dest
                                    << std::endl;
                        destPort = conns[WAN_ADDR];
                        std::string current_ip = readSourceIP(buffer);
                        int current_lan_port = readSourcePort(buffer);
                        bool found = false;
                        for (auto entry : NAPT_table) {
                            std::cout << "LAN PORT " << entry.lan_port << " LAN2 " << current_lan_port << std::endl;
                            if (entry.lan_ip == current_ip &&
                                entry.lan_port == current_lan_port) {
                                updateSourceIPAndPort(buffer, WAN_IP, entry.wan_port);
                                found = true;
                                break;
                            }
                        }
                        if(!found) {
                            int new_port = generateNewPort();
                            NAPT_entry entry = {
                                current_ip,
                                readSourcePort(buffer),
                                new_port
                            };
                            NAPT_table.push_back(entry);
                            updateSourceIPAndPort(buffer, WAN_IP, new_port);
                        }
                        std::cout << "Here: " << std::endl;
                        if (getProtocol(ipHeader) == TCP) {
                            unsigned short sum = computeTCPChecksum(ipHeader, hdr, len);
                            updateTCPChecksum(hdr, sum);
                        } else if (getProtocol(ipHeader) == UDP) {
                            unsigned short sum = computeUDPChecksum(ipHeader, hdr, len);
                            updateUDPChecksum(hdr, sum);
                            
                        }
                    } else if(dest == WAN_IP){
                        int destinationPort = readDestPort(buffer);
                        bool found = false;
                        for(auto entry: NAPT_table) {
                            if(destinationPort == entry.wan_port) {
                                updateDestIPAndPort(buffer, entry.lan_ip, entry.lan_port);
                                destPort = conns[entry.lan_ip];
                                found = true;
                                break;
                            }

                        }

                        if(!found) {
                            break;
                        }

                        if (getProtocol(ipHeader) == TCP) {
                            unsigned short sum = computeTCPChecksum(ipHeader, hdr, len);
                            updateTCPChecksum(hdr, sum);
                        } else if (getProtocol(ipHeader) == UDP) {
                            unsigned short sum = computeUDPChecksum(ipHeader, hdr, len);
                            updateUDPChecksum(hdr, sum);
                        }
                    } else {
                        destPort = conns[dest];
                    }

                    std::cout << unorderedMapToString(conns) << std::endl;
                    std::cout << "Source IP: " << readSourceIP(buffer) << std::endl;
                    std:: cout << "Updated Checksum " << computeAndUpdateChecksum(buffer) << std::endl;
                    std::cout << "Read Checksum " << read_checksum(buffer) << std::endl;    

                    /*
                        if (!checkACL(buffer, acl_table)) {
                            break;
                        }
                    */

                    if (send(destPort, buffer, num_bytes, 0) != num_bytes) {
                        std::cout << buffer << std::endl;
                        perror("send");
                        exit(EXIT_FAILURE);
                    }

                    std::cout << "Packet routed to " << dest << std::endl;
                    break;
                }
            }    
        }
    }
}

int main() {
    std::string szLine;

    // First line is the router's LAN IP and the WAN IP
    std::getline(std::cin, szLine);
    size_t dwPos = szLine.find(' ');
    auto szLanIp = szLine.substr(0, dwPos);
    auto szWanIp = szLine.substr(dwPos + 1);
    WAN_IP = szWanIp;
    std::cout << "Server's LAN IP: " << szLanIp << std::endl << "Server's WAN IP: " << WAN_IP << std::endl;

    std::vector<std::string> addrs;
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
        addrs.push_back(line);
    }

    std::vector<NAPT_entry> NAPT_table;
    std::cout << "___a" << std::endl;
    while (std::getline(std::cin, line) && !line.empty()) {
        std::vector<std::string> parts = split(line);
        struct NAPT_entry entry = {
            parts[0],
            std::stoi(parts[1]),
            std::stoi(parts[2])
        };
        std::cout << parts[0] << std::endl;
        std::cout << parts[1] << std::endl;
        std::cout << parts[2] << std::endl;
        std::cout << "___b" << std::endl;
        NAPT_table.push_back(entry);
    }

    std::vector<ACL_entry> acl_table;
    while (std::getline(std::cin, line) && !line.empty()) {
        struct ACL_entry entry;
        std::vector<std::string> parts = split(line);

        size_t pos = parts[0].find("/");
        entry.src_ip = parts[0].substr(0, pos);
        entry.src_sub = std::stoi(parts[0].substr(pos + 1));

        size_t dashPos = parts[1].find("-");
        entry.src_start = std::stoi(parts[1].substr(0, dashPos));
        entry.src_end = std::stoi(parts[1].substr(dashPos + 1));

        pos = parts[2].find("/");
        entry.dest_ip = parts[2].substr(0, pos);
        entry.dest_sub = std::stoi(parts[2].substr(pos + 1));

        dashPos = parts[3].find("-");
        entry.dest_start = std::stoi(parts[3].substr(0, dashPos));
        entry.dest_end = std::stoi(parts[3].substr(dashPos + 1));

        acl_table.push_back(entry);
    }

    for (auto entry : acl_table) {
      std::cout << entry.src_ip << " " << entry.src_sub << " " << entry.src_start
                << " " << entry.src_end << " " << entry.dest_ip << " "
                << entry.dest_sub << " " << entry.dest_start << " " << entry.dest_end
                << std::endl;
    }

    std::unordered_map<std::string, int> conns = setupSocket(addrs);
    route(conns, NAPT_table, acl_table);

    return 0;
}
