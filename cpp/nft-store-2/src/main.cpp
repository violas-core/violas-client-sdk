/**
 * @file main.cpp
 * @author Hunter Sun (huntersun2018@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-03-10
 *
 * @copyright Copyright (c) 2022
 *
 */
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
map<string, handle> create_std_commands(client2_ptr client, string url);
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

        commands.merge(create_std_commands(client, args.url));
        auto nft_cmds = create_nft_commands(client, args.url);
        commands.merge(nft_cmds);

        assert(nft_cmds.size() == 0); //  nft_cmds must be empty.

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

            if (auto iter = commands.find(cmd);
                iter != end(commands))
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

                console->add_history(line);
            }
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
        throw invalid_argument(err.data());
    }
}

void check_account_index(client2_ptr client, size_t account_index)
{
    if (account_index >= client->get_all_accounts().size())
        throw invalid_argument("The account index is more than the size of all accounts.");
}

template <typename T>
T get_from_stream(istringstream &params, client2_ptr client, string_view err_info = "index or address")
{
    Address addr;
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

/**
 * @brief Create a cli commands object
 *
 * @param client
 * @param url
 * @return map<string, handle>
 */
map<string, handle> create_std_commands(client2_ptr client, string url)
{
    return {
        {"std-add-account", [=](istringstream &params)
         {
             client->create_next_account();
         }},
        {"std-list-accounts", [=](istringstream &params)
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
        {"std-create-child-account", [=](istringstream &params)
         {
             Address addr;
             AuthenticationKey auth_key;

             check_istream_eof(params, "authentication key");
             params >> auth_key;

             copy(begin(auth_key) + 16, begin(auth_key) + 32, begin(addr));

             client->create_child_vasp_account(0, {addr}, auth_key, "VLS", 0, true);
         }},
    };
}
/**
 * @brief Create a store commands object
 *
 * @param client
 * @param url
 * @return map<string, handle>
 */
map<string, handle> create_store_commands(client2_ptr client, string url)
{
    using namespace violas::nft;
    client->allow_custom_script(true);

    auto type_tag = make_struct_type_tag({VIOLAS_LIB_ADDRESS}, "Portrait", "Portrait");
    auto store = make_shared<violas::nft::Store>(client, type_tag);

    auto [index, address] = client->create_next_account(dt::AccountAddress{NFT_STORE_ADMIN_ADDRESS});
    cout << index << " : " << bytes_to_hex(address.value) << endl;

    tie(index, address) = client->create_next_account();
    cout << index << " : " << bytes_to_hex(address.value) << endl;

    tie(index, address) = client->create_next_account();
    cout << index << " : " << bytes_to_hex(address.value) << endl;

    tie(index, address) = client->create_next_account();
    cout << index << " : " << bytes_to_hex(address.value) << endl;

    return map<string, handle>{
        {"deploy", [=](istringstream &params)
         {
             client->allow_publishing_module(true);

             // 1.  deploy nft store
             client->publish_module(ACCOUNT_ROOT_ID, "move/build/package/stdlib/compiled/Compare.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/build/modules/0_Map.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/build/modules/1_NonFungibleToken.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/build/modules/4_NftStore2.mv");
             client->publish_module(ACCOUNT_ROOT_ID, "move/build/modules/5_Portrait.mv");

             auto accounts = client->get_all_accounts();
             auto &a0 = accounts[0];
             auto &a1 = accounts[1];
             auto &a2 = accounts[2];
             auto &a3 = accounts[3];

             try
             {
                 client->create_designated_dealer_ex("VLS", a0.index, {NFT_STORE_ADMIN_ADDRESS}, a0.auth_key, "NFT Store Admin", true);
                 cout << a0.index << " : " << a0.address.value << "\tRole : DD" << endl;

                 client->create_parent_vasp_account(a1.address, a1.auth_key, "NFT VASP", true);
                 cout << a1.index << " : " << a1.address.value << "\tRole : VASP" << endl;

                 client->create_child_vasp_account(1, a2.address, a2.auth_key, "VLS", 0, true);
                 cout << a2.index << " : " << a2.address.value << "\tRole : Child VASP" << endl;

                 client->create_child_vasp_account(1, a3.address, a3.auth_key, "VLS", 0, true);
                 cout << a3.index << " : " << a3.address.value << "\tRole : Child VASP" << endl;
             }
             catch (const std::exception &e)
             {
                 std::cerr << e.what() << '\n';
             }

             cout << "All modules were published successfully." << endl;
         }},
        {"store-initalize", [=](istringstream &args)
         {
             size_t account_index = 0;
             args >> account_index;
             check_account_index(client, account_index);

             store->initialize(account_index);
         }},
        {"store-register-nft", [=](istringstream &args)
         {
             size_t account_index = 0;
             args >> account_index;
             check_account_index(client, account_index);

             store->register_nft(account_index);
         }},
        {"store-accept-nft", [=](istringstream &args)
         {
             check_istream_eof(args, "account_index");

             size_t account_index = 0;
             args >> account_index;
             check_account_index(client, account_index);

             store->accept_nft(account_index);
         }},
        {"store-list-orders", [=](istringstream &params)
         {
             cout << store->list_orders();
         }},
        {"store-make-order", [=](istringstream &args)
         {
             check_istream_eof(args, "account_index, nft_token_id, price, currency");

             size_t account_index = 0;
             violas::nft::TokenId nft_token_id = {0};
             uint64_t price = 0;
             string currency;

             args >> account_index >> nft_token_id >> price >> currency;

             check_account_index(client, account_index);

             store->make_order(account_index, nft_token_id, price * MICRO_COIN, currency);
         }},
        {"store-revoke-order", [=](istringstream &args)
         {
             check_istream_eof(args, "account_index, order_id");

             size_t account_index;
             violas::nft::Id order_id;

             args >> account_index >> order_id;

             check_account_index(client, account_index);

             store->revoke_order(account_index, order_id);
         }},
        {"store-trade-order", [=](istringstream &args)
         {
             check_istream_eof(args, "account_index, order_id, currency");

             size_t account_index;
             nft::Id order_id;
             string currency;

             args >> account_index >> order_id >> currency;

             check_account_index(client, account_index);

             store->trade_order(account_index,
                                currency,
                                order_id);
         }},
        {"store-get-account-info", [=](istringstream &params)
         {
             check_istream_eof(params, "usage : account_index or account_address");

             Address address;

             address = get_from_stream<Address>(params, client);

             auto account_info = store->get_account_info(address);

             if (account_info)
                 cout << *account_info << endl;
         }},
        {"store-query-events", [=](istringstream &args)
         {
             check_istream_eof(args, "account_index_or_address [made, revoked, sold, bought, traded] start limit");

             string event_type;
             Address address;
             uint64_t start = 0;
             uint64_t limit = 10;

             address = get_from_stream<Address>(args, client);

             args >> event_type >> start >> limit;

             if (event_type == "made")
             {
                 auto handle = store->get_event_handle(address, event_type::made);
                 if (handle)
                 {
                     auto events = store->query_order_events<MadeOrderEvent>(*handle, start, limit);
                     cout << events << endl;
                 }
             }
             else if (event_type == "revoked")
             {
                 auto handle = store->get_event_handle(address, event_type::revoked);
                 if (handle)
                 {
                     auto events = store->query_order_events<RevokedOrderEvent>(*handle, start, limit);
                     cout << events << endl;
                 }
             }
             else if (event_type == "sold")
             {
                 auto handle = store->get_event_handle(address, event_type::sold);
                 if (handle)
                 {
                     auto events = store->query_order_events<SoldOrderEvent>(*handle, start, limit);
                     cout << events << endl;
                 }
             }
             else if (event_type == "bought")
             {
                 auto handle = store->get_event_handle(address, event_type::bought);
                 if (handle)
                 {
                     auto events = store->query_order_events<BoughtOrderEvent>(*handle, start, limit);
                     cout << events << endl;
                 }
             }
             else if (event_type == "traded")
             {
                 auto handle = store->get_event_handle(address, event_type::traded);
                 if (handle)
                 {
                     auto events = store->query_order_events<TradedOrderEvent>(*handle, start, limit);
                     cout << events << endl;
                 }
             }
             else
                 throw invalid_argument(fmt("event type '", event_type, "' is unknown"));
         }},
    };
}

map<string, handle> create_nft_commands(client2_ptr client, string url)
{
    using namespace violas::nft;
    auto nft = make_shared<NonFungibleToken<Portrait>>(client);

    return map<string, handle>{
        // {"deploy", [=](istringstream &params)
        //  {
        //      // deploy_stdlib(client);
        //      nft->deploy();
        //  }},
        {"nft-register", [=](istringstream &params)
         {
             check_istream_eof(params, "nft_total_amount, admin_address");

             uint64_t total = 1000;
             Address address;
             params >> total >> address;

             nft->register_instance(total, address);
         }},
        {"nft-accept", [=](istringstream &params)
         {
             size_t account_index = 0;
             params >> account_index;

             nft->accept(account_index);
         }},
        {"nft-mint", [=](istringstream &params)
         {
             // auto addr = get_from_stream<violas::nft::Address>(params, client);
             check_istream_eof(params, "account_index description ipfs_cid receiver_address");

             size_t account_index;
             string description;
             string ipfs_cid;
             Address receiver;

             params >> account_index >> description >> ipfs_cid >> receiver;

             Portrait::mint(client, account_index, description, ipfs_cid, {receiver});
         }},
        {"nft-burn", [=](istringstream &params)
         {
             size_t account_index;
             TokenId token_id;

             params >> account_index >> token_id;

             nft->burn(account_index, token_id);
         }},
        {"nft-transfer", [=](istringstream &params)
         {
             size_t account_index = 0;
             Address receiver;
             string metadata;

             check_istream_eof(params, "usage : transfer account_index account_address token_id_or_index metadata");

             params >> account_index;

             check_istream_eof(params, "receiver address");
             receiver = get_from_stream<Address>(params, client);

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
             auto addr = get_from_stream<Address>(params, client);

             auto opt_balance = nft->balance(addr);
             if (opt_balance)
             {
                 int i = 0;
                 for (const auto &item : *opt_balance)
                 {
                     cout << i++ << " - " << item << endl;
                 }
             }
         }},
        {"nft-owner", [=](istringstream &params)
         {
             TokenId id;

             params >> id;

             auto owner = nft->get_owner(id);
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

             auto receiver = nft->get_owner(token_id);
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
             auto opt_info = nft->get_nft_info();

             if (opt_info != nullopt)
                 cout << *opt_info << endl;
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
