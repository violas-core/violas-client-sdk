#include <iostream>
#include <fstream>
#include <string>
#include <functional>

#include <utils.hpp>
#include <violas_sdk2.hpp>
#include <argument.hpp>
#include <console.hpp>
#include <json_rpc.hpp>
#include <ed25519.hpp>
#include "nft_store.hpp"
#include <violas_client2.hpp>
#include <wallet.hpp>

using namespace std;
using namespace violas;

void depoloy(client_ptr client);
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

        test(args);

        auto rpc_cli = json_rpc::Client::create(args.url);

        auto client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);

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

void intialize(client2_ptr client)
{
    //client->allow_publishing_module(true);

    // 1.  deploy nft store
    ifstream ifs("move/stdlib/nft-store.mv", ios::binary);

    if(!ifs.is_open())
        throw runtime_error("nft::initalize failed, failed to open 'move/stdlib/nft-store.mv'");
    
    bytes module_bytecode(istreambuf_iterator<char>(ifs), {});

    client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, module_bytecode);

    // 2. deploy nft store

    // 3. register NFT type to store
}

map<string, handle> create_commands(client2_ptr client, string url)
{
    client->allow_custom_script(true);

    TypeTag tag(VIOLAS_STDLIB_ADDRESS, "MountWuyi", "Tea");

    return map<string, handle>{
        {"deploy", [=](istringstream &params)
         {
             client->allow_publishing_module(true);

             // 1.  deploy nft store
             client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, "move/stdlib/modules/Compare.mv");
             client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, "move/stdlib/modules/Map.mv");
             client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, "move/stdlib/modules/NonFungibleToken.mv");
             client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, "move/stdlib/modules/NftStore.mv");
             client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, "move/tea/modules/MountWuyi.mv");
         }},
        {"initalize", [=](istringstream &params)
         {
             nft::Store store(client);

             store.initalize(tag);
         }},
        {"register", [=](istringstream &params)
         {
             nft::Store store(client);

             store.register_nft(tag);
         }},
        {"test", [=](istringstream &params)
         {
             nft::Store store(client);

             store.register_nft(tag);
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

    client->add_currency(Client2::ACCOUNT_DD_ID, currency_code);

    client->add_currency_for_designated_dealer(currency_code, Client2::TESTNET_DD_ADDRESS);

    client->mint(currency_code, 0, 1'000 * MICRO_COIN, Client2::TESTNET_DD_ADDRESS, 0);
}

void test_nft_store()
{
    client2_ptr client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);
}