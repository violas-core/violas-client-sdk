#pragma once

#include <vector>
#include <tuple>
#include <diem_types.hpp>
#include "ed25519.hpp"

namespace violas
{
    std::array<uint8_t, 32> sha_256(uint8_t *data, size_t len);
    std::array<uint8_t, 64> sha_512(uint8_t *data, size_t len);
    std::array<uint8_t, 32> sha3_256(uint8_t *data, size_t len);
    
    class Wallet
    {
    public:
        // using Entropy = std::array<uint8_t, 33>;
        using Key = std::array<uint8_t, 32>;

    private:
        // 32 bytes entropy + 1 byte hash
        std::array<uint8_t, 33> m_entropy_hash;

        Key m_seed;

        Key main_key;

        void generate_seed(std::string_view salt = "DIEM");

        void extract_main_key();

        Key extend_child_private_key(uint64_t index);

        std::vector<crypto::ed25519::PrivateKey> m_private_keys;

        Wallet();

    public:
        Wallet(Key &&entropy);
        ~Wallet();

        static Wallet generate_from_random();

        static Wallet generate_from_mnemonic(std::string_view mnemonic);

        std::string export_mnemonic();

        static Key pub_key_to_auth_key(const crypto::ed25519::PublicKey &pub_key);
        static diem_types::AccountAddress pub_key_account_address(const crypto::ed25519::PublicKey &pub_key);
        

        // return account index and raw public key
        std::tuple<size_t, diem_types::AccountAddress> create_next_account();

        struct Account
        {
            size_t index;
            diem_types::AccountAddress address;
            std::array<uint8_t, 32> auth_key;
        };

        std::vector<Account> get_all_accounts();

        std::array<uint8_t, 32>
        get_account_pub_key(size_t index);

        crypto::ed25519::PrivateKey
        get_account_priv_key(size_t index);

        static void run_test_case();
    };
}
