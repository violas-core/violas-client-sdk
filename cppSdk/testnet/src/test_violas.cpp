#include <iostream>
#include <fstream>
#include <violas_sdk.hpp>

using namespace std;

void run_test_case(
    const string &host,
    uint16_t port,
    const string &mnemonic_file,
    const string &mint_key_file,
    const string &script_files_path);

int main(int argc, char *argv[])
{
    ofstream file("log.txt");
    streambuf *mylog = clog.rdbuf(file.rdbuf());

    try
    {
        if (argc <= 4)
        {
            cout << "usage : test_violas host port mnemonic_file mint_key_file script_files_path";
            return -1;
        }

        run_test_case(argv[1], stol(argv[2]), argv[3], argv[4],
                      argc <= 5 ? "../scripts" : argv[5]);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    clog.rdbuf(mylog);

    return 0;
}

void run_test_case(
    const string &host,
    uint16_t port,
    const string &mnemonic_file,
    const string &mint_key_file,
    const string &script_files_path)
{
    using namespace Violas;

    auto client = Client::create(host, port, "", mint_key_file, true, "", mnemonic_file);

    client->test_validator_connection();
    cout << "succeed to test validator connection ." << endl;

    auto s = client->create_next_account(true);
    auto o1 = client->create_next_account(true);
    auto u1 = client->create_next_account(true);
    auto u2 = client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        cout << "Account index : " << account.index
             << ", address : " << account.address
             << endl;
    }

    client->mint_coins(0, 10);
    client->mint_coins(1, 10);
    client->mint_coins(2, 10);
    client->mint_coins(3, 10);
    cout << "account 0' balance is " << client->get_balance(0) << endl
         << "account 1' balance is " << client->get_balance(1) << endl;

    cout << "Transfer 1 libra coin from account 0 to account 1 ..." << endl;
    client->transfer_coins_int(0, accounts[1].address, 1 * MICRO_LIBRO_COIN);
    cout << "account 0' balance is " << client->get_balance(0) << endl
         << "account 1' balance is " << client->get_balance(1) << endl;

    auto print_txn = [client](uint64_t account_index) {
        auto seq_num = client->get_sequence_number(account_index) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(account_index, seq_num);
        cout << "txn = " << txn << endl;
    };

    int account_index = 0;
    replace_mv_with_addr("../../cppSdk/scripts/token.mv",
                         "token.mv",
                         accounts[account_index].address);
    client->publish_module(account_index, "token.mv");
    print_txn(account_index);

    replace_mv_with_addr("../../cppSdk/scripts/publish.mv",
                         "publish.mv",
                         accounts[account_index].address);

    client->execute_script(0, "publish.mv", vector<string>({"b\"00\""}));
    print_txn(account_index);

    replace_mv_with_addr("../../cppSdk/scripts/create_token.mv",
                         "create_token.mv",
                         accounts[account_index].address);
    client->execute_script(account_index, "create_token.mv", vector<string>({uint256_to_string(accounts[1].address), "b\"00\""}));
    print_txn(account_index);

    for_each(begin(accounts)+1, end(accounts), [=](const auto &account) {
        client->execute_script(account.index, "publish.mv", vector<string>({"b\"414243\""}));
        print_txn(account_index);
    });

    replace_mv_with_addr("../../cppSdk/scripts/mint.mv",
                         "mint.mv",
                         accounts[account_index].address);
    client->execute_script(1, "mint.mv",
                           vector<string>({"0", uint256_to_string(accounts[2].address), "10", "b\"00\""}));
    print_txn(account_index);

    auto balance = client->get_account_resource_uint64(2, accounts[0].address);
    cout << "the balance of token1 of account 2 is " << balance << endl;

    replace_mv_with_addr("../../cppSdk/scripts/transfer.mv",
                         "transfer.mv",
                         accounts[account_index].address);
    client->execute_script(2, 
                            "transfer.mv", 
                            vector<string>({"0", uint256_to_string(accounts[3].address), "1", "b\"00\""}));

    balance = client->get_account_resource_uint64(3, accounts[0].address);
    cout << "the balance of token1 of account 3 is " << balance << endl;

    // auto token = Token::create(client, accounts[1].address, "token1", script_files_path);
    // token->deploy(1);
    // cout << "account 1 deployed token successfully ." << endl;

    // token->publish(1);
    // token->publish(0);
    // cout << "all accounts published the token ." << endl;

    // token->mint(1, accounts[1].address, 1000);
    // cout << "mint 1000 coin for account 1 ." << endl;

    // token->transfer(1, accounts[0].address, 800);
    // cout << "transfer 1000 coins from account 1 to account 0 ." << endl;

    // auto balacne = token->get_account_balance(accounts[1].address);
    // cout << "account 1's token balance is " << balacne << endl;

    // balacne = token->get_account_balance(accounts[0].address);
    // cout << "account 0's token balance is " << balacne << endl;

    cout << "get txn by range ..." << endl;
    auto txn_events = client->get_txn_by_range(100, 10, true);
    for (const auto &[t, e] : txn_events)
    {
        // cout << "txn = " << t << endl
        //      << "event = " << e << endl;
    }
}