#pragma once
#include <client.hpp>

namespace nft
{
    class Store
    {
    private:
        /* data */
        violas::client_ptr _client;

    public:
        Store(violas::client_ptr client);

        ~Store();

        void initalize(const violas::TypeTag &tag);

        void register_nft(const violas::TypeTag &tag);

        void offer();

        void revoke();

        void trade();
    };

} // namespace nft
