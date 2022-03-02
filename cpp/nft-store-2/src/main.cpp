#include <iostream>
#include <string>
// violas sdk header files
#include <utils.hpp>
#include <argument.hpp>
#include <console.hpp>
#include <json_rpc.hpp>
#include <violas_client2.hpp>
#include "nft_store.hpp"

using namespace std;
using namespace violas;

using handle = function<void(istringstream &params)>;
map<string, handle> create_commands(client2_ptr client, string url);

int main(int argc, char *argv[])
{
    cout << "NFT Store is getting started ..." << endl;

    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto rpc_cli = json_rpc::Client::create(args.url);

        auto client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);
        client->allow_custom_script(true);
        cout << "Allowed custom script." << endl;

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

    // return map<string, handle>{
    //     {"deploy", [=](istringstream &params)
    //      {
    //          client->allow_publishing_module(true);

    //          // 1.  deploy nft store
    //          client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/Compare.mv");
    //          client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/Map.mv");
    //          client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/NonFungibleToken.mv");
    //          client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/NftStore.mv");
    //          client->publish_module(ACCOUNT_ROOT_ID, "move/tea/modules/MountWuyi.mv");
    //      }},
    //     {"initalize", [=](istringstream &params)
    //      {
    //          store->initialize();
    //      }},
    //     {"register", [=](istringstream &params)
    //      {
    //          store->register_nft();
    //      }},
    //     {"list-orders", [=](istringstream &params)
    //      {
    //          cout << store->list_orders();
    //      }},
    //     {"make-order", [=](istringstream &params)
    //      {
    //          size_t account_index;
    //          nft::Id nft_token_id;
    //          uint64_t price;
    //          string currency;
    //          double incentive;

    //          params >> account_index >> nft_token_id >> price >> currency >> incentive;

    //          store->make_order(account_index, nft_token_id, price, currency, incentive);
    //      }},
    //     {"revoke-order", [=](istringstream &params)
    //      {
    //          size_t account_index;
    //          nft::Id order_id;

    //          params >> account_index >> order_id;

    //          store->revoke_order(account_index, order_id);
    //      }},
    //     {"sign-trading-order", [=](istringstream &params)
    //      {
    //          size_t account_index;
    //          nft::Id order_id;
    //          nft::Address address;
    //          string currency;

    //          params >> account_index >> currency >> address >> order_id;

    //          auto signed_txn = store->sign_trading_order(account_index,
    //                                                      currency,
    //                                                      dt::AccountAddress{address},
    //                                                      order_id);

    //          store->submit_trading_order(2, move(signed_txn));
    //      }},
    //     {"submit-trading-order", [=](istringstream &params)
    //      {
    //          size_t account_index;
    //          nft::Id order_id;
    //          dt::SignedTransaction signed_txn;

    //          params >> account_index >> order_id;

    //          store->submit_trading_order(account_index, move(signed_txn));
    //      }},
    //     {"get-account-info", [=](istringstream &params)
    //      {
    //          nft::Address address;

    //          auto accounts = client->get_all_accounts();

    //          params >> address;

    //          auto account_info = store->get_account_info(address);
    //      }},
    //     {"query-events", [=](istringstream &params)
    //      {
    //          string event_type;
    //          nft::Address address;

    //          auto accounts = client->get_all_accounts();

    //          params >> event_type >> address;

    //          if (event_type == "made")
    //          {
    //              auto made_order_events = store->get_made_order_events(address, 0, 10);
    //              for (auto &event : made_order_events)
    //              {
    //                  cout << "order id : " << (event.order_id)
    //                       << ", nft id : " << bytes_to_hex(event.nft_token_id)
    //                       << ", price : " << event.price
    //                       << ", currency code : " << bytes_to_string(event.currency_code)
    //                       << ", sale_incentive : " << event.sale_incentive
    //                       << endl;
    //              }
    //          }
    //          else if (event_type == "revoked")
    //          {
    //          }
    //      }},
    // };
    return {};
}
