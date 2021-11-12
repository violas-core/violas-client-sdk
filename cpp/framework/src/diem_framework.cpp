#include "diem_framework.hpp"

namespace diem_framework {


using namespace serde;
using namespace diem_types;

Script encode_add_currency_to_account_script(TypeTag currency) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 7, 7, 17, 25, 8, 42, 16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 1, 6, 12, 0, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 97, 100, 100, 95, 99, 117, 114, 114, 101, 110, 99, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 3, 11, 0, 56, 0, 2},
                std::vector<TypeTag> {std::move(currency)},
                std::vector<TransactionArgument> {},
            };
}
TransactionPayload encode_add_currency_to_account_script_function(TypeTag currency) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "add_currency_to_account" },
                    std::vector<TypeTag> {std::move(currency)},
                    std::vector<std::vector<uint8_t>> {},
                }
            };
}
Script encode_add_recovery_rotation_capability_script(AccountAddress recovery_address) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 2, 4, 4, 3, 8, 10, 5, 18, 15, 7, 33, 106, 8, 139, 1, 16, 0, 0, 0, 1, 0, 2, 1, 0, 0, 3, 0, 1, 0, 1, 4, 2, 3, 0, 1, 6, 12, 1, 8, 0, 2, 8, 0, 5, 0, 2, 6, 12, 5, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 15, 82, 101, 99, 111, 118, 101, 114, 121, 65, 100, 100, 114, 101, 115, 115, 21, 75, 101, 121, 82, 111, 116, 97, 116, 105, 111, 110, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 101, 120, 116, 114, 97, 99, 116, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 23, 97, 100, 100, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 4, 3, 5, 11, 0, 17, 0, 10, 1, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(recovery_address)}}},
            };
}
TransactionPayload encode_add_recovery_rotation_capability_script_function(AccountAddress recovery_address) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "add_recovery_rotation_capability" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {recovery_address.bcsSerialize()},
                }
            };
}
Script encode_add_validator_and_reconfigure_script(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 6, 3, 6, 15, 5, 21, 24, 7, 45, 91, 8, 136, 1, 16, 0, 0, 0, 1, 0, 2, 1, 3, 0, 1, 0, 2, 4, 2, 3, 0, 0, 5, 4, 1, 0, 2, 6, 12, 3, 0, 1, 5, 1, 10, 2, 2, 6, 12, 5, 4, 6, 12, 3, 10, 2, 5, 2, 1, 3, 10, 68, 105, 101, 109, 83, 121, 115, 116, 101, 109, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 15, 86, 97, 108, 105, 100, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 14, 103, 101, 116, 95, 104, 117, 109, 97, 110, 95, 110, 97, 109, 101, 13, 97, 100, 100, 95, 118, 97, 108, 105, 100, 97, 116, 111, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 6, 18, 10, 0, 10, 1, 17, 0, 10, 3, 17, 1, 11, 2, 33, 12, 4, 11, 4, 3, 14, 11, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 39, 11, 0, 10, 3, 17, 2, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U8Vector {std::move(validator_name)}}, {TransactionArgument::Address {std::move(validator_address)}}},
            };
}
TransactionPayload encode_add_validator_and_reconfigure_script_function(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "ValidatorAdministrationScripts" } },
                    Identifier { "add_validator_and_reconfigure" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(validator_name, s);
            std::move(s).bytes();
            }), validator_address.bcsSerialize()},
                }
            };
}
TransactionPayload encode_add_vasp_domain_script_function(AccountAddress address, std::vector<uint8_t> domain) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "add_vasp_domain" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(domain, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_burn_script(TypeTag token, uint64_t sliding_nonce, AccountAddress preburn_address) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 17, 7, 34, 45, 8, 79, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 1, 1, 1, 4, 2, 6, 12, 3, 0, 2, 6, 12, 5, 3, 6, 12, 3, 5, 1, 9, 0, 4, 68, 105, 101, 109, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 4, 98, 117, 114, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 1, 7, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 56, 0, 2},
                std::vector<TypeTag> {std::move(token)},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(preburn_address)}}},
            };
}
Script encode_burn_txn_fees_script(TypeTag coin_type) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 7, 7, 17, 25, 8, 42, 16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 1, 6, 12, 0, 1, 9, 0, 14, 84, 114, 97, 110, 115, 97, 99, 116, 105, 111, 110, 70, 101, 101, 9, 98, 117, 114, 110, 95, 102, 101, 101, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 3, 11, 0, 56, 0, 2},
                std::vector<TypeTag> {std::move(coin_type)},
                std::vector<TransactionArgument> {},
            };
}
TransactionPayload encode_burn_txn_fees_script_function(TypeTag coin_type) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "burn_txn_fees" },
                    std::vector<TypeTag> {std::move(coin_type)},
                    std::vector<std::vector<uint8_t>> {},
                }
            };
}
TransactionPayload encode_burn_with_amount_script_function(TypeTag token, uint64_t sliding_nonce, AccountAddress preburn_address, uint64_t amount) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "burn_with_amount" },
                    std::vector<TypeTag> {std::move(token)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), preburn_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(amount, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_cancel_burn_script(TypeTag token, AccountAddress preburn_address) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 8, 7, 18, 24, 8, 42, 16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 2, 6, 12, 5, 0, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 11, 99, 97, 110, 99, 101, 108, 95, 98, 117, 114, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 4, 11, 0, 10, 1, 56, 0, 2},
                std::vector<TypeTag> {std::move(token)},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(preburn_address)}}},
            };
}
TransactionPayload encode_cancel_burn_with_amount_script_function(TypeTag token, AccountAddress preburn_address, uint64_t amount) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "cancel_burn_with_amount" },
                    std::vector<TypeTag> {std::move(token)},
                    std::vector<std::vector<uint8_t>> {preburn_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(amount, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_create_child_vasp_account_script(TypeTag coin_type, AccountAddress child_address, std::vector<uint8_t> auth_key_prefix, bool add_all_currencies, uint64_t child_initial_balance) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 8, 1, 0, 2, 2, 2, 4, 3, 6, 22, 4, 28, 4, 5, 32, 35, 7, 67, 122, 8, 189, 1, 16, 6, 205, 1, 4, 0, 0, 0, 1, 1, 0, 0, 2, 0, 1, 1, 1, 0, 3, 2, 3, 0, 0, 4, 4, 1, 1, 1, 0, 5, 3, 1, 0, 0, 6, 2, 6, 4, 6, 12, 5, 10, 2, 1, 0, 1, 6, 12, 1, 8, 0, 5, 6, 8, 0, 5, 3, 10, 2, 10, 2, 5, 6, 12, 5, 10, 2, 1, 3, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 18, 87, 105, 116, 104, 100, 114, 97, 119, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 25, 99, 114, 101, 97, 116, 101, 95, 99, 104, 105, 108, 100, 95, 118, 97, 115, 112, 95, 97, 99, 99, 111, 117, 110, 116, 27, 101, 120, 116, 114, 97, 99, 116, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 8, 112, 97, 121, 95, 102, 114, 111, 109, 27, 114, 101, 115, 116, 111, 114, 101, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 2, 1, 0, 1, 1, 5, 3, 25, 10, 0, 10, 1, 11, 2, 10, 3, 56, 0, 10, 4, 6, 0, 0, 0, 0, 0, 0, 0, 0, 36, 3, 10, 5, 22, 11, 0, 17, 1, 12, 5, 14, 5, 10, 1, 10, 4, 7, 0, 7, 0, 56, 1, 11, 5, 17, 3, 5, 24, 11, 0, 1, 2},
                std::vector<TypeTag> {std::move(coin_type)},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(child_address)}}, {TransactionArgument::U8Vector {std::move(auth_key_prefix)}}, {TransactionArgument::Bool {add_all_currencies} }, {TransactionArgument::U64 {child_initial_balance} }},
            };
}
TransactionPayload encode_create_child_vasp_account_script_function(TypeTag coin_type, AccountAddress child_address, std::vector<uint8_t> auth_key_prefix, bool add_all_currencies, uint64_t child_initial_balance) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountCreationScripts" } },
                    Identifier { "create_child_vasp_account" },
                    std::vector<TypeTag> {std::move(coin_type)},
                    std::vector<std::vector<uint8_t>> {child_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(auth_key_prefix, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<bool>::serialize(add_all_currencies, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(child_initial_balance, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_create_designated_dealer_script(TypeTag currency, uint64_t sliding_nonce, AccountAddress addr, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 27, 7, 44, 72, 8, 116, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 1, 1, 1, 4, 2, 6, 12, 3, 0, 5, 6, 12, 5, 10, 2, 10, 2, 1, 6, 6, 12, 3, 5, 10, 2, 10, 2, 1, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 24, 99, 114, 101, 97, 116, 101, 95, 100, 101, 115, 105, 103, 110, 97, 116, 101, 100, 95, 100, 101, 97, 108, 101, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 1, 10, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 11, 3, 11, 4, 10, 5, 56, 0, 2},
                std::vector<TypeTag> {std::move(currency)},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(addr)}}, {TransactionArgument::U8Vector {std::move(auth_key_prefix)}}, {TransactionArgument::U8Vector {std::move(human_name)}}, {TransactionArgument::Bool {add_all_currencies} }},
            };
}
TransactionPayload encode_create_designated_dealer_script_function(TypeTag currency, uint64_t sliding_nonce, AccountAddress addr, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountCreationScripts" } },
                    Identifier { "create_designated_dealer" },
                    std::vector<TypeTag> {std::move(currency)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), addr.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(auth_key_prefix, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(human_name, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<bool>::serialize(add_all_currencies, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_create_parent_vasp_account_script(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 27, 7, 44, 74, 8, 118, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 1, 1, 1, 4, 2, 6, 12, 3, 0, 5, 6, 12, 5, 10, 2, 10, 2, 1, 6, 6, 12, 3, 5, 10, 2, 10, 2, 1, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 26, 99, 114, 101, 97, 116, 101, 95, 112, 97, 114, 101, 110, 116, 95, 118, 97, 115, 112, 95, 97, 99, 99, 111, 117, 110, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 1, 10, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 11, 3, 11, 4, 10, 5, 56, 0, 2},
                std::vector<TypeTag> {std::move(coin_type)},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(new_account_address)}}, {TransactionArgument::U8Vector {std::move(auth_key_prefix)}}, {TransactionArgument::U8Vector {std::move(human_name)}}, {TransactionArgument::Bool {add_all_currencies} }},
            };
}
TransactionPayload encode_create_parent_vasp_account_script_function(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountCreationScripts" } },
                    Identifier { "create_parent_vasp_account" },
                    std::vector<TypeTag> {std::move(coin_type)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), new_account_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(auth_key_prefix, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(human_name, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<bool>::serialize(add_all_currencies, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_create_recovery_address_script() {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 2, 4, 4, 3, 8, 10, 5, 18, 12, 7, 30, 90, 8, 120, 16, 0, 0, 0, 1, 0, 2, 1, 0, 0, 3, 0, 1, 0, 1, 4, 2, 3, 0, 1, 6, 12, 1, 8, 0, 2, 6, 12, 8, 0, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 15, 82, 101, 99, 111, 118, 101, 114, 121, 65, 100, 100, 114, 101, 115, 115, 21, 75, 101, 121, 82, 111, 116, 97, 116, 105, 111, 110, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 101, 120, 116, 114, 97, 99, 116, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 7, 112, 117, 98, 108, 105, 115, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3, 5, 10, 0, 11, 0, 17, 0, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {},
            };
}
TransactionPayload encode_create_recovery_address_script_function() {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "create_recovery_address" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {},
                }
            };
}
Script encode_create_validator_account_script(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 22, 7, 36, 72, 8, 108, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 0, 2, 6, 12, 3, 0, 4, 6, 12, 5, 10, 2, 10, 2, 5, 6, 12, 3, 5, 10, 2, 10, 2, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 24, 99, 114, 101, 97, 116, 101, 95, 118, 97, 108, 105, 100, 97, 116, 111, 114, 95, 97, 99, 99, 111, 117, 110, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 9, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 11, 3, 11, 4, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(new_account_address)}}, {TransactionArgument::U8Vector {std::move(auth_key_prefix)}}, {TransactionArgument::U8Vector {std::move(human_name)}}},
            };
}
TransactionPayload encode_create_validator_account_script_function(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountCreationScripts" } },
                    Identifier { "create_validator_account" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), new_account_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(auth_key_prefix, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(human_name, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_create_validator_operator_account_script(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 22, 7, 36, 81, 8, 117, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 0, 2, 6, 12, 3, 0, 4, 6, 12, 5, 10, 2, 10, 2, 5, 6, 12, 3, 5, 10, 2, 10, 2, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 33, 99, 114, 101, 97, 116, 101, 95, 118, 97, 108, 105, 100, 97, 116, 111, 114, 95, 111, 112, 101, 114, 97, 116, 111, 114, 95, 97, 99, 99, 111, 117, 110, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 9, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 11, 3, 11, 4, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(new_account_address)}}, {TransactionArgument::U8Vector {std::move(auth_key_prefix)}}, {TransactionArgument::U8Vector {std::move(human_name)}}},
            };
}
TransactionPayload encode_create_validator_operator_account_script_function(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountCreationScripts" } },
                    Identifier { "create_validator_operator_account" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), new_account_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(auth_key_prefix, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(human_name, s);
            std::move(s).bytes();
            })},
                }
            };
}
TransactionPayload encode_create_vasp_domains_script_function() {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "create_vasp_domains" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {},
                }
            };
}
Script encode_freeze_account_script(uint64_t sliding_nonce, AccountAddress to_freeze_account) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 14, 7, 28, 66, 8, 94, 16, 0, 0, 0, 1, 0, 2, 0, 1, 0, 1, 3, 2, 1, 0, 2, 6, 12, 5, 0, 2, 6, 12, 3, 3, 6, 12, 3, 5, 15, 65, 99, 99, 111, 117, 110, 116, 70, 114, 101, 101, 122, 105, 110, 103, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 14, 102, 114, 101, 101, 122, 101, 95, 97, 99, 99, 111, 117, 110, 116, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 7, 10, 0, 10, 1, 17, 1, 11, 0, 10, 2, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(to_freeze_account)}}},
            };
}
TransactionPayload encode_freeze_account_script_function(uint64_t sliding_nonce, AccountAddress to_freeze_account) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "freeze_account" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), to_freeze_account.bcsSerialize()},
                }
            };
}
TransactionPayload encode_initialize_diem_consensus_config_script_function(uint64_t sliding_nonce) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "SystemAdministrationScripts" } },
                    Identifier { "initialize_diem_consensus_config" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            })},
                }
            };
}
TransactionPayload encode_peer_to_peer_by_signers_script_function(TypeTag currency, uint64_t amount, std::vector<uint8_t> metadata) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "PaymentScripts" } },
                    Identifier { "peer_to_peer_by_signers" },
                    std::vector<TypeTag> {std::move(currency)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(amount, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(metadata, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_peer_to_peer_with_metadata_script(TypeTag currency, AccountAddress payee, uint64_t amount, std::vector<uint8_t> metadata, std::vector<uint8_t> metadata_signature) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 7, 1, 0, 2, 2, 2, 4, 3, 6, 16, 4, 22, 2, 5, 24, 29, 7, 53, 96, 8, 149, 1, 16, 0, 0, 0, 1, 1, 0, 0, 2, 0, 1, 0, 0, 3, 2, 3, 1, 1, 0, 4, 1, 3, 0, 1, 5, 1, 6, 12, 1, 8, 0, 5, 6, 8, 0, 5, 3, 10, 2, 10, 2, 0, 5, 6, 12, 5, 3, 10, 2, 10, 2, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 18, 87, 105, 116, 104, 100, 114, 97, 119, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 27, 101, 120, 116, 114, 97, 99, 116, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 8, 112, 97, 121, 95, 102, 114, 111, 109, 27, 114, 101, 115, 116, 111, 114, 101, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 12, 11, 0, 17, 0, 12, 5, 14, 5, 10, 1, 10, 2, 11, 3, 11, 4, 56, 0, 11, 5, 17, 2, 2},
                std::vector<TypeTag> {std::move(currency)},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(payee)}}, {TransactionArgument::U64 {amount} }, {TransactionArgument::U8Vector {std::move(metadata)}}, {TransactionArgument::U8Vector {std::move(metadata_signature)}}},
            };
}
TransactionPayload encode_peer_to_peer_with_metadata_script_function(TypeTag currency, AccountAddress payee, uint64_t amount, std::vector<uint8_t> metadata, std::vector<uint8_t> metadata_signature) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "PaymentScripts" } },
                    Identifier { "peer_to_peer_with_metadata" },
                    std::vector<TypeTag> {std::move(currency)},
                    std::vector<std::vector<uint8_t>> {payee.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(amount, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(metadata, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(metadata_signature, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_preburn_script(TypeTag token, uint64_t amount) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 7, 1, 0, 2, 2, 2, 4, 3, 6, 16, 4, 22, 2, 5, 24, 21, 7, 45, 95, 8, 140, 1, 16, 0, 0, 0, 1, 1, 0, 0, 2, 0, 1, 0, 0, 3, 2, 3, 1, 1, 0, 4, 1, 3, 0, 1, 5, 1, 6, 12, 1, 8, 0, 3, 6, 12, 6, 8, 0, 3, 0, 2, 6, 12, 3, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 18, 87, 105, 116, 104, 100, 114, 97, 119, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 27, 101, 120, 116, 114, 97, 99, 116, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 7, 112, 114, 101, 98, 117, 114, 110, 27, 114, 101, 115, 116, 111, 114, 101, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 10, 10, 0, 17, 0, 12, 2, 11, 0, 14, 2, 10, 1, 56, 0, 11, 2, 17, 2, 2},
                std::vector<TypeTag> {std::move(token)},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {amount} }},
            };
}
TransactionPayload encode_preburn_script_function(TypeTag token, uint64_t amount) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "preburn" },
                    std::vector<TypeTag> {std::move(token)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(amount, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_publish_shared_ed25519_public_key_script(std::vector<uint8_t> public_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 6, 7, 13, 31, 8, 44, 16, 0, 0, 0, 1, 0, 1, 0, 2, 6, 12, 10, 2, 0, 22, 83, 104, 97, 114, 101, 100, 69, 100, 50, 53, 53, 49, 57, 80, 117, 98, 108, 105, 99, 75, 101, 121, 7, 112, 117, 98, 108, 105, 115, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 11, 0, 11, 1, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U8Vector {std::move(public_key)}}},
            };
}
TransactionPayload encode_publish_shared_ed25519_public_key_script_function(std::vector<uint8_t> public_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "publish_shared_ed25519_public_key" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(public_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_register_validator_config_script(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 11, 7, 18, 27, 8, 45, 16, 0, 0, 0, 1, 0, 1, 0, 5, 6, 12, 5, 10, 2, 10, 2, 10, 2, 0, 15, 86, 97, 108, 105, 100, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 10, 115, 101, 116, 95, 99, 111, 110, 102, 105, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 7, 11, 0, 10, 1, 11, 2, 11, 3, 11, 4, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(validator_account)}}, {TransactionArgument::U8Vector {std::move(consensus_pubkey)}}, {TransactionArgument::U8Vector {std::move(validator_network_addresses)}}, {TransactionArgument::U8Vector {std::move(fullnode_network_addresses)}}},
            };
}
TransactionPayload encode_register_validator_config_script_function(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "ValidatorAdministrationScripts" } },
                    Identifier { "register_validator_config" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {validator_account.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(consensus_pubkey, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(validator_network_addresses, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(fullnode_network_addresses, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_remove_validator_and_reconfigure_script(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 6, 3, 6, 15, 5, 21, 24, 7, 45, 94, 8, 139, 1, 16, 0, 0, 0, 1, 0, 2, 1, 3, 0, 1, 0, 2, 4, 2, 3, 0, 0, 5, 4, 1, 0, 2, 6, 12, 3, 0, 1, 5, 1, 10, 2, 2, 6, 12, 5, 4, 6, 12, 3, 10, 2, 5, 2, 1, 3, 10, 68, 105, 101, 109, 83, 121, 115, 116, 101, 109, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 15, 86, 97, 108, 105, 100, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 14, 103, 101, 116, 95, 104, 117, 109, 97, 110, 95, 110, 97, 109, 101, 16, 114, 101, 109, 111, 118, 101, 95, 118, 97, 108, 105, 100, 97, 116, 111, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 6, 18, 10, 0, 10, 1, 17, 0, 10, 3, 17, 1, 11, 2, 33, 12, 4, 11, 4, 3, 14, 11, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 39, 11, 0, 10, 3, 17, 2, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U8Vector {std::move(validator_name)}}, {TransactionArgument::Address {std::move(validator_address)}}},
            };
}
TransactionPayload encode_remove_validator_and_reconfigure_script_function(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "ValidatorAdministrationScripts" } },
                    Identifier { "remove_validator_and_reconfigure" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(validator_name, s);
            std::move(s).bytes();
            }), validator_address.bcsSerialize()},
                }
            };
}
TransactionPayload encode_remove_vasp_domain_script_function(AccountAddress address, std::vector<uint8_t> domain) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "remove_vasp_domain" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(domain, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_rotate_authentication_key_script(std::vector<uint8_t> new_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 2, 2, 4, 3, 6, 15, 5, 21, 18, 7, 39, 124, 8, 163, 1, 16, 0, 0, 0, 1, 1, 0, 0, 2, 0, 1, 0, 0, 3, 1, 2, 0, 0, 4, 3, 2, 0, 1, 6, 12, 1, 8, 0, 0, 2, 6, 8, 0, 10, 2, 2, 6, 12, 10, 2, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 21, 75, 101, 121, 82, 111, 116, 97, 116, 105, 111, 110, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 101, 120, 116, 114, 97, 99, 116, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 114, 101, 115, 116, 111, 114, 101, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 25, 114, 111, 116, 97, 116, 101, 95, 97, 117, 116, 104, 101, 110, 116, 105, 99, 97, 116, 105, 111, 110, 95, 107, 101, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 4, 1, 9, 11, 0, 17, 0, 12, 2, 14, 2, 11, 1, 17, 2, 11, 2, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U8Vector {std::move(new_key)}}},
            };
}
TransactionPayload encode_rotate_authentication_key_script_function(std::vector<uint8_t> new_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "rotate_authentication_key" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(new_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_rotate_authentication_key_with_nonce_script(uint64_t sliding_nonce, std::vector<uint8_t> new_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 2, 4, 4, 3, 8, 20, 5, 28, 23, 7, 51, 159, 1, 8, 210, 1, 16, 0, 0, 0, 1, 0, 3, 1, 0, 1, 2, 0, 1, 0, 0, 4, 2, 3, 0, 0, 5, 3, 1, 0, 0, 6, 4, 1, 0, 2, 6, 12, 3, 0, 1, 6, 12, 1, 8, 0, 2, 6, 8, 0, 10, 2, 3, 6, 12, 3, 10, 2, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 21, 75, 101, 121, 82, 111, 116, 97, 116, 105, 111, 110, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 101, 120, 116, 114, 97, 99, 116, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 114, 101, 115, 116, 111, 114, 101, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 25, 114, 111, 116, 97, 116, 101, 95, 97, 117, 116, 104, 101, 110, 116, 105, 99, 97, 116, 105, 111, 110, 95, 107, 101, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 3, 12, 10, 0, 10, 1, 17, 0, 11, 0, 17, 1, 12, 3, 14, 3, 11, 2, 17, 3, 11, 3, 17, 2, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U8Vector {std::move(new_key)}}},
            };
}
TransactionPayload encode_rotate_authentication_key_with_nonce_script_function(uint64_t sliding_nonce, std::vector<uint8_t> new_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "rotate_authentication_key_with_nonce" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(new_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_rotate_authentication_key_with_nonce_admin_script(uint64_t sliding_nonce, std::vector<uint8_t> new_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 2, 4, 4, 3, 8, 20, 5, 28, 25, 7, 53, 159, 1, 8, 212, 1, 16, 0, 0, 0, 1, 0, 3, 1, 0, 1, 2, 0, 1, 0, 0, 4, 2, 3, 0, 0, 5, 3, 1, 0, 0, 6, 4, 1, 0, 2, 6, 12, 3, 0, 1, 6, 12, 1, 8, 0, 2, 6, 8, 0, 10, 2, 4, 6, 12, 6, 12, 3, 10, 2, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 21, 75, 101, 121, 82, 111, 116, 97, 116, 105, 111, 110, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 101, 120, 116, 114, 97, 99, 116, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 31, 114, 101, 115, 116, 111, 114, 101, 95, 107, 101, 121, 95, 114, 111, 116, 97, 116, 105, 111, 110, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 25, 114, 111, 116, 97, 116, 101, 95, 97, 117, 116, 104, 101, 110, 116, 105, 99, 97, 116, 105, 111, 110, 95, 107, 101, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 3, 12, 11, 0, 10, 2, 17, 0, 11, 1, 17, 1, 12, 4, 14, 4, 11, 3, 17, 3, 11, 4, 17, 2, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U8Vector {std::move(new_key)}}},
            };
}
TransactionPayload encode_rotate_authentication_key_with_nonce_admin_script_function(uint64_t sliding_nonce, std::vector<uint8_t> new_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "rotate_authentication_key_with_nonce_admin" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(new_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_rotate_authentication_key_with_recovery_address_script(AccountAddress recovery_address, AccountAddress to_recover, std::vector<uint8_t> new_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 8, 7, 15, 42, 8, 57, 16, 0, 0, 0, 1, 0, 1, 0, 4, 6, 12, 5, 5, 10, 2, 0, 15, 82, 101, 99, 111, 118, 101, 114, 121, 65, 100, 100, 114, 101, 115, 115, 25, 114, 111, 116, 97, 116, 101, 95, 97, 117, 116, 104, 101, 110, 116, 105, 99, 97, 116, 105, 111, 110, 95, 107, 101, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 6, 11, 0, 10, 1, 10, 2, 11, 3, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(recovery_address)}}, {TransactionArgument::Address {std::move(to_recover)}}, {TransactionArgument::U8Vector {std::move(new_key)}}},
            };
}
TransactionPayload encode_rotate_authentication_key_with_recovery_address_script_function(AccountAddress recovery_address, AccountAddress to_recover, std::vector<uint8_t> new_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "rotate_authentication_key_with_recovery_address" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {recovery_address.bcsSerialize(), to_recover.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(new_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_rotate_dual_attestation_info_script(std::vector<uint8_t> new_url, std::vector<uint8_t> new_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 10, 5, 12, 13, 7, 25, 61, 8, 86, 16, 0, 0, 0, 1, 0, 1, 0, 0, 2, 0, 1, 0, 2, 6, 12, 10, 2, 0, 3, 6, 12, 10, 2, 10, 2, 15, 68, 117, 97, 108, 65, 116, 116, 101, 115, 116, 97, 116, 105, 111, 110, 15, 114, 111, 116, 97, 116, 101, 95, 98, 97, 115, 101, 95, 117, 114, 108, 28, 114, 111, 116, 97, 116, 101, 95, 99, 111, 109, 112, 108, 105, 97, 110, 99, 101, 95, 112, 117, 98, 108, 105, 99, 95, 107, 101, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 7, 10, 0, 11, 1, 17, 0, 11, 0, 11, 2, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U8Vector {std::move(new_url)}}, {TransactionArgument::U8Vector {std::move(new_key)}}},
            };
}
TransactionPayload encode_rotate_dual_attestation_info_script_function(std::vector<uint8_t> new_url, std::vector<uint8_t> new_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "rotate_dual_attestation_info" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(new_url, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(new_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_rotate_shared_ed25519_public_key_script(std::vector<uint8_t> public_key) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 6, 7, 13, 34, 8, 47, 16, 0, 0, 0, 1, 0, 1, 0, 2, 6, 12, 10, 2, 0, 22, 83, 104, 97, 114, 101, 100, 69, 100, 50, 53, 53, 49, 57, 80, 117, 98, 108, 105, 99, 75, 101, 121, 10, 114, 111, 116, 97, 116, 101, 95, 107, 101, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 11, 0, 11, 1, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U8Vector {std::move(public_key)}}},
            };
}
TransactionPayload encode_rotate_shared_ed25519_public_key_script_function(std::vector<uint8_t> public_key) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "AccountAdministrationScripts" } },
                    Identifier { "rotate_shared_ed25519_public_key" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(public_key, s);
            std::move(s).bytes();
            })},
                }
            };
}
TransactionPayload encode_set_gas_constants_script_function(uint64_t sliding_nonce, uint64_t global_memory_per_byte_cost, uint64_t global_memory_per_byte_write_cost, uint64_t min_transaction_gas_units, uint64_t large_transaction_cutoff, uint64_t intrinsic_gas_per_byte, uint64_t maximum_number_of_gas_units, uint64_t min_price_per_gas_unit, uint64_t max_price_per_gas_unit, uint64_t max_transaction_size_in_bytes, uint64_t gas_unit_scaling_factor, uint64_t default_account_size) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "SystemAdministrationScripts" } },
                    Identifier { "set_gas_constants" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(global_memory_per_byte_cost, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(global_memory_per_byte_write_cost, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(min_transaction_gas_units, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(large_transaction_cutoff, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(intrinsic_gas_per_byte, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(maximum_number_of_gas_units, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(min_price_per_gas_unit, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(max_price_per_gas_unit, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(max_transaction_size_in_bytes, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(gas_unit_scaling_factor, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(default_account_size, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_set_validator_config_and_reconfigure_script(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 15, 7, 29, 68, 8, 97, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 0, 5, 6, 12, 5, 10, 2, 10, 2, 10, 2, 0, 2, 6, 12, 5, 10, 68, 105, 101, 109, 83, 121, 115, 116, 101, 109, 15, 86, 97, 108, 105, 100, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 10, 115, 101, 116, 95, 99, 111, 110, 102, 105, 103, 29, 117, 112, 100, 97, 116, 101, 95, 99, 111, 110, 102, 105, 103, 95, 97, 110, 100, 95, 114, 101, 99, 111, 110, 102, 105, 103, 117, 114, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 10, 10, 0, 10, 1, 11, 2, 11, 3, 11, 4, 17, 0, 11, 0, 10, 1, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::Address {std::move(validator_account)}}, {TransactionArgument::U8Vector {std::move(consensus_pubkey)}}, {TransactionArgument::U8Vector {std::move(validator_network_addresses)}}, {TransactionArgument::U8Vector {std::move(fullnode_network_addresses)}}},
            };
}
TransactionPayload encode_set_validator_config_and_reconfigure_script_function(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "ValidatorAdministrationScripts" } },
                    Identifier { "set_validator_config_and_reconfigure" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {validator_account.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(consensus_pubkey, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(validator_network_addresses, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(fullnode_network_addresses, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_set_validator_operator_script(std::vector<uint8_t> operator_name, AccountAddress operator_account) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 19, 7, 33, 68, 8, 101, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 3, 0, 1, 5, 1, 10, 2, 2, 6, 12, 5, 0, 3, 6, 12, 10, 2, 5, 2, 1, 3, 15, 86, 97, 108, 105, 100, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 23, 86, 97, 108, 105, 100, 97, 116, 111, 114, 79, 112, 101, 114, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 14, 103, 101, 116, 95, 104, 117, 109, 97, 110, 95, 110, 97, 109, 101, 12, 115, 101, 116, 95, 111, 112, 101, 114, 97, 116, 111, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 4, 5, 15, 10, 2, 17, 0, 11, 1, 33, 12, 3, 11, 3, 3, 11, 11, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 39, 11, 0, 10, 2, 17, 1, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U8Vector {std::move(operator_name)}}, {TransactionArgument::Address {std::move(operator_account)}}},
            };
}
TransactionPayload encode_set_validator_operator_script_function(std::vector<uint8_t> operator_name, AccountAddress operator_account) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "ValidatorAdministrationScripts" } },
                    Identifier { "set_validator_operator" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(operator_name, s);
            std::move(s).bytes();
            }), operator_account.bcsSerialize()},
                }
            };
}
Script encode_set_validator_operator_with_nonce_admin_script(uint64_t sliding_nonce, std::vector<uint8_t> operator_name, AccountAddress operator_account) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 6, 3, 6, 15, 5, 21, 26, 7, 47, 103, 8, 150, 1, 16, 0, 0, 0, 1, 0, 2, 0, 3, 0, 1, 0, 2, 4, 2, 3, 0, 1, 5, 4, 1, 0, 2, 6, 12, 3, 0, 1, 5, 1, 10, 2, 2, 6, 12, 5, 5, 6, 12, 6, 12, 3, 10, 2, 5, 2, 1, 3, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 15, 86, 97, 108, 105, 100, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 23, 86, 97, 108, 105, 100, 97, 116, 111, 114, 79, 112, 101, 114, 97, 116, 111, 114, 67, 111, 110, 102, 105, 103, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 14, 103, 101, 116, 95, 104, 117, 109, 97, 110, 95, 110, 97, 109, 101, 12, 115, 101, 116, 95, 111, 112, 101, 114, 97, 116, 111, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 6, 18, 11, 0, 10, 2, 17, 0, 10, 4, 17, 1, 11, 3, 33, 12, 5, 11, 5, 3, 14, 11, 1, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 39, 11, 1, 10, 4, 17, 2, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U8Vector {std::move(operator_name)}}, {TransactionArgument::Address {std::move(operator_account)}}},
            };
}
TransactionPayload encode_set_validator_operator_with_nonce_admin_script_function(uint64_t sliding_nonce, std::vector<uint8_t> operator_name, AccountAddress operator_account) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "ValidatorAdministrationScripts" } },
                    Identifier { "set_validator_operator_with_nonce_admin" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(operator_name, s);
            std::move(s).bytes();
            }), operator_account.bcsSerialize()},
                }
            };
}
Script encode_tiered_mint_script(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress designated_dealer_address, uint64_t mint_amount, uint64_t tier_index) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 21, 7, 38, 59, 8, 97, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 1, 1, 1, 4, 2, 6, 12, 3, 0, 4, 6, 12, 5, 3, 3, 5, 6, 12, 3, 5, 3, 3, 1, 9, 0, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 11, 116, 105, 101, 114, 101, 100, 95, 109, 105, 110, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 1, 9, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 10, 3, 10, 4, 56, 0, 2},
                std::vector<TypeTag> {std::move(coin_type)},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(designated_dealer_address)}}, {TransactionArgument::U64 {mint_amount} }, {TransactionArgument::U64 {tier_index} }},
            };
}
TransactionPayload encode_tiered_mint_script_function(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress designated_dealer_address, uint64_t mint_amount, uint64_t tier_index) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "tiered_mint" },
                    std::vector<TypeTag> {std::move(coin_type)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), designated_dealer_address.bcsSerialize(), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(mint_amount, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(tier_index, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_unfreeze_account_script(uint64_t sliding_nonce, AccountAddress to_unfreeze_account) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 14, 7, 28, 68, 8, 96, 16, 0, 0, 0, 1, 0, 2, 0, 1, 0, 1, 3, 2, 1, 0, 2, 6, 12, 5, 0, 2, 6, 12, 3, 3, 6, 12, 3, 5, 15, 65, 99, 99, 111, 117, 110, 116, 70, 114, 101, 101, 122, 105, 110, 103, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 16, 117, 110, 102, 114, 101, 101, 122, 101, 95, 97, 99, 99, 111, 117, 110, 116, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 7, 10, 0, 10, 1, 17, 1, 11, 0, 10, 2, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::Address {std::move(to_unfreeze_account)}}},
            };
}
TransactionPayload encode_unfreeze_account_script_function(uint64_t sliding_nonce, AccountAddress to_unfreeze_account) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "unfreeze_account" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), to_unfreeze_account.bcsSerialize()},
                }
            };
}
TransactionPayload encode_update_diem_consensus_config_script_function(uint64_t sliding_nonce, std::vector<uint8_t> config) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "SystemAdministrationScripts" } },
                    Identifier { "update_diem_consensus_config" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<std::vector<uint8_t>>::serialize(config, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_update_diem_version_script(uint64_t sliding_nonce, uint64_t major) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 10, 7, 24, 51, 8, 75, 16, 0, 0, 0, 1, 0, 2, 0, 1, 0, 1, 3, 0, 1, 0, 2, 6, 12, 3, 0, 3, 6, 12, 3, 3, 11, 68, 105, 101, 109, 86, 101, 114, 115, 105, 111, 110, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 3, 115, 101, 116, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 7, 10, 0, 10, 1, 17, 1, 11, 0, 10, 2, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U64 {major} }},
            };
}
TransactionPayload encode_update_diem_version_script_function(uint64_t sliding_nonce, uint64_t major) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "SystemAdministrationScripts" } },
                    Identifier { "update_diem_version" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(major, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_update_dual_attestation_limit_script(uint64_t sliding_nonce, uint64_t new_micro_xdx_limit) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 4, 3, 4, 10, 5, 14, 10, 7, 24, 71, 8, 95, 16, 0, 0, 0, 1, 0, 2, 0, 1, 0, 1, 3, 0, 1, 0, 2, 6, 12, 3, 0, 3, 6, 12, 3, 3, 15, 68, 117, 97, 108, 65, 116, 116, 101, 115, 116, 97, 116, 105, 111, 110, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 19, 115, 101, 116, 95, 109, 105, 99, 114, 111, 100, 105, 101, 109, 95, 108, 105, 109, 105, 116, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 7, 10, 0, 10, 1, 17, 1, 11, 0, 10, 2, 17, 0, 2},
                std::vector<TypeTag> {},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U64 {new_micro_xdx_limit} }},
            };
}
TransactionPayload encode_update_dual_attestation_limit_script_function(uint64_t sliding_nonce, uint64_t new_micro_xdx_limit) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "update_dual_attestation_limit" },
                    std::vector<TypeTag> {},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(new_micro_xdx_limit, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_update_exchange_rate_script(TypeTag currency, uint64_t sliding_nonce, uint64_t new_exchange_rate_numerator, uint64_t new_exchange_rate_denominator) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 7, 1, 0, 6, 2, 6, 4, 3, 10, 16, 4, 26, 2, 5, 28, 25, 7, 53, 99, 8, 152, 1, 16, 0, 0, 0, 1, 0, 2, 1, 1, 2, 0, 1, 3, 0, 1, 0, 2, 4, 2, 3, 0, 0, 5, 4, 3, 1, 1, 2, 6, 2, 3, 3, 1, 8, 0, 2, 6, 12, 3, 0, 2, 6, 12, 8, 0, 4, 6, 12, 3, 3, 3, 1, 9, 0, 4, 68, 105, 101, 109, 12, 70, 105, 120, 101, 100, 80, 111, 105, 110, 116, 51, 50, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 20, 99, 114, 101, 97, 116, 101, 95, 102, 114, 111, 109, 95, 114, 97, 116, 105, 111, 110, 97, 108, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 24, 117, 112, 100, 97, 116, 101, 95, 120, 100, 120, 95, 101, 120, 99, 104, 97, 110, 103, 101, 95, 114, 97, 116, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 5, 1, 11, 10, 0, 10, 1, 17, 1, 10, 2, 10, 3, 17, 0, 12, 4, 11, 0, 11, 4, 56, 0, 2},
                std::vector<TypeTag> {std::move(currency)},
                std::vector<TransactionArgument> {{TransactionArgument::U64 {sliding_nonce} }, {TransactionArgument::U64 {new_exchange_rate_numerator} }, {TransactionArgument::U64 {new_exchange_rate_denominator} }},
            };
}
TransactionPayload encode_update_exchange_rate_script_function(TypeTag currency, uint64_t sliding_nonce, uint64_t new_exchange_rate_numerator, uint64_t new_exchange_rate_denominator) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "update_exchange_rate" },
                    std::vector<TypeTag> {std::move(currency)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(sliding_nonce, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(new_exchange_rate_numerator, s);
            std::move(s).bytes();
            }), ({
            auto s = BcsSerializer();
            Serializable<uint64_t>::serialize(new_exchange_rate_denominator, s);
            std::move(s).bytes();
            })},
                }
            };
}
Script encode_update_minting_ability_script(TypeTag currency, bool allow_minting) {
    return Script {
                std::vector<uint8_t> {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 8, 7, 18, 28, 8, 46, 16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 2, 6, 12, 1, 0, 1, 9, 0, 4, 68, 105, 101, 109, 22, 117, 112, 100, 97, 116, 101, 95, 109, 105, 110, 116, 105, 110, 103, 95, 97, 98, 105, 108, 105, 116, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 4, 11, 0, 10, 1, 56, 0, 2},
                std::vector<TypeTag> {std::move(currency)},
                std::vector<TransactionArgument> {{TransactionArgument::Bool {allow_minting} }},
            };
}
TransactionPayload encode_update_minting_ability_script_function(TypeTag currency, bool allow_minting) {
    return TransactionPayload {
                TransactionPayload::ScriptFunction {
                    ModuleId { std::array<uint8_t, 16>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, Identifier { "TreasuryComplianceScripts" } },
                    Identifier { "update_minting_ability" },
                    std::vector<TypeTag> {std::move(currency)},
                    std::vector<std::vector<uint8_t>> {({
            auto s = BcsSerializer();
            Serializable<bool>::serialize(allow_minting, s);
            std::move(s).bytes();
            })},
                }
            };
}

} // end of namespace diem_framework
