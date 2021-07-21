#ifndef NFT_HPP
#define NFT_HPP
#include <string>
#include "violas_sdk2.hpp"

namespace violas
{
    //template<typename T>
    class NonFungibleToken
    {
        client_ptr _client;

    public:
        NonFungibleToken(client_ptr clipent);

        virtual ~NonFungibleToken() {}

        void register_instance();

        void mint();

        void burn();

        void transfer();
    };
}

#endif