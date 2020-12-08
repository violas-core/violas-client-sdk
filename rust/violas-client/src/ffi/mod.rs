use crate::{violas_account, violas_client::ViolasClient, AccountAddress, AccountStatus};
use anyhow::{format_err, Error};
use cpp::cpp;
use libra_types::{
    account_config::CORE_CODE_ADDRESS, chain_id::ChainId,
    transaction::authenticator::AuthenticationKey, waypoint::Waypoint,
};
use move_core_types::{
    identifier::Identifier,
    language_storage::{StructTag, TypeTag},
};
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
#include <fstream>
#include <iterator>
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

        void free_rust_string(const char * rust_str)
        {
            rust!( client_free_string [
                rust_str : *mut c_char as "const char *"
                ] {
                    if !rust_str.is_null() {
                        unsafe {
                            CString::from_raw(rust_str);
                        }
                    }
                });
        }

        void check_result(bool ret)
        {
            if( !ret )
            {
                const char* last_error = rust!(Client_get_last_error [] -> *const c_char as "const char*" {
                    get_last_error()
                });

                string error(last_error);
                free_rust_string(last_error);

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

                accounts[i].index = i;
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
        execute_script_file(size_t account_index,
                       std::string_view script_file_name,
                       const std::vector<TypeTag> &type_tags,
                       const std::vector<TransactionAugment> & arguments,
                       bool is_blocking) override
        {
            ifstream script(script_file_name.data(), ios::binary);

            if(!script.is_open())
                throw runtime_error(string("execute_script_file : script file ") + script_file_name.data() + "is not exist !");
            istreambuf_iterator<char> fbeg(script), fend;
            vector<uint8_t> script_bytecode(fbeg, fend);

            execute_script(account_index, script_bytecode, type_tags, arguments, is_blocking);
        }

        /**
         * @brief Execute script bytecode with arguments
         *
         * @param account_index account index of wallet
         * @param script_file_name script file name with path
         * @param type_tags transaction TypeTag vector for script
         * @param arguments transaction argument vector for script
         */
        virtual void
        execute_script(size_t account_index,
                        const std::vector<uint8_t>& script,
                        const std::vector<TypeTag> &type_tags,
                        const std::vector<TransactionAugment> &arguments,
                        bool is_blocking) override
        {
            auto in_script = script.data();
            uint64_t in_scirpt_len = script.size();

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
                in_script : *const u8 as "const uint8_t *",
                in_scirpt_len : usize as "size_t",
                in_c_type_tags : *const RustTypeTag as "const CTypeTag *",
                in_c_type_tags_len : usize as "size_t",
                in_args : *const *const c_char as "const char * *",
                in_args_len : usize as "size_t",
                is_blocking : bool as "bool"
                ] -> bool as "bool" {

                    let script_bytecode: Vec<u8> = slice::from_raw_parts(in_script, in_scirpt_len).to_vec();
                    let type_tags : Vec<TypeTag> = slice::from_raw_parts(in_c_type_tags, in_c_type_tags_len)
                                                            .iter()
                                                            .map(|x| make_type_tag(x))
                                                            .collect();

                    let args : Vec<&str> = slice::from_raw_parts(in_args, in_args_len)
                                        .iter()
                                        .map( |x| CStr::from_ptr(*x).to_str().unwrap() )
                                        .collect();

                    let ret = rust_violas_client.execute_script(
                                account_index as u64,
                                script_bytecode,
                                type_tags,
                                &args,
                                is_blocking);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::execute_script, {}",e);
                            set_last_error(err);
                            false
                        }
                    }

            });

            check_result(ret);
        }

        /**
         * @brief Query accout status infomation
         *
         * @param address - the address of account
         * @return std::string
         */
        virtual std::string
        query_account_info(const Address & address) override
        {
            auto in_address = address.data();
            char * json_string = nullptr;
            char ** out_json_string = &json_string;

            bool ret = rust!( client_query_account_info [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.query_account_info(AccountAddress::new(*in_address));

                    match ret {
                        Ok(opt) => match opt {
                            (Some(view), _version) => {
                                let json_currencies = serde_json::to_string(&view).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new reserves detail error")
                                    .into_raw();
                                true
                            }
                            (None, _) => {
                                set_last_error(format_err!(
                                    "no info for account {}",
                                    AccountAddress::new(*in_address)
                                ));
                                false
                            }
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to get exchagne reserves with error, {}",
                                e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
        }

        /**
         * @brief Query transaction inforamtion
         *
         * @return string::string
         */
        virtual std::string
        query_transaction_info( const Address &address,
                                uint64_t sequence_number,
                                bool is_fetching_events) override
        {
            auto in_address = address.data();
            char * json_string = nullptr;
            char ** out_json_string = &json_string;

            bool ret = rust!( client_query_transaction_info_by_acc_seq [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                sequence_number : u64 as "uint64_t",
                is_fetching_events : bool as "bool",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.query_transaction_info(AccountAddress::new(*in_address), sequence_number, is_fetching_events);

                    match ret {
                        Ok(opt) => match opt {
                            Some(view) => {
                                let json_currencies = serde_json::to_string(&view).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new CString error")
                                    .into_raw();
                                true
                            }
                            None => {
                                set_last_error(format_err!(
                                    "no transaction info for account {} and sequence number {}",
                                    AccountAddress::new(*in_address),
                                    sequence_number
                                ));
                                false
                            }
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to get exchagne reserves with error, {}",
                                e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
        }
        /**
         * @brief query transaction inforamtion by range
         *
         * @param start_version     start version
         * @param limit             limit of range, amount of queried transaction
         * @param is_fetching_event whether fectching event or not
         * @return std::string  with json format
         */
        virtual std::string
        query_transaction_info(uint64_t start_version,
                               uint64_t limit,
                               bool is_fetching_events) override
        {
            char * json_string = nullptr;
            char ** out_json_string = &json_string;

            bool ret = rust!( client_query_transaction_info_by_range [
                rust_violas_client : &mut ViolasClient as "void *",
                start_version : u64 as "uint64_t",
                limit : u64 as "uint64_t",
                is_fetching_events : bool as "bool",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.query_transaction_by_range(start_version, limit, is_fetching_events);

                    match ret {
                        Ok(views) => {
                            let json_currencies = serde_json::to_string(&views).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new CString error")
                                    .into_raw();
                                true
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to query transaction info with error, {}",
                                e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
        }
        ///
        /// Query events
        ///
        virtual std::string
        query_events(const Address &address,
                     event_type type,
                     uint64_t start_version,
                     uint64_t limit) override
        {
            auto in_address = address.data();
            char * json_string = nullptr;
            char ** out_json_string = &json_string;
            bool in_event_type = (type == event_type::sent)? true : false;

            bool ret = rust!( client_query_events [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                in_event_type : bool as "bool",
                start_version : u64 as "uint64_t",
                limit : u64 as "uint64_t",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.query_events(
                                                        AccountAddress::new(*in_address),
                                                        in_event_type,
                                                        start_version, limit);

                    match ret {
                        Ok(events_account ) => {
                            let json_currencies = serde_json::to_string(&events_account).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new CString error")
                                    .into_raw();
                                true
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to query events with error, {}", e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// multi currency methods
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        /// get the balance of currency for the account address
        virtual uint64_t
        get_currency_balance(const Address &address,
                             std::string_view currency_code) override
        {
            auto in_address = address.data();
            auto in_currency_code = currency_code.data();

            uint64_t balance = rust!( client_get_currency_balance [
                                rust_violas_client : &mut ViolasClient as "void *",
                                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                                in_currency_code : *const c_char as "const char *"
                            ] -> u64 as "uint64_t" {
                                let ret = rust_violas_client.get_currency_balance(
                                    make_currency_tag(in_currency_code),
                                    AccountAddress::new(*in_address));
                                match ret {
                                    Ok(balance) => balance,
                                    Err(e) => {
                                        let err = format_err!("ffi::get_currency_balance, {}",e);
                                        set_last_error(err);
                                        u64::MAX
                                    }
                                }
                        });

            return balance;
        }

        /// Get all currency info
        virtual std::string
        get_all_currency_info() override
        {
            char * all_currency_info = nullptr;
            auto out_all_currency_info = &all_currency_info;

            rust!( client_get_all_currency_info [
                rust_violas_client : &mut ViolasClient as "void *",
                out_all_currency_info : *mut *mut c_char as "char **"
            ] -> bool as "bool" {
                let ret = rust_violas_client.get_all_currency_info( );
                match ret {
                    Ok(all_currency_info) => {
                        let json_currencies_info = serde_json::to_string(&all_currency_info).unwrap();
                        *out_all_currency_info = CString::new(json_currencies_info)
                            .expect("new reserves detail error")
                            .into_raw();
                        true
                    },
                    Err(e) => {
                        let err = format_err!("ffi::get_all_currency_info, {}",e);
                        set_last_error(err);
                        false
                    }
                }
            });

            string temp = all_currency_info;
            free_rust_string(all_currency_info);

            return temp;
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
                            let err = format_err!("ffi::mint_currency_for_designated_dealer, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }

        /// create a designated dealer account
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
                            let err = format_err!("ffi::create_designated_dealer_account, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }

        /// create a designated dealer account with extension
        virtual void
        create_designated_dealer_ex(
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

            bool ret = rust!( client_create_designated_dealer_ex [
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

                    let ret = rust_violas_client.create_designated_dealer_ex(
                                    make_currency_tag(in_currency_code),
                                    nonce,
                                    AccountAddress::new(*in_address),
                                    AuthenticationKey::new(*in_auth_key),
                                    CStr::from_ptr(in_human_name).to_str().unwrap().as_bytes().to_owned(),
                                    CStr::from_ptr(in_base_url).to_str().unwrap().as_bytes().to_owned(),
                                    in_compliance_public_key.to_owned().to_vec(),
                                    add_all_currencies,
                                    true);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::create_designated_dealer_account, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }
        //
        // Create parent VASP account
        //
        virtual void
        create_parent_vasp_account(std::string_view currency_code,
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

            bool ret = rust!( client_create_parent_vasp_account [
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

                    let ret = rust_violas_client.create_parent_vasp_account(
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
                            let err = format_err!("ffi::create_parent_vasp_account, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });

            check_result(ret);
        }

        /**
         * @brief recover account of wallet from specified
         *
         * @param mnemonic_file_path mnemonic file path
         */
        virtual void
        recover_wallet_accounts(std::string_view mnemonic_file_path) override
        {
            //auto in_address = address.data();
            auto in_file_path_str = mnemonic_file_path.data();

            bool ret = rust!( client_recover_wallet_account [
                rust_violas_client : &mut ViolasClient as "void *",
                in_file_path_str: *const c_char as "const char *"
                ] -> bool as "bool" {
                    let ret = rust_violas_client.recover_wallet_accounts(
                        CStr::from_ptr(in_file_path_str).to_str().unwrap());
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::recover_wallet_accounts, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });
            check_result(ret);
        }

        /**
         * @brief rotate authentication key with nonce
         *
         * @param account_index     account index
         * @param sliding_nonce     sliding nonce, default is 0
         * @param new_auth_key      the new authentication key
         * @param is_blocking       if blocking and waiting for result
         */
        virtual void
        rotate_authentication_key_with_nonce(size_t account_index,
                                             uint64_t sliding_nonce,
                                             const AuthenticationKey &new_auth_key,
                                             bool is_blocking) override
        {
            //auto in_address = address.data();
            auto in_new_auth_key = new_auth_key.data();

            bool ret = rust!( client_rotate_authentication_key_with_nonce [
                rust_violas_client : &mut ViolasClient as "void *",
                account_index : u64 as "size_t",
                sliding_nonce : u64 as "uint64_t",
                //in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                in_new_auth_key : &[u8;ADDRESS_LENGTH*2] as "const uint8_t *",
                is_blocking : bool as "bool"
                ] -> bool as "bool" {
                    let ret = rust_violas_client.rotate_authentication_key_with_nonce(
                        account_index,
                        sliding_nonce,
                        //AccountAddress::new(*in_address),
                        AuthenticationKey::new(*in_new_auth_key),
                        is_blocking);
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::rotate_authentication_key_with_nonce, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });
            check_result(ret);
        }

         /**
         * @brief Save private key
         *
         * @param account_index account index
         * @param path_file_str a file name string with path
         */
        virtual void
        save_private_key(size_t account_index, std::string_view path_file_str) override
        {
            //auto in_address = address.data();
            auto in_path_file_str = path_file_str.data();

            bool ret = rust!( client_save_private_key [
                rust_violas_client : &mut ViolasClient as "void *",
                account_index: usize as "size_t",
                in_path_file_str: *const c_char as "const char *"
                ] -> bool as "bool" {
                    let ret = rust_violas_client.save_private_key(
                        account_index,
                        CStr::from_ptr(in_path_file_str).to_str().unwrap());
                    match ret {
                        Ok(_) => true,
                        Err(e) => {
                            let err = format_err!("ffi::save_private_key, {}",e);
                            set_last_error(err);
                            false
                        }
                    }
            });
            check_result(ret);
        }
        /**
         * @brief Update daul attestation limit
         *
         * @param sliding_nonce         sliding nonce
         * @param new_micro_lbr_limit   the new limit based micro LBR amount
         */
        virtual void
        update_dual_attestation_limit(uint64_t sliding_nonce, uint64_t new_micro_lbr_limit) override
        {
            bool ret = rust!( client_update_dual_attestation_limit [
                rust_violas_client : &mut ViolasClient as "void *",
                sliding_nonce : u64 as "uint64_t",
                new_micro_lbr_limit : u64 as "uint64_t"
                ] -> bool as "bool" {

                    let ret = rust_violas_client.update_dual_attestation_limit(
                            sliding_nonce,
                            new_micro_lbr_limit,
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

        //
        //  Exchnage interface
        //
        virtual std::string //json string
        get_exchange_currencies(const Address &address) override
        {
            auto in_address = address.data();
            char * json_string = nullptr;
            char ** out_json_string = &json_string;

            bool ret = rust!( client_get_exchange_currencies [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {
                    let resource_tag = StructTag {
                        address: CORE_CODE_ADDRESS,
                        module: Identifier::new("Exchange").unwrap(),
                        name: Identifier::new("RegisteredCurrencies").unwrap(),
                        type_params: vec![],
                    };

                    let ret: Result<Option<violas_account::exchange::RegisteredCurrencies>, Error> =
                    rust_violas_client.get_account_resource(&AccountAddress::new(*in_address), &resource_tag);

                    match ret {
                        Ok(opt) => match opt {
                            Some(view) => {
                                let json_currencies = serde_json::to_string(&view).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new reserves detail")
                                    .into_raw();
                                true
                            }
                            None => {
                                false
                            }
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to get exchagne reserves with error, {}",
                                e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
        }

        //
        //  get exchange reservers
        //  return json string
        //
        virtual std::string
        get_exchange_reserves(const Address & address ) override
        {
            auto in_address = address.data();
            char * json_string = nullptr;
            char ** out_json_string = &json_string;

            bool ret = rust!( client_get_exchange_reserves [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {
                    let resource_tag = StructTag {
                        address: CORE_CODE_ADDRESS,
                        module: Identifier::new("Exchange").unwrap(),
                        name: Identifier::new("Reserves").unwrap(),
                        type_params: vec![],
                    };

                    let ret: Result<Option<violas_account::exchange::Reserves>, Error> =
                    rust_violas_client.get_account_resource(&AccountAddress::new(*in_address), &resource_tag);

                    match ret {
                        Ok(opt) => match opt {
                            Some(view) => {
                                let json_currencies = serde_json::to_string(&view).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new reserves detail error")
                                    .into_raw();
                                true
                            }
                            None => {
                                false
                            }
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to get exchagne reserves with error, {}",
                                e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
        }

        //
        //  get liquidity balance
        //  return json string
        //
        virtual std::string
        get_liquidity_balance(const Address & address ) override
        {
            auto in_address = address.data();
            char * json_string = nullptr;
            char ** out_json_string = &json_string;

            bool ret = rust!( client_get_liquidity_balance [
                rust_violas_client : &mut ViolasClient as "void *",
                in_address : &[u8;ADDRESS_LENGTH] as "const uint8_t *",
                out_json_string : *mut *mut c_char as "char **"
                ] -> bool as "bool" {
                    let resource_tag = StructTag {
                        address: CORE_CODE_ADDRESS,
                        module: Identifier::new("Exchange").unwrap(),
                        name: Identifier::new("Tokens").unwrap(),
                        type_params: vec![],
                    };

                    let ret: Result<Option<violas_account::exchange::Tokens>, Error> =
                    rust_violas_client.get_account_resource(&AccountAddress::new(*in_address), &resource_tag);

                    match ret {
                        Ok(opt) => match opt {
                            Some(view) => {
                                let json_currencies = serde_json::to_string(&view).unwrap();
                                *out_json_string = CString::new(json_currencies)
                                    .expect("new reserves detail error")
                                    .into_raw();
                                true
                            }
                            None => {
                                false
                            }
                        },
                        Err(e) => {
                            set_last_error(format_err!(
                                "failed to get exchagne liquidity balance with error, {}",
                                e
                            ));
                            false
                        }
                    }
            });

            check_result(ret);

            string result = json_string;
            free_rust_string(json_string);

            return result;
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
