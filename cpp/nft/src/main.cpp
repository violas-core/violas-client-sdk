#include <iostream>
#include <map>
#include <functional>
#include <random>
#include <violas_sdk2.hpp>
#include <utils.h>
#include <argument.hpp>
#include <bcs_serde.hpp>
#include <json_rpc.hpp>
#include <utils.h>
#include <console.hpp>
#include <ssl_aes.hpp>
#include "tea.hpp"

using namespace std;
using namespace violas;

const TypeTag tea_tag(VIOLAS_STDLIB_ADDRESS, "MountWuyi", "Tea");

void deploy_stdlib(client_ptr client);
void register_mountwuyi_tea_nft(client_ptr client);
void mint_tea_nft(client_ptr client);
void transfer(client_ptr client);
void query_nft(client_ptr client, string url);
TokenId compute_token_id(const Tea &t);
optional<Address> get_owner(string url, Address admin, TokenId token_id);

int main(int argc, char *argv[])
{
    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto client = Client::create(args.chain_id, args.url, args.mint_key, args.mnemonic, args.waypoint);

        cout << "NFT Management 1.0" << endl;

        auto admin = client->create_next_account();
        auto dealer1 = client->create_next_account();
        auto dealer2 = client->create_next_account();

        cout << "Admin      : " << admin.address << "\n"
             << "Dealer 1   : " << dealer1.address << "\n"
             << "Dealer 2   : " << dealer2.address << endl;

        auto console = Console::create("NFT$");

        console->add_completion("exit");

        using handler = function<void(istringstream & params)>;
        //using handler = void (*)(client_ptr client);
        map<string, handler> commands = {
            {"deploy", [=](istringstream &params)
             { deploy_stdlib(client); }},
            {"register", [=](istringstream &params)
             { register_mountwuyi_tea_nft(client); }},
            {"mint", [=](istringstream &params)
             { mint_tea_nft(client); }},
            {"transfer", [=](istringstream &params)
             { transfer(client); }},
            {"query", [=](istringstream &params)
             {
                 query_nft(client, args.url);
             }},
            {"get_owner", [=](istringstream &params)
             {
                 TokenId id;

                 params >> id;

                 auto opt_addr = get_owner(args.url, admin.address, id);
                 if (opt_addr != nullopt)
                     cout << *opt_addr << endl;
             }}};

        for (auto cmd : commands)
        {
            console->add_completion(cmd.first);
        }

        //
        //  Loop to read a line
        //
        for (auto line = trim(console->read_line());
             line != "exit";
             line = trim(console->read_line()))
        {
            istringstream iss(line);
            string cmd;

            // Read a command
            iss >> cmd;

            auto iter = commands.find(cmd);
            if (iter != end(commands))
            {
                iter->second(iss);
            }

            console->add_history(line);
        }

        // using handler = function<void(client_ptr ptr)>;
        // //using handler = void (*)(client_ptr client);
        // map<int, handler> handlers = {
        //     {0, deploy_stdlib},
        //     {1, register_mountwuyi_tea_nft},
        //     {2, mint_tea_nft},
        //     {3, transfer},
        //     {4, [=](client_ptr client)
        //      {
        //          query_nft(client, args.url);
        //      }},
        // };

        // size_t index;
        // do
        // {
        //     cout << "0 - Deploy all modules\n"
        //             "1 - Register Tea NFT\n"
        //             "2 - Mint a Tea NFT\n"
        //             "3 - Transfer Tea NFT from dealer 1 to dealer 2\n"
        //             "4 - view Tea NFT\n"
        //          << "Please input function index :";

        //     cin >> index;

        //     auto fun = handlers.find(index);
        //     if (fun != end(handlers))
        //         fun->second(client);
        //     else
        //         break;
        // } while (true);
    }
    catch (const std::exception &e)
    {
        std::cerr << color::RED
                  << "Exceptions : " << e.what()
                  << color::RESET
                  << endl;
    }

    return 0;
}

