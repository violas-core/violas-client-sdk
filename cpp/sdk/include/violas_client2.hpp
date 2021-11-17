#pragma once
#include "json_rpc.hpp"

namespace violas
{
    class Client2
    {
    private:
        /* data */
        json_rpc::client_ptr json_rpc_client;

    public:
        Client2(/* args */);
        ~Client2();
    };
    
    Client2::Client2(/* args */)
    {
    }
    
    Client2::~Client2()
    {
    }
    
}