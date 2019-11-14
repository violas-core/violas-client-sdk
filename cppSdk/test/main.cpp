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
     // ofstream file("log.txt");
     // streambuf *log = clog.rdbuf(file.rdbuf());

     try
     {
          //assert(test_libra_client() == true);

          assert(test_violas_client() == true);
     }
     catch (const std::exception &e)
     {
          ERROR << e.what() << endl;
     }

     // clog.rdbuf(log);

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
     cout << "running Libra test ..." << endl;

     // auto host = "18.220.66.235";
     // uint16_t port = 40001;
     const uint64_t account_amout = 5;

     // auto client = Violas::client::create(host,
     //                                      port,
     //                                      "violas_consensus_peers.config.toml",
     //                                      "temp_faucet_keys",
     //                                      false,
     //                                      "faucet.testnet.libra.org",
     //                                      "mnemonic");

     auto client = Violas::client::create("localhost",
                                          44487,
                                          "/tmp/02c904ab576c83d61463984208f155c6/0/consensus_peers.config.toml",
                                          "/tmp/9481f8402299ade9b18bed1fa39d21a2/temp_faucet_keys",
                                          false,
                                          "faucet.testnet.libra.org",
                                          "mnemonic");
     client->test_validator_connection();

     for (int i = 0; i < account_amout; i++)
     {
          auto account = client->create_next_account(true);

          LOG << "Created account #" << account.first << " address " << account.second << endl;
     }

     auto accounts = client->get_all_accounts();

     for (auto const &account : accounts)
     {
          LOG << "Index : " << account.index << "\t"
              << "Address : " << account.address << "\t"
              << "Sequence : " << account.sequence_number << "\t"
              << "Status : " << account.status << endl;
     }

     uint64_t index = 0;

     //
     //  test mint coins
     //
     auto balance = client->get_balance(index);

     LOG << "Mint 2 coins to account 0 " << endl;

     client->mint_coins(0, 100, true);

     assert(balance + 100 == client->get_balance(index));
     LOG << "succeeded to test minting coins " << endl;

     //
     //  print account's information before transferring coins
     //
     LOG << "Address " << index
         << "'s balance is " << client->get_balance(index)
         << ", sequence number is " << client->get_sequence_number(index) << endl;

     LOG << "Address " << index + 1
         << "'s balance is " << client->get_balance(index + 1)
         << ", sequence number is " << client->get_sequence_number(index + 1) << endl;

     //
     //  transfer coins 1 cion(1000000 micro coin) from account 0 to account 1
     //
     for (int i = 1; i < account_amout; i++)
     {
          client->transfer_coins_int(0, accounts[i].address, 10 * micro_libra_coin, 0, 0, true);
          cout << "Transferred one coin from account 0 to account 1 ..." << endl;
     }
     //
     //  print account's information before transferring coins
     //
     LOG << "Address " << index
         << "'s balance is " << client->get_balance(index)
         << ", sequence number is " << client->get_sequence_number(index) << endl;

     LOG << "Address " << index + 1
         << "'s balance is " << client->get_balance(index + 1)
         << ", sequence number is " << client->get_sequence_number(index + 1) << endl;

     //
     //   module token is published by account 2
     //
     string module = "scripts/token";
     client->compile(2, module + ".mvir", true); //my_module.mvir
     client->publish_module(2, module + ".mv");

     //
     //   script publish is executed by account 2 and account 4
     //
     string script = "scripts/publish";
     client->compile(2, script + ".mvir");

     client->execute_script(2, script + ".mv", vector<string>{});

     client->execute_script(4, script + ".mv", vector<string>{});

     //
     //   get transaction and event with sequence 0 for account 4
     //
     client->get_committed_txn_by_acc_seq(4, 0);

     //
     //   mint  and transfer stable coin DToken from account 2 to account 4
     //
     script = "scripts/mint";
     client->compile(2, script + ".mvir");
     client->execute_script(2, script + ".mv", vector<string>{uint256_to_string(accounts[4].address), "10"});
     client->get_committed_txn_by_acc_seq(2, client->get_sequence_number(4) - 1);

     LOG << "finished all test jobs !" << endl;

     return true;
}