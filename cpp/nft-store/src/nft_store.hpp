#pragma once
#include <violas_client2.hpp>

namespace nft
{
    class Store
    {
    private:
        /* data */
        violas::client2_ptr _client;

    public:
        Store(violas::client2_ptr client);

        ~Store();

        void initialize(const diem_types::TypeTag &tag);

        void register_nft(const diem_types::TypeTag &tag);

        void offer();

        void revoke();

        void trade();
    };

} // namespace nft
