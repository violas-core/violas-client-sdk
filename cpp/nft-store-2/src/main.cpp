#include <iostream>
#include <string>
// violas sdk header files
#include <utils.hpp>
#include <argument.hpp>
#include <console.hpp>
#include <json_rpc.hpp>
#include <violas_client2.hpp>
#include "nft_store.hpp"
#include "nft/nft.hpp"
#include "nft/portrait.hpp"

using namespace std;
using namespace violas;

using handle = function<void(istringstream &params)>;
map<string, handle> create_store_commands(client2_ptr client, string url);
map<string, handle> create_nft_commands(client2_ptr client, string url);

int main(int argc, char *argv[])
{
    cout << "NFT Store 2 is getting started ..." << endl;

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

        console->add_completion("exit");

        auto commands = create_store_commands(client, args.url);
        for (auto cmd : commands)
        {
            console->add_completion(cmd.first);
        }

        auto nft_cmds = create_nft_commands(client, args.url);
        for (auto cmd : nft_cmds)
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
            
            if(iter == end(commands))
                iter = nft_cmds.find(cmd);

            if (iter != end(nft_cmds))
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

void check_istream_eof(istream &is, string_view err)
{
    if (is.eof())
    {
        ostringstream oss;

        oss << err;
        __throw_invalid_argument(oss.str().c_str());
    }
}

template <typename T>
T get_from_stream(istringstream &params, client2_ptr client, string_view err_info = "index or address")
{
    violas::nft::Address addr;
    int account_index = -1;

    check_istream_eof(params, err_info);

    string temp;

    params >> temp;
    istringstream iss(temp);

    if (temp.length() == sizeof(T) * 2)
    {
        iss >> addr;
    }
    else
    {
        auto accounts = client->get_all_accounts();

        iss >> account_index;
        if (account_index >= accounts.size())
            __throw_invalid_argument("account index is out of account size.");
        else
            addr = accounts[account_index].address.value;
    }

    return addr;
}

void mint_portrait(client2_ptr client, const violas::nft::Address addr)
{
}

map<string, handle> create_store_commands(client2_ptr client, string url)
{
    using namespace violas::nft;
    client->allow_custom_script(true);

    auto type_tag = make_struct_type_tag({STD_LIB_ADDRESS}, "MountWuyi", "Tea");
    auto store = make_shared<violas::nft::Store>(client, type_tag);

    return map<string, handle>{
        {"deploy", [=](istringstream &params)
         {
             client->allow_publishing_module(true);

             // 1.  deploy nft store
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/Compare.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/Map.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/NonFungibleToken.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/stdlib/modules/NftStore2.mv");
             // client->publish_module(ACCOUNT_ROOT_ID, "move/tea/modules/MountWuyi.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/tea/modules/Portrait.mv");
         }},
        {"store-initalize", [=](istringstream &params)
         {
             store->initialize();
         }},
        {"store-register", [=](istringstream &params)
         {
             store->register_nft();
         }},
        {"store-list-orders", [=](istringstream &params)
         {
             // cout << store->list_orders();
         }},
        {"store-make-order", [=](istringstream &params)
         {
             size_t account_index;
             violas::nft::TokenId nft_token_id;
             uint64_t price;
             string currency;
             double incentive;

             params >> account_index >> nft_token_id >> price >> currency >> incentive;

             store->make_order(account_index, nft_token_id, price, currency, incentive);
         }},
        {"store-revoke-order", [=](istringstream &params)
         {
             size_t account_index;
             violas::nft::Id order_id;

             params >> account_index >> order_id;

             store->revoke_order(account_index, order_id);
         }},
        {"store-trade-order", [=](istringstream &params)
         {
             size_t account_index;
             nft::Id order_id;
             nft::Address address;
             string currency;

             params >> account_index >> currency >> address >> order_id;

             store->trade_order(account_index,
                                currency,
                                order_id);
         }},
        {"store-get-account-info", [=](istringstream &params)
         {
             nft::Address address;

             auto accounts = client->get_all_accounts();

             params >> address;

             auto account_info = store->get_account_info(address);
         }},
        {"store-query-events", [=](istringstream &params)
         {
             string event_type;
             nft::Address address;

             auto accounts = client->get_all_accounts();

             // params >> event_type >> address;

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

map<string, handle> create_nft_commands(client2_ptr client, string url)
{
    using namespace violas::nft;
    using Address = violas::nft::Address;
    auto nft = make_shared<NonFungibleToken<Portrait>>(client, url);

    return map<string, handle>{
        // {"deploy", [=](istringstream &params)
        //  {
        //      // deploy_stdlib(client);
        //      nft->deploy();
        //  }},
        {"nft-register", [=](istringstream &params)
         {
             check_istream_eof(params, "NFT total number");

             uint64_t total = 1000;
             params >> total;

             // register_mountwuyi_tea_nft(client, total);
             nft->register_instance(total);
         }},
        {"nft-accept", [=](istringstream &params)
         {
             size_t account_index = 0;
             params >> account_index;

             // accept(client, account_index);
             nft->accept(account_index);
         }},
        {"nft-mint", [=](istringstream &params)
         {
             auto addr = get_from_stream<violas::nft::Address>(params, client);
             mint_portrait(client, addr);
         }},
        {"nft-burn", [=](istringstream &params)
         {
             TokenId token_id;

             params >> token_id;

             // burn_tea_nft(client, token_id);
             nft->burn(token_id);
         }},
        {"nft-transfer", [=](istringstream &params)
         {
             size_t account_index = 0;
             violas::nft::Address receiver;
             string metadata;

             check_istream_eof(params, "usage : transfer account_index account_address token_id_or_index metadata");

             params >> account_index;

             check_istream_eof(params, "receiver address");
             receiver = get_from_stream<violas::nft::Address>(params, client);

             check_istream_eof(params, "index or token id");
             string token_id_or_index;

             params >> token_id_or_index;

             if (!params.eof())
                 params >> metadata;

             if (token_id_or_index.length() == 64)
             {
                 TokenId token_id;
                 istringstream iss(token_id_or_index);

                 iss >> token_id;

                 nft->transfer_by_token_id(account_index, receiver, token_id, string_to_bytes(metadata));
             }
             else
             {
                 uint64_t token_index;
                 istringstream iss(token_id_or_index);

                 iss >> token_index;

                 nft->transfer_by_token_index(account_index, receiver, token_index, string_to_bytes(metadata));
             }
         }},
        {"nft-balance", [=](istringstream &params)
         {
             auto addr = get_from_stream<violas::nft::Address>(params, client);

             auto opt_balance = nft->balance(addr);
             if (opt_balance)
             {
                 // int i = 0;
                 //  for (const auto &tea : *opt_balance)
                 //  {
                 //      cout << i++ << " - " << tea << endl;
                 //  }
                 // cout << *opt_balance;
             }
         }},
        {"nft-owner", [=](istringstream &params)
         {
             TokenId id;

             params >> id;

             auto owner = nft->get_owner(url, id);
             if (owner != nullopt)
             {
                 // print the address of owner
                 cout << *owner << endl;
             }
             else
                 cout << "cannot find owner." << endl;
         }},
        {"nft-trace", [=](istringstream &params)
         {
             TokenId token_id;

             check_istream_eof(params, "token id");
             params >> token_id;

             auto receiver = nft->get_owner(url, token_id);
             if (receiver != nullopt)
             {
                 size_t i = 0;
                 //  for (const auto receiver : *receivers)
                 //  {
                 //      cout << i++ << "  -  " << receiver << endl;
                 //  }
             }
         }},
        {"nft-info", [=](istringstream &params)
         {
             auto opt_info = nft->get_nft_info(url);

             // if (opt_info != nullopt)
             //     cout << *opt_info << endl;
         }},
        {"nft-create_child_account", [=](istringstream &params)
         {
             Address addr;
             AuthenticationKey auth_key;

             check_istream_eof(params, "authentication key");
             params >> auth_key;

             copy(begin(auth_key) + 16, begin(auth_key) + 32, begin(addr));

             client->create_child_vasp_account(0, {addr}, auth_key, "VLS", 0, true);
         }},
        {"nft-add-account", [=](istringstream &params)
         {
             client->create_next_account();
         }},
        {"nft-list-accounts", [=](istringstream &params)
         {
             auto accounts = client->get_all_accounts();

             cout << color::CYAN
                  << left << setw(10) << "index"
                  << left << setw(40) << "Address"
                  << left << setw(40) << "Authentication key"
                  << color::RESET << endl;

             int i = 0;
             for (auto &account : accounts)
             {
                 cout << left << setw(10) << i++
                      << left << setw(40) << account.address.value
                      << left << setw(40) << account.auth_key
                      << endl;
             }
         }},
        {"nft-query-events", [=](istringstream &params)
         {
             string event_type;

             check_istream_eof(params, "(usage) : query_event [minted, burned, sent, received] address start limit");
             params >> event_type;

             auto addr = get_from_stream<Address>(params, client);
             uint64_t start = 0, limit = 10;

             if (!params.eof())
                 params >> start;

             if (!params.eof())
                 params >> limit;

             if (event_type == "minted")
             {
                 auto opt_event_handle = nft->get_event_handle(EventType::minted, addr);
                 if (opt_event_handle == nullopt)
                     __throw_runtime_error("event handle doesn't exist.");

                 auto events = nft->query_events<MintedEvent>(*opt_event_handle, addr, start, limit);

                 cout << color::CYAN << "Minted events list (" << opt_event_handle->counter << ")" << color::RESET << endl;
                 cout << events << endl;
             }
             else if (event_type == "burned")
             {
                 auto opt_event_handle = nft->get_event_handle(EventType::burned, addr);
                 if (opt_event_handle == nullopt)
                     __throw_runtime_error("event handle doesn't exist.");

                 auto events = nft->query_events<BurnedEvent>(*opt_event_handle, addr, start, limit);

                 cout << color::CYAN << "Burned events list (" << opt_event_handle->counter << ")" << color::RESET << endl;
                 cout << events << endl;
             }
             else if (event_type == "sent")
             {
                 auto opt_event_handle = nft->get_event_handle(EventType::sent, addr);
                 if (opt_event_handle == nullopt)
                     __throw_runtime_error("event handle doesn't exist.");

                 auto events = nft->query_events<SentEvent>(*opt_event_handle, addr, start, limit);

                 cout << color::CYAN << "Sent events list (" << opt_event_handle->counter << ")" << color::RESET << endl;
                 cout << events << endl;
             }
             else if (event_type == "received")
             {
                 auto opt_event_handle = nft->get_event_handle(EventType::received, addr);
                 if (opt_event_handle == nullopt)
                     __throw_runtime_error("event handle doesn't exist.");

                 auto events = nft->query_events<ReceivedEvent>(*opt_event_handle, addr, start, limit);

                 cout << color::CYAN << "Received events list (" << opt_event_handle->counter << ")" << color::RESET << endl;
                 cout << events << endl;
             }
             else
             {
                 __throw_invalid_argument("event type is invalid, please input [minted, burned, sent, received]");
             }

             //  for (auto &event : events)
             //  {
             //      //cout << event << endl;
             //  }
         }},
    };
}