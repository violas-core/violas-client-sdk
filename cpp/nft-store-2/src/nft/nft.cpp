//
//  NonFugibleToken template class implementation
//
#include <iostream>
#include <string>
#include <sstream>
#include <optional>
#include <cassert>

#include <violas_client2.hpp>
#include <utils.hpp>
#include <account_state_2.hpp>
#include <diem_types.hpp>
#include "nft.hpp"

using namespace std;
using namespace violas;
namespace dt = diem_types;

namespace violas::nft
{
    template <typename T>
    NonFungibleToken<T>::NonFungibleToken(client2_ptr client) : _client(client)
    {
    }

    // NonFungibleToken::~NonFungibleToken()
    // {
    // }

    template <typename T>
    void NonFungibleToken<T>::deploy()
    {
        _client->allow_publishing_module(true);
        _client->allow_custom_script(true);

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
                _client->publish_module(ACCOUNT_ROOT_ID, module);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    template <typename T>
    void NonFungibleToken<T>::register_instance(uint64_t total_number, Address admin)
    {
        //
        //  Rgiester NFT and set admin address
        //
        cout << "Registering NFT and set admin account address " << admin << endl;
        auto [sender, sn] = _client->execute_script_file(ACCOUNT_ROOT_ID,
                                                         "move/build/scripts/nft_register.mv",
                                                         {T::type_tag()},
                                                         make_txn_args(uint64_t(total_number),
                                                                       dt::AccountAddress{admin},
                                                                       T::module_name()));

        _client->check_txn_vm_status(sender, sn, "failed to execute nft_register");

        cout << "Register NFT successfully." << endl;
    }

    template <typename T>
    void NonFungibleToken<T>::burn(size_t account_index, TokenId token_id)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/build/scripts/nft_burn.mv",
                                                         {T::type_tag()},
                                                         make_txn_args(token_id));

        _client->check_txn_vm_status(sender, sn, "NonFungibleToken::burn");
    }

    template <typename T>
    void NonFungibleToken<T>::accept(size_t account_index)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/build/scripts/nft_accept.mv",
                                                         {T::type_tag()},
                                                         {});

        _client->check_txn_vm_status(sender, sn, "nft::accept");
    }

    template <typename T>
    void NonFungibleToken<T>::transfer_by_token_index(uint64_t account_index,
                                                      Address receiver,
                                                      uint64_t token_index,
                                                      std::vector<uint8_t> metadata)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/stdlib/scripts/nft_transfer_by_token_index.mv",
                                                         {T::type_tag()},
                                                         make_txn_args(dt::AccountAddress{receiver}, token_index, metadata));

        _client->check_txn_vm_status(sender, sn, "nft::transfer_by_token_index");
    }

    template <typename T>
    void NonFungibleToken<T>::transfer_by_token_id(uint64_t account_index,
                                                   Address receiver,
                                                   TokenId token_id,
                                                   std::vector<uint8_t> metadata)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/build/scripts/nft_transfer_by_token_id.mv",
                                                         {T::type_tag()},
                                                         make_txn_args(dt::AccountAddress{receiver}, token_id, metadata));
        _client->check_txn_vm_status(sender, sn, "nft::transfer_by_token_id");
    }

    template <typename T>
    std::optional<std::vector<T>> NonFungibleToken<T>::balance(const Address &addr)
    {
        dt::StructTag tag{
            {VIOLAS_LIB_ADDRESS},
            {"NonFungibleToken"},
            {"NFT"},
            {T::type_tag()}};

        auto state = _client->get_account_state({addr});

        try
        {
            return state->get_resource<std::vector<T>>(tag);
        }
        catch (const std::exception &e)
        {
            std::cerr << color::RED << e.what() << color::RESET << endl;
        }

        return {};
    }

    template <typename T>
    optional<NftInfo> NonFungibleToken<T>::get_nft_info()
    {
        auto state = _client->get_account_state(violas::ROOT_ADDRESS);

        dt::StructTag tag{
            {VIOLAS_LIB_ADDRESS},
            "NonFungibleToken",
            "Configuration",
            {T::type_tag()}};

        return state->get_resource<NftInfo>(tag);
    }

    template <typename T>
    optional<Address> NonFungibleToken<T>::get_owner(const TokenId &token_id)
    {
        auto opt_nft_info = get_nft_info();
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
    optional<NFT<T>> NonFungibleToken<T>::get_account(const Address &address)
    {
        auto state = _client->get_account_state({address});

        dt::StructTag tag{
            {VIOLAS_LIB_ADDRESS},
            {"NonFungibleToken"},
            {"NFT"},
            {T::type_tag()}};

        return state->get_resource<NFT<T>>(tag);
    }

    template <typename T>
    std::optional<EventHandle> NonFungibleToken<T>::get_event_handle(EventType event_type,
                                                                     const Address &address)
    {
        if (event_type == minted)
        {
            auto nft_info_opt = get_nft_info();
            if (nft_info_opt)
                return nft_info_opt->mint_event;
        }
        else if (event_type == burned)
        {
            auto nft_info_opt = get_nft_info();
            if (nft_info_opt)
                return nft_info_opt->burn_event;
        }
        else if (event_type == sent)
        {
            auto opt_account = get_account(address);
            if (opt_account)
                return opt_account->sent_event;
        }
        else
        {
            assert(event_type == received);
            auto opt_account = get_account(address);
            if (opt_account)
                return opt_account->received_event;
        }

        return {};
    }

    template <typename T>
    template <typename EVENT>
    std::vector<EVENT> NonFungibleToken<T>::query_events(const EventHandle &event_handle,
                                                         const Address &address,
                                                         uint64_t start,
                                                         uint64_t limit)
    {
        auto events = _client->query_events<EVENT>(event_handle, start, limit);

        return events;
    }
}

std::ostream &operator<<(ostream &os, const vector<violas::nft::MintedEvent> &minted_events)
{
    cout << color::YELLOW
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(40) << "Receiver Address"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : minted_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(40) << e.receiver
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::BurnedEvent> &burnedevents)
{
    cout << color::YELLOW
         << color::CYAN
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : burnedevents)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::SentEvent> &sent_events)
{
    cout << color::YELLOW
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(40) << "Payee"
         << left << setw(20) << "Metadata"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : sent_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(40) << e.payee
             << left << setw(20) << bytes_to_hex(e.metadata)
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<violas::nft::ReceivedEvent> &received_events)
{
    cout << color::YELLOW
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(40) << "Payer"
         << left << setw(20) << "Metadata"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : received_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(40) << e.payer
             << left << setw(20) << bytes_to_hex(e.metadata)
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const violas::nft::NftInfo &nft_info)
{
    os << "NonFungibleToken Info { \n\t"
       << "total : " << nft_info.total << "\n\t"
       << "amount : " << nft_info.amount << "\n\t"
       << "admin address: " << nft_info.admin << "\n\t"
       << "minted amount : " << nft_info.mint_event.counter << "\n\t"
       << "burned amount : "
       << nft_info.burn_event.counter << "\n"
       << "}";

    return os;
}