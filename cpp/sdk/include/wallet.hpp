#pragma once

#include "ed25519.hpp"
#include <vector>

namespace violas
{
    using Key = std::array<uint8_t, 33>;

    class Wallet
    {
    private:
        /* data */
        Key key;
        std::array<uint8_t, 64> seed;

        std::vector<Key> derived_keys;
        
        void generate_seed();

    public:
        Wallet(/* args */);
        ~Wallet();

        static Wallet load_mnemonic(std::string_view mnemonic);
        std::string export_mnemonic();

        void create_next_account();

        std::vector<std::array<uint8_t, 16>>
        get_all_accounts();

        std::array<uint8_t, 16>
        get_account_address(size_t index);

        static void run_test_case();
    };
}
