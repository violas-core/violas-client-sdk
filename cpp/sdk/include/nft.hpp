#ifndef NFT_HPP
#define NFT_HPP
#include <string>
//#include "violas_sdk2.hpp"

namespace violas
{
    template<typename T>
    class NonFungibleToken
    {
        violas::client_ptr _client;

    public:
        NonFungibleToken(client_ptr client);

        virtual ~NonFungibleToken() {}
        
        void  deploy();
        
        void register_instance(uint64_t total_number);

        void accept(size_t account);

        void mint();

        void burn();

        void transfer();
    };
}

#endif