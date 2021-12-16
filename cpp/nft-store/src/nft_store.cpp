#include <tuple>
#include <violas_client2.hpp>
#include <utils.hpp>
#include "nft_store.hpp"

using namespace std;
using namespace violas;
const diem_types::AccountAddress NFT_STORE_ADMIN_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x11, 0x22};

namespace nft
{
    Store::Store(violas::client2_ptr client, const diem_types::TypeTag &nft) : _client(client), _nft_type_tag(nft)
    {
        auto [index, address] = _client->create_next_account(NFT_STORE_ADMIN_ADDRESS);
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

        // try_catch([=]()
        //           { _client->create_designated_dealer_ex("VLS", 0, admin.address, admin.auth_key,
        //                                                  "NFT Store admin", true); });
        _client->create_designated_dealer_ex("VLS",
                                             0,
                                             NFT_STORE_ADMIN_ADDRESS,
                                             admin.auth_key,
                                             "NFT Store administator",
                                             true);

        _client->create_parent_vasp_account(sale_parent.address, sale_parent.auth_key, "sales parent account");
        _client->create_parent_vasp_account(salge_agent_parent.address, salge_agent_parent.auth_key, "sales parent account");

        _client->execute_script_file(0, "move/stdlib/scripts/nft_store_initialize.mv", {}, {});

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
                                                         "move/stdlib/scripts/nft_store_accept.mv",
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
            "move/stdlib/scripts/nft_store_make_order.mv",
            {_nft_type_tag, make_struct_type_tag(STD_LIB_ADDRESS, currency, currency)},
            {make_txn_args(nft_id, price, uint64_t(MICRO_COIN * incentive), uint64_t(MICRO_COIN))});

        _client->check_txn_vm_status(sender, sn, "Store::register_nft");
    }

    std::vector<Order>
    Store::list_orders()
    {
        std::vector<Order> orders;

        // StructTag tag{
        //     Address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        //     "NonFungibleToken",
        //     "NFT",
        //     {StructTag{T::module_address(), T::module_name(), T::resource_name()}}};

        // violas::AccountState state(rpc_cli);

        try
        {
            auto state = _client->get_account_state(NFT_STORE_ADMIN_ADDRESS);
            return state.get_resource<std::vector<Order>>(make_struct_type_tag(VIOLAS_LIB_ADDRESS, "NftStore", "OrderList"));
        }
        catch (const std::exception &e)
        {
            std::cerr << color::RED << e.what() << color::RESET << endl;
        }        

        return {};
    }

    std::vector<MadeOrderEvent>
    list_made_order_events(Address address, uint64_t start, uint64_t limit)
    {
        std::vector<MadeOrderEvent> events;

        return events;
    }
}