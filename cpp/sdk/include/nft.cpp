//
//  NonFugibleToken template class implementation
//
#include <iostream>
#include <string>
#include <sstream>
#include <optional>
#include <client.hpp>
#include "utils.hpp"
#include "nft.hpp"

using namespace std;

namespace violas
{

    template <typename T>
    NonFungibleToken<T>::NonFungibleToken(client_ptr client, string url) : _client(client), _url(url)
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
        //auto &dealer1 = accounts[1];
        //auto &dealer2 = accounts[2];

        _client->create_parent_vasp_account("VLS", 0, admin.address, admin.auth_key, "Tea VASP", "", admin.pub_key, true);
        //_client->create_child_vasp_account("VLS", 0, dealer1.address, dealer1.auth_key, true, 0, true);
        //_client->create_child_vasp_account("VLS", 0, dealer2.address, dealer2.auth_key, true, 0, true);

        //
        //  Rgiester NFT Tea and set address for admin
        //
        cout << "Registering Tea NFT for admin account ..." << endl;
        _client->execute_script_file(VIOLAS_ROOT_ACCOUNT_ID,
                                     "move/stdlib/scripts/nft_register.mv",
                                     {T::type_tag()},
                                     {uint64_t(total_number), admin.address});

        accept(admin.index);
        //accept(dealer1.index);
        //accept(dealer2.index);

        cout << "Register NFT successfully." << endl;
    }

    template <typename T>
    void NonFungibleToken<T>::burn(TokenId token_id)
    {
        auto accounts = _client->get_all_accounts();
        auto &admin = accounts[0];

        _client->execute_script_file(admin.index,
                                     "move/stdlib/scripts/nft_burn.mv",
                                     {T::type_tag()},
                                     {vector<uint8_t>(begin(token_id), end(token_id))});
    }

    template <typename T>
    void NonFungibleToken<T>::accept(size_t account_index)
    {
        _client->execute_script_file(account_index,
                                     "move/stdlib/scripts/nft_accept.mv",
                                     {T::type_tag()},
                                     {});
    }

    template <typename T>
    void NonFungibleToken<T>::transfer(uint64_t account_index, Address receiver, uint64_t token_index)
    {
        _client->execute_script_file(account_index,
                                     "move/stdlib/scripts/nft_transfer_via_index.mv",
                                     {T::type_tag()},
                                     {receiver, token_index, vector<uint8_t>{'p', 'a', 'y'}});
    }

    template <typename T>
    template <typename RESOURCE>
    optional<RESOURCE> NonFungibleToken<T>::get_nfts(string url, Address addr)
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
            return state.get_resource<RESOURCE>(addr, tag);
        }
        catch (const std::exception &e)
        {
            std::cerr << color::RED << e.what() << color::RESET << endl;
        }

        return {};
    }

    template <typename T>
    optional<NftInfo> NonFungibleToken<T>::get_nft_info(string url)
    {
        using namespace json_rpc;
        auto rpc_cli = json_rpc::Client::create(url);

        violas::AccountState state(rpc_cli);

        auto type_tag = T::type_tag();

        violas::StructTag tag{
            Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
            "NonFungibleToken",
            "Info",
            {violas::StructTag{type_tag.address, type_tag.module_name, type_tag.resource_name}}};

        return state.get_resource<NftInfo>(VIOLAS_ROOT_ADDRESS, tag);
    }

    template <typename T>
    optional<vector<Address>> NonFungibleToken<T>::get_owners(string url, const TokenId &token_id)
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

    template <typename T>
    string NonFungibleToken<T>::get_event_handle(EventType event_type,
                                                 const violas::Address &address)
    {
        switch (event_type)
        {
        case minted:
        case burned:
        {
            auto nft_info_opt = get_nft_info(_url);
            if (nft_info_opt != nullopt)
            {
                ostringstream oss;

                if (event_type == minted)
                    oss << nft_info_opt->mint_event.guid;
                else
                    oss << nft_info_opt->burn_event.guid;

                return oss.str();
            }
        }
        break;

        default:
            break;
        }

        return string();
    }

    template <typename T>
    template <typename EVENT>
    std::vector<EVENT> NonFungibleToken<T>::query_events(EventType event_type,
                                                         const violas::Address &address,
                                                         uint64_t start,
                                                         uint64_t limit)
    {
        std::vector<EVENT> events;

        using namespace json_rpc;
        auto rpc_cli = json_rpc::Client::create(_url);

        string key = get_event_handle(event_type, address);

        auto result = rpc_cli->get_events(key, start, limit);

        return events;
    }
}
