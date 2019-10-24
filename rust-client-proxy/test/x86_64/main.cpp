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
    cout << "running test ..." << endl;

    auto client = Violas::client::create("ac.testnet.libra.org",
                                         8000,
                                         "../../../../libra/scripts/cli/consensus_peers.config.toml",
                                         "", false,
                                         "faucet",
                                         "mnemonic");

    bool ret = client->test_validator_connection();

    auto Account = client->create_next_account(true);

    cout << "finished all test jobs !" << endl;

    return ret;
}