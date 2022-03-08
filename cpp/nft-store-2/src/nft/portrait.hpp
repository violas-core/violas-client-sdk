#pragma once
#include <diem_types.hpp>

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

    public:
        Portrait(/* args */);
        ~Portrait();

        BcsSerde &serde(BcsSerde &bs)
        {
            return bs && description && ipfs_cid;
        }

        static dt::TypeTag type_tag()
        {
            //static dt::StructTag tag(module_address(), {module_name()}, {resource_name()}, {});            
            //return {dt::TypeTag::Struct{tag}};
            
            return make_struct_type_tag({violas::STD_LIB_ADDRESS}, "Portrait", "Portrait");
        }

        static dt::AccountAddress module_address() { return {violas::STD_LIB_ADDRESS}; }
        static std::string module_name() { return "Portrait"; }
        static std::string resource_name() { return "Portrait"; }
    };

    Portrait::Portrait(/* args */)
    {
    }

    Portrait::~Portrait()
    {
    }

}

std::ostream &operator<<(std::ostream &os, const violas::nft::Portrait &portrait)
{
    return os;
}