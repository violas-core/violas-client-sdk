#pragma once
#include <optional>
#include <violas_client2.hpp>
#include <bcs_serde.hpp>

namespace violas::nft
{
    using Id = std::array<uint8_t, 32>;

    inline const Address NFT_STORE_ADMIN_ADDRESS = {{00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x11, 0x22}};

    struct Order
    {
        bytes nft_token_id;
        uint64_t price;
        bytes currency;
        Address provider;
        uint64_t timestamp;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && nft_token_id && price && currency && provider && timestamp;
        }
    };

    struct AccountInfo
    {
        violas::EventHandle made_order_event_handle;
        violas::EventHandle revoked_order_event_handle;
        violas::EventHandle sold_order_event_handle;
        violas::EventHandle bought_order_event_handle;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && made_order_event_handle && revoked_order_event_handle && sold_order_event_handle && bought_order_event_handle;
        }
    };

    struct MadeOrderEvent : public EventBase
    {
        bytes order_id;
        bytes nft_token_id;
        uint64_t price;
        bytes currency;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && order_id && nft_token_id && price && currency;
        }
    };

    struct RevokedOrderEvent : public EventBase
    {
        bytes order_id;
        bytes nft_token_id;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && order_id && nft_token_id;
        }
    };

    struct SoldOrderEvent : public EventBase
    {
        bytes order_id;
        bytes nft_token_id;
        Address buyer;
        uint64_t price;
        bytes currency;
        uint64_t fee_rate;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && order_id && nft_token_id && buyer && price && currency && fee_rate;
        }
    };

    struct BoughtOrderEvent : public EventBase
    {
        bytes order_id;
        bytes nft_token_id;
        Address seller;
        uint64_t price;
        bytes currency;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && order_id && nft_token_id && seller && price && currency;
        }
    };

    struct TradedOrderEvent : public EventBase
    {
        bytes order_id;
        bytes nft_token_id;
        Address seller;
        Address buyer;
        uint64_t price;
        bytes currency;
        uint64_t fee_rate;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && order_id && nft_token_id && seller && buyer && price && currency && fee_rate;
        }
    };

    enum event_type
    {
        made,
        revoked,
        sold,
        bought,
        traded
    };

    class Store
    {
    private:
        /* data */
        violas::client2_ptr _client;
        diem_types::TypeTag _nft_type_tag;

    public:
        Store(violas::client2_ptr client, const diem_types::TypeTag &nft);

        ~Store();

        void initialize(size_t account_index = 0);

        void register_nft(size_t account_index = 0);

        void accept_nft(size_t account_index);

        void make_order(size_t account_index,
                        Id nft_token_id,
                        uint64_t price,
                        std::string_view currency);

        void revoke_order(size_t account_index,
                          Id order_id);

        void trade_order(size_t account_index,
                         std::string_view currency,
                         Id order_id);

        std::optional<AccountInfo>
        get_account_info(Address address);

        std::vector<Order>
        list_orders();

        std::optional<EventHandle> get_event_handle(Address address, event_type type);

        template <typename T>
        std::vector<T>
        query_order_events(EventHandle handle, uint64_t start, uint64_t limit)
        {
            return _client->query_events<T>(handle, start, limit);
        }
    };

} // namespace nft

//
//  print all structures
//
std::ostream &operator<<(std::ostream &os, const violas::nft::AccountInfo &orders);

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::Order> &orders);

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::MadeOrderEvent> &orders);

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::RevokedOrderEvent> &orders);

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::SoldOrderEvent> &orders);

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::BoughtOrderEvent> &orders);

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::TradedOrderEvent> &orders);