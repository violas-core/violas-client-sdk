#[cfg(target_arch = "x86_64")]
#[allow(non_snake_case)]
pub mod x86_64 {
    //extern crate hex;
    use client::client_proxy::ClientProxy;
    use client::AccountStatus;
    use std::ffi::{CStr, CString};
    use std::os::raw::{c_char, c_uchar};
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
            println!("x86_64.rs: destory_native_client_proxy enters ...");
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

    #[repr(C)]
    //#[derive(Copy, Clone)]
    pub struct AccountAndIndex {
        pub address: [c_uchar; 32],
        pub index: u64,
    }

    #[no_mangle]
    pub extern "C" fn libra_create_next_account(
        raw_ptr: u64,
        sync_with_validator: bool,
    ) -> AccountAndIndex {
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

        let accountAndIndex = client
            .create_next_account(sync_with_validator)
            .expect("failed to call create_next_account");

        println!(
            "Created/retrieved account #{} address {}",
            accountAndIndex.index,
            hex::encode(accountAndIndex.address)
        );

        let mut account = AccountAndIndex {
            index: accountAndIndex.index as u64,
            address: [0; 32],
        };

        for (dst, src) in account
            .address
            .iter_mut()
            .zip(accountAndIndex.address.as_ref())
        {
            *dst = *src
        }
        //ptr::copy(accountAndIndex.address.as_ref(), &account.address, 32);

        account
    }

    #[repr(C)]
    pub struct Account {
        pub address: [c_uchar; 32],
        pub index: u64,
        pub sequence_number: u64,
        pub status: i64,
    }

    #[repr(C)]
    pub struct Accounts {
        len: u64,
        data: *mut Account,
    }

    #[no_mangle]
    pub extern "C" fn libra_get_all_accounts(raw_ptr: u64) -> Accounts {
        //
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

        if client.accounts.is_empty() {
            println!("No user accounts");
        } else {
            for (ref index, ref account) in client.accounts.iter().enumerate() {
                println!(
                    "User account index: {}, address: {}, sequence number: {}, status: {:?}",
                    index,
                    hex::encode(&account.address),
                    account.sequence_number,
                    account.status,
                );
            }
        }

        if let Some(faucet_account) = &client.faucet_account {
            println!(
                "Faucet account address: {}, sequence_number: {}, status: {:?}",
                hex::encode(&faucet_account.address),
                faucet_account.sequence_number,
                faucet_account.status,
            );
        }
        let mut accounts: Vec<Account> = Vec::new();

        for (i, ref acc) in client.accounts.iter().enumerate() {
            let mut accout = Account {
                address: [0; 32],
                index: i as u64,
                sequence_number: acc.sequence_number,
                status: match &acc.status {
                    AccountStatus::Local => 0,
                    AccountStatus::Persisted => 1,
                    AccountStatus::Unknown => -1,
                },
            };

            let bytes = &acc.address.to_vec();
            accout.address.copy_from_slice(bytes);

            accounts.push(accout);
        }

        let data = accounts.as_mut_ptr();
        let len = accounts.len() as u64;
        std::mem::forget(accounts);
        //
        Accounts { len, data }
    }

    #[no_mangle]
    pub extern "C" fn libra_free_all_accounts_buf(buf: Accounts) {
        //
        let s = unsafe { std::slice::from_raw_parts_mut(buf.data, buf.len as usize) };
        let s = s.as_mut_ptr();
        unsafe {
            Box::from_raw(s);
        }

        println!("free_all_accounts_buf entered");
    }

    #[no_mangle]
    pub extern "C" fn set_accounts() {
        //
    }

    #[no_mangle]
    pub extern "C" fn libra_get_balance(raw_ptr: u64, index: u64) -> f64 {
        // convert raw ptr to object client
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

        let balance = client
            .get_balance(&["b", index.to_string().as_str()])
            .unwrap();
        balance.parse::<f64>().unwrap()
    }

    #[no_mangle]
    pub extern "C" fn libra_get_sequence_number(raw_ptr: u64, index: u64) -> u64 {
        // convert raw ptr to object client
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

        let sequence_num = client
            .get_sequence_number(&["sequence", index.to_string().as_str()])
            .unwrap();

        sequence_num
    }

    #[no_mangle]
    pub extern "C" fn libra_mint_coins(
        raw_ptr: u64,
        index: u64,
        num_coins: u64,
        is_blocking: bool,
    ) {
        // convert raw ptr to object client
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

        client
            .mint_coins(
                &[
                    "mintb",
                    index.to_string().as_str(),
                    num_coins.to_string().as_str(),
                ],
                is_blocking,
            )
            .unwrap();
    }
}
