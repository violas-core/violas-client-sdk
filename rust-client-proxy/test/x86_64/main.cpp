//
//
// compiling : clang++ -std=c++2a -stdlib=libc++ -g main.cpp -o test

#include <iostream>
#include <fstream>
#include <assert.h>
#include "rust_client_proxy.hpp"
#include "libra_client.hpp"

using namespace std;

bool test_libra_client();

int main(int argc, const char *argv[])
{
    ofstream file("log.txt");
    streambuf *err = clog.rdbuf(file.rdbuf());

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

    auto account = client->create_next_account(true);

    cout << "C++       Created account #" << account.first << " address " << account.second << endl;

    account = client->create_next_account(true);

    cout << "C++       Created account #" << account.first << " address " << account.second << endl;

    auto accounts = client->get_all_accounts();

    for (auto const &account : accounts)
    {
        cout << "Index : " << account.index << "\t"
             << "Address : " << account.address << "\t"
             << "Sequence : " << account.sequence_number << "\t"
             << "Status : " << account.status << endl;
    }

    auto balance = client->get_balance(0);
    cout << "Address 0's balance is " << balance << endl;

    cout << "finished all test jobs !" << endl;

    return ret;
}