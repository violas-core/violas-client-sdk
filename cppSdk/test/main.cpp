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

        assert(test_violas_client() == true);

        //assert(test_vstake() == true);

        cout << "\nFinished all test jobs !" << endl;
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

    auto client = Libra::client::create("ac.testnet.libra.org",
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

    // auto host = "18.220.66.235";
    // uint16_t port = 40001;

    // auto client = Violas::client::create(host,
    //                                      port,
    //                                      "violas_consensus_peers.config.toml",
    //                                      "temp_faucet_keys",
    //                                      false,
    //                                      "faucet.testnet.libra.org",
    //                                      "mnemonic");

    auto client = Libra::client::create("localhost",
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

    auto get_violas_balance = [client](uint64_t account_index, const uint256 &account_reource_path) -> uint64_t {
        return client->get_account_resource_uint64(account_index, account_reource_path);
    };

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
        LOG << "Account # " << sso1 << "'s balance of VStake " << governor << " : " << get_violas_balance(sso1, accounts[governor].address) << endl;
        LOG << "Account # " << sso2 << "'s balance of VStake " << governor << " : " << get_violas_balance(sso2, accounts[governor].address) << endl;

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
            << "VStake-1 : " << balance_to_string(get_violas_balance(account.index, accounts[1].address)) << ", "
            << "VStake-2 : " << balance_to_string(get_violas_balance(account.index, accounts[2].address)) << endl;
    }

    return true;
}

#if __cplusplus >= 201703L
bool test_vstake()
{
    cout << "running test vstake ...\n"
         << endl;

    using namespace Violas;

    // auto host = "18.220.66.235";
    // uint16_t port = 40001;

    // auto client = Violas::client::create(host,
    //                                      port,
    //                                      "violas_consensus_peers.config.toml",
    //                                      "temp_faucet_keys",
    //                                      false,
    //                                      "faucet.testnet.libra.org",
    //                                      "mnemonic");

    auto client = Libra::client::create("localhost",
                                        39745,
                                        "/tmp/65e58a1ef0eb427d25e843df76570757/0/consensus_peers.config.toml",
                                        "/tmp/1655bc456184141676176251ddb5e5dd/temp_faucet_keys",
                                        false,
                                        "faucet.testnet.libra.org",
                                        "mnemonic");

    client->test_validator_connection();

    const uint64_t account_amount = 4;
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

        LOG << "\n\tIndex : " << account.index
            << "\n\tAddress : " << account.address
            << "\n\tSequence : " << account.sequence_number
            << "\n\tStatus : " << account.status
            << "\n\tVToken Balance : " << balance
            << endl;
    }

    auto vstake1 = VStake::create(client, accounts[1].address, "V1");
    auto vstake2 = VStake::create(client, accounts[2].address, "V2");

    vstake1->deploy(1);

    vstake1->publish(1);
    vstake1->publish(2);
    vstake1->publish(3);

    vstake2->deploy(2);

    vstake2->publish(1);
    vstake2->publish(2);
    vstake2->publish(3);

    vstake2->mint(2, accounts[1].address, 1000); // * MICRO_LIBRO_COIN
    vstake2->transfer(1, accounts[2].address, 500);
    vstake2->transfer(1, accounts[3].address, 500);

    auto micro_to_double = [](uint64_t amount) -> double {
        return (double)amount / MICRO_LIBRO_COIN;
    };
    clog.precision(10);
    //
    //  mint
    //
    LOG << "account 2's balance is " << vstake1->get_account_balance(2) << endl;
    vstake1->mint(1, accounts[2].address, 100 * MICRO_LIBRO_COIN);
    LOG << "account 2's balance is " << vstake1->get_account_balance(2) << ", " << micro_to_double(vstake1->get_account_balance(2)) << endl;
    //
    //  transfer
    //
    LOG << "account 3's balance is " << vstake1->get_account_balance(3) << ", " << micro_to_double(vstake1->get_account_balance(3)) << endl;
    vstake1->transfer(2, accounts[3].address, 50 * MICRO_LIBRO_COIN);

    LOG << "account 2's balance is "
        << "V1 : " << vstake1->get_account_balance(2) << "(" << micro_to_double(vstake1->get_account_balance(2)) << "), "
        << "V2 : " << vstake2->get_account_balance(2) << "(" << micro_to_double(vstake2->get_account_balance(2)) << ")"
        << endl;
    LOG << "account 3's balance is "
        << "V1 : " << vstake1->get_account_balance(3) << "(" << micro_to_double(vstake1->get_account_balance(3)) << "), "
        << "V2 : " << vstake2->get_account_balance(3) << "(" << micro_to_double(vstake2->get_account_balance(3)) << "), "
        << endl;
    //
    //  get transaction detail
    //
    auto [txn, events] = client->get_committed_txn_by_acc_seq(2, client->get_sequence_number(2) - 1);
    // LOG << "Committed Transaction : \n"
    //     << txn << endl
    //     << "Events:\n"
    //     << events << endl;

    return true;
}
#else
bool test_vstake()
{
    return true;
}
#endif