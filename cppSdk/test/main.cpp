//
//
// compiling : clang++ -std=c++2a -stdlib=libc++ -g main.cpp -o test

#include <iostream>
#include <fstream>
#include <assert.h>
#include "libra_client.hpp"

using namespace std;

bool test_libra_client();
bool test_violas_client();

int main(int argc, const char *argv[])
{
     ofstream file("log.txt");
     streambuf *log = clog.rdbuf(file.rdbuf());

     try
     {
          //assert(test_libra_client() == true);

          assert(test_violas_client() == true);
     }
     catch (const std::exception &e)
     {
          std::cerr << e.what() << '\n';
     }

     clog.rdbuf(log);

     return 0;
}

bool test_libra_client()
{
     cout << "running Libra test ..." << endl;

     auto client = Violas::client::create("ac.testnet.libra.org",
                                          8000,
                                          "../../../libra/scripts/cli/consensus_peers.config.toml",
                                          "",
                                          false,
                                          "faucet.testnet.libra.org",
                                          "mnemonic");

     client->test_validator_connection();

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

     //
     //  test mint coins
     //
     auto balance = client->get_balance(index);

     cout << "Mint 2 coins to account 0 " << endl;

     client->mint_coins(0, 2, true);

     assert(balance + 2 == client->get_balance(index));
     cout << "succeeded to test minting coins " << endl;

     //
     //  print account's information before transferring coins
     //
     cout << "Address " << index
          << "'s balance is " << client->get_balance(index)
          << ", sequence number is " << client->get_sequence_number(index) << endl;

     cout << "Address " << index + 1
          << "'s balance is " << client->get_balance(index + 1)
          << ", sequence number is " << client->get_sequence_number(index + 1) << endl;

     //
     //  transfer coins 1 cion(1000000 micro coin) from account 0 to account 1
     //
     client->transfer_coins_int(0, accounts[1].address, 1 * micro_libra_coin, 0, 0, true);
     cout << "Transferred one coin from account 0 to account 1 ..." << endl;

     //
     //  print account's information before transferring coins
     //
     cout << "Address " << index
          << "'s balance is " << client->get_balance(index)
          << ", sequence number is " << client->get_sequence_number(index) << endl;

     cout << "Address " << index + 1
          << "'s balance is " << client->get_balance(index + 1)
          << ", sequence number is " << client->get_sequence_number(index + 1) << endl;

     cout << "finished all test jobs !" << endl;

     return true;
}

bool test_violas_client()
{
     auto host = "18.220.66.235";
     uint16_t port = 40001;

     cout << "running Libra test ..." << endl;

     auto client = Violas::client::create(host,
                                          port,
                                          "violas_consensus_peers.config.toml",
                                          "temp_faucet_keys",
                                          false,
                                          "faucet.testnet.libra.org",
                                          "mnemonic");

     // auto client = Violas::client::create("localhost",
     //                                      44487,
     //                                      "/tmp/02c904ab576c83d61463984208f155c6/0/consensus_peers.config.toml",
     //                                      "/tmp/9481f8402299ade9b18bed1fa39d21a2/temp_faucet_keys",
     //                                      false,
     //                                      "faucet.testnet.libra.org",
     //                                      "mnemonic");
     client->test_validator_connection();

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

     //
     //  test mint coins
     //
     auto balance = client->get_balance(index);

     cout << "Mint 2 coins to account 0 " << endl;

     client->mint_coins(0, 2, true);

     assert(balance + 2 == client->get_balance(index));
     cout << "succeeded to test minting coins " << endl;

     //
     //  print account's information before transferring coins
     //
     cout << "Address " << index
          << "'s balance is " << client->get_balance(index)
          << ", sequence number is " << client->get_sequence_number(index) << endl;

     cout << "Address " << index + 1
          << "'s balance is " << client->get_balance(index + 1)
          << ", sequence number is " << client->get_sequence_number(index + 1) << endl;

     //
     //  transfer coins 1 cion(1000000 micro coin) from account 0 to account 1
     //
     client->transfer_coins_int(0, accounts[1].address, 1 * micro_libra_coin, 0, 0, true);
     cout << "Transferred one coin from account 0 to account 1 ..." << endl;

     //
     //  print account's information before transferring coins
     //
     cout << "Address " << index
          << "'s balance is " << client->get_balance(index)
          << ", sequence number is " << client->get_sequence_number(index) << endl;

     cout << "Address " << index + 1
          << "'s balance is " << client->get_balance(index + 1)
          << ", sequence number is " << client->get_sequence_number(index + 1) << endl;

     client->compile(0, "my_module.mvir", true);

     client->publish_module(0, "my_module.mv");
     cout << "Successfully published module" << endl;

     client->compile(0, "custom_script.mvir");
     client->execute_script(0, "custom_script.mv", vector<string>{"10"});

     cout << "finished all test jobs !" << endl;

     return true;
}