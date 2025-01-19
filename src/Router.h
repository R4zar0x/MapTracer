#pragma once
#include <string>


class Router
{
public:
    Router(const std::string& address = "0.0.0.0") :
        address(address)
    {}

    std::string getStringAddress() const;
    bool operator==(const Router& other) const;

private:
    std::string address;
};
