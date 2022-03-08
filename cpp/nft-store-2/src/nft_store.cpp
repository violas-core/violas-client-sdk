#include <fstream>
#include <tuple>
#include <iomanip>
#include <violas_client2.hpp>
#include <utils.hpp>
#include "nft_store.hpp"

using namespace std;
using namespace violas;

const diem_types::AccountAddress NFT_STORE_ADMIN_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x11, 0x22};

namespace violas::nft
{
    Store::Store(violas::client2_ptr client, const diem_types::TypeTag &nft) : _client(client), _nft_type_tag(nft)
    {
        auto [index, address] = _client->create_next_account(NFT_STORE_ADMIN_ADDRESS);
        cout << index << " : " << bytes_to_hex(address.value) << endl;

        tie(index, address) = _client->create_next_account();
        cout << index << " : " << bytes_to_hex(address.value) << endl;

        tie(index, address) = _client->create_next_account();
        cout << index << " : " << bytes_to_hex(address.value) << endl;

        tie(index, address) = _client->create_next_account();
        cout << index << " : " << bytes_to_hex(address.value) << endl;
    }

    Store::~Store()
    {
    }

    void Store::initialize()
    {
        auto accounts = _client->get_all_accounts();
        auto &admin = accounts[0];
        auto &sale_parent = accounts[1];
        auto &salge_agent_parent = accounts[2];
        auto &customer = accounts[3];

        // try_catch([=]()
        //           { _client->create_designated_dealer_ex("VLS", 0, admin.address, admin.auth_key,
        //                                                  "NFT Store admin", true); });
        _client->create_designated_dealer_ex("VLS",
                                             0,
                                             NFT_STORE_ADMIN_ADDRESS,
                                             admin.auth_key,
                                             "NFT Store administator",
                                             true);

        _client->create_parent_vasp_account(sale_parent.address, sale_parent.auth_key, "sales parent account", true);
        _client->create_parent_vasp_account(salge_agent_parent.address, salge_agent_parent.auth_key, "sales parent account", true);
        _client->create_child_vasp_account(2, customer.address, customer.auth_key, "VLS", 0, true);

        auto [sender, sn] = _client->execute_script_file(0, "move/build/scripts/nft_store_2_initialize.mv",
                                                         {},
                                                         make_txn_args(sale_parent.address));
        _client->check_txn_vm_status(sender, sn, "Store::initialize");

        
        this->register_account(1);
        this->register_account(2);
    }

    void Store::register_nft()
    {
        auto [sender, sn] = _client->execute_script_file(0,
                                                         "move/stdlib/scripts/nft_store_register_nft.mv",
                                                         {_nft_type_tag},
                                                         {});

        _client->check_txn_vm_status(sender, sn, "Store::register_nft");
    }

    void Store::register_account(size_t account_index)
    {
        auto [sender, sn] = _client->execute_script_file(account_index,
                                                         "move/build/scripts/nft_store_2_accept.mv",
                                                         {_nft_type_tag},
                                                         {});

        _client->check_txn_vm_status(sender, sn, "Store::register_account");
    }

    void Store::make_order(
        size_t account_index,
        Id nft_id,
        uint64_t price,
        std::string_view currency,
        double incentive)
    {
        auto [sender, sn] = _client->execute_script_file(
            account_index,
            "move/build/scripts/nft_store_2_make_order.mv",
            {_nft_type_tag, make_struct_type_tag(STD_LIB_ADDRESS, currency, currency)},
            {make_txn_args(nft_id, price, uint64_t(MICRO_COIN * incentive), uint64_t(MICRO_COIN))});

        _client->check_txn_vm_status(sender, sn, "Store::make_order");
    }

    void Store::revoke_order(size_t account_index,
                             Id order_id)
    {
        auto [sender, sn] = _client->execute_script_file(
            account_index,
            "move/build/scripts/nft_store_2_revoke_order.mv",
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
        auto state = _client->get_account_state(NFT_STORE_ADMIN_ADDRESS);

        auto opt_orders = state.get_resource<std::vector<Order>>(
            make_struct_tag(VIOLAS_LIB_ADDRESS,
                            "NftStore",
                            "OrderList",
                            {make_struct_type_tag(VIOLAS_LIB_ADDRESS,
                                                  "MountWuyi",
                                                  "Tea")}));
        if (opt_orders)
            return *opt_orders;
        else
            return {};
    }

    std::optional<AccountInfo>
    Store::get_account_info(Address address)
    {
        auto state = _client->get_account_state(dt::AccountAddress{address});

        auto opt_account_info = state.get_resource<AccountInfo>(
            make_struct_tag(VIOLAS_LIB_ADDRESS,
                            "NftStore",
                            "Account",
                            {make_struct_type_tag(VIOLAS_LIB_ADDRESS,
                                                  "MountWuyi",
                                                  "Tea")}));
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