//#[cfg(target_arch = "x86_64")]
#[allow(non_snake_case)]
pub mod x86_64 {
    use crate::client_proxy::{AccountEntry, ClientProxy, IndexAndSequence};
    //use crate::move_compiler;
    use crate::violas_account::*;
    use crate::AccountStatus;
    use anyhow::{bail, format_err, Error};
    use libra_types::waypoint::Waypoint;
    use libra_types::{
        access_path::AccessPath,
        account_address::AccountAddress,
        account_config::CORE_CODE_ADDRESS,
        account_config::{
            //association_address, lbr_type_tag,
            ACCOUNT_RECEIVED_EVENT_PATH,
            ACCOUNT_SENT_EVENT_PATH,
        },
        account_state::AccountState,
        transaction::{authenticator::AuthenticationKey, *},
    };
    use std::{
        cell::RefCell,
        convert::TryFrom,
        ffi::{CStr, CString},
        io::Write,
        os::raw::{c_char, c_uchar},
        path::Path,
        result::Result,
        str::FromStr,
        *,
    };
    use tempdir::TempDir;

    pub const LENGTH: usize = 16;

    thread_local! {
        static LAST_ERROR: RefCell<String> = RefCell::new(String::new());
    }

    #[allow(dead_code)]
    fn set_last_error(err: Error) {
        LAST_ERROR.with(|prev| {
            *prev.borrow_mut() = format!("{:?}", err);
        });
    }
    #[no_mangle]
    pub extern "C" fn libra_get_last_error() -> *const c_char {
        LAST_ERROR.with(|prev| {
            let err = prev.borrow_mut();
            CString::new(err.clone()).unwrap().into_raw()
        })
    }
    ///
    /// free rust string
    ///
    #[no_mangle]
    pub extern "C" fn libra_free_string(s: *mut c_char) {
        unsafe {
            if s.is_null() {
                return;
            }
            CString::from_raw(s)
        };
    }
    //
    //  create libra client proxy
    //
    #[no_mangle]
    pub extern "C" fn violas_create_client(
        c_url: *const c_char,
        c_mint_key: *const c_char,
        sync_on_wallet_recovery: bool,
        faucet_server: *const c_char,
        c_mnemonic: *const c_char,
        in_waypoint: *const c_char,
        out_clinet_ptr: *mut u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| {
                let client = ClientProxy::new(
                    CStr::from_ptr(c_url).to_str().unwrap(),
                    CStr::from_ptr(c_mint_key).to_str().unwrap(),
                    sync_on_wallet_recovery,
                    Some(CStr::from_ptr(faucet_server).to_str().unwrap().to_owned()),
                    Some(CStr::from_ptr(c_mnemonic).to_str().unwrap().to_owned()),
                    Waypoint::from_str(CStr::from_ptr(in_waypoint).to_str().unwrap()).unwrap(),
                )
                .unwrap();

                *out_clinet_ptr = Box::into_raw(Box::new(client)) as u64;
            });

