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
pub struct RustCurrencyTag {
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

cpp! {{
#include "client.hpp"

using namespace std;

namespace violas
{
    // currency tag for C
    struct CCurrencyTag {
        uint8_t address[ADDRESS_LENGTH];
        const char* module_name;
        const char* resource_name;

        CCurrencyTag(const CurrencyTag & currency_tag)
        {
            copy(begin(currency_tag.address), end(currency_tag.address), address);
            module_name = currency_tag.module_name.data();
            resource_name = currency_tag.resource_name.data();
        }
    };

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
        create_next_account() override
        {
            AddressAndIndex addr_index;
            auto out_index = &addr_index.index;
            auto out_addr = &addr_index.address[0];

            bool ret = rust!(Client_create_next_account [
                                        rust_violas_client : &mut ViolasClient as "void *",
                                        out_index : &mut usize as "size_t *",
                                        out_addr : &mut [c_uchar; ADDRESS_LENGTH] as "uint8_t *"
                                        ] -> bool as "bool" {
                let result = rust_violas_client.create_next_account(true);
                match result {
                    Ok(addr_index) => {
                        out_addr.copy_from_slice(&addr_index.address.as_ref());
                        *out_index = addr_index.index;
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
                auto out_sequence_num = &account.sequence_number;
                auto out_status = &account.status;

                rust!(client_get_account[
                                        rust_violas_client : &mut ViolasClient as "void *",
                                        i : usize as "size_t",
                                        out_address : &mut [c_uchar; ADDRESS_LENGTH] as "uint8_t *",
                                        out_auth_key : &mut [c_uchar; ADDRESS_LENGTH * 2] as "uint8_t *",
                                        out_sequence_num : &mut u64 as "uint64_t *",
                                        out_status : &mut AccountStatus as "AccountStatus *"
                                        ] {
                    let account = & rust_violas_client.accounts[i];

                    out_address.copy_from_slice(&account.address.as_ref());
                    out_auth_key.copy_from_slice(&account.authentication_key.as_ref().unwrap());
                    *out_sequence_num = account.sequence_number;
                    *out_status = account.status.clone();
                });
            }

            return accounts;
        }

        virtual void
        create_testnet_account( std::string_view currency_code,
                                const AuthenticationKey &auth_key) override
        {
            auto in_currency_code = currency_code.data();
            auto in_auth_key = auth_key.data();

            bool ret = rust!( client_create_testnet_account [
                            rust_violas_client : &mut ViolasClient as "void *",
                            in_currency_code : *const c_char as "const char *",
                            in_auth_key : &[u8;AUTH_KEY_LENGTH] as "const uint8_t *"
                            ] -> bool as "bool" {
                                let auth_key = AuthenticationKey::new(*in_auth_key);

                                let ret = rust_violas_client.create_testing_account(
                                                make_currency_tag(in_currency_code),
                                                auth_key.derived_address(),
                                                auth_key.prefix().to_vec(),
                                                false,
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

        /// Add a currency to current account
        virtual void
        add_currency(   size_t sender_account_ref_id,
                        std::string_view currency_code) override
        {
            auto in_currency_code = currency_code.data();

            bool ret = rust!( client_add_currency [
                                rust_violas_client : &mut ViolasClient as "void *",
                                sender_account_ref_id : usize as "size_t",
                                in_currency_code : *const c_char as "const char *"
                            ] -> bool as "bool" {

                                let ret = rust_violas_client.add_currency(
                                    sender_account_ref_id,
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
                                    CStr::from_ptr(in_currency_code)
                                        .to_str()
                                        .unwrap()
                                        .as_bytes()
                                        .to_owned()
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
