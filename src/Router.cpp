#include "Router.h"

std::string Router::getStringAddress() const 
{ 
    return address; 
}

bool Router::operator==(const Router& other) const
{
    return address == other.address;
}