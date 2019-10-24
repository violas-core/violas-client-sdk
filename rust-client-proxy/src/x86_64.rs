#[cfg(target_arch = "x86_64")]
#[allow(non_snake_case)]
pub mod x86_64 {
    //extern crate hex;
    use client::client_proxy::ClientProxy;
    use std::ffi::{CStr, CString};
    use std::os::raw::c_char;
    use std::*;

    //
    //
    //
    #[no_mangle]
    pub extern "C" fn create_libra_client_proxy(
        c_host: *const c_char,
        c_port: u16,
        c_validator_set_file: *const c_char,
        c_faucet_account_file: *const c_char,
        c_sync_on_wallet_recovery: bool,
        c_faucet_server: *const c_char,
        c_mnemonic_file: *const c_char,
    ) -> u64 {
        let ret = panic::catch_unwind(|| {
            let host = unsafe { CStr::from_ptr(c_host).to_str().unwrap() };
            let port = c_port as u16;
            let validator_set_file =
                unsafe { CStr::from_ptr(c_validator_set_file).to_str().unwrap() };
            let faucet_account_file =
                unsafe { CStr::from_ptr(c_faucet_account_file).to_str().unwrap() };
            let sync_on_wallet_recovery = c_sync_on_wallet_recovery as bool;
            let faucet_server =
                unsafe { CStr::from_ptr(c_faucet_server).to_str().unwrap().to_owned() };
            let mnemonic_file =
                unsafe { CStr::from_ptr(c_mnemonic_file).to_str().unwrap().to_owned() };

            println!(
                "create_native_client_proxy arguments : 
                host = {}, 
                port = {}, 
                validator_set_file = {},
                faucet_account_file = {},
                sync_on_wallet_recovery = {},
                faucet_server = {},
                mnemonic_file = {}",
                host,
                port,
                validator_set_file,
                faucet_account_file,
                sync_on_wallet_recovery,
                faucet_server,
                mnemonic_file,
            );
            //
            // new Client Proxy
            //
            ClientProxy::new(
                host,
                port,
                validator_set_file,
                faucet_account_file,
                sync_on_wallet_recovery,
                Some(faucet_server),
                Some(mnemonic_file),
            )
            .unwrap()
        })
        .ok();

        //
        //  Check the result and then return a raw pointer
        //
        let raw_ptr = match ret {
            Some(value) => Box::into_raw(Box::new(value)) as u64,
            None => {
                println!("failed to new client proxy ");
                0
            }
        };

        raw_ptr
    }
    /// Destory the raw ClientProxy pointer
    #[no_mangle]
    pub extern "C" fn destory_libra_client_proxy(raw_ptr: u64) {
        if raw_ptr != 0 {
            let _proxy = unsafe { Box::from_raw(raw_ptr as *mut ClientProxy) };
            println!("destory_native_client_proxy enters ...");
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_test_validator_connection(raw_ptr: u64) -> bool {
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
        //
        let ret = client.test_validator_connection();

        println!("test_validator_connection enters ...");
        //return the boolean result
        match ret {
            Ok(_) => true,
            Err(_) => false,
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_create_next_account(raw_ptr: u64, sync_with_validator: bool) {
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
        //
        match client.create_next_account(sync_with_validator) {
            Ok(account_data) => println!(
                "Created/retrieved account #{} address {}",
                account_data.index,
                hex::encode(account_data.address)
            ),
            Err(e) => println!("Error creating account, {}", e),
        }
    }

    #[no_mangle]
    pub extern "C" fn get_all_accounts() {
        //
    }

    #[no_mangle]
    pub extern "C" fn set_accounts() {
        //
    }
}
