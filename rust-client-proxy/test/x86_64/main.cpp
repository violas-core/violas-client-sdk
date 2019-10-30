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
                                         "",
                                         false,
                                         "faucet.testnet.libra.org",
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

    uint64_t index = 0;

    //client->mint_coins(0, 10, true);

    auto balance = client->get_balance(index);
    cout << "Address " << index << "'s balance is " << balance << endl;

    uint64_t sequence_num = client->get_sequence_number(index);
    cout << "Address " << index << "'s sequence number is " << sequence_num << endl;

    client->transfer_coins_int(0, accounts[1].address, 10, 0, 0, true);

    balance = client->get_balance(index);
    cout << "Address " << index << "'s balance is " << balance << endl;

    index = 1;
    balance = client->get_balance(index);
    cout << "Address " << index << "'s balance is " << balance << endl;

    cout << "finished all test jobs !" << endl;

    return ret;
}