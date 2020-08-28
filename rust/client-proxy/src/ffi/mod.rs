use crate::{violas_account, violas_client::ViolasClient, AccountAddress, AccountStatus};
use anyhow::{format_err, Error};
use cpp::cpp;
use libra_types::{
    chain_id::ChainId, transaction::authenticator::AuthenticationKey, waypoint::Waypoint,
};
use move_core_types::language_storage::TypeTag;
use std::{
    cell::RefCell,
    ffi::{CStr, CString},
    os::raw::{c_char, c_uchar},
    slice,
    str::FromStr,
};

const ADDRESS_LENGTH: usize = 16;
const AUTH_KEY_LENGTH: usize = 32;

thread_local! {
    static LAST_ERROR: RefCell<String> = RefCell::new(String::new());
}

fn set_last_error(err: Error) {
    LAST_ERROR.with(|prev| {
        *prev.borrow_mut() = format!("{:?}", err);
    });
}

fn get_last_error() -> *const c_char {
    LAST_ERROR.with(|prev| {
        let err = prev.borrow_mut();
        CString::new(err.clone()).unwrap().into_raw()
    })
}

fn free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe {
            CString::from_raw(s);
        }
    }
}

// for Rust
#[repr(C)]
pub struct RustTypeTag {
    address: [c_uchar; ADDRESS_LENGTH],
    module_name: *const c_char,
    resource_name: *const c_char,
}

fn make_currency_tag(currency_code: *const c_char) -> TypeTag {
    unsafe {
        let code = CStr::from_ptr(currency_code).to_str().unwrap();
        violas_account::make_currency_tag(code)
    }
}

fn make_type_tag(rust_type_tag: &RustTypeTag) -> TypeTag {
    unsafe {
        violas_account::make_type_tag(
            &AccountAddress::new(rust_type_tag.address),
            CStr::from_ptr(rust_type_tag.module_name).to_str().unwrap(),
            CStr::from_ptr(rust_type_tag.resource_name)
                .to_str()
                .unwrap(),
        )
    }
}