            if ret.is_ok() {
                true
            } else {
                set_last_error(format_err!(
                    "catch panic at function 'violas_create_client' !'"
                ));
                false
            }
        }
    }

    ///
    /// Destory the raw ClientProxy pointer
    ///
    #[no_mangle]
    pub extern "C" fn libra_destory_client_proxy(raw_ptr: u64) {
        if raw_ptr != 0 {
            let _proxy = unsafe { Box::from_raw(raw_ptr as *mut ClientProxy) };
            //println!("x86_64.rs: destory_native_client_proxy enters ...");
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_test_validator_connection(raw_ptr: u64) -> bool {
        let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
        let ret = client.test_validator_connection();
        //return the boolean result
        match ret {
            Ok(_) => true,
            Err(_) => false,
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_create_next_address(_raw_ptr: u64, _sync_with_validator: bool) -> bool {
        // let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

        // let (address, _) = client.wallet.new_address().unwrap();

        true
    }

    #[repr(C)]
    pub struct AccountAndIndex {
        pub address: [c_uchar; LENGTH],
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

        // println!(
        //     "Created/retrieved account #{} address {}",
        //     accountAndIndex.index,
        //     hex::encode(accountAndIndex.address)
        // );

        let mut account = AccountAndIndex {
            index: accountAndIndex.index as u64,
            address: [0; LENGTH],
        };

        account
            .address
            .copy_from_slice(&accountAndIndex.address.to_vec());

        account
    }

    #[repr(C)]
    pub struct Account {
        pub address: [c_uchar; LENGTH],
        pub auth_key: [c_uchar; 32],
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
        let mut accounts: Vec<Account> = Vec::new();

        for (i, ref acc) in client.accounts.iter().enumerate() {
            let mut accout = Account {
                address: [0; LENGTH],
                auth_key: [0; 32],
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
            let auth_key = acc.authentication_key.as_ref().unwrap();
            accout.auth_key.copy_from_slice(&auth_key);

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

        //println!("free_all_accounts_buf entered");
    }

    #[no_mangle]
    pub extern "C" fn set_accounts() {
        //
    }

    #[no_mangle]
    pub extern "C" fn libra_get_balance(
        raw_ptr: u64,
        address: &[c_uchar; LENGTH],
        out_balance: *mut u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                // convert raw ptr to object client
                let proxy = &mut *(raw_ptr as *mut ClientProxy);
                let ret = proxy.get_account_resource_and_update(AccountAddress::new(*address));
                match ret {
                    Ok(account_view) => {
                        *out_balance = account_view.balances[0].amount;
                        true
                    }
                    Err(e) => {
                        set_last_error(format_err!("get_account_resource_and_update, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch panic at function 'libra_get_balance' !'"
                ));
                false
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_get_sequence_number(
        raw_ptr: u64,
        address: &[c_uchar; LENGTH],
        result: &mut u64,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<u64, Error> {
            // convert raw ptr to object client
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

            let account_state = proxy
                .client
                .get_account_state(AccountAddress::new(*address), true)?;
            if let Some(view) = account_state.0 {
                Ok(view.sequence_number)
            } else {
                bail!("No account exists at {:?}", address);
            }
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(seq_num) => {
                    *result = seq_num;
                    true
                }
                Err(e) => {
                    set_last_error(e);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_get_sequence_number' !"
            ));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_mint_coins(
        raw_ptr: u64,
        index: u64,
        num_coins: u64,
        is_blocking: bool,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

            client.mint_coins(
                &[
                    "mintb",
                    index.to_string().as_str(),
                    num_coins.to_string().as_str(),
                ],
                is_blocking,
            )
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(_) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!("catch panic at function (libra_mint_coins) !"));
            false
        }
    }

    #[repr(C)]
    pub struct IndexAndSeq {
        pub index: u64,
        pub sequence_number: u64,
    }
    /// Transfer num_coins from sender account to receiver. If is_blocking = true,
    /// it will keep querying validator till the sequence number is bumped up in validator.
    #[no_mangle]
    pub extern "C" fn libra_transfer_coins_int(
        raw_ptr: u64,
        sender_account_ref_id: usize,
        receiver_addr: &[u8; 16],
        micro_coins: u64,
        gas_unit_price: u64,
        max_gas_amount: u64,
        is_blocking: bool,
        result: &mut IndexAndSeq,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<IndexAndSequence, Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let receiver_address = AccountAddress::new(*receiver_addr);
            let receiver_auth_key_prefix: Vec<u8> = vec![];

            client.transfer_coins_int(
                sender_account_ref_id,
                &receiver_address,
                receiver_auth_key_prefix,
                micro_coins,
                match gas_unit_price {
                    0 => None,
                    _ => Some(gas_unit_price),
                },
                match max_gas_amount {
                    0 => None,
                    _ => Some(max_gas_amount),
                },
                is_blocking,
            )
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(value) => {
                    // save the result
                    result.index = match value.account_index {
                        AccountEntry::Index(i) => i as u64,
                        _AccountAddress => 0,
                    };
                    result.sequence_number = value.sequence_number;
                    true
                }
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_transfer_coins_int' !"
            ));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_compile(
        raw_ptr: u64,
        account_index_or_addr: *const c_char,
        script_path: *const c_char,
        is_module: bool,
        _temp_path: *const c_char,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let (address, _) = client.get_account_address_from_parameter(unsafe {
                CStr::from_ptr(account_index_or_addr).to_str().unwrap()
            })?;
            let file_path = unsafe { CStr::from_ptr(script_path).to_str().unwrap().to_string() };
            let temp_path = unsafe { CStr::from_ptr(_temp_path).to_str().unwrap().to_string() };
            // replace sender tag with local address

            let compile = |temp_dir: &Path| -> Result<(), Error> {
                let temp_source_path = temp_dir.join("temp.mvir");
                let mut temp_source_file = fs::File::create(temp_source_path.clone())?;
                let mut code = fs::read_to_string(file_path.clone())?;
                code = code.replace("{{sender}}", &format!("0x{}", address));
                writeln!(temp_source_file, "{}", code)?;

                //
                // handle dependencies
                //
                let _deps_path = handle_dependencies(
                    client,
                    address.to_string(),
                    temp_source_path.clone(),
                    is_module,
                )?;
                //
                // compile the source code
                //
                // let args = compiler_proxy::Args {
                //     module_input: is_module,
                //     address: Some(address.to_string()), //
                //     no_stdlib: false,
                //     no_verify: false,
                //     source_path: temp_source_path.clone(),
                //     list_dependencies: false,
                //     deps_path: deps_path, //Option(String::from_str(dependencies_path.to_str())),
                //     output_source_maps: false,
                // };
                // compiler_proxy::compile(args)?;

                let output_path = path::PathBuf::from(file_path);
                fs::copy(
                    temp_source_path.with_extension("mv"),
                    output_path.with_extension("mv"),
                )?;

                Ok(())
            };

            if temp_path == "" {
                let tempDir = TempDir::new("")?;
                compile(tempDir.path())?;
            } else {
                let tempDir = Path::new(&temp_path);
                compile(tempDir)?;
            };
            Ok(())
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(_) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!("catch panic at function (libra_compile) !"));
            false
        }
    }

    fn handle_dependencies(
        client: &mut ClientProxy,
        _address: String,
        source_path: path::PathBuf,
        _is_module: bool,
    ) -> Result<Option<String>, Error> {
        //
        // get all dependencies
        //
        // let args = compiler_proxy::Args {
        //     module_input: is_module,
        //     address: Some(address),
        //     no_stdlib: false,
        //     no_verify: false,
        //     source_path: source_path.clone(),
        //     list_dependencies: true, //specify the this flag for getting all dependencies
        //     deps_path: None,
        //     output_source_maps: false,
        // };
        // compiler_proxy::compile(args)?;

        let dependencies_path = source_path.with_extension("depir");
        //let mut tmp_output_file = fs::File::create(output_path)?;
        let code = fs::read_to_string(dependencies_path.clone())?;
        //
        //
        //
        let paths: Vec<AccessPath> = serde_json::from_str(code.as_str())?;
        let mut dependencies = vec![];
        for path in paths {
            if path.address != CORE_CODE_ADDRESS {
                if let Some(blob) = client.client.get_account_blob(path.address)? {
                    let account_state = AccountState::try_from(&blob)?;

                    if let Some(code) = account_state.get(&path.path) {
                        dependencies.push(code.clone());
                    }
                }
            }
        }
        if dependencies.is_empty() {
            return Ok(None);
        }

        let dependencies_path = source_path.with_extension("dep");
        let mut file = std::fs::File::create(dependencies_path.clone())?;
        file.write_all(&serde_json::to_vec(&dependencies)?)?;
        Ok(Some(String::from(dependencies_path.to_str().unwrap())))
    }

    #[no_mangle]
    pub extern "C" fn libra_publish_module(
        raw_ptr: u64,
        account_index: u64,
        module_file: *const c_char,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let module = unsafe { CStr::from_ptr(module_file).to_str().unwrap() };
            let index = account_index.to_string();

            client.publish_module(&["publish", index.as_str(), module])
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(_) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_publish_module' !"
            ));
            false
        }
    }

    /// association transaction with local faucet account
    #[no_mangle]
    pub fn violas_publish_module_with_association_account(
        raw_ptr: u64,
        module_file_name: *const c_char,
    ) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let module = unsafe { CStr::from_ptr(module_file_name).to_str().unwrap() };

            proxy.publish_module_with_association_account(module)
        });

        if result.is_ok() {
            match result.unwrap() {
                Ok(()) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "panic at function (libra_get_committed_txn_by_acc_seq) !"
            ));
            false
        }
    }

    #[repr(C)]
    pub struct ScriptArgs {
        len: u64,
        data: *const *const c_char, // C char* array
    }

    #[no_mangle]
    pub extern "C" fn libra_execute_script(
        raw_ptr: u64,
        account_index: u64,
        script_file: *const c_char,
        script_args: &ScriptArgs,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            // convert raw ptr to object client

            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let index = account_index.to_string();
            let script = unsafe { CStr::from_ptr(script_file).to_str().unwrap() };
            let mut args = vec!["execute", index.as_str(), script];
            let s = unsafe { slice::from_raw_parts(script_args.data, script_args.len as usize) };

            for i in 0..s.len() {
                let arg = unsafe { CStr::from_ptr(s[i]).to_str().unwrap() };
                args.push(arg);
            }
            //
            //  execute script
            //
            client.execute_script(&args) //&["execute", index.as_str(), script]
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(_) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_execute_script' !"
            ));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn violas_execute_script_with_association_account(
        raw_ptr: u64,
        script_file: *const c_char,
        script_args: &ScriptArgs,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            //
            // convert raw ptr to object client
            //
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let script = unsafe { CStr::from_ptr(script_file).to_str().unwrap() };
            let mut args = vec![script];
            let s = unsafe { slice::from_raw_parts(script_args.data, script_args.len as usize) };

            for i in 0..s.len() {
                let arg = unsafe { CStr::from_ptr(s[i]).to_str().unwrap() };
                args.push(arg);
            }
            //
            //  execute script
            //
            client.execcute_script_with_association_account(&args)
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(_) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_execute_script_with_association_account' !"
            ));
            false
        }
    }
    ///
    ///
    ///
    #[no_mangle]
    pub extern "C" fn libra_get_committed_txn_by_acc_seq(
        raw_ptr: u64,
        address: &[c_uchar; LENGTH],
        sequence_num: u64,
        out_transaction: *mut *mut c_char,
        out_event: *mut *mut c_char,
    ) -> bool {
        //
        let result = panic::catch_unwind(|| -> Result<(String, String), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

            match proxy
                .client
                .get_txn_by_acc_seq(AccountAddress::new(*address), sequence_num, true)
            {
                Ok(txn_view) => {
                    let mut txn = String::new();
                    let events = String::new();

                    match txn_view {
                        Some(txn_view) => {
                            txn = format!("{:#?}", txn_view);
                        }
                        None => {}
                    };

                    Ok((txn, events))
                }
                Err(e) => bail!(
                    "Error getting committed transaction by account and sequence number, {}",
                    e,
                ),
            }
        });

        let mut ret = false;
        if result.is_ok() {
            match result.unwrap() {
                Ok((txn, event)) => {
                    unsafe {
                        //c_char = Box::new(CString::new(txn));
                        *out_transaction = CString::new(txn)
                            .expect("new transaction detail")
                            .into_raw();
                        *out_event = CString::new(event).expect("new event detail").into_raw();
                    }
                    ret = true
                }
                Err(err) => set_last_error(err),
            }
        } else {
            set_last_error(format_err!(
                "panic at function (libra_get_committed_txn_by_acc_seq) !"
            ));
        }

        ret
    }

    #[repr(C)]
    struct txn_events {
        transaction: *mut c_char,
        events: *mut c_char,
    }

    #[repr(C)]
    pub struct all_txn_events {
        data: *mut txn_events,
        len: u64,
        cap: u64,
    }

    #[no_mangle]
    pub extern "C" fn libra_get_txn_by_range(
        raw_ptr: u64,
        start_version: u64,
        limit: u64,
        fetch_events: bool,
        out_all_txn_events: *mut all_txn_events,
    ) -> bool {
        let result = panic::catch_unwind(|| {
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

            match client
                .client
                .get_txn_by_range(start_version, limit, fetch_events)
            {
                Ok(comm_txns_and_events) => {
                    let mut vec_txn_events: Vec<txn_events> = vec![];
                    for txn_view in comm_txns_and_events {
                        let txn_format = format!("{:#?}", txn_view);
                        let all_events = format!("{:#?}", txn_view.events);

                        let output = txn_events {
                            transaction: CString::new(txn_format)
                                .expect("new transaction detail")
                                .into_raw(),
                            events: CString::new(all_events)
                                .expect("new transaction detail")
                                .into_raw(),
                        };
                        vec_txn_events.push(output);
                    }
                    unsafe {
                        (*out_all_txn_events).data = vec_txn_events.as_mut_ptr();
                        (*out_all_txn_events).len = vec_txn_events.len() as u64;
                        (*out_all_txn_events).cap = vec_txn_events.capacity() as u64;
                    }
                    std::mem::forget(vec_txn_events);
                }
                Err(e) => set_last_error(format_err!(
                    "Error getting committed transactions by range, {}",
                    e
                )),
            }
        });

        if result.is_ok() {
            true
        } else {
            set_last_error(format_err!("panic at function libra_get_txn_by_range"));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_free_all_txn_events(all_txn_events: *mut all_txn_events) {
        if all_txn_events.is_null() {
            return;
        }

        unsafe {
            let vec_txn_events: Vec<txn_events> = Vec::from_raw_parts(
                (*all_txn_events).data,
                (*all_txn_events).len as usize,
                (*all_txn_events).cap as usize,
            );

            for txn_events in vec_txn_events {
                libra_free_string(txn_events.transaction);
                libra_free_string(txn_events.events);
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn libra_get_account_resource(
        raw_ptr: u64,
        account_index_or_addr: *const c_char,
        c_account_path_addr: *const c_char,
        token_index: u64,
        balance: &mut u64,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<u64, Error> {
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let (address, _) = client.get_account_address_from_parameter(unsafe {
                CStr::from_ptr(account_index_or_addr).to_str().unwrap()
            })?;

            if let Some(blob) = client.client.get_account_blob(address)? {
                let account_state = AccountState::try_from(&blob)?;
                // debugging
                // for (movie, review) in &map {
                //     println!("{:?}: \"{:?}\"", movie, review);
                // }
                let account_path_addr =
                    unsafe { CStr::from_ptr(c_account_path_addr).to_str().unwrap() };
                let addr = AccountAddress::from_hex_literal(account_path_addr).unwrap();

                let ar = ViolasAccountResource::make_from(&addr, &account_state)?;

                let index: usize = token_index as usize;
                if index >= ar.tokens.len() {
                    bail!(format!(
                        "token index {} is more than token length {}",
                        index,
                        ar.tokens.len()
                    ));
                }

                return Ok(ar.tokens[index].balance);
            }

            bail!("Account hasn't published the module")
        });

        *balance = u64::max_value(); //set invalid balance;

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(value) => {
                    *balance = value;
                    true
                }
                Err(err) => {
                    set_last_error(err);
                    true
                }
            }
        } else {
            set_last_error(format_err!("panic at libra_get_account_resource()"));
            false
        }
    }

    /// association transaction with local faucet account
    #[no_mangle]
    pub fn libra_execute_script_with_faucet_account(
        raw_ptr: u64,
        script_file_name: *const c_char,
        script_args: &ScriptArgs,
    ) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let script = unsafe { CStr::from_ptr(script_file_name).to_str().unwrap() };
            let args1 =
                unsafe { slice::from_raw_parts(script_args.data, script_args.len as usize) };

            if proxy.faucet_account.is_none() {
                bail!("No faucet account loaded");
            }
            let sender = proxy.faucet_account.as_ref().unwrap();
            let sender_address = sender.address;
            let script_bytes = fs::read(script)?;

            let args = args1
                .iter()
                .map(|arg| unsafe { CStr::from_ptr(*arg).to_str().unwrap() })
                .filter_map(|arg| parse_transaction_argument(arg).ok())
                .collect();
            let program = TransactionPayload::Script(Script::new(script_bytes, vec![], args));

            let txn = proxy.create_txn_to_submit(program, sender, None, None)?;
            let mut sender_mut = proxy.faucet_account.as_mut().unwrap();
            let resp = proxy.client.submit_transaction(Some(&mut sender_mut), txn);
            proxy.wait_for_transaction(
                sender_address,
                proxy.faucet_account.as_ref().unwrap().sequence_number,
            );

            resp
        });

        if result.is_ok() {
            match result.unwrap() {
                Ok(()) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "panic at function (libra_get_committed_txn_by_acc_seq) !"
            ));
            false
        }
    }

    ///  Allow executing arbitrary script in the network.
    #[no_mangle]
    pub fn libra_enable_custom_script(raw_ptr: u64) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

            proxy.enable_custom_script(&["enable_custom_script"], true)
        });

        if result.is_ok() {
            match result.unwrap() {
                Ok(()) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "panic at function (libra_get_committed_txn_by_acc_seq) !"
            ));
            false
        }
    }

    ///   Only allow executing predefined script in the move standard library in the network.
    #[no_mangle]
    pub fn libra_disable_custom_script(raw_ptr: u64) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };

            proxy.disable_custom_script(&["disble_custom_script"], true)
        });

        if result.is_ok() {
            match result.unwrap() {
                Ok(()) => true,
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "panic at function (libra_get_committed_txn_by_acc_seq) !"
            ));
            false
        }
    }

    #[repr(C)]
    pub struct c_str_array {
        data: *mut *mut c_char,
        len: u64,
        cap: u64,
    }

    #[no_mangle]
    pub extern "C" fn violas_free_str_array(str_array: *mut c_str_array) {
        if str_array.is_null() {
            return;
        }

        unsafe {
            let strs: Vec<*mut c_char> = Vec::from_raw_parts(
                (*str_array).data,
                (*str_array).len as usize,
                (*str_array).cap as usize,
            );

            for str in strs {
                libra_free_string(str);
            }
        }
    }

    /// get events
    #[no_mangle]
    pub fn libra_get_events(
        raw_client: u64,
        address: &[c_uchar; LENGTH],
        event_type: bool, //ture for sent, false for received
        start_seq_number: u64,
        limit: u64,
        out_all_txn_events: *mut c_str_array,
        out_last_event_state: *mut *mut c_char,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let path = if event_type {
                    ACCOUNT_SENT_EVENT_PATH.to_vec()
                } else {
                    ACCOUNT_RECEIVED_EVENT_PATH.to_vec()
                };
                let access_path = AccessPath::new(AccountAddress::new(*address), path);

                match proxy
                    .client
                    .get_events_by_access_path(access_path, start_seq_number, limit)
                {
                    Ok((evnets, last_event_state)) => {
                        let mut vec_events: Vec<*mut c_char> = vec![];
                        for event_view in evnets {
                            let event = format!("{:#?}", event_view);

                            vec_events.push(
                                CString::new(event)
                                    .expect("new event info failed")
                                    .into_raw(),
                            );
                        }
                        (*out_all_txn_events).data = vec_events.as_mut_ptr();
                        (*out_all_txn_events).len = vec_events.len() as u64;
                        (*out_all_txn_events).cap = vec_events.capacity() as u64;

                        std::mem::forget(vec_events);

                        *out_last_event_state = CString::new(format!("{:#?}", last_event_state))
                            .expect("new last event state failed")
                            .into_raw();
                        true
                    }
                    Err(e) => {
                        set_last_error(format_err!("failed to get events with error, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!("catch panic at function 'libra_get_events' !'"));
                false
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Violas Interfaces
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    ///
    #[repr(C)]
    pub struct ViolasTypeTag {
        address: [c_uchar; LENGTH],
        module: *const c_char,
        name: *const c_char,
    }
    /// add a new currency
    #[no_mangle]
    pub fn violas_add_currency(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        exchange_rate_denom: u64,
        exchange_rate_num: u64,
        is_synthetic: bool,
        scaling_factor: u64,
        fractional_part: u64,
        currency_code: *const c_uchar,
        currency_code_len: u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let data = slice::from_raw_parts(currency_code, currency_code_len as usize);
                let type_tag = currency_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                );

                //
                match proxy.add_currency(
                    type_tag,
                    exchange_rate_denom,
                    exchange_rate_num,
                    is_synthetic,
                    scaling_factor,
                    fractional_part,
                    data.to_vec(),
                    true,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!("failed to add currency with error, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch a panic at function 'violas_add_currency' !'"
                ));
                false
            }
        }
    }

    /// register currency for an account
    #[no_mangle]
    pub fn violas_register_currency(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        account_index: u64,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let type_tag = currency_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                );
                // register currency
                match proxy.register_currency(type_tag, account_index as usize, is_blocking) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!("failed to add currency with error, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch a panic at function 'violas_add_currency' !'"
                ));
                false
            }
        }
    }

    /// register currency for an account
    #[no_mangle]
    pub fn violas_register_currency_with_association_account(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let type_tag = currency_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                );
                // register currency
                match proxy.register_currency_with_association_account(type_tag, is_blocking) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to add currency with association, error : {}",
                            e
                        ));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch a panic at function 'violas_add_currency' !'"
                ));
                false
            }
        }
    }

    /// mint currency for an account
    #[no_mangle]
    pub fn violas_mint_currency(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        receiver: &[c_uchar; 32],
        amount: u64,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let type_tag = currency_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                );

                let auth_key = AuthenticationKey::new(*receiver);

                // register currency
                match proxy.mint_currency(
                    type_tag,
                    &auth_key.derived_address(),
                    auth_key.prefix().to_vec(),
                    amount,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!("failed to add currency with error, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch a panic at function 'violas_add_currency' !'"
                ));
                false
            }
        }
    }

    /// transfer currency from sender to receiver
    #[no_mangle]
    pub fn violas_transfer_currency(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        sender_account_index: u64,
        receiver: &[c_uchar; 32],
        amount: u64,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let type_tag = currency_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                );

                let auth_key = AuthenticationKey::new(*receiver);

                // register currency
                match proxy.transfer_currency(
                    type_tag,
                    sender_account_index as usize,
                    &auth_key.derived_address(),
                    auth_key.prefix().to_vec(),
                    amount,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!("failed to add currency with error, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch a panic at function 'violas_add_currency' !'"
                ));
                false
            }
        }
    }

    ///
    #[no_mangle]
    pub fn violas_get_currency_balance(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        addr: &[u8; 16],
        out_balance: &mut u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> Option<u64> {
                let proxy = &mut *(raw_client as *mut ClientProxy);
                let tag = currency_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                );

                match proxy.get_balance_of_currency(tag, AccountAddress::new(*addr)) {
                    Ok(balance) => Some(balance),
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to get balance of currency with error, {}",
                            e
                        ));
                        None
                    }
                }
            });

            *out_balance = u64::max_value();

            if ret.is_ok() {
                match ret.unwrap() {
                    Some(balance) => {
                        *out_balance = balance;
                        true
                    }
                    None => false,
                }
            } else {
                set_last_error(format_err!(
                    "catch a panic at function 'violas_add_currency' !'"
                ));
                false
            }
        }
    }
}
