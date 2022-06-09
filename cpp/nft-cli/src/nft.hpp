#pragma once
#include <array>
#include <string>
#include <vector>
#include <violas_client2.hpp>
#include <co_helper.hpp>
#include <serde.hpp>

namespace nft
{
    using Address = std::array<uint8_t, 16>;
    using TokenId = std::array<uint8_t, 32>;
    using Bytes = std::vector<uint8_t>;

    struct Token
    {
        std::string cpu;
        std::string ram;
        std::string disk;
        std::string bandwidth;

        TokenId get_token_id() const
        {
            BcsSerde bcs_serde;

            auto t = *this;
            t.serde(bcs_serde);

            auto bytes = bcs_serde.bytes();

            return violas::sha3_256(bytes.data(), bytes.size());
        }

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && cpu && ram && disk && bandwidth;
        }
    };

    struct AccountInfo
    {
        std::vector<Token> tokens;
        violas::EventHandle minted_event;
        violas::EventHandle sent_event;
        violas::EventHandle received_event;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && tokens;
        }
    };

    struct GlobalInfo
    {
        violas::EventHandle minted_token_event;
        violas::EventHandle shared_token_event;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && minted_token_event && shared_token_event;
        }
    };

    struct MintedEvent : public violas::EventBase
    {

    };

    struct BurnedEvent : public violas::EventBase
    {

    };

    struct SentEvent : public violas::EventBase
    {

    };

    struct ReceivedEvent : public violas::EventBase
    {

    };

    struct TransferredEvent : public violas::EventBase
    {

    };

    struct SharedTokenEvent : public violas::EventBase
    {
        uint64_t sn; // sequence number for table
        Address sender;
        Address receiver;
        Bytes token_id;
        std::string message;

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && sender && receiver && token_id && message;
        }
    };

    class Client
    {
    private:
        /* data */
        violas::client2_ptr m_violas_client;

    public:
        Client(violas::client2_ptr violas_client);
        ~Client();

        Task<void> initialize();

        void accept();

        Task<void>
        await_accept(size_t account_index);

        Task<void>
        await_mint_token(size_t account_index, std::string_view hdfs_path);

        Task<void>
        await_share_token(size_t account_index, Address receiver, const TokenId &token_id, std::string_view message = "");

        Task<void>
        await_share_token(size_t account_index, Address receiver, size_t token_index, std::string_view message = "");

        Task<std::vector<Token>>
        await_get_tokens(Address address);

        Task<std::vector<SharedTokenEvent>>
        query_shared_token_events_history(Address &&address, TokenId &&id);

    private:
        void make_recursive_query_shared_token(Address addr, TokenId token_id);
    };

}