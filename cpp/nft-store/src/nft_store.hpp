#pragma once
#include <violas_client2.hpp>

namespace nft
{
    using Id = std::array<uint8_t, 32>;

    class Store
    {
    private:
        /* data */
        violas::client2_ptr _client;
        diem_types::TypeTag _nft_type_tag;

    public:
        Store(violas::client2_ptr client, const diem_types::TypeTag &nft);

        ~Store();

        void initialize();

        void register_nft();

        void register_account(size_t account_index);

        void make_order(size_t account_index,
                        Id nft_token_id,
                        uint64_t price,
                        std::string_view currency,
                        double incentive);

        void revoke_order();

        void trade_order();
    };

} // namespace nft
