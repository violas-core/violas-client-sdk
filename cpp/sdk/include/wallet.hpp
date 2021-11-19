#pragma once

#include "ed25519.hpp"
#include <vector>

namespace violas
{
    using Key = std::array<uint8_t, 32>;

    class Wallet
    {
    private:
        /* data */
        Key root_key;

        std::vector<Key> derived_keys;

    public:
        Wallet(/* args */);
        ~Wallet();

        void create_next_account();

        std::vector<std::array<uint8_t, 16>>
        get_all_accounts();

        std::array<uint8_t, 16>
        get_account_address(size_t index);

        static void run_test_case();
    };
}
