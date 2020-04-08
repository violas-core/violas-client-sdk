#include <iostream>
#include <fstream>
#include <violas_sdk.hpp>
#include <cassert>

using namespace std;

void run_test_case(
    const string &host,
    uint16_t port,
    const string &mnemonic_file,
    const string &mint_key_file,
    const string &script_files_path);

void run_test_token(const string &host,
                    uint16_t port,
                    const string &mnemonic_file,
                    const string &mint_key_file);

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

        // run_test_case(argv[1], stol(argv[2]), argv[3], argv[4],
        //               argc <= 5 ? "../scripts" : argv[5]);
        run_test_token(argv[1], stol(argv[2]), argv[3], argv[4]);
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

    for_each(begin(accounts) + 1, end(accounts), [=](const auto &account) {
        client->execute_script(account.index, "publish.mv", vector<string>({"b\"414243\""}));
        print_txn(account_index);
    });

    replace_mv_with_addr("../../cppSdk/scripts/mint.mv",
                         "mint.mv",
                         accounts[account_index].address);
    client->execute_script(1, "mint.mv",
                           vector<string>({"0", uint256_to_string(accounts[2].address), "10", "b\"00\""}));
    print_txn(account_index);

    auto balance = client->get_account_resource_uint64(2, accounts[0].address, 0);
    cout << "the balance of token1 of account 2 is " << balance << endl;

    replace_mv_with_addr("../../cppSdk/scripts/transfer.mv",
                         "transfer.mv",
                         accounts[account_index].address);
    client->execute_script(2,
                           "transfer.mv",
                           vector<string>({"0", uint256_to_string(accounts[3].address), "1", "b\"00\""}));

    balance = client->get_account_resource_uint64(3, accounts[0].address, 0);
    cout << "the balance of token1 of account 3 is " << balance << endl;

    cout << "get txn by range ..." << endl;
    auto txn_events = client->get_txn_by_range(100, 10, true);
    for (const auto &[t, e] : txn_events)
    {
        // cout << "txn = " << t << endl
        //      << "event = " << e << endl;
    }
}

//
//  测试流程
//
// 1. 所有人物角色分别获取平台币
// 2. S将合约publish module发布到链上
// 3. S调用publish进行合约注册
// 4. S调用create_token发行币种Ta，Tb，并分别指定owner Oa，Ob
// 5. Oa，Ob，U1，U2 分别调用publish进行合约注册
// 6. Oa调用mint给U1铸Ta币种的100块钱
// 7. Ob调用mint给U2铸Tb币种的100块钱
// 8. U1调用transfer给U2转账Ta币种的50块钱
// 9. U2调用transfer给U1转账Tb币种的50块钱
// 10. 现在U1有Ta 50块，Tb 50块，U2也是一样
// 11. U1调用make_order币币交易挂单卖出Ta 10块，买入Tb 20块, 调用参数为make_order(Ta, 10, Tb, 20, data)
// 12. U2调用take_order去取U1挂的单， 成功后：U1有Ta 40块，Tb 70块， U2有Ta 60块，Tb 30块。
//
void run_test_token(const string &host,
                    uint16_t port,
                    const string &mnemonic_file,
                    const string &mint_key_file)
{
    using namespace Violas;

    auto client = Client::create(host, port, "", mint_key_file, true, "", mnemonic_file);

    client->test_validator_connection();
    cout << "succeed to test validator connection ." << endl;

    auto s = client->create_next_account(true);
    auto o1 = client->create_next_account(true);
    auto o2 = client->create_next_account(true);
    auto u1 = client->create_next_account(true);
    auto u2 = client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        client->mint_coins(account.index, 1);

        cout << "Account index : " << account.index
             << ", address : " << account.address
             << ", balane : " << client->get_balance(account.index)
             << endl;
    }

    auto print_txn = [client](uint64_t account_index) {
        auto seq_num = client->get_sequence_number(account_index) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(account_index, seq_num);
        cout << "txn = " << txn << endl;
    };

    uint64_t supervisor = 0,
             owner1 = 1,
             owner2 = 2,
             user1 = 3,
             user2 = 4;

    string script_files_path = "../../cppSdk/scripts";
    auto token = Token::create(client, accounts[supervisor].address, "token1", script_files_path);

    token->deploy(supervisor);
    print_txn(0);
    cout << "account 1 deployed token successfully ." << endl;

    token->publish(supervisor);
    print_txn(0);
    cout << "supervisor deployed the Violas Token Module successfuly." << endl;

    token->create_token(supervisor, accounts[owner1].address, "Token A");
    print_txn(0);
    cout << "create token A successfully." << endl;

    token->create_token(supervisor, accounts[owner2].address, "Token B");
    print_txn(0);
    cout << "create token B successfully." << endl;

    for (size_t i = 1; i < accounts.size(); i++)
    {
        token->publish((uint64_t)i);
        print_txn(0);
    }
    //
    // 6. Oa调用mint给U1铸Ta币种的100块钱
    //
    token->mint(owner1, 0, accounts[user1].address, 100);
    print_txn(owner1);
    cout << "" << endl;

    auto balance = token->get_account_balance(user1, 0);
    cout << "the balance of token A of user 1 is " << balance << endl;
    assert(balance = 100);
    //
    // 7. Ob调用mint给U2铸Tb币种的100块钱
    //
    token->mint(owner2, 1, accounts[user2].address, 100);
    print_txn(owner2);
    cout << "Owner2 mint 100 coins to user2" << endl;

    balance = token->get_account_balance(user2, 1);
    cout << "the balance of token B of user 1 is " << balance << endl;
    assert(balance = 100);

    token->transfer(user1, 0, accounts[user2].address, 50);
    print_txn(owner1);
    balance = token->get_account_balance(user2, 0);
    assert(balance == 50);

    token->transfer(user2, 1, accounts[user1].address, 50);
    print_txn(owner2);
    balance = token->get_account_balance(user1, 1);
    assert(balance == 50);
}