#include <iostream>
#include <map>
#include <violas_sdk2.hpp>
#include <utils.h>
#include <argument.hpp>

using namespace std;
using namespace violas;

void deploy_stdlib(client_ptr client);
void register_mountwuyi_tea_nft(client_ptr client);

int main(int argc, char *argv[])
{
    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto client = Client::create(args.chain_id, args.url, args.mint_key, args.mnemonic, args.waypoint);

        client->allow_publishing_module(true);
        client->allow_custom_script();

        client->create_next_account();
        client->create_next_account();

        deploy_stdlib(client);
        register_mountwuyi_tea_nft(client);
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
    TypeTag tea_tag(VIOLAS_STDLIB_ADDRESS, "MountWuyi", "Tea");

    auto accounts = client->get_all_accounts();

    client->execute_script_file(VIOLAS_ROOT_ACCOUNT_ID,
                                "move/stdlib/scripts/register_nft.mv",
                                {tea_tag},
                                {uint64_t(1000), accounts[0].address});

    client->execute_script_file(0,
                                "move/tea/scripts/mint_mountwuyi_tea_nft.mv",
                                {tea_tag},
                                {});
}
