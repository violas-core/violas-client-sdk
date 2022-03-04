#pragma once

#include <string>
#include <map>
#include <optional>
#include <diem_types.hpp>
#include "json_rpc.hpp"

namespace violas
{
    //using dt == diem_types;
    using Address = diem_types::AccountAddress;


    class AccountState2
    {
        std::map<std::vector<uint8_t>, std::vector<uint8_t>> _resources;
        json_rpc::client_ptr _client;

    public:
        AccountState2(const std::string &hex);

        AccountState2(json_rpc::client_ptr client) : _client(client) {}

        template <typename T>
        std::optional<T> get_resource(Address address, dt::StructTag tag)
        {
            get_account_state(address);

            dt::AccessPath path { tag };

            BcsSerde serde;
            //serde &&path;

            auto iter = _resources.find(serde.bytes());
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
        void get_account_state(Address address);
    };
}