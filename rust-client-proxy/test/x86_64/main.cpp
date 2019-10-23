//
//
// compiling : clang++ -std=c++2a -stdlib=libc++ -g main.cpp -o test

#include <iostream>
#include <assert.h>
#include "rust_client_proxy.hpp"
#include "libra_client.hpp"

using namespace std;

bool test_libra_client();

int main(int argc, const char *argv[])
{
    cout << "running test ..." << endl;

    // cout << "add(3, 4) = " << add(3, 4) << endl;

    // cout << "create_native_client_proxy " << create_native_client_proxy("localhost", 8000, "validator", "", false, "faucet", "mnemonic") << endl;

    try
    {
        assert(test_libra_client() == true);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

bool test_libra_client()
{
    auto client = libra_client::create_libra_client("localhost",
                                                    8000,
                                                    "~/Projects/work/libra/scripts/cli/consensus_peers.config.toml",
                                                    "", false,
                                                    "faucet",
                                                    "mnemonic");

    return true;
}