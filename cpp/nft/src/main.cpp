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

TokenId compute_token_id(const Tea &t);
void check_istream_eof(istream &is, string_view err);

void deploy_stdlib(client_ptr client);

void register_mountwuyi_tea_nft(client_ptr client, uint64_t total);

void accept(client_ptr client, size_t account_index);

void mint_tea_nft(client_ptr client, Address addr);

void burn_tea_nft(client_ptr client, const TokenId &token_id);

void transfer(client_ptr client, size_t account_index, Address receiver, uint64_t index);

optional<NftTea> get_nft(string url, Address addr);

optional<vector<Address>> get_owners(string url, const TokenId &token_id);

optional<Address> get_last_owner(string url, const TokenId &token_id);

optional<NftInfo> get_nft_info(string url);

using handle = function<void(istringstream &params)>;
map<string, handle> create_commands(client_ptr client, string url);

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

        auto console = Console::create("NFT$ ");

        console->add_completion("exit");

        auto commands = create_commands(client, args.url);
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
        std::cerr << color::RED
                  << "Exceptions : " << e.what()
                  << color::RESET
                  << endl;
    }

    return 0;
}

template <typename T>
T get_from_stream(istringstream &params, client_ptr client, string_view err_info = "The index or address")
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
            addr = accounts[account_index].address;
    }

    return addr;
}

map<string, handle> create_commands(client_ptr client, string url)
{
    return map<string, handle>{
        {"deploy", [=](istringstream &params)
         { deploy_stdlib(client); }},
        {"register", [=](istringstream &params)
         {
             check_istream_eof(params, "NFT total number");

             uint64_t total = 1000;
             params >> total;

             register_mountwuyi_tea_nft(client, total);
         }},
        {"accept", [=](istringstream &params)
         {
             size_t account_index = 0;
             params >> account_index;

             accept(client, account_index);
         }},
        {"mint", [=](istringstream &params)
         {
             auto addr = get_from_stream<Address>(params, client);
             mint_tea_nft(client, addr);
         }},
        {"burn", [=](istringstream &params)
         {
             TokenId token_id;

             params >> token_id;

             burn_tea_nft(client, token_id);
         }},
        {"transfer", [=](istringstream &params)
         {
             size_t account_index = 0, nft_index = 0;
             Address receiver;

             check_istream_eof(params, "account_index");

             params >> account_index;

             check_istream_eof(params, "receiver address");
             receiver = get_from_stream<Address>(params, client);

             check_istream_eof(params, "nft_index");
             params >> nft_index;

             transfer(client, account_index, receiver, nft_index);
         }},
        {"balance", [=](istringstream &params)
         {
             auto addr = get_from_stream<Address>(params, client);

             auto opt_nft_tea = get_nft(url, addr);
             if (opt_nft_tea != nullopt)
             {
                 int i = 0;
                 for (const auto &tea : opt_nft_tea->teas)
                 {
                     cout << i++ << " - " << tea << endl;
                 }
             }
         }},
        {"owner", [=](istringstream &params)
         {
             TokenId id;

             params >> id;

             auto opt_addr = get_last_owner(url, id);
             if (opt_addr != nullopt)
                 cout << *opt_addr << endl;
         }},
        {"trace", [=](istringstream &params)
         {
             TokenId token_id;

             check_istream_eof(params, "token id");
             params >> token_id;

             auto receivers = get_owners(url, token_id);
             if (receivers != nullopt)
             {
                 for (const auto receiver : *receivers)
                 {
                     cout << receiver << endl;
                 }
             }
         }},
        {"info", [=](istringstream &oarans)
         {
             auto opt_info = get_nft_info(url);

             if (opt_info != nullopt)
                 cout << *opt_info << endl;
         }},
    };
}

