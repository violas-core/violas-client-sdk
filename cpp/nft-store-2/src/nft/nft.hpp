#pragma once

#include <string>
#include <array>
#include <map>
#include <violas_client2.hpp>

namespace violas::nft
{
    using TokenId = std::array<uint8_t, 32>;

    struct NftInfo
    {
        bool limited;
        uint64_t total;
        uint64_t amount;
        Address admin;
        std::map<std::vector<uint8_t>, Address> owners;
        violas::EventHandle mint_event;
        violas::EventHandle burn_event;
        violas::EventHandle transferred_event;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && limited && total && amount && admin && owners && mint_event && burn_event && transferred_event;
        }
    };

    template <typename T>
    struct NFT
    {
        std::vector<T> tokens;
        violas::EventHandle sent_event;
        violas::EventHandle received_event;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && tokens && sent_event && received_event;
        }
    };    

    struct MintedEvent : public EventBase
    {
        std::vector<uint8_t> token_id;
        Address receiver;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id && receiver;            
        }
    };

    struct BurnedEvent : public EventBase
    {
        std::vector<uint8_t> token_id;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id;
        }
    };

    struct SentEvent : public EventBase
    {
        std::vector<uint8_t> token_id;
        Address payee;
        std::vector<uint8_t> metadata;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id && payee && metadata;
        }
    };

    struct ReceivedEvent : public EventBase
    {
        std::vector<uint8_t> token_id;
        Address payer;
        std::vector<uint8_t> metadata;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && token_id && payer && metadata;
        }
    };

    struct TransferredEvent : public EventBase
    {
        std::vector<uint8_t> token_id;
        Address payer;
        Address peeer;
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
        violas::client2_ptr _client;

    public:
        NonFungibleToken(client2_ptr client);

        virtual ~NonFungibleToken() {}

        void deploy();

        void register_instance(uint64_t total_number, Address admin);

        void accept(size_t account);

        // void mint();

        void burn(size_t account_index, TokenId token_id);

        void transfer_by_token_index(uint64_t account_index,
                                     Address receiver,
                                     uint64_t token_index,
                                     std::vector<uint8_t> metadata = {});

        void transfer_by_token_id(uint64_t account_index,
                                  Address receiver,
                                  TokenId token_id,
                                  std::vector<uint8_t> metadata = {});

        std::optional<std::vector<T>> balance(const Address &addr);

        std::optional<Address> get_owner(const TokenId &token_id);

        std::optional<NftInfo> get_nft_info();

        std::optional<NFT<T>> get_account(const Address &address);

        std::optional<EventHandle> get_event_handle(EventType event_type,
                                                    const Address &address);

        template <typename EVENT>
        std::vector<EVENT> query_events(const EventHandle &event_handle,
                                        const Address &address,
                                        uint64_t start,
                                        uint64_t limit);
    };

    template <typename T>
    using nft_ptr = std::shared_ptr<NonFungibleToken<T>>;

}

// std::ostream &operator<<(std::ostream &os, const diem_types::AccountAddress &address);
// std::istream &operator>>(std::istream &os, const diem_types::AccountAddress &address);
// std::ostream &operator<<(std::ostream &os, const violas::nft::MintedEvent &minted_event);
// std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::MintedEvent> &minted_events);
// std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::BurnedEvent> &burnedevents);
// std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::SentEvent> &sent_events);
// std::ostream &operator<<(std::ostream &os, const violas::nft::NftInfo &nft_info);

#include "nft.cpp"
