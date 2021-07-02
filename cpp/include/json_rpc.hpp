#ifndef JSON_RPC
#define JSON_RPC
#include <string>
#include <vector>

namespace json_rpc
{
    struct Currency
    {
        std::string code;
    };

    struct AccountStateProof
    {
        // hex-encoded bcs bytes
        std::string ledger_info_to_transaction_info_proof; //[json_name = "ledger_info_to_transaction_info_proof"];
        // hex-encoded bcs bytes
        std::string transaction_info; //[json_name = "transaction_info"];
        // hex-encoded bcs bytes
        std::string transaction_info_to_account_proof; //[json_name = "transaction_info_to_account_proof"];
    };

    struct AccountStateWithProof
    {
        uint64_t version;
        // hex-encoded bcs bytes
        std::string blob;
        // hex-encoded bcs bytes
        AccountStateProof proof;
    };

    struct Client
    {
        static std::shared_ptr<Client>
        create(std::string url);

        virtual ~Client() {}

        virtual std::vector<Currency>
        get_currencies() = 0;

        virtual AccountStateWithProof
        get_account_state_blob(std::string account_address) = 0;
    };

    using client_ptr = std::shared_ptr<Client>;
}

#endif