cpp! {{

#include <sstream>
#include <iomanip>
#include <string>
#include "client.hpp"

using namespace std;

namespace violas
{
    // currency tag for C
    struct CTypeTag {
        uint8_t address[ADDRESS_LENGTH];
        const char* module_name;
        const char* resource_name;

        CTypeTag(const TypeTag & currency_tag)
        {
            copy(begin(currency_tag.address), end(currency_tag.address), address);
            module_name = currency_tag.module_name.data();
            resource_name = currency_tag.resource_name.data();
        }
    };

    template <size_t N>
    std::ostream &operator<<(std::ostream &os, const array<uint8_t, N> &bytes)
    {
        for (auto v : bytes)
        {
            os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
        }

        return os << std::dec;
    }

    class ClientImp : public Client
    {
    private:
        //raw rust ViolasClient pointer
        void * rust_violas_client;

        void check_result(bool ret)
        {
            if( !ret )
            {
                const char* last_error = rust!(Client_get_last_error [] -> *const c_char as "const char*" {
                    get_last_error()
                });

                string error(last_error);
                rust!( client_free_string [last_error : *mut c_char as "const char*"] {
                    free_string(last_error);
                });

                throw runtime_error(error);
            }
        }

    public:
        ClientImp(  uint8_t chain_id,
                    std::string_view url,
                    std::string_view mint_key,
                    std::string_view mnemonic,
                    std::string_view waypoint)
        {
            auto c_url = url.data();
            auto c_mint_key = mint_key.data();
            auto c_mnemonic = mnemonic.data();
            auto c_waypoint = waypoint.data();

            this->rust_violas_client = rust!(Client_constructor [
                    chain_id : u8 as "uint8_t",
                    c_url : * const c_char as "const char *",
                    c_mint_key : * const c_char as "const char *",
                    c_mnemonic : * const c_char as "const char *",
                    c_waypoint : * const c_char as "const char *"]-> *mut ViolasClient as "void *" {
                let client = ViolasClient::new(
                                                ChainId::new(chain_id),
                                                CStr::from_ptr(c_url).to_str().unwrap(),
                                                CStr::from_ptr(c_mint_key).to_str().unwrap() ,
                                                CStr::from_ptr(c_mint_key).to_str().unwrap(),
                                                true,
                                                None,
                                                Some(CStr::from_ptr(c_mnemonic).to_str().unwrap().to_owned()),
                                                Waypoint::from_str(CStr::from_ptr(c_waypoint).to_str().unwrap()).unwrap()).unwrap();
                Box::into_raw(Box::new(client))
            });
        }

        virtual ~ClientImp()
        {
            rust!(Client_destructor [rust_violas_client : * mut ViolasClient as "void *"] {
                let _client = unsafe {
                    Box::from_raw(rust_violas_client);
                };
            });
        }

        virtual void
        test_connection() override
        {
            bool ret = rust!(Client_test_connection [rust_violas_client : &mut ViolasClient as "void *"] -> bool as "bool" {
                let result = rust_violas_client.test_validator_connection();
                match result {
                    Ok(_) => true,
                    Err(_) => false,
                }
            });

            check_result(ret);
        }

        virtual AddressAndIndex
        create_next_account(const std::optional<Address> &address = std::nullopt) override
        {
            AddressAndIndex addr_index;
            bool is_addr_available = address != std::nullopt;
            auto in_address = is_addr_available ?(*address).data() :  nullptr;
            auto out_index = &addr_index.index;
            auto out_addr = &addr_index.address[0];

            bool ret = rust!(Client_create_next_account [
                                        rust_violas_client : &mut ViolasClient as "void *",
                                        is_addr_available : bool as "bool",
                                        in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                                        out_index : &mut usize as "size_t *",
                                        out_addr : &mut [c_uchar; ADDRESS_LENGTH] as "uint8_t *"
                                        ] -> bool as "bool" {
                let result = rust_violas_client.create_next_account(
                    match is_addr_available {
                    true => Some(AccountAddress::new(*in_address)),
                    false => None
                    },
                    true, );
                match result {
                    Ok((address, index)) => {
                        out_addr.copy_from_slice(&address.as_ref());
                        *out_index = index;
                        true
                    },
                    Err(e) => {
                        set_last_error(e);
                        false
                    }
                }
            });

            check_result(ret);

            return addr_index;
        }

        virtual std::vector<Account>
        get_all_accounts() override
        {
            size_t accounts_size = rust!(client_get_accounts_size[
                                        rust_violas_client : &mut ViolasClient as "void *"]
                                        -> usize as "size_t" {
                rust_violas_client.accounts.len()
            });

            std::vector<Account> accounts(accounts_size, Account());

            for(size_t i = 0; i<accounts.size(); i++)
            {
                auto & account = accounts[i];

                auto out_address = &account.address[0];
                auto out_auth_key = &account.auth_key[0];
                auto out_pubkey = account.pub_key.data();
                auto out_sequence_num = &account.sequence_number;
                auto out_status = &account.status;

                rust!(client_get_account[
                                        rust_violas_client : &mut ViolasClient as "void *",
                                        i : usize as "size_t",
                                        out_address : &mut [c_uchar; ADDRESS_LENGTH] as "uint8_t *",
                                        out_auth_key : &mut [c_uchar; ADDRESS_LENGTH * 2] as "uint8_t *",
                                        out_pubkey : &mut [c_uchar; ADDRESS_LENGTH * 2] as "uint8_t *",
                                        out_sequence_num : &mut u64 as "uint64_t *",
                                        out_status : &mut AccountStatus as "AccountStatus *"
                                        ] {
                    let account = & rust_violas_client.accounts[i];

                    out_address.copy_from_slice(&account.address.as_ref());
                    out_auth_key.copy_from_slice(&account.authentication_key.as_ref().unwrap());
                    if account.key_pair.is_some() {
                        out_pubkey.copy_from_slice(&account.key_pair.as_ref().unwrap().public_key.to_bytes());
                    }
                    *out_sequence_num = account.sequence_number;
                    *out_status = account.status.clone();
                });
            }

            return accounts;
        }

        virtual void
        create_validator_account( std::string_view currency_code,
                                const AuthenticationKey &auth_key,
                                std::string_view human_name) override
        {
            auto in_currency_code = currency_code.data();
            auto in_auth_key = auth_key.data();
            auto in_human_name = human_name.data();

            bool ret = rust!( client_create_testnet_account [
                            rust_violas_client : &mut ViolasClient as "void *",
                            in_currency_code : *const c_char as "const char *",
                            in_auth_key : &[u8;AUTH_KEY_LENGTH] as "const uint8_t *",
                            in_human_name : *const c_char as "const char *"
                            ] -> bool as "bool" {
                                let auth_key = AuthenticationKey::new(*in_auth_key);

                                let ret = rust_violas_client.create_validator_account(
                                                0,
                                                auth_key.derived_address(),
                                                auth_key.prefix().to_vec(),
                                                CStr::from_ptr(in_human_name).to_str().unwrap().as_bytes().to_owned(),
                                                true);
                                match ret {
                                    Ok(_) => true,
                                    Err(e) => {
                                        let err = format_err!("ffi::create_testnet_account, {}",e);
                                        set_last_error(err);
                                        false
                                    }
                                }
                        });

            check_result(ret);
        }

        virtual void
        mint_for_testnet(   std::string_view currency_code,
                            const Address &receiver_address,
                            uint64_t amount) override
        {
            auto in_currency_code = currency_code.data();
            auto in_receiver_address = receiver_address.data();

            bool ret = rust!( client_mint_for_testnet [
                                rust_violas_client : &mut ViolasClient as "void *",
                                in_currency_code : *const c_char as "const char *",
                                in_receiver_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                                amount : u64 as "uint64_t"
                            ] -> bool as "bool" {

                                let ret = rust_violas_client.mint_for_testnet(
                                                make_currency_tag(in_currency_code),
                                                AccountAddress::new(*in_receiver_address),
                                                amount,
                                                true);
                                match ret {
                                    Ok(_) => true,
                                    Err(e) => {
                                        let err = format_err!("ffi::create_testnet_account, {}",e);
                                        set_last_error(err);
                                        false
                                    }
                                }
                        });

            check_result(ret);
        }

        virtual void
        transfer(size_t sender_account_ref_id,
                const Address &receiver_address,
                std::string_view currency_code,
                uint64_t amount,
                uint64_t gas_unit_price,
                uint64_t max_gas_amount,
                std::string_view gas_currency_code) override
        {
            auto in_currency_tag = currency_code.data();
            auto in_gas_currency_tag = gas_currency_code.data();
            auto in_receiver_address = receiver_address.data();

            bool ret = rust!(
                client_transfer_currency [
                    rust_violas_client : &mut ViolasClient as "void *",
                    sender_account_ref_id : usize as "size_t",
                    in_receiver_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                    in_currency_tag : *const c_char as "const char *",
                    amount : u64 as "uint64_t",
                    gas_unit_price : u64 as "uint64_t",
                    max_gas_amount : u64 as "uint64_t",
                    in_gas_currency_tag : *const c_char as "const char *"
                ] -> bool as "bool" {
                    let ret = rust_violas_client.transfer_currency(
                            sender_account_ref_id,
                            &AccountAddress::new(*in_receiver_address),
                            make_currency_tag(in_currency_tag),
                            amount,
                            Some(gas_unit_price),
                            Some(max_gas_amount),
                            Some(make_currency_tag(in_gas_currency_tag)),
                            true);

                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::transfer, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
                }
            );

            check_result(ret);
        }

        //
        //  Modify VM publishing option
        //
        virtual void
        allow_custom_script() override
        {
            bool ret = rust!( client_allow_custom_script
                [
                    rust_violas_client : &mut ViolasClient as "void *"
                ] -> bool as "bool" {

                        let ret = rust_violas_client.allow_custom_script(true);
                        match ret {
                            Ok(_) => true,
                            Err(e) => {
                                let err = format_err!("ffi::allow_custom_script, {}",e);
                                set_last_error(err);
                                false
                            }
                        }
                });

            check_result(ret);
        }

        //
        //  Allow  to publish custom module
        //  note that calling method needs violas root privilege
        virtual void
        allow_publishing_module(bool enabled) override
        {
            bool ret = rust!( client_allow_publishing_module
                [
                    rust_violas_client : &mut ViolasClient as "void *",
                    enabled : bool as "bool"
                ] -> bool as "bool" {

                        let ret = rust_violas_client.allow_publishing_module(enabled, true);
                        match ret {
                            Ok(_) => true,
                            Err(e) => {
                                let err = format_err!("ffi::allow_publishing_module, {}",e);
                                set_last_error(err);
                                false
                            }
                        }
                });

            check_result(ret);
        }

        //
        //  publish a module file
        //  if account_index is ASSOCIATION_ID then publish module with association root account
        virtual void
        publish_module(size_t account_index,
                       std::string_view module_file_name) override
        {
            auto in_module_file_name = module_file_name.data();

            bool ret = rust!( client_publish_module [
                rust_violas_client : &mut ViolasClient as "void *",
                account_index : usize as "size_t",
                in_module_file_name : *const c_char as "const char *"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.publish_module(
                                account_index,
                                CStr::from_ptr(in_module_file_name).to_str().unwrap());
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::publish_module, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }
        //
        //  Execute script file with specified arguments
        //
        virtual void
        execute_script(size_t account_index,
                       std::string_view script_file_name,
                       const std::vector<TypeTag> &type_tags,
                       const std::vector<TransactionAugment> & arguments) override
        {
            auto in_script_file_name = script_file_name.data();

            vector<CTypeTag> c_type_tags;
            for(const auto & type_tag : type_tags)
            {
                c_type_tags.push_back(CTypeTag(type_tag));
            }

            vector<string> str_args;
            for(const auto & arg : arguments)
            {
                std::visit([&](auto&& var) {
                    using T = std::decay_t<decltype(var)>;
                    if constexpr (std::is_same_v<T,  vector<uint8_t> >)
                    {
                        ostringstream oss;

                        oss << "b\"";
                        for (uint8_t byte : var)
                        {
                            oss << hex << setw(2) << setfill('0') << (uint32_t)byte;
                        }
                        oss << "\"";
                        str_args.push_back(oss.str());
                    }
                    else if constexpr (std::is_same_v<T, Address>)
                    {
                        ostringstream oss;
                        oss << var;

                        str_args.push_back(oss.str());
                    }
                    else if constexpr (std::is_same_v<T, __uint128_t>)
                    {
                        auto str128 = to_string((uint64_t)(var >> 64)) + to_string((uint64_t)var);
                        str_args.push_back(str128);
                    }
                    else
                    {
                        str_args.push_back(to_string(var));
                    }
                }, arg);
            }
            vector<const char*> args;
            for(const auto & arg : str_args)
                args.push_back(arg.data());

            auto in_c_type_tags = c_type_tags.data();
            auto in_c_type_tags_len = c_type_tags.size();
            auto in_args = args.data();
            auto in_args_len = args.size();

            bool ret = rust!( client_execute_script [
                rust_violas_client : &mut ViolasClient as "void *",
                account_index : usize as "size_t",
                in_script_file_name : *const c_char as "const char *",
                in_c_type_tags : *const RustTypeTag as "const CTypeTag *",
                in_c_type_tags_len : usize as "size_t",
                in_args : *const *const c_char as "const char * *",
                in_args_len : usize as "size_t"
                ] -> bool as "bool" {

                    let type_tags : Vec<TypeTag> = slice::from_raw_parts(in_c_type_tags, in_c_type_tags_len)
                                                            .iter()
                                                            .map(|x| make_type_tag(x))
                                                            .collect();

                    let args : Vec<&str> = slice::from_raw_parts(in_args, in_args_len)
                                        .iter()
                                        .map( |x| CStr::from_ptr(*x).to_str().unwrap() )
                                        .collect();

                    let ret = rust_violas_client.execute_script_ex(
                                account_index as u64,
                                CStr::from_ptr(in_script_file_name).to_str().unwrap(),
                                type_tags,
                                &args);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::publish_module, {}",e);
                            set_last_error(err);
                            false
                        }
                    }

            });

            check_result(ret);
        }

        /// Add a currency to current account
        virtual void
        add_currency(size_t account_index,
                     std::string_view currency_code) override
        {
            auto in_currency_code = currency_code.data();

            bool ret = rust!( client_add_currency [
                                rust_violas_client : &mut ViolasClient as "void *",
                                account_index : usize as "size_t",
                                in_currency_code : *const c_char as "const char *"
                            ] -> bool as "bool" {

                                let ret = rust_violas_client.add_currency(
                                    account_index,
                                    make_currency_tag(in_currency_code),
                                    true);
                                match ret {
                                    Ok(_) => true,
                                    Err(e) => {
                                        let err = format_err!("ffi::add_currency, {}",e);
                                        set_last_error(err);
                                        false
                                    }
                                }
                        });

            check_result(ret);
        }

        // Call this method with root association privilege
        virtual void
        publish_curency(std::string_view currency_code) override
        {
            auto in_currency_code = currency_code.data();

            bool ret = rust!( client_publish_currency [
                                rust_violas_client : &mut ViolasClient as "void *",
                                in_currency_code : *const c_char as "const char *"
                            ] -> bool as "bool" {

                                let ret = rust_violas_client.publish_currency(
                                    CStr::from_ptr(in_currency_code).to_str().unwrap()
                                    );
                                match ret {
                                    Ok(_) => true,
                                    Err(e) => {
                                        let err = format_err!("ffi::publish_curency, {}",e);
                                        set_last_error(err);
                                        false
                                    }
                                }
                        });

            check_result(ret);
        }

        // Register currency with association root account
        virtual void
        register_currency(std::string_view currency_code,
                           uint64_t exchange_rate_denom,
                           uint64_t exchange_rate_num,
                           bool is_synthetic,
                           uint64_t scaling_factor,
                           uint64_t fractional_part) override
        {
            auto in_currency_code = currency_code.data();

            bool ret = rust!(
                client_register_currency [
                    rust_violas_client : &mut ViolasClient as "void *",
                    in_currency_code : *const c_char as "const char *",
                    exchange_rate_denom : u64 as "uint64_t",
                    exchange_rate_num : u64 as "uint64_t",
                    is_synthetic : bool as "bool",
                    scaling_factor : u64 as "uint64_t",
                    fractional_part : u64 as "uint64_t"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.register_currency(
                        make_currency_tag(in_currency_code),
                        exchange_rate_denom,
                        exchange_rate_num,
                        is_synthetic,
                        scaling_factor,
                        fractional_part,
                        CStr::from_ptr(in_currency_code).to_str().unwrap().as_bytes().to_owned(),
                        true);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::register_currency, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
                }
            );

            check_result(ret);
        }

        /// add currency for the designated dealer account
        virtual void
        add_currency_for_designated_dealer(
            std::string_view currency_code,
            const Address &dd_address) override
        {
            auto in_currency_code = currency_code.data();
            auto in_dd_address = dd_address.data();

            bool ret = rust!(
                client_add_currency_for_designated_dealer [
                    rust_violas_client : &mut ViolasClient as "void *",
                    in_currency_code : *const c_char as "const char *",
                    in_dd_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *"
                    ] -> bool as "bool" {

                    let ret = rust_violas_client.add_currency_for_designated_dealer(
                                    make_currency_tag(in_currency_code),
                                    AccountAddress::new(*in_dd_address),
                                    true);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::add_currency_for_designated_dealer, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }

        /// mint currency for dd account
        virtual void
        mint_currency_for_designated_dealer(
            std::string_view currency_code,
            uint64_t sliding_nonce,
            const Address &dd_address,
            uint64_t amount,
            uint64_t tier_index) override
        {
            auto in_currency_code = currency_code.data();
            auto in_dd_address = dd_address.data();

            bool ret = rust!( client_mint_currency_for_designated_dealer [
                rust_violas_client : &mut ViolasClient as "void *",
                in_currency_code : *const c_char as "const char *",
                sliding_nonce : u64 as "uint64_t",
                in_dd_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                amount : u64 as "uint64_t",
                tier_index : u64 as "uint64_t"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.mint_currency(
                                    make_currency_tag(in_currency_code),
                                    sliding_nonce,
                                    AccountAddress::new(*in_dd_address),
                                    amount,
                                    tier_index,
                                    true);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::add_currency_for_designated_dealer, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }

        virtual void
        create_designated_dealer_account(
            std::string_view currency_code,
            uint64_t nonce,
            const Address &new_account_address,
            const AuthenticationKey &auth_key,
            std::string_view human_name,
            std::string_view base_url,
            PublicKey compliance_public_key,
            bool add_all_currencies) override
        {
            auto in_currency_code = currency_code.data();
            auto in_address = new_account_address.data();
            auto in_auth_key = auth_key.data();
            auto in_human_name = human_name.data();
            auto in_base_url = base_url.data();
            auto in_compliance_public_key = compliance_public_key.data();

            bool ret = rust!( client_create_designated_dealer_account [
                rust_violas_client : &mut ViolasClient as "void *",
                in_currency_code : *const c_char as "const char *",
                nonce : u64 as "uint64_t",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                in_auth_key : &[u8;ADDRESS_LENGTH*2] as "const uint8_t *",
                in_human_name : *const c_char as "const char *",
                in_base_url : *const c_char as "const char *",
                in_compliance_public_key :  &[u8;ADDRESS_LENGTH*2] as "const uint8_t *",
                add_all_currencies : bool as "bool"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.create_designated_dealer_account(
                                    make_currency_tag(in_currency_code),
                                    nonce,
                                    AccountAddress::new(*in_address),
                                    AuthenticationKey::new(*in_auth_key).prefix().to_vec(),
                                    CStr::from_ptr(in_human_name).to_str().unwrap().as_bytes().to_owned(),
                                    CStr::from_ptr(in_base_url).to_str().unwrap().as_bytes().to_owned(),
                                    in_compliance_public_key.to_owned().to_vec(),
                                    add_all_currencies,
                                    true);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::create_system_account, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }

        virtual void
        update_account_authentication_key(
                                        const Address &address,
                                        const AuthenticationKey &auth_key) override
        {
            auto in_address = address.data();
            auto in_auth_key = auth_key.data();

            bool ret = rust!( client_update_account_authentication_key [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                in_auth_key : &[u8;ADDRESS_LENGTH*2] as "const uint8_t *"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.update_account_authentication_key(
                                    AccountAddress::new(*in_address),
                                    AuthenticationKey::new(*in_auth_key),
                                    );
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::create_system_account, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);

        }
    };

    std::shared_ptr<Client>
    Client::create(
                uint8_t chain_id,
                std::string_view url,
                std::string_view mint_key,
                std::string_view mnemonic,
                std::string_view waypoint)
    {
        return make_shared<ClientImp>(chain_id, url, mint_key, mnemonic, waypoint);
    }

}  // namespace violas

}} //end of macro cpp!
