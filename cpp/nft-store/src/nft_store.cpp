#include <violas_client2.hpp>
#include <utils.hpp>
#include "nft_store.hpp"

using namespace violas;
const diem_types::AccountAddress NFT_STORE_ADMIN_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x11, 0x22};

namespace nft
{
    Store::Store(violas::client2_ptr client) : _client(client)
    {
        auto [index, address] = _client->create_next_account(NFT_STORE_ADMIN_ADDRESS);
        std::cout << index << " : " << bytes_to_hex(address.value) << std::endl;
    }

    Store::~Store()
    {
    }

    void Store::initialize(const diem_types::TypeTag &tag)
    {
        _client->create_next_account();
        _client->create_next_account();
        _client->create_next_account();

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

        _client->execute_script_file(0, "move/stdlib/scripts/nft_accept.mv", {}, {});
    }

    void Store::register_nft(const diem_types::TypeTag &tag)
    {
        _client->execute_script_file(0, "move/stdlib/scripts/nft_store_register_nft.mv", {}, {});
    }
}