void deploy_stdlib(client_ptr client)
{
    client->allow_publishing_module(true);
    client->allow_custom_script();

    string modules[] =
        {"move/stdlib/modules/Compare.mv",
         "move/stdlib/modules/Map.mv",
         "move/stdlib/modules/NonFungibleToken.mv",
         "move/tea/modules/MountWuyi.mv"};

    for (auto module : modules)
    {
        cout << "Deploying module " << module << " ..." << endl;

        try
        {
            client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, module);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void register_mountwuyi_tea_nft(client_ptr client)
{
    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];
    auto &dealer2 = accounts[2];

    client->create_parent_vasp_account("VLS", 0, admin.address, admin.auth_key, "Tea VASP", "", admin.pub_key, true);
    client->create_child_vasp_account("VLS", 0, dealer1.address, dealer1.auth_key, true, 0, true);
    client->create_child_vasp_account("VLS", 0, dealer2.address, dealer2.auth_key, true, 0, true);

    //
    //  Rgiester NFT Tea and set address for admin
    //
    cout << "Registering Tea NFT for admin account ..." << endl;
    client->execute_script_file(VIOLAS_ROOT_ACCOUNT_ID,
                                "move/stdlib/scripts/nft_register.mv",
                                {tea_tag},
                                {uint64_t(1000), admin.address});
    cout << "Register NFT for admin successfully." << endl;

    //
    //  Accept NFT for dealer account
    //
    client->execute_script_file(dealer1.index,
                                "move/stdlib/scripts/nft_accept.mv",
                                {tea_tag},
                                {});

    client->execute_script_file(dealer2.index,
                                "move/stdlib/scripts/nft_accept.mv",
                                {tea_tag},
                                {});

    cout << "Accept NFT for dealer successfully. " << endl;
}

void mint_tea_nft(client_ptr client)
{
    cout << "minting Tea NFT ... " << endl;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];
    auto &dealer2 = accounts[2];

    default_random_engine e(clock());
    uniform_int_distribution<unsigned> u(0, 26);

    cout << "u(e) = " << u(e) << endl;

    vector<uint8_t> identity = {'1', '2', '3', '4', '5', '6', uint8_t('a' + u(e)), uint8_t('a' + u(e))};
    string wuyi = "MountWuyi";
    vector<uint8_t> manufacturer(wuyi.begin(), wuyi.end());

    client->execute_script_file(admin.index,
                                "move/tea/scripts/mint_mountwuyi_tea_nft.mv",
                                {},
                                {identity, uint8_t(0), manufacturer, dealer1.address});

    cout << "Mint a Tea NFT to dealer 1" << endl;
}

void burn_tea_nft(client_ptr client)
{
    cout << "minting Tea NFT ... " << endl;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];
    auto &dealer2 = accounts[2];

    vector<uint8_t> identity = {1, 1, 2, 2, 3, 3, 4, 4};
    string wuyi = "MountWuyi";
    vector<uint8_t> manufacturer(wuyi.begin(), wuyi.end());

    client->execute_script_file(admin.index,
                                "move/tea/scripts/mint_mountwuyi_tea_nft.mv",
                                {},
                                {identity, uint8_t(0), manufacturer, dealer1.address});

    cout << "Mint a Tea NFT to dealer 1" << endl;
}

void transfer(client_ptr client)
{
    cout << "transfer Tea NFT ... " << endl;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];
    auto &dealer2 = accounts[2];

    client->execute_script_file(dealer1.index,
                                "move/stdlib/scripts/nft_transfer_via_index.mv",
                                {tea_tag},
                                {dealer2.address, uint64_t(0), vector<uint8_t>{0x1, 0x2, 0x3}});
}

void query_nft(client_ptr client, string url)
{
    using namespace json_rpc;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];
    auto &dealer2 = accounts[2];

    auto rpc_cli = json_rpc::Client::create(url);

    StructTag tag{
        Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        "NonFungibleToken",
        "NonFungibleToken",
        {StructTag{Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, "MountWuyi", "Tea"}}};

    violas::AccountState state(rpc_cli);

    auto opt_tea = state.get_resource<vector<Tea>>(dealer2.address, tag);
    if (opt_tea != std::nullopt)
    {
        for (const auto &t : *opt_tea)
        {
            cout << t << endl;
        }
    }
}

optional<Address> get_owner(string url, Address admin, TokenId token_id)
{
    using namespace json_rpc;
    auto rpc_cli = json_rpc::Client::create(url);

    violas::AccountState state(rpc_cli);

    StructTag tag{
        Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        "NonFungibleToken",
        "Info",
        {StructTag{Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, "MountWuyi", "Tea"}}};

    auto opt_tea = state.get_resource<TeaInfo>(VIOLAS_ROOT_ADDRESS, tag);
    if (opt_tea != nullopt)
    {
        vector<uint8_t> id;
        copy(begin(token_id), end(token_id), back_inserter<>(id));

        auto iter = opt_tea->owners.find(id);
        if (iter != end(opt_tea->owners))
        {
            return iter->second;
        }
    }

    return {};
}

TokenId compute_token_id(const Tea &tea)
{
    BcsSerde serde;
    auto t = tea;

    serde &&t;

    auto bytes = serde.bytes();

    auto token_id = sha3_256(bytes.data(), bytes.size());

    return token_id;
}