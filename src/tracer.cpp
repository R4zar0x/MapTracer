#include "tracer.h"


HANDLE createICMPHandle()
{
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "IcmpCreateFile failed" << std::endl;
    }
    return hIcmpFile;
}

unsigned long resolveHostnameToIP(const char* hostname)
{
    unsigned long ipaddr = inet_addr(hostname);
    if (ipaddr != INADDR_NONE)
    {
        return ipaddr;
    }

    struct addrinfo* result = NULL;
    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
    {
        std::cerr << "Unable to resolve hostname: " << hostname << std::endl;
        return INADDR_NONE;
    }

    ipaddr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
    freeaddrinfo(result);
    return ipaddr;
}

void* allocateReplyBuffer(size_t& reply_size, const size_t data_size)
{
    reply_size = sizeof(ICMP_ECHO_REPLY) + data_size;
    void* buffer = malloc(reply_size);
    if (!buffer)
    {
        std::cerr << "Unable to allocate memory" << std::endl;
    }
    return buffer;
}

std::vector<std::string> tracerouteIPs(HANDLE hIcmpFile, unsigned long ipaddr, void* reply_buffer, size_t reply_size)
{
    const int max_hops = 30;
    char send_data[32] = "Data Buffer";
    std::vector<std::string> ip_list;

    for (int ttl = 1; ttl <= max_hops; ttl++)
    {
        IP_OPTION_INFORMATION ip_options = {};
        ip_options.Ttl = ttl;

        DWORD dwRetVal = IcmpSendEcho(
            hIcmpFile, ipaddr, send_data, sizeof(send_data),
            &ip_options, reply_buffer, reply_size, 1000
        );

        if (dwRetVal != 0)
        {
            auto* pEchoReply = (PICMP_ECHO_REPLY)reply_buffer;
            struct in_addr reply_addr;
            reply_addr.S_un.S_addr = pEchoReply->Address;
            ip_list.push_back(inet_ntoa(reply_addr));

            if (pEchoReply->Status == IP_SUCCESS)
            {
                break;
            }
        }
        else
        {
            ip_list.push_back("Request timed out");
        }
    }

    return ip_list;
}

std::pair<bool, std::vector<std::string>> traceroute(const char* hostname)
{   
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return { false, {} };
    }

    unsigned long ipaddr = resolveHostnameToIP(hostname);
    if (ipaddr == INADDR_NONE)
    {
        return { false, {} };
    }

    HANDLE hIcmpFile = createICMPHandle();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
    {
        return { false, {} };
    }

    size_t reply_size = 0;
    void* reply_buffer = allocateReplyBuffer(reply_size, 32);
    if (!reply_buffer)
    {
        IcmpCloseHandle(hIcmpFile);
        return { false, {} };
    }

    std::vector<std::string> ip_list = tracerouteIPs(hIcmpFile, ipaddr, reply_buffer, reply_size);

    free(reply_buffer);
    IcmpCloseHandle(hIcmpFile);

    return { true, ip_list };
}