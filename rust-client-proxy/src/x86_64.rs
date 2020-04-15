//#[cfg(target_arch = "x86_64")]
#[allow(non_snake_case)]
pub mod x86_64 {
    use crate::{compiler_proxy, violas_account};
    use anyhow::{bail, format_err, Error};
    use cli::client_proxy::{AccountEntry, ClientProxy};
    use cli::AccountStatus;
    use libra_types::{
        access_path::AccessPath, account_address::AccountAddress,
        account_config::CORE_CODE_ADDRESS, account_state::AccountState, transaction::*,
    }; //ADDRESS_LENGTH access_path::AccessPath,
    use std::cell::RefCell;
    use std::ffi::{CStr, CString};
    use std::os::raw::{c_char, c_uchar};
    use std::{convert::TryFrom, io::Write, path::Path, result::Result, *};
    use tempdir::TempDir;
    //use transaction_builder::get_transaction_name;

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
    pub extern "C" fn libra_create_client_proxy(
        c_host: *const c_char,
        c_port: u16,
        _c_validator_set_file: *const c_char,
        c_faucet_account_file: *const c_char,
        c_sync_on_wallet_recovery: bool,
        c_faucet_server: *const c_char,
        c_mnemonic_file: *const c_char,
    ) -> u64 {
        let ret = panic::catch_unwind(|| -> Result<ClientProxy, Error> {
            let host = unsafe { CStr::from_ptr(c_host).to_str().unwrap() };
            let port = c_port as u16;
            //let validator_set_file =
            //    unsafe { CStr::from_ptr(c_validator_set_file).to_str().unwrap() };
            let faucet_account_file =
                unsafe { CStr::from_ptr(c_faucet_account_file).to_str().unwrap() };
            let sync_on_wallet_recovery = c_sync_on_wallet_recovery as bool;
            let faucet_server =
                unsafe { CStr::from_ptr(c_faucet_server).to_str().unwrap().to_owned() };
            let mnemonic_file =
                unsafe { CStr::from_ptr(c_mnemonic_file).to_str().unwrap().to_owned() };
            let url = format!("http://{}:{}", host, port);
            println!("{}", url);

            //
            // new Client Proxy
            //
            ClientProxy::new(
                url.as_str(),
                faucet_account_file,
                sync_on_wallet_recovery,
                Some(faucet_server),
                Some(mnemonic_file),
                None,
            )
        });

        //
        //  Check the result and then return a raw pointer
        //
        // let raw_ptr = match ret.is_ok() {
        //     Some(value) => Box::into_raw(Box::new(value)) as u64,
        //     None => {
        //         set_last_error(format_err!("failed to new client proxy "));
        //         0
        //     }
        // };

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(value) => Box::into_raw(Box::new(value)) as u64,
                Err(err) => {
                    set_last_error(err);
                    0
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_create_client_proxy' !'"
            ));
            0
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

        //println!("free_all_accounts_buf entered");
    }

    #[no_mangle]
    pub extern "C" fn set_accounts() {
        //
    }

    #[no_mangle]
    pub extern "C" fn libra_get_balance(
        raw_ptr: u64,
        account_index_or_addr: *const c_char,
        result: &mut f64,
    ) -> bool {
        if raw_ptr == 0 || account_index_or_addr.is_null() {
            set_last_error(format_err!(
                "raw ptr is null, or account index or address is null"
            ));
            return false;
        }

        let ret = panic::catch_unwind(|| -> Result<f64, Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let index_or_addr = unsafe { CStr::from_ptr(account_index_or_addr).to_str().unwrap() };
            let balance = client
                .get_balance(&["b", index_or_addr])
                .unwrap_or_else(|err| {
                    println!("failed to get balance, {}", err);
                    String::from("0")
                });
            let value = balance.parse::<f64>().unwrap();
            Ok(value)
        });

        if ret.is_ok() {
            match ret.unwrap() {
                Ok(value) => {
                    *result = value;
                    true
                }
                Err(err) => {
                    set_last_error(err);
                    false
                }
            }
        } else {
            set_last_error(format_err!(
                "catch panic at function 'libra_get_balance' !'"
            ));
            false
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
        let ret = panic::catch_unwind(|| -> Result<cli::client_proxy::IndexAndSequence, Error> {
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
                let deps_path = handle_dependencies(
                    client,
                    address.to_string(),
                    temp_source_path.clone(),
                    is_module,
                )?;
                //
                // compile the source code
                //
                let args = compiler_proxy::Args {
                    module_input: is_module,
                    address: Some(address.to_string()), //
                    no_stdlib: false,
                    no_verify: false,
                    source_path: temp_source_path.clone(),
                    list_dependencies: false,
                    deps_path: deps_path, //Option(String::from_str(dependencies_path.to_str())),
                    output_source_maps: false,
                };
                compiler_proxy::compile(args)?;

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
        address: String,
        source_path: path::PathBuf,
        is_module: bool,
    ) -> Result<Option<String>, Error> {
        //
        // get all dependencies
        //
        let args = compiler_proxy::Args {
            module_input: is_module,
            address: Some(address),
            no_stdlib: false,
            no_verify: false,
            source_path: source_path.clone(),
            list_dependencies: true, //specify the this flag for getting all dependencies
            deps_path: None,
            output_source_maps: false,
        };
        compiler_proxy::compile(args)?;

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

            //client.publish_module(&["publish", index.as_str(), module])
            client.publish_module_with_faucet_account(&["publish", index.as_str(), module])
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

                let ar = violas_account::ViolasAccountResource::make_from(&addr, &account_state)?;

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

    // association transaction with local faucet account
    // Publish move module with faucet account
    // #[no_mangle]
    // pub fn publish_module_with_faucet_account(&mut self, space_delim_strings: &[&str]) -> Result<()>
    // {
    //     ensure!(self.faucet_account.is_some(), "No faucet account loaded");
    //     let sender = self.faucet_account.as_ref().unwrap();
    //     let sender_address = sender.address;
    //     let module_bytes = fs::read(space_delim_strings[2])?;
    //     let program  = TransactionPayload::Module(Module::new(module_bytes));

    //     let txn = self.create_txn_to_submit(program, sender, None, None)?;
    //     let mut sender_mut = self.faucet_account.as_mut().unwrap();
    //     let resp = self.client.submit_transaction(Some(&mut sender_mut), txn);
    //     self.wait_for_transaction(
    //         sender_address,
    //         self.faucet_account.as_ref().unwrap().sequence_number,
    //     );

    //     resp
    // }

    /// association transaction with local faucet account
    #[no_mangle]
    pub fn publish_module_with_faucet_account(
        raw_ptr: u64,
        module_file_name: *const c_char,
    ) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ClientProxy) };
            let module = unsafe { CStr::from_ptr(module_file_name).to_str().unwrap() };

            if proxy.faucet_account.is_some() {
                bail!("No faucet account loaded");
            }
            let sender = proxy.faucet_account.as_ref().unwrap();
            let sender_address = sender.address;
            let module_bytes = fs::read(module)?;
            let program = TransactionPayload::Module(Module::new(module_bytes));

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
}
