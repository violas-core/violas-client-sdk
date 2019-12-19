//
//
// compiling : clang++ -std=c++2a -stdlib=libc++ -g main.cpp -o test

#include <iostream>
#include <fstream>
#include <assert.h>
#include "violas_sdk.hpp"

using namespace std;

bool test_libra_client();
bool test_violas_client();
bool test_violas_token();

int main(int argc, const char *argv[])
{
    //ofstream file("log.txt");
    //streambuf *mylog = clog.rdbuf(file.rdbuf());

    try
    {
        //assert(test_libra_client() == true);

        // assert(test_violas_client() == true);

        assert(test_violas_token() == true);

        cout << "\nFinished all test jobs !" << endl;
    }
    catch (const std::exception &e)
    {
        CERR << e.what() << endl;
    }

    //clog.rdbuf(mylog);

    return 0;
}

bool test_libra_client()
{
    using namespace Violas;
    cout << "running Libra test ..." << endl;

    auto client = Client::create("ac.testnet.libra.org",
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
    using namespace Violas;
    cout << "running Libra test ..." << endl;

    // auto host = "18.220.66.235";
    // uint16_t port = 40001;

    // auto client = Violas::client::create(host,
    //                                      port,
    //                                      "violas_consensus_peers.config.toml",
    //                                      "temp_faucet_keys",
    //                                      false,
    //                                      "faucet.testnet.libra.org",
    //                                      "mnemonic");

    auto client = Client::create("localhost",
                                 39745,
                                 "/tmp/65e58a1ef0eb427d25e843df76570757/0/consensus_peers.config.toml",
                                 "/tmp/1655bc456184141676176251ddb5e5dd/temp_faucet_keys",
                                 false,
                                 "faucet.testnet.libra.org",
                                 "mnemonic");

    client->test_validator_connection();

    const uint64_t account_amount = 10;
    for (uint64_t i = 0; i < account_amount; i++)
    {
        client->create_next_account(true);
    }

    auto accounts = client->get_all_accounts();

    for (auto const &account : accounts)
    {
        COUT << "\n\tIndex : " << account.index
            << "\n\tAddress : " << account.address
            << "\n\tSequence : " << account.sequence_number
            << "\n\tStatus : " << account.status
            << "\n\tToken Balance : " << client->get_balance(account.index)
            << endl;
    }

    uint64_t chairman = 0;

    //
    //  Account #0 as Governor mint and transer 100 Token
    //
    auto balance = client->get_balance(chairman);

    client->mint_coins(0, 200, true);
    assert(balance + 200 == client->get_balance(chairman));

    COUT << "\n\n董事长转帐100个Token给每个州长" << endl;

    client->transfer_coins_int(chairman, accounts[1].address, 100 * MICRO_LIBRO_COIN);
    COUT << "Address 1's balance is " << client->get_balance(1) << endl;

    client->transfer_coins_int(chairman, accounts[2].address, 100 * MICRO_LIBRO_COIN);
    COUT << "Address 2's balance is " << client->get_balance(2) << endl;

    auto get_violas_balance = [client](uint64_t account_index, const uint256 &account_reource_path) -> uint64_t {
        return client->get_account_resource_uint64(account_index, account_reource_path);
    };

    //                                         州长索引   SSO用户1        SSO用户2        minted稳定币数量   用户1        用户2         transferrd稳定币数量
    auto test_mint_stable_coion = [&](uint64_t governor, uint64_t sso1, uint64_t sso2, uint64_t vstake, uint64_t u1, uint64_t u2, uint64_t transerred_vstake) {
        COUT << "\n\n"
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
        // COUT << "州长转帐10个Token给每个SSO发币商" << endl;

        client->transfer_coins_int(governor, accounts[sso1].address, 10 * MICRO_LIBRO_COIN);
        COUT << "Governor (" << governor << ") transer 10 Token(Libra) to user #" << sso1 << endl;
        client->transfer_coins_int(governor, accounts[sso2].address, 10 * MICRO_LIBRO_COIN);
        COUT << "Governor (" << governor << ") transer 10 Token(Libra) to user #" << sso2 << endl;

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
        COUT << "Account # " << sso1 << "'s balance of VStake " << governor << " : " << get_violas_balance(sso1, accounts[governor].address) << endl;
        COUT << "Account # " << sso2 << "'s balance of VStake " << governor << " : " << get_violas_balance(sso2, accounts[governor].address) << endl;

        //client->get_committed_txn_by_acc_seq(2, client->get_sequence_number(2) - 1);

        //
        //   SSO transfer Token to user
        //
        // COUT << "SSO转帐1个Token给每个用户" << endl;
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

    COUT << "\n\n"
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
        COUT << "Account " << account.index << "'s balances ------ "
            << "Token : " << client->get_balance(account.index) << ", "
            << "VStake-1 : " << balance_to_string(get_violas_balance(account.index, accounts[1].address)) << ", "
            << "VStake-2 : " << balance_to_string(get_violas_balance(account.index, accounts[2].address)) << endl;
    }

    return true;
}

bool test_violas_token()
{
    using namespace Violas;

    cout << "running test vstake ...\n"
         << endl;

    auto host = "52.151.2.66"; //"18.220.66.235";
    uint16_t port = 40001;

    auto client = Client::create(host,
                                 port,
                                 "consensus_peers.config.toml",
                                 "temp_faucet_keys",
                                 false,
                                 "", //"faucet.testnet.libra.org", //libra testnet use this url to get test libra coin
                                 "mnemonic");

    // auto client = Client::create("localhost",
    //                              46243,
    //                              "/tmp/b35b5e6bfc503b6385b37926bcf97086/0/consensus_peers.config.toml",
    //                              "/tmp/12269974404173c769d775ce64a3b1a1/temp_faucet_keys",
    //                              false,
    //                              "faucet.testnet.libra.org",
    //                              "mnemonic");

    client->test_validator_connection();

    const uint64_t account_amount = 10;
    for (uint64_t i = 0; i < account_amount; i++)
    {
        client->create_next_account(true);
    }

    client->mint_coins(0, 10);

    auto accounts = client->get_all_accounts();

    for (auto const &account : accounts)
    {
        client->transfer_coins_int(0, account.address, 1 * MICRO_LIBRO_COIN);

        uint64_t balance = client->get_balance(account.index);
        // assert(balance > 0);

        cout << "\n\tIndex : " << account.index
             << "\n\tAddress : " << account.address
             << "\n\tSequence : " << account.sequence_number
             << "\n\tStatus : " << account.status
             << "\n\tToken Balance : " << balance
             << endl;
    }
    //
    //  所有角色的账户的索引
    //  董事长的账户 : 0
    //  州长1的账户  : 1
    //  州长2的账户  : 2
    //  SSO账户1    : 3
    //  SSO账户2    : 4
    //  董事长的VStake1 : 8
    //  董事长的VStake2 : 9
    //
    //董事长用账户8发行VStake1给州长1, 转帐1个VStake
    auto vstake1 = Violas::Token::create(client, accounts[8].address, "VStake1");
    vstake1->deploy(8);
    vstake1->publish(8);
    vstake1->publish(1);
    vstake1->mint(8, accounts[1].address, 1 * MICRO_LIBRO_COIN);
    cout << "州长1的 VStake 1 balnce " << vstake1->get_account_balance(1) << endl;

    //董事长用账户8发行VStake1给州长2, 转帐1个VStake
    auto vstake2 = Violas::Token::create(client, accounts[9].address, "VStake2");
    vstake2->deploy(9);
    vstake2->publish(9);
    vstake2->publish(2);
    vstake2->mint(9, accounts[2].address, 1 * MICRO_LIBRO_COIN);
    cout << "州长2的 VStake 2 balnce " << vstake2->get_account_balance(2) << endl;

    //6个州长发行6种稳定币
    vector<string> names = {"ABCUSD", "HIJUDS", "XYZUSD", "BCDCAN", "CDESDG", "DEFHKD"};
    vector<Violas::token_ptr> tokens;

    assert(accounts.size() >= names.size() + 1);

    for (size_t i = 0; i < names.size(); i++)
    {
        auto vstake = Violas::Token::create(client, accounts[i + 1].address, names.at(i));

        vstake->deploy(i + 1);

        vstake->publish(i + 1);

        tokens.push_back(vstake);
    }

    for (auto vstake : tokens)
    {
        cout << "VStake's name is " << vstake->name() << ", address is " << vstake->address() << endl;
    }

    auto &token1 = tokens[0];
    auto &token2 = tokens[1];

    token1->publish(1);
    token1->publish(2);
    token1->publish(3);

    token2->publish(1);
    token2->publish(2);
    token2->publish(3);

    token1->mint(1, accounts[2].address, 100 * MICRO_LIBRO_COIN);
    token1->transfer(2, accounts[3].address, 50 * MICRO_LIBRO_COIN);

    token2->mint(2, accounts[1].address, 1000); // * MICRO_LIBRO_COIN
    token2->transfer(1, accounts[2].address, 500);
    token2->transfer(1, accounts[3].address, 500);

    auto micro_to_double = [](uint64_t amount) -> double {
        if (is_valid_balance(amount))
            return (double)amount / MICRO_LIBRO_COIN;
        else
            return 0;
    };

    auto balance_to_string = [](uint64_t value) -> string {
        if (is_valid_balance(value))
            return to_string(value);
        else
            return "N/A";
    };

    clog.precision(10);

    for (auto &account : accounts)
    {
        auto index = account.index;
        auto balance1 = token1->get_account_balance(index);
        auto balance2 = token2->get_account_balance(index);

        cout << "account " << index << "'s balance is "
             << "T1 : " << balance_to_string(balance1) << "(" << micro_to_double(balance1) << "), "
             << "T2 : " << balance_to_string(balance2) << "(" << micro_to_double(balance2) << "), "
             << endl;
    }
    //
    //  get transaction detail
    //
#if __cplusplus >= 201703L
    auto [txn, events] = client->get_committed_txn_by_acc_seq(2, client->get_sequence_number(2) - 1);
#else
    auto txn_events = client->get_committed_txn_by_acc_seq(2, client->get_sequence_number(2) - 1);
    auto &txn = txn_events.first;
    auto &events = txn_events.second;
#endif

    cout << "Committed Transaction : \n"
         << txn << endl
         << "Events:\n"
         << events << endl;

#if __cplusplus >= 201703L
    auto all_txn_events = client->get_txn_by_range(100, 10, true);
    for (auto [txn, events] : all_txn_events)
    {
        //cout << txn << endl;
        //cout << events << endl;
    }
#endif

    return true;
}
