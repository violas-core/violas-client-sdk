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

    client->create_next_account(true);
    client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        cout << "Account index : " << account.index
             << ", address : " << account.address
             << endl;
    }

    client->mint_coins(0, 10);
    client->mint_coins(1, 10);
    cout << "account 0' balance is " << client->get_balance(0) << endl
         << "account 1' balance is " << client->get_balance(1) << endl;

    cout << "Transfer 1 libra coin from account 0 to account 1 ..." << endl;
    client->transfer_coins_int(0, accounts[1].address, 1 * MICRO_LIBRO_COIN);
    cout << "account 0' balance is " << client->get_balance(0) << endl
         << "account 1' balance is " << client->get_balance(1) << endl;

    transform_mv_to_json("/home/hunter/libra/target/token.mv", "token.json");
    client->publish_module(0, "token.json");

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

    auto [txn, event] = client->get_committed_txn_by_acc_seq(0, client->get_sequence_number(0) - 1);
    cout << "txn = " << txn << endl
         << "event = " << event << endl;

    cout << "get txn by range ..." << endl;
    auto txn_events = client->get_txn_by_range(100, 10, true);
    for (const auto &[t, e] : txn_events)
    {
        // cout << "txn = " << t << endl
        //      << "event = " << e << endl;
    }
}