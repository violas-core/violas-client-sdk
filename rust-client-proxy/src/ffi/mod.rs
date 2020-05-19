use crate::client_proxy::ClientProxy;
use cpp::cpp;
use libra_types::waypoint::Waypoint;
use std::{ffi::CStr, os::raw::c_char, str::FromStr};

cpp! {{

#include <string>
#include <string_view>
#include  "client.hpp"

using namespace std;

Client::Client(std::string_view url, std::string_view mint_key, std::string_view mnemonic, std::string_view waypoint)
{
    auto c_url = url.data();
    auto c_mint_key = mint_key.data();
    auto c_mnemonic = mnemonic.data();
    auto c_waypoint = waypoint.data();

    this->internal = rust!(Client_constructor [
            c_url : * const c_char as "const char *",
            c_mint_key : * const c_char as "const char *",
            c_mnemonic : * const c_char as "const char *",
            c_waypoint : * const c_char as "cosnt char *"]-> *mut ClientProxy as "void *" {
        let client = ClientProxy::new(CStr::from_ptr(c_url).to_str().unwrap(),
                                        CStr::from_ptr(c_mint_key).to_str().unwrap() ,
                                        true,
                                        None,
                                        Some(CStr::from_ptr(c_mnemonic).to_str().unwrap().to_owned()),
                                        Waypoint::from_str(CStr::from_ptr(c_waypoint).to_str().unwrap()).unwrap()).unwrap();
        Box::into_raw(Box::new(client))
    });
}

Client::~Client()
{
    rust!(Client_destructor [internal : * mut ClientProxy as "void *"] {
        let _client = unsafe {
            Box::from_raw(internal);
        };
    });
}

void Client::test_connection()
{
    bool ret = rust!(Client_test_connection [internal : &mut ClientProxy as "void *"] -> bool as "bool" {
        let result = internal.test_validator_connection();
        match result {
            Ok(_) => true,
            Err(_) => false,
        }
    });

    if( !ret )
        throw runtime_error("Not able to connect to validator at {}");
}

}}
