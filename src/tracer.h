#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <iostream>
//#include <vector>
//#include <string>
//#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include "functions.h"


HANDLE createICMPHandle();

unsigned long resolveHostnameToIP(const char* hostname);

void* allocateReplyBuffer(size_t& reply_size, const size_t data_size);

std::vector<std::string> tracerouteIPs(HANDLE hIcmpFile, unsigned long ipaddr, void* reply_buffer, size_t reply_size);

std::pair<bool, std::vector<std::string>> traceroute(const char* hostname);