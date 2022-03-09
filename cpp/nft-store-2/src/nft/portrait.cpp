#include "portrait.hpp"

namespace violas::nft
{
    void Portrait::mint(violas::client2_ptr client,
                        size_t account_index,
                        std::string_view description,
                        std::string_view ipfs_cid,
                        dt::AccountAddress receiver)
    {
        auto [sender, sn] = client->execute_script_file(account_index,
                                                        "move/build/scripts/mint_portrait_nft.mv",
                                                        {T::type_tag()},
                                                        make_txn_args(description, ipfs_cid, receiver));

        client->check_txn_vm_status(sender, sn, "Portrait::mint");
    }

}