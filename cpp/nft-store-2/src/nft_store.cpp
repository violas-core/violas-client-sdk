#include <fstream>
#include <tuple>
#include <iomanip>
#include <violas_client2.hpp>
#include <utils.hpp>
#include "nft_store.hpp"

using namespace std;
using namespace violas;

namespace violas::nft
{
    const string script_path_prefix = "move/build/scripts/nft_store_2_";

    Store::Store(violas::client2_ptr client, const diem_types::TypeTag &nft) : _client(client), _nft_type_tag(nft)
    {
    }

    Store::~Store()
    {
    }

    void Store::initialize(size_t account_index)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/build/scripts/nft_store_2_initialize.mv",
                                                         {},
                                                         {});
        _client->check_txn_vm_status(sender, sn, "Store::initialize");
    }

    void Store::register_nft(size_t account_index)
    {
        auto [sender, sn] = _client->execute_script_file(0,
                                                         "move/build/scripts/nft_store_2_register_nft.mv",
                                                         {_nft_type_tag},
                                                         {});

        _client->check_txn_vm_status(sender, sn, "Store::register_nft");
    }

    void Store::accept_nft(size_t account_index)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/build/scripts/nft_store_2_accept.mv",
                                                         {_nft_type_tag},
                                                         {});

        _client->check_txn_vm_status(sender, sn, "Store::accept_nft");
    }

    void Store::make_order(
        size_t account_index,
        Id nft_id,
        uint64_t price,
        std::string_view currency)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         script_path_prefix + "make_order.mv",
                                                         {_nft_type_tag, make_struct_type_tag(STD_LIB_ADDRESS, currency, currency)},
                                                         {make_txn_args(nft_id, price)});

        _client->check_txn_vm_status(sender, sn, "Store::make_order");
    }

    void Store::revoke_order(size_t account_index,
                             Id order_id)
    {
        auto [sender, sn] = _client->execute_script_file(
            account_index,
            script_path_prefix + "revoke_order.mv",
            {_nft_type_tag},
            {make_txn_args(order_id)});

        _client->check_txn_vm_status(sender, sn, "Store::revoke_order");
    }

    void Store::trade_order(size_t account_index,
                            std::string_view currency,
                            Id order_id)
    {
        ifstream ifs("move/build/scripts/nft_store_2_trade_order.mv", ios::in | ios::binary);
        bytes script_bytecode(istreambuf_iterator<char>(ifs), {});
        dt::Script script{script_bytecode,
                          {_nft_type_tag, make_struct_type_tag(STD_LIB_ADDRESS, currency, currency)},
                          {make_txn_args(order_id)}};

        auto [sender, sn] = _client->execute_script_bytecode(account_index, script_bytecode,
                                                             {_nft_type_tag, make_struct_type_tag(STD_LIB_ADDRESS, currency, currency)},
                                                             {make_txn_args(order_id)});

        _client->check_txn_vm_status(sender, sn, "Store::submit_trading_order");
    }

    std::vector<Order>
    Store::list_orders()
    {
        auto state = _client->get_account_state({NFT_STORE_ADMIN_ADDRESS});
        if (!state)
            return {};

        auto opt_orders = state->get_resource<std::vector<Order>>(
            make_struct_tag({VIOLAS_LIB_ADDRESS},
                            "NftStore",
                            "OrderList",
                            {_nft_type_tag}));
        if (opt_orders)
            return *opt_orders;
        else
            return {};
    }

    std::optional<AccountInfo>
    Store::get_account_info(Address address)
    {
        auto state = _client->get_account_state(dt::AccountAddress{address});
        if (!state)
            return {};

        auto opt_account_info = state->get_resource<AccountInfo>(
            make_struct_tag(VIOLAS_LIB_ADDRESS,
                            "NftStore2",
                            "Account",
                            {_nft_type_tag}));
        if (opt_account_info)
            return *opt_account_info;
        else
            return {};
    }

    std::vector<MadeOrderEvent>
    Store::get_made_order_events(Address address, uint64_t start, uint64_t limit)
    {
        auto account_info = this->get_account_info(address);
        if (account_info)
        {
            return _client->query_events<MadeOrderEvent>(account_info->made_order_events, start, limit);
        }
        else
            return {};
    }
}

std::ostream &operator<<(std::ostream &os, const violas::nft::AccountInfo &account_info)
{
    os << "made order counter = " << account_info.made_order_events.counter << "\n"
       << "revoked order counter = " << account_info.revoked_order_events.counter << "\n"
       << "traded order counter = " << account_info.traded_order_events.counter;

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<nft::Order> &orders)
{
    // Print talbe header
    os << color::YELLOW
       << left << setw(8) << "index"
       << left << setw(66) << "NFT Token Id"
       << right << setw(10) << "Price"
       << left << setw(5) << ""
       << left << setw(16) << "Sale Incentive"
       << left << setw(34) << "Provider"
       << left << setw(20) << "Order Id"
       << color::RESET << endl;

    int i = 0;
    for (auto order : orders)
    {
        BcsSerde serde;

        order.serde(serde);
        auto bytes = serde.bytes();
        auto order_id = sha3_256(bytes.data(), bytes.size());

        os << left << setw(8) << i++
           << left << setw(66) << bytes_to_hex(order.nft_token_id)
           << right << setw(10) << order.price
           << left << setw(5) << bytes_to_string(order.currency_code)
           << left << setw(16) << order.sale_incentive
           << left << setw(34) << bytes_to_hex(order.provider)
           << left << setw(20) << bytes_to_hex(order_id)
           << endl;
    }

    return os;
}