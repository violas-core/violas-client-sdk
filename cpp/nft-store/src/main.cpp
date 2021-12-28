#include <iostream>
#include <fstream>
#include <string>
#include <functional>

#include <utils.hpp>
#include <argument.hpp>
#include <console.hpp>
#include <json_rpc.hpp>
#include <ed25519.hpp>
#include "nft_store.hpp"
#include <violas_client2.hpp>
#include <wallet.hpp>

using namespace std;
using namespace violas;

void depoloy(client2_ptr client);
void test(const Arguments &args);

using handle = function<void(istringstream &params)>;
map<string, handle> create_commands(client2_ptr client, string url);

int main(int argc, char *argv[])
{
    cout << "NFT Store is getting started ..." << endl;

    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        // test(args);

        auto rpc_cli = json_rpc::Client::create(args.url);

        auto client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);
        client->allow_custom_script(true);

        auto console = Console::create("NFT$ ");
        const string exit = "exit";

        console->add_completion(exit);

        auto commands = create_commands(client, args.url);
        for (auto cmd : commands)
        {
            console->add_completion(cmd.first);
        }

        //
        //  Loop to read a line
        //
        for (auto line = trim(console->read_line());
             line != exit;
             line = trim(console->read_line()))
        {
            istringstream iss(line);
            string cmd;

            // Read a command
            iss >> cmd;

            auto iter = commands.find(cmd);
            if (iter != end(commands))
            {
                try
                {
                    iter->second(iss);
                }
                catch (const std::invalid_argument &e)
                {
                    std::cerr << "Invalid argument : " << e.what() << endl;
                }
                catch (runtime_error &e)
                {
                    std::cerr << color::RED
                              << "Runtime error : " << e.what()
                              << color::RESET << endl;
                }
            }

            console->add_history(line);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

map<string, handle> create_commands(client2_ptr client, string url)
{
    client->allow_custom_script(true);

    auto store = make_shared<nft::Store>(client,
                                         make_struct_type_tag(VIOLAS_LIB_ADDRESS, "MountWuyi", "Tea"));

    return map<string, handle>{
        {"deploy", [=](istringstream &params)
         {
             client->allow_publishing_module(true);

             // 1.  deploy nft store
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/Compare.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/Map.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/NonFungibleToken.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/NftStore.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/tea/modules/MountWuyi.mv");
         }},
        {"initalize", [=](istringstream &params)
         {
             store->initialize();
         }},
        {"register", [=](istringstream &params)
         {
             store->register_nft();
         }},
        {"list-orders", [=](istringstream &params)
         {
             cout << store->list_orders();
         }},
        {"make-order", [=](istringstream &params)
         {
             size_t account_index;
             nft::Id nft_token_id;
             uint64_t price;
             string currency;
             double incentive;

             params >> account_index >> nft_token_id >> price >> currency >> incentive;

             store->make_order(account_index, nft_token_id, price, currency, incentive);
         }},
        {"revoke-order", [=](istringstream &params)
         {
             size_t account_index;
             nft::Id order_id;

             params >> account_index >> order_id;

             store->revoke_order(account_index, order_id);
         }},
        {"sign-trading-order", [=](istringstream &params)
         {
             size_t account_index;
             nft::Id order_id;
             nft::Address address;

             params >> account_index >> address >> order_id;

             auto signed_txn = store->sign_trading_order(account_index, dt::AccountAddress{address}, order_id);
             store->submit_trading_order(2, move(signed_txn));
         }},
        {"submit-trading-order", [=](istringstream &params)
         {
             size_t account_index;
             nft::Id order_id;
             dt::SignedTransaction signed_txn;

             params >> account_index >> order_id;

             store->submit_trading_order(account_index, move(signed_txn));
         }},
        {"get-account-info", [=](istringstream &params)
         {
             nft::Address address;

             auto accounts = client->get_all_accounts();

             params >> address;

             auto account_info = store->get_account_info(address);
         }},
        {"query-events", [=](istringstream &params)
         {
             string event_type;
             nft::Address address;

             auto accounts = client->get_all_accounts();

             params >> event_type >> address;

             if (event_type == "made")
             {
                 auto made_order_events = store->get_made_order_events(address, 0, 10);
                 for (auto &event : made_order_events)
                 {
                     cout << "order id : " << (event.order_id)
                          << ", nft id : " << bytes_to_hex(event.nft_token_id)
                          << ", price : " << event.price
                          << ", currency code : " << bytes_to_string(event.currency_code)
                          << ", sale_incentive : " << event.sale_incentive
                          << endl;
                 }
             }
             else if (event_type == "revoked")
             {
             }
         }},
    };
}

void test(const Arguments &args)
{
    violas::Wallet::run_test_case();

    ed25519::run_test_case();

    client2_ptr client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);

    client->create_next_account();
    client->create_next_account();
    auto accounts = client->get_all_accounts();
    auto &account = accounts[0];

    cout << bytes_to_hex(accounts[0].address.value) << endl;

    client->allow_custom_script(true);

    try_catch([=]()
              {
        uint64_t sn = client->create_parent_vasp_account(account.address, account.auth_key, "test", false); 
        client->add_currency(0, "XUS"); });

    auto &child = accounts[1];

    client->create_child_vasp_account(0, child.address, child.auth_key, "VLS", 0, false);

    string currency_code = "EUR";

    client->regiester_stable_currency(currency_code, 1, 1, 1'000'000, 1'000'000);

    client->add_currency(ACCOUNT_DD_ID, currency_code);

    client->add_currency_for_designated_dealer(currency_code, TESTNET_DD_ADDRESS);

    client->mint(currency_code, 0, 1'000 * MICRO_COIN, TESTNET_DD_ADDRESS, 0);
}

void test_nft_store(const Arguments &args)
{
    client2_ptr client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);
    // nft::Store store(client);

    // store.initialize(make_struct_type_tag(STD_LIB_ADDRESS, "Tea", "Tea"));
}