void check_istream_eof(istream &is, string_view err)
{
    if (is.eof())
    {
        ostringstream oss;

        oss << "missed argument " << err;
        __throw_invalid_argument(oss.str().c_str());
    }
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

void register_mountwuyi_tea_nft(client_ptr client, uint64_t total)
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
                                {uint64_t(total), admin.address});
    cout << "Register NFT for admin successfully." << endl;

    //accept(client, admin.index);
    accept(client, dealer1.index);
    accept(client, dealer2.index);

    cout << "Accept NFT for dealer successfully. " << endl;
}
//
//  Accept NFT for dealer account
//
void accept(client_ptr client, size_t account_index)
{
    client->execute_script_file(account_index,
                                "move/stdlib/scripts/nft_accept.mv",
                                {tea_tag},
                                {});
}

void mint_tea_nft(client_ptr client, Address addr)
{
    cout << "minting Tea NFT ... " << endl;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];
    auto &dealer1 = accounts[1];

    default_random_engine e(clock());
    uniform_int_distribution<unsigned> u(0, 25);

    vector<uint8_t> sn = {'1', '2', '3', '4', '5', '6', uint8_t('a' + u(e)), uint8_t('a' + u(e))};
    string wuyi = "MountWuyi";
    string pa = "MountWuyi City";
    vector<uint8_t> manufacturer(wuyi.begin(), wuyi.end());

    client->execute_script_file(admin.index,
                                "move/tea/scripts/mint_mountwuyi_tea_nft.mv",
                                {},
                                {
                                    uint8_t(0),
                                    manufacturer,
                                    vector<uint8_t>(begin(pa), end(pa)),
                                    uint64_t(0),
                                    sn,
                                    addr,
                                });

    cout << "Mint a Tea NFT to dealer 1" << endl;
}

void burn_tea_nft(client_ptr client, const TokenId &token_id)
{
    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];

    client->execute_script_file(admin.index,
                                "move/stdlib/scripts/nft_burn.mv",
                                {tea_tag},
                                {vector<uint8_t>(begin(token_id), end(token_id))});

    cout << "burned NFT with token id " << token_id << endl;
}

void transfer(client_ptr client, size_t account_index, Address receiver, uint64_t nft_index)
{
    cout << "transfer Tea NFT ... " << endl;

    auto accounts = client->get_all_accounts();
    //auto &admin = accounts[0];
    //auto &dealer1 = accounts[1];
    //auto &dealer2 = accounts[2];

    client->execute_script_file(account_index,
                                "move/stdlib/scripts/nft_transfer_via_index.mv",
                                {tea_tag},
                                {receiver, nft_index, vector<uint8_t>{0x1, 0x2, 0x3}});
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

optional<NftTea> get_nft(string url, Address addr)
{
    using namespace json_rpc;

    auto rpc_cli = json_rpc::Client::create(url);

    StructTag tag{
        Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        "NonFungibleToken",
        "NonFungibleToken",
        {StructTag{Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, "MountWuyi", "Tea"}}};

    violas::AccountState state(rpc_cli);

    try
    {
        return state.get_resource<NftTea>(addr, tag);
    }
    catch (const std::exception &e)
    {
        std::cerr << color::RED << e.what() << color::RESET << endl;
    }

    return {};
}

optional<NftInfo> get_nft_info(string url)
{
    using namespace json_rpc;
    auto rpc_cli = json_rpc::Client::create(url);

    violas::AccountState state(rpc_cli);

    StructTag tag{
        Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        "NonFungibleToken",
        "Info",
        {StructTag{Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, "MountWuyi", "Tea"}}};

    return state.get_resource<NftInfo>(VIOLAS_ROOT_ADDRESS, tag);
}

optional<vector<Address>> get_owners(string url, const TokenId &token_id)
{
    auto opt_nft_info = get_nft_info(url);
    if (opt_nft_info != nullopt)
    {
        vector<uint8_t> id;
        copy(begin(token_id), end(token_id), back_inserter<>(id));

        auto iter = opt_nft_info->owners.find(id);
        if (iter != end(opt_nft_info->owners))
        {
            return iter->second;
        }
    }

    return {};
}

optional<Address> get_last_owner(string url, const TokenId &token_id)
{
    auto owners = get_owners(url, token_id);
    if (owners != nullopt)
    {
        return *(owners->rbegin());
    }

    return {};
}
