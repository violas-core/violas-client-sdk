//
//  NonFugibleToken template class implementation
//
#include <iostream>
#include <string>
#include <sstream>
#include <optional>
#include <client.hpp>
#include <cassert>
#include "utils.hpp"
#include "nft.hpp"

using namespace std;

namespace violas::nft
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

        try
        {
            _client->create_parent_vasp_account("VLS", 0, admin.address, admin.auth_key, "NFT VASP", "", admin.pub_key, true);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }        

        //
        //  Rgiester NFT and set admin address
        //
        cout << "Registering NFT and set admin account address " << admin.address << endl;
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
    void NonFungibleToken<T>::transfer_by_token_index(uint64_t account_index,
                                                      Address receiver,
                                                      uint64_t token_index,
                                                      std::vector<uint8_t> metadata)
    {
        _client->execute_script_file(account_index,
                                     "move/stdlib/scripts/nft_transfer_by_token_index.mv",
                                     {T::type_tag()},
                                     {receiver, token_index, metadata});
    }

    template <typename T>
    void NonFungibleToken<T>::transfer_by_token_id(uint64_t account_index,
                                                   Address receiver,
                                                   TokenId token_id,
                                                   std::vector<uint8_t> metadata)
    {
        _client->execute_script_file(account_index,
                                     "move/stdlib/scripts/nft_transfer_by_token_id.mv",
                                     {T::type_tag()},
                                     {receiver, std::vector<uint8_t>(begin(token_id), end(token_id)), metadata});
    }

    template <typename T>
    std::optional<std::vector<T>> NonFungibleToken<T>::balance(const Address &addr)
    {
        using namespace json_rpc;

        auto rpc_cli = json_rpc::Client::create(_url);

        StructTag tag{
            Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
            "NonFungibleToken",
            "NFT",
            {StructTag{T::module_address(), T::module_name(), T::resource_name()}}};

        violas::AccountState state(rpc_cli);

        try
        {
            return state.get_resource<std::vector<T>>(addr, tag);
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
            "Configuration",
            {violas::StructTag{type_tag.address, type_tag.module_name, type_tag.resource_name}}};

        return state.get_resource<NftInfo>(VIOLAS_ROOT_ADDRESS, tag);
    }

    template <typename T>
    optional<Address> NonFungibleToken<T>::get_owner(string url, const TokenId &token_id)
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
    optional<Account> NonFungibleToken<T>::get_account(const violas::Address &address)
    {
        using namespace json_rpc;
        auto rpc_cli = json_rpc::Client::create(_url);

        violas::AccountState state(rpc_cli);

        auto type_tag = T::type_tag();

        violas::StructTag tag{
            Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
            "NonFungibleToken",
            "Account",
            {violas::StructTag{type_tag.address, type_tag.module_name, type_tag.resource_name}}};

        return state.get_resource<Account>(address, tag);
    }

    template <typename T>
    std::optional<EventHandle> NonFungibleToken<T>::get_event_handle(EventType event_type,
                                                                     const violas::Address &address)
    {
        // switch (event_type)
        // {
        // case minted:
        // case burned:
        // {
        //     auto nft_info_opt = get_nft_info(_url);
        //     if (nft_info_opt != nullopt)
        //     {
        //         ostringstream oss;

        //         if (event_type == minted)
        //             oss << nft_info_opt->mint_event.guid;
        //         else
        //             oss << nft_info_opt->burn_event.guid;

        //         return oss.str();
        //     }
        // }
        // break;
        // case sent:
        // case received:
        // {
        //     auto opt_account = get_account(address);
        //     if (opt_account != nullopt)
        //     {
        //         ostringstream oss;

        //         if (event_type == sent)
        //             oss << opt_account->sent_event.guid;
        //         else
        //             oss << opt_account->received_event.guid;

        //         return oss.str();
        //     }
        // }
        // break;

        // default:
        //     break;
        // }

        if (event_type == minted)
        {
            auto nft_info_opt = get_nft_info(_url);
            if (nft_info_opt != nullopt)
            {
                return nft_info_opt->mint_event;
            }
        }
        else if (event_type == burned)
        {
            auto nft_info_opt = get_nft_info(_url);            
            if (nft_info_opt != nullopt)
            {
                return nft_info_opt->burn_event;
            }
        }
        else if (event_type == sent)
        {
            auto opt_account = get_account(address);
            if (opt_account != nullopt)
            {
                return opt_account->sent_event;
            }
        }
        else
        {
            assert(event_type == received);
            auto opt_account = get_account(address);
            if (opt_account != nullopt)
            {
                return opt_account->received_event;
            }
        }

        return {};
    }

    template <typename T>
    template <typename EVENT>
    std::vector<EVENT> NonFungibleToken<T>::query_events(const EventHandle &event_handle,
                                                         const violas::Address &address,
                                                         uint64_t start,
                                                         uint64_t limit)
    {
        std::vector<EVENT> nft_events;

        using namespace json_rpc;
        auto rpc_cli = json_rpc::Client::create(_url);       

        auto events = rpc_cli->get_events(bytes_to_hex(event_handle.guid), start, limit);

        for (auto &e : events)
        {
            EVENT nft_event;
            BcsSerde serde(std::get<UnknownEvent>(e.event).bytes);

            serde &&nft_event;

            nft_event.sequence_number = e.sequence_number;
            nft_event.transaction_version = e.transaction_version;

            nft_events.emplace_back(nft_event);
        }

        return nft_events;
    }
}
