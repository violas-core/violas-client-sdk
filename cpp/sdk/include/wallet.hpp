#pragma once

#include "ed25519.hpp"
#include <vector>

namespace violas
{    
    class Wallet
    {
    public:
        using Key = std::array<uint8_t, 32>;

    private:
        /* data */
        std::array<uint8_t, 33> key;
        Key seed;

        Key main_key;

        //std::vector<Key> derived_keys;
        
        void generate_seed(std::string_view salt = "DIEM");

        void extract_main_key();

        Key extend_child_private_key(size_t index);

        std::vector<ed25519::PrivateKey> m_private_keys;

    public:
        Wallet(/* args */);
        ~Wallet();

        static Wallet generate_from_mnemonic(std::string_view mnemonic);
        
        std::string export_mnemonic();

        void create_next_account();

        std::vector<std::array<uint8_t, 16>>
        get_all_accounts();

        std::array<uint8_t, 16>
        get_account_address(size_t index);

        static void run_test_case();
    };
}
