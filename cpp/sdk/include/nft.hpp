#pragma once

#include <string>
#include <array>
#include <map>
#include "violas_sdk2.hpp"

namespace violas::nft
{
    using TokenId = std::array<uint8_t, 32>;

    struct NftInfo
    {
        bool limited;
        uint64_t total;
        uint64_t amount;
        violas::Address admin;
        std::map<std::vector<uint8_t>, std::vector<violas::Address>> owners;
        violas::EventHandle mint_event;
        violas::EventHandle burn_event;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && limited && total && amount && admin && owners && mint_event && burn_event;
        }
    };

    struct Account
    {
        violas::EventHandle sent_event;
        violas::EventHandle received_event;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && sent_event && received_event;
        }
    };

    struct Event
    {
        uint64_t sequence_number;
        uint64_t transaction_version;
    };

    struct MintedEvent : public Event
    {
        std::vector<uint8_t> token_id;
        violas::Address receiver;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id && receiver;
        }
    };

    struct BurnedEvent : public Event
    {
        std::vector<uint8_t> token_id;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id;
        }
    };

    struct SentEvent : public Event
    {
        std::vector<uint8_t> token_id;
        violas::Address payee;
        std::vector<uint8_t> metadata;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id && payee && metadata;
        }
    };

    struct ReceivedEvent : public Event
    {
        std::vector<uint8_t> token_id;
        violas::Address payer;
        std::vector<uint8_t> metadata;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id && payer && metadata;
        }
    };

    enum EventType
    {
        minted,
        burned,
        sent,
        received
    };

    template <typename T>
    TokenId compute_token_id(const T &t)
    {
        BcsSerde serde;
        auto temp = t;

        serde &&temp;

        auto bytes = serde.bytes();

        auto token_id = sha3_256(bytes.data(), bytes.size());

        return token_id;
    }
    //
    //      NonFungibleToken
    //
    template <typename T>
    class NonFungibleToken
    {
    protected:
        violas::client_ptr _client;
        std::string _url; //json rpc url

    public:
        NonFungibleToken(client_ptr client, std::string url);

        virtual ~NonFungibleToken() {}

        void deploy();

        void register_instance(uint64_t total_number);

        void accept(size_t account);

        //void mint();

        void burn(TokenId token_id);

        void transfer_by_token_index(uint64_t account_index,
                                     Address receiver,
                                     uint64_t token_index,
                                     std::vector<uint8_t> metadata = {});

        void transfer_by_token_id(uint64_t account_index,
                                  Address receiver,
                                  TokenId token_id,
                                  std::vector<uint8_t> metadata = {});

        std::optional<std::vector<T>> balance(const Address &addr);

        std::optional<std::vector<Address>> get_owners(std::string url, const TokenId &token_id);

        std::optional<NftInfo> get_nft_info(std::string url);

        std::optional<Account> get_account(const violas::Address &address);

        std::optional<EventHandle> get_event_handle(EventType event_type,
                                                    const violas::Address &address);

        template <typename EVENT>
        std::vector<EVENT> query_events(const EventHandle &event_handle,
                                        const violas::Address &address,
                                        uint64_t start,
                                        uint64_t limit);
    };

    template <typename T>
    using nft_ptr = std::shared_ptr<NonFungibleToken<T>>;

}

#include "nft.cpp"

// std::ostream &operator<<(std::ostream &os, const violas::nft::MintedEvent &minted_event);