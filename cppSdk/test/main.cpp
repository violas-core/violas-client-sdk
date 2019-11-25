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
bool test_vstake();

int main(int argc, const char *argv[])
{
     // ofstream file("log.txt");
     // streambuf *log = clog.rdbuf(file.rdbuf());

     try
     {
          //assert(test_libra_client() == true);

          //assert(test_violas_client() == true);

          assert(test_vstake() == true);
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
     client->transfer_coins_int(0, accounts[1].address, 1 * MICRO_LIBRO_COIN, 0, 0, true);
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

     auto host = "18.220.66.235";
     uint16_t port = 40001;

     auto client = Violas::client::create(host,
                                          port,
                                          "violas_consensus_peers.config.toml",
                                          "temp_faucet_keys",
                                          false,
                                          "faucet.testnet.libra.org",
                                          "mnemonic");

     // auto client = Violas::client::create("localhost",
     //                                      34193,
     //                                      "/tmp/4a3e24e555ba466f2d04299ebd26581f/0/consensus_peers.config.toml",
     //                                      "/tmp/35771165f7de9f14e9419fceadde4d49/temp_faucet_keys",
     //                                      false,
     //                                      "faucet.testnet.libra.org",
     //                                      "mnemonic");

     client->test_validator_connection();

     const uint64_t account_amount = 10;
     for (uint64_t i = 0; i < account_amount; i++)
     {
          client->create_next_account(true);
     }

     auto accounts = client->get_all_accounts();

     for (auto const &account : accounts)
     {
          LOG << "\n\tIndex : " << account.index
              << "\n\tAddress : " << account.address
              << "\n\tSequence : " << account.sequence_number
              << "\n\tStatus : " << account.status
              << "\n\tVToken Balance : " << client->get_balance(account.index)
              << endl;
     }

     uint64_t chairman = 0;

     //
     //  Account #0 as Governor mint and transer 100 VToken
     //
     auto balance = client->get_balance(chairman);

     client->mint_coins(0, 200, true);
     assert(balance + 200 == client->get_balance(chairman));

     LOG << "\n\n董事长转帐100个VToken给每个州长" << endl;

     client->transfer_coins_int(chairman, accounts[1].address, 100 * MICRO_LIBRO_COIN);
     LOG << "Address 1's balance is " << client->get_balance(1) << endl;

     client->transfer_coins_int(chairman, accounts[2].address, 100 * MICRO_LIBRO_COIN);
     LOG << "Address 2's balance is " << client->get_balance(2) << endl;

     //                                         州长索引   SSO用户1        SSO用户2        minted稳定币数量   用户1        用户2         transferrd稳定币数量
     auto test_mint_stable_coion = [&](uint64_t governor, uint64_t sso1, uint64_t sso2, uint64_t vstake, uint64_t u1, uint64_t u2, uint64_t transerred_vstake) {
          LOG << "\n\n"
              << format("州长(%d)为SSO(%d)和SSO(%d)铸造(%d)个稳定币(%d), 每个SSO转帐给用户(%d)和(%d)稳定币(%d)个", governor, sso1, sso2, vstake, governor, u1, u2, transerred_vstake)
              << endl;
          //
          //   all scripts
          //
          string module = "../scripts/token";
          string publish_script = "../scripts/publish";
          string mint_script = "../scripts/mint";
          string transfer_script = "../scripts/transfer";
          //
          //   compiles all scripts for governor
          //
          client->compile(governor, module + ".mvir", true);
          // publish the module to validator node
          client->publish_module(governor, module + ".mv");

          client->compile(governor, publish_script + ".mvir");
          client->compile(governor, mint_script + ".mvir");
          client->compile(governor, transfer_script + ".mvir");

          //
          // Governor transfers VStake to SSO
          //
          // LOG << "州长转帐10个VToken给每个SSO发币商" << endl;

          client->transfer_coins_int(governor, accounts[sso1].address, 10 * MICRO_LIBRO_COIN);
          LOG << "Governor (" << governor << ") transer 10 VToken(Libra) to user #" << sso1 << endl;
          client->transfer_coins_int(governor, accounts[sso2].address, 10 * MICRO_LIBRO_COIN);
          LOG << "Governor (" << governor << ") transer 10 VToken(Libra) to user #" << sso2 << endl;

          //
          //   executing publish script for registering stable 1
          //
          client->execute_script(governor, publish_script + ".mv", vector<string>{});
          client->execute_script(sso1, publish_script + ".mv", vector<string>{});
          client->execute_script(sso2, publish_script + ".mv", vector<string>{});

          //
          // Governor mints VStake and transfer cions to SSO user #3 and #4
          //
          client->execute_script(governor, mint_script + ".mv", vector<string>{uint256_to_string(accounts[sso1].address), to_string(vstake)});
          client->execute_script(governor, mint_script + ".mv", vector<string>{uint256_to_string(accounts[sso2].address), to_string(vstake)});
          LOG << "Account # " << sso1 << "'s balance of VStake " << governor << " : " << client->get_violas_balance(sso1, accounts[governor].address) << endl;
          LOG << "Account # " << sso2 << "'s balance of VStake " << governor << " : " << client->get_violas_balance(sso2, accounts[governor].address) << endl;

          //client->get_committed_txn_by_acc_seq(2, client->get_sequence_number(2) - 1);

          //
          //   SSO transfer VToken to user
          //
          // LOG << "SSO转帐1个VToken给每个用户" << endl;
          client->transfer_coins_int(sso1, accounts[u1].address, 1 * MICRO_LIBRO_COIN);
          client->transfer_coins_int(sso2, accounts[u2].address, 1 * MICRO_LIBRO_COIN);
          //
          //   user registers VStake
          //
          client->execute_script(u1, publish_script + ".mv", vector<string>{});
          client->execute_script(u2, publish_script + ".mv", vector<string>{});
          //
          //   transfer VStake coin from SSO to user
          //
          client->execute_script(sso1, transfer_script + ".mv", vector<string>{uint256_to_string(accounts[u1].address), to_string(transerred_vstake)});
          client->execute_script(sso1, transfer_script + ".mv", vector<string>{uint256_to_string(accounts[u2].address), to_string(transerred_vstake)});
          client->execute_script(sso2, transfer_script + ".mv", vector<string>{uint256_to_string(accounts[u1].address), to_string(transerred_vstake)});
          client->execute_script(sso2, transfer_script + ".mv", vector<string>{uint256_to_string(accounts[u2].address), to_string(transerred_vstake)});
     };

     //"州长(1) 为SSO用户(3)和SSO用户(4) 铸稳定币(1) 1000个, 转帐给用户5和6稳定币10个"
     test_mint_stable_coion(1, 3, 4, 1000, 5, 6, 100);

     //"州长(2) 为SSO用户(3) 和SSO用户(4)铸稳定币(1) 2000个， 转帐给用户5和6稳定币10个"
     test_mint_stable_coion(2, 3, 4, 2000, 5, 6, 200);

     LOG << "\n\n"
         << "All balances of all accounts"
         << endl;

     auto balance_to_string = [](uint64_t value) -> string {
          if (is_valid_balance(value))
               return to_string(value);
          else
               return "N/A";
     };

     for (auto &account : accounts)
     {
          LOG << "Account " << account.index << "'s balances ------ "
              << "VToken : " << client->get_balance(account.index) << ", "
              << "VStake-1 : " << balance_to_string(client->get_violas_balance(account.index, accounts[1].address)) << ", "
              << "VStake-2 : " << balance_to_string(client->get_violas_balance(account.index, accounts[2].address)) << endl;
     }

     cout << "\n\n"
          << "finished all test jobs !" << endl;

     return true;
}

bool test_vstake()
{
     using namespace Violas;

     cout << "running Libra test ..." << endl;

     auto host = "18.220.66.235";
     uint16_t port = 40001;

     auto client = Violas::client::create(host,
                                          port,
                                          "violas_consensus_peers.config.toml",
                                          "temp_faucet_keys",
                                          false,
                                          "faucet.testnet.libra.org",
                                          "mnemonic");

     // auto client = Violas::client::create("localhost",
     //                                      34193,
     //                                      "/tmp/4a3e24e555ba466f2d04299ebd26581f/0/consensus_peers.config.toml",
     //                                      "/tmp/35771165f7de9f14e9419fceadde4d49/temp_faucet_keys",
     //                                      false,
     //                                      "faucet.testnet.libra.org",
     //                                      "mnemonic");

     client->test_validator_connection();

     const uint64_t account_amount = 4;
     for (uint64_t i = 0; i < account_amount; i++)
     {
          client->create_next_account(true);
     }

     auto accounts = client->get_all_accounts();

     for (auto const &account : accounts)
     {
          LOG << "\n\tIndex : " << account.index
              << "\n\tAddress : " << account.address
              << "\n\tSequence : " << account.sequence_number
              << "\n\tStatus : " << account.status
              << "\n\tVToken Balance : " << client->get_balance(account.index)
              << endl;
     }

     //uint64_t chairman = 0;

     auto v1 = VStake::create(client, accounts[1].address, "V1");

     return true;
}