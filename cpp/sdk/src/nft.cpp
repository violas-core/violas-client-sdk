#include <iostream>
#include <string>
#include <client.hpp>
#include "nft.hpp"

using namespace std;

namespace violas
{
    template <typename T>
    NonFungibleToken<T>::NonFungibleToken(client_ptr client) : _client(client)
    {
    }

    // NonFungibleToken::~NonFungibleToken()
    // {
    // }

    template <typename T>
    void NonFungibleToken<T>::deploy()
    {
        _client->allow_publishing_module(true);
        _client->allow_custom_script();

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
                _client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, module);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    template <typename T>
    void NonFungibleToken<T>::register_instance(uint64_t total_number)
    {
        auto accounts = _client->get_all_accounts();
        auto &admin = accounts[0];
        auto &dealer1 = accounts[1];
        auto &dealer2 = accounts[2];

        _client->create_parent_vasp_account("VLS", 0, admin.address, admin.auth_key, "Tea VASP", "", admin.pub_key, true);
        _client->create_child_vasp_account("VLS", 0, dealer1.address, dealer1.auth_key, true, 0, true);
        _client->create_child_vasp_account("VLS", 0, dealer2.address, dealer2.auth_key, true, 0, true);

        //
        //  Rgiester NFT Tea and set address for admin
        //
        cout << "Registering Tea NFT for admin account ..." << endl;
        _client->execute_script_file(VIOLAS_ROOT_ACCOUNT_ID,
                                     "move/stdlib/scripts/nft_register.mv",
                                     {T::type_tag()},
                                     {uint64_t(total_number), admin.address});

        cout << "Register NFT for admin successfully." << endl;

        //accept(client, admin.index);
        accept(dealer1.index);
        accept(dealer2.index);

        cout << "Accept NFT for dealer successfully. " << endl;
    }

    template <typename T>
    void NonFungibleToken<T>::accept(size_t account_index)
    {
        _client->execute_script_file(account_index,
                                     "move/stdlib/scripts/nft_accept.mv",
                                     {T::type_tag()},
                                     {});
    }
}
