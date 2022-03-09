#pragma once
#include <diem_types.hpp>
#include <violas_client2.hpp>
//#include "nft.hpp"

namespace violas::nft
{
    using bytes = std::vector<uint8_t>;
    namespace dt = diem_types;
    // namespace df = diem_framework;

    class Portrait
    {
    private:
        /* data */
        bytes description;
        bytes ipfs_cid;

        friend std::ostream &operator<<(std::ostream &os, const violas::nft::Portrait &portrait);

    public:
        Portrait(/* args */);
        ~Portrait();

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && description && ipfs_cid;
        }

        static dt::TypeTag type_tag()
        {
            // static dt::StructTag tag(module_address(), {module_name()}, {resource_name()}, {});
            // return {dt::TypeTag::Struct{tag}};

            return make_struct_type_tag({violas::VIOLAS_LIB_ADDRESS}, "Portrait", "Portrait");
        }

        static dt::AccountAddress module_address() { return {violas::VIOLAS_LIB_ADDRESS}; }
        static std::string module_name() { return "Portrait"; }
        static std::string resource_name() { return "Portrait"; }

        static void mint(violas::client2_ptr client,
                         size_t account_index,
                         std::string_view description,
                         std::string_view ipfs_cid,
                         dt::AccountAddress receiver);
    };

    Portrait::Portrait(/* args */)
    {
    }

    Portrait::~Portrait()
    {
    }

    void Portrait::mint(violas::client2_ptr client,
                        size_t account_index,
                        std::string_view description,
                        std::string_view ipfs_cid,
                        dt::AccountAddress receiver)
    {
        auto [sender, sn] = client->execute_script_file(account_index,
                                                        "move/build/scripts/mint_portrait_nft.mv",
                                                        {},
                                                        make_txn_args(description, ipfs_cid, receiver));

        client->check_txn_vm_status(sender, sn, "Portrait::mint");
    }

    std::ostream &operator<<(std::ostream &os, const violas::nft::Portrait &portrait)
    {
        os << bytes_to_string(portrait.description) << "\t"
           << bytes_to_string(portrait.ipfs_cid) << "\t"
           << bytes_to_hex(compute_token_id(portrait));

        return os;
    }
}
