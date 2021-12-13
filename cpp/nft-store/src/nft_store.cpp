#include <violas_sdk2.hpp>
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

    void Store::initalize(const violas::TypeTag &tag)
    {
        auto accounts = _client->get_all_accounts();
        auto &admin = accounts[0];

        try_catch([=]()
                  { _client->create_designated_dealer_ex("VLS", 0, admin.address, admin.auth_key,
                                                         "NFT Store admin", true); });

        _client->execute_script_file(0, "move/stdlib/scripts/nft_accept.mv", {tag}, {});

        _client->execute_script_file(0, "move/stdlib/scripts/nft_store_initialize.mv", {}, {});
    }

    void Store::register_nft(const violas::TypeTag &tag)
    {
        _client->execute_script_file(0, "move/stdlib/scripts/nft_store_register_nft.mv", {tag}, {});
    }
}