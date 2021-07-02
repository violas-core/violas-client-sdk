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

using namespace std;
using namespace violas;

const TypeTag tea_tag(VIOLAS_STDLIB_ADDRESS, "MountWuyi", "Tea");

void deploy_stdlib(client_ptr client);
void register_mountwuyi_tea_nft(client_ptr client);
void mint_tea_nft(client_ptr client);
void transfer(client_ptr client);
void query_nft(client_ptr client, string url);

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

        using handler = function<void(client_ptr ptr)>;
        //using handler = void (*)(client_ptr client);
        map<int, handler> handlers = {
            {0, deploy_stdlib},
            {1, register_mountwuyi_tea_nft},
            {2, mint_tea_nft},
            {3, transfer},
            {4, [=](client_ptr client)
             {
                 query_nft(client, args.url);
             }},
        };

        size_t index;
        do
        {
            cout << "0 - Deploy all modules\n"
                    "1 - Register Tea NFT\n"
                    "2 - Mint a Tea NFT\n"
                    "3 - Transfer Tea NFT from dealer 1 to dealer 2\n"
                    "4 - view Tea NFT\n"
                 << "Please input function index :";

            cin >> index;

            auto fun = handlers.find(index);
            if (fun != end(handlers))
                fun->second(client);
            else
                break;
        } while (true);
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
    uniform_int_distribution<unsigned> u(0, 255);

    cout << "u(e) = " << u(e) << endl;

    vector<uint8_t> identity = {1, 1, 2, 2, 3, 3, 4, (uint8_t)u(e)};
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

struct Tea
{
    vector<uint8_t> identity;
    uint8_t kind;
    vector<uint8_t> manufacture;
    uint64_t date;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && identity && kind && manufacture && date;
    }
};

void query_nft(client_ptr client, string url)
{
    using namespace json_rpc;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];
    auto &dealer2 = accounts[2];

    auto rpc_cli = json_rpc::Client::create(url);

    // ostringstream oss;
    // oss << dealer2.address;

    // auto as = rpc_cli->get_account_state_blob(oss.str());

    // cout << as.blob << endl;

    // auto bytes = hex_to_bytes(as.blob);

    // vector<uint8_t> data;
    // {
    //     BcsSerde serde(move(bytes));
    //     serde &&data;
    // }

    // //std::pair<vector<uint8_t>, vector<uint8_t>> data;

    // map<vector<uint8_t>, vector<uint8_t>> account_data;

    // BcsSerde serde(move(data));
    // serde &&account_data;

    TypeTag tag{Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, "MountWuyi", "Tea"};
    // {
    //     BcsSerde serde;
    //     auto tag_ser = (serde && tag).bytes();

    //     auto size = tag_ser.size();
    // }
    violas::AccountState state(rpc_cli);

    auto t = state.get_resource<Tea>(dealer2.address, tag);
    if (t != std::nullopt)
    {
    }
}