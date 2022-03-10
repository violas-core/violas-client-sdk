#pragma once

#include <string>
#include <map>
#include <optional>
#include <variant>
#include <diem_types.hpp>
#include "json_rpc.hpp"

namespace violas1
{
    namespace dt = diem_types;    

    struct ResourcePath
    {
        std::variant<dt::ModuleId, dt::StructTag> path;

        ResourcePath(const dt::StructTag &tag)
        {
            path = tag;
        }

        ResourcePath(const dt::ModuleId &module_id)
        {
            path = module_id;
        }

        inline std::vector<uint8_t> bcsSerialize() const
        {
            auto serializer = serde::BcsSerializer();
            serde::Serializable<ResourcePath>::serialize(*this, serializer);
            return std::move(serializer).bytes();
        }
    };

    class AccountState2
    {
        std::map<std::vector<uint8_t>, std::vector<uint8_t>> _resources;
        json_rpc::client_ptr _client;

    public:
        AccountState2(const std::string &hex);

        AccountState2(json_rpc::client_ptr client) : _client(client) {}

        template <typename T>
        std::optional<T> get_resource(dt::AccountAddress address, dt::StructTag tag)
        {
            get_account_state(address);

            ResourcePath path{tag};

            auto iter = _resources.find(path.bcsSerialize());
            if (iter != end(_resources))
            {
                T t;

                BcsSerde serde(iter->second);

                serde &&t;

                return t;
            }
            else
                return {};
        }

    protected:
        void get_account_state(dt::AccountAddress address)
        {
            using namespace std;

            ostringstream oss;
            oss << address.value;

            auto as = _client->get_account_state_blob(oss.str());

            if (as.blob.empty())
                __throw_runtime_error("Failed to get account state due to blob is null.");

            // cout << "blob = " <<  as.blob << endl;

            auto bytes = hex_to_bytes(as.blob);

            // Deserialize to vector
            vector<uint8_t> data;
            {
                BcsSerde serde(bytes);
                serde &&data;
            }

            // Deserialize to map
            BcsSerde serde(data);

            serde &&_resources;
        }
    };
}

template <>
template <typename Serializer>
void serde::Serializable<violas::ResourcePath>::serialize(const violas::ResourcePath &obj, Serializer &serializer)
{
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.path)>::serialize(obj.path, serializer);
    serializer.decrease_container_depth();
}