use crate::{violas_client::ViolasClient, AccountStatus};
use anyhow::Error;
use cpp::cpp;
use libra_types::{chain_id::ChainId, waypoint::Waypoint};
use std::{
    cell::RefCell,
    ffi::{CStr, CString},
    os::raw::{c_char, c_uchar},
    str::FromStr,
};

const ADDRESS_LENGTH: usize = 16;

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

cpp! {{
#include "client.hpp"

using namespace std;

namespace violas
{

    class ClientImp : public Client
    {
    private:
        //raw rust ViolasClient pointer
        void * raw_violas_client;

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

            this->raw_violas_client = rust!(Client_constructor [
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
            rust!(Client_destructor [raw_violas_client : * mut ViolasClient as "void *"] {
                let _client = unsafe {
                    Box::from_raw(raw_violas_client);
                };
            });
        }

        virtual void
        test_connection() override
        {
            bool ret = rust!(Client_test_connection [raw_violas_client : &mut ViolasClient as "void *"] -> bool as "bool" {
                let result = raw_violas_client.test_validator_connection();
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
                                        raw_violas_client : &mut ViolasClient as "void *",
                                        out_index : &mut usize as "size_t *",
                                        out_addr : &mut [c_uchar; ADDRESS_LENGTH] as "uint8_t *"
                                        ] -> bool as "bool" {
                let result = raw_violas_client.create_next_account(true);
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
                                        raw_violas_client : &mut ViolasClient as "void *"]
                                        -> usize as "size_t" {
                raw_violas_client.accounts.len()
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
                                        raw_violas_client : &mut ViolasClient as "void *",
                                        i : usize as "size_t",
                                        out_address : &mut [c_uchar; ADDRESS_LENGTH] as "uint8_t *",
                                        out_auth_key : &mut [c_uchar; ADDRESS_LENGTH * 2] as "uint8_t *",
                                        out_sequence_num : &mut u64 as "uint64_t *",
                                        out_status : &mut AccountStatus as "AccountStatus *"
                                        ] {
                    let account = & raw_violas_client.accounts[i];

                    out_address.copy_from_slice(&account.address.as_ref());
                    out_auth_key.copy_from_slice(&account.authentication_key.as_ref().unwrap());
                    *out_sequence_num = account.sequence_number;
                    *out_status = account.status.clone();
                });
            }

            return accounts;
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
