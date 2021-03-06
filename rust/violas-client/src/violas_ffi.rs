//#[cfg(target_arch = "x86_64")]
#[allow(non_snake_case)]
pub mod x86_64 {
    use crate::diem_client_proxy::{AccountEntry, IndexAndSequence};
    //use crate::move_compiler;
    use crate::violas_account::*;
    use crate::violas_client::ViolasClient;
    use crate::AccountStatus;
    use anyhow::{bail, format_err, Error};
    use diem_types::{
        access_path::AccessPath, account_address::AccountAddress,
        account_config::CORE_CODE_ADDRESS, account_config::*, account_state::AccountState,
        chain_id::ChainId, transaction::authenticator::AuthenticationKey, waypoint::Waypoint,
    };
    use move_core_types::{identifier::Identifier, language_storage::StructTag};
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
    pub extern "C" fn diem_get_last_error() -> *const c_char {
        LAST_ERROR.with(|prev| {
            let err = prev.borrow_mut();
            CString::new(err.clone()).unwrap().into_raw()
        })
    }
    ///
    /// free rust string
    ///
    #[no_mangle]
    pub extern "C" fn diem_free_string(s: *mut c_char) {
        unsafe {
            if s.is_null() {
                return;
            }
            CString::from_raw(s)
        };
    }
    //
    //  create diem client proxy
    //
    #[no_mangle]
    pub extern "C" fn violas_create_client(
        chain_id: u8,
        c_url: *const c_char,
        c_mint_key: *const c_char,
        sync_on_wallet_recovery: bool,
        _faucet_server: *const c_char,
        c_mnemonic: *const c_char,
        in_waypoint: *const c_char,
        out_clinet_ptr: *mut u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| {
                let faucet_account_file = CStr::from_ptr(c_mint_key).to_str().unwrap();
                // Faucet and TreasuryCompliance use the same keypair for now
                let treasury_compliance_account_file = faucet_account_file.clone();

                let client = ViolasClient::new(
                    ChainId::new(chain_id),
                    CStr::from_ptr(c_url).to_str().unwrap(),
                    faucet_account_file,
                    faucet_account_file,
                    treasury_compliance_account_file,
                    sync_on_wallet_recovery,
                    None, //Some(CStr::from_ptr(faucet_server).to_str().unwrap().to_owned()),
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
    /// Destory the raw ViolasClient pointer
    ///
    #[no_mangle]
    pub extern "C" fn diem_destory_client_proxy(raw_ptr: u64) {
        if raw_ptr != 0 {
            let _proxy = unsafe { Box::from_raw(raw_ptr as *mut ViolasClient) };
        }
    }

    #[no_mangle]
    pub extern "C" fn diem_test_validator_connection(raw_ptr: u64) -> bool {
        let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
        let ret = client.test_validator_connection();
        //return the boolean result
        match ret {
            Ok(_) => true,
            Err(_) => false,
        }
    }

    #[no_mangle]
    pub extern "C" fn diem_create_next_address(_raw_ptr: u64, _sync_with_validator: bool) -> bool {
        // let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

        // let (address, _) = client.wallet.new_address().unwrap();

        true
    }

    #[repr(C)]
    pub struct AccountAndIndex {
        pub address: [c_uchar; LENGTH],
        pub index: u64,
    }

    #[no_mangle]
    pub extern "C" fn diem_create_next_account(
        raw_ptr: u64,
        sync_with_validator: bool,
    ) -> AccountAndIndex {
        let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

        let (address, index) = client
            .create_next_account(None, sync_with_validator)
            .expect("failed to call create_next_account");

        // println!(
        //     "Created/retrieved account #{} address {}",
        //     accountAndIndex.index,
        //     hex::encode(accountAndIndex.address)
        // );

        let mut account = AccountAndIndex {
            index: index as u64,
            address: [0; LENGTH],
        };

        account.address.copy_from_slice(&address.to_vec());

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
    pub extern "C" fn diem_get_all_accounts(raw_ptr: u64) -> Accounts {
        //
        let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
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
    pub extern "C" fn diem_free_all_accounts_buf(buf: Accounts) {
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
    pub extern "C" fn diem_get_balance(
        raw_ptr: u64,
        address: &[c_uchar; LENGTH],
        out_balance: *mut u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                // convert raw ptr to object client
                let proxy = &mut *(raw_ptr as *mut ViolasClient);
                let ret = proxy
                    .client
                    .get_account(AccountAddress::new(*address), true);
                match ret {
                    Ok(account) => match account.0 {
                        Some(account_view) => {
                            *out_balance = account_view.balances[2].amount; //LBR balance
                            true
                        }
                        None => {
                            set_last_error(format_err!("get_account error, no account data"));
                            false
                        }
                    },
                    Err(e) => {
                        set_last_error(format_err!("get_account error, {}", e));
                        false
                    }
                }
            });
            if ret.is_ok() {
                ret.unwrap()
            } else {
                set_last_error(format_err!(
                    "catch panic at function 'diem_get_balance' !'"
                ));
                false
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn diem_get_sequence_number(
        raw_ptr: u64,
        address: &[c_uchar; LENGTH],
        result: &mut u64,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<u64, Error> {
            // convert raw ptr to object client
            let proxy = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

            let account_state = proxy
                .client
                .get_account(AccountAddress::new(*address), true)?;
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
                "catch panic at function 'diem_get_sequence_number' !"
            ));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn diem_mint_coins(
        raw_ptr: u64,
        index: u64,
        num_coins: u64,
        is_blocking: bool,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

            client.mint_coins(
                &[
                    "mintb",
                    index.to_string().as_str(),
                    num_coins.to_string().as_str(),
                    "LBR",
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
            set_last_error(format_err!("catch panic at function (diem_mint_coins) !"));
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
    pub extern "C" fn diem_transfer_coins_int(
        raw_ptr: u64,
        sender_account_ref_id: usize,
        receiver_addr: &[u8; 16],
        micro_coins: u64,
        _gas_unit_price: u64,
        _max_gas_amount: u64,
        is_blocking: bool,
        result: &mut IndexAndSeq,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<IndexAndSequence, Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
            let receiver_address = AccountAddress::new(*receiver_addr);

            client.transfer_currency(
                sender_account_ref_id,
                &receiver_address,
                xdx_type_tag(),
                micro_coins,
                None,
                None,
                None,
                is_blocking,
            )?;

            Ok(IndexAndSequence {
                account_index: AccountEntry::Index(sender_account_ref_id),
                sequence_number: 0,
            })
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
                "catch panic at function 'diem_transfer_coins_int' !"
            ));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn diem_compile(
        raw_ptr: u64,
        account_index_or_addr: *const c_char,
        script_path: *const c_char,
        is_module: bool,
        _temp_path: *const c_char,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            // convert raw ptr to object client
            let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
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
            set_last_error(format_err!("catch panic at function (diem_compile) !"));
            false
        }
    }

    fn handle_dependencies(
        client: &mut ViolasClient,
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
                if let (Some(blob), _) = client.client.get_account_state_blob(path.address)? {
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

    /// association transaction with local faucet account
    #[no_mangle]
    pub fn violas_publish_module(
        raw_ptr: u64,
        sender_ref_id: usize,
        module_file_name: *const c_char,
    ) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
            let module = unsafe { CStr::from_ptr(module_file_name).to_str().unwrap() };

            proxy.publish_module(sender_ref_id, module)
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
                "panic at function (diem_get_committed_txn_by_acc_seq) !"
            ));
            false
        }
    }

    #[repr(C)]
    pub struct ScriptArgs {
        len: u64,
        data: *const *const c_char, // C char* array
    }

    // #[no_mangle]
    // pub extern "C" fn diem_execute_script(
    //     raw_ptr: u64,
    //     account_index: u64,
    //     script_file: *const c_char,
    //     script_args: &ScriptArgs,
    // ) -> bool {
    //     let ret = panic::catch_unwind(|| -> Result<(), Error> {
    //         // convert raw ptr to object client

    //         let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
    //         let index = account_index.to_string();
    //         let script = unsafe { CStr::from_ptr(script_file).to_str().unwrap() };
    //         let mut args = vec!["execute", index.as_str(), script];
    //         let s = unsafe { slice::from_raw_parts(script_args.data, script_args.len as usize) };

    //         for i in 0..s.len() {
    //             let arg = unsafe { CStr::from_ptr(s[i]).to_str().unwrap() };
    //             args.push(arg);
    //         }
    //         //
    //         //  execute script
    //         //
    //         client.execute_script(&args) //&["execute", index.as_str(), script]
    //     });

    //     if ret.is_ok() {
    //         match ret.unwrap() {
    //             Ok(_) => true,
    //             Err(err) => {
    //                 set_last_error(err);
    //                 false
    //             }
    //         }
    //     } else {
    //         set_last_error(format_err!(
    //             "catch panic at function 'diem_execute_script' !"
    //         ));
    //         false
    //     }
    // }
    ///
    ///
    ///
    #[no_mangle]
    pub extern "C" fn diem_get_committed_txn_by_acc_seq(
        raw_ptr: u64,
        address: &[c_uchar; LENGTH],
        sequence_num: u64,
        fetch_event: bool,
        out_transaction: *mut *mut c_char,
    ) -> bool {
        //
        let result = panic::catch_unwind(|| -> Result<String, Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

            match proxy.client.get_txn_by_acc_seq(
                AccountAddress::new(*address),
                sequence_num,
                fetch_event,
            ) {
                Ok(txn_view) => {
                    let mut txn = String::new();
                    match txn_view {
                        Some(txn_view) => {
                            txn = format!("{:#?}", txn_view);
                        }
                        None => {}
                    };

                    Ok(txn)
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
                Ok(txn) => {
                    unsafe {
                        //c_char = Box::new(CString::new(txn));
                        *out_transaction = CString::new(txn)
                            .expect("new transaction detail")
                            .into_raw();
                    }
                    ret = true
                }
                Err(err) => set_last_error(err),
            }
        } else {
            set_last_error(format_err!(
                "panic at function \"diem_get_committed_txn_by_acc_seq\" !"
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
    pub extern "C" fn diem_get_txn_by_range(
        raw_ptr: u64,
        start_version: u64,
        limit: u64,
        fetch_events: bool,
        out_all_txn_events: *mut all_txn_events,
    ) -> bool {
        let result = panic::catch_unwind(|| {
            let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

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
            set_last_error(format_err!("panic at function diem_get_txn_by_range"));
            false
        }
    }

    #[no_mangle]
    pub extern "C" fn diem_free_all_txn_events(all_txn_events: *mut all_txn_events) {
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
                diem_free_string(txn_events.transaction);
                diem_free_string(txn_events.events);
            }
        }
    }

    // #[no_mangle]
    // pub extern "C" fn diem_get_account_resource(
    //     raw_ptr: u64,
    //     account_index_or_addr: *const c_char,
    //     c_account_path_addr: *const c_char,
    //     token_index: u64,
    //     balance: &mut u64,
    // ) -> bool {
    //     let ret = panic::catch_unwind(|| -> Result<u64, Error> {
    //         let client = unsafe { &mut *(raw_ptr as *mut ViolasClient) };
    //         let (address, _) = client.get_account_address_from_parameter(unsafe {
    //             CStr::from_ptr(account_index_or_addr).to_str().unwrap()
    //         })?;

    //         if let (Some(blob), _) = client.client.get_account_state_blob(address)? {
    //             let account_state = AccountState::try_from(&blob)?;
    //             // debugging
    //             // for (movie, review) in &map {
    //             //     println!("{:?}: \"{:?}\"", movie, review);
    //             // }
    //             let account_path_addr =
    //                 unsafe { CStr::from_ptr(c_account_path_addr).to_str().unwrap() };
    //             let addr = AccountAddress::from_hex_literal(account_path_addr).unwrap();

    //             let ar = ViolasAccountResource::make_from(&addr, &account_state)?;

    //             let index: usize = token_index as usize;
    //             if index >= ar.tokens.len() {
    //                 bail!(format!(
    //                     "token index {} is more than token length {}",
    //                     index,
    //                     ar.tokens.len()
    //                 ));
    //             }

    //             return Ok(ar.tokens[index].balance);
    //         }

    //         bail!("Account hasn't published the module")
    //     });

    //     *balance = u64::max_value(); //set invalid balance;

    //     if ret.is_ok() {
    //         match ret.unwrap() {
    //             Ok(value) => {
    //                 *balance = value;
    //                 true
    //             }
    //             Err(err) => {
    //                 set_last_error(err);
    //                 true
    //             }
    //         }
    //     } else {
    //         set_last_error(format_err!("panic at diem_get_account_resource()"));
    //         false
    //     }
    // }

    ///  Allow executing arbitrary script in the network.
    #[no_mangle]
    pub fn diem_enable_custom_script(raw_ptr: u64) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let proxy = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

            proxy.allow_custom_script(true)
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
                "panic at function (diem_get_committed_txn_by_acc_seq) !"
            ));
            false
        }
    }

    ///   Only allow executing predefined script in the move standard diemry in the network.
    #[no_mangle]
    pub fn diem_disable_custom_script(raw_ptr: u64) -> bool {
        let result = panic::catch_unwind(|| -> Result<(), Error> {
            let _proxy = unsafe { &mut *(raw_ptr as *mut ViolasClient) };

            //proxy.disable_custom_script(&["disble_custom_script"], true)
            Ok(())
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
                "panic at function (diem_get_committed_txn_by_acc_seq) !"
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
                diem_free_string(str);
            }
        }
    }

    /// get events
    #[no_mangle]
    pub fn diem_get_events(
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
                let proxy = &mut *(raw_client as *mut ViolasClient);
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
                set_last_error(format_err!("catch panic at function 'diem_get_events' !'"));
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
    /// execute script with currency type tag
    #[no_mangle]
    pub extern "C" fn violas_execute_script(
        raw_ptr: u64,
        tag: &ViolasTypeTag,
        sender_ref_id: u64,
        in_script_file_name: *const c_char,
        in_script_args: &ScriptArgs,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            unsafe {
                let client = { &mut *(raw_ptr as *mut ViolasClient) };
                let script_file_name = { CStr::from_ptr(in_script_file_name).to_str().unwrap() };
                let module_name = CStr::from_ptr(tag.module).to_str().unwrap();
                let type_tags = if module_name.is_empty() {
                    vec![]
                } else {
                    vec![make_type_tag(
                        &AccountAddress::new(tag.address),
                        module_name,
                        CStr::from_ptr(tag.name).to_str().unwrap(),
                    )]
                };

                let script_args =
                    { slice::from_raw_parts(in_script_args.data, in_script_args.len as usize) };
                let mut args: Vec<&str> = vec![];

                for i in 0..script_args.len() {
                    let arg = CStr::from_ptr(script_args[i]).to_str().unwrap();
                    args.push(arg);
                }

                let script_bytecode = fs::read(script_file_name)?;

                //
                //  execute script
                //
                client.execute_script(sender_ref_id, script_bytecode, type_tags, &args, true)
            }
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
                "catch panic at function 'violas_execute_script' !"
            ));
            false
        }
    }

    /// execute script with currency type tag
    #[no_mangle]
    pub extern "C" fn violas_execute_script_ex(
        raw_ptr: u64,
        v_t_tags: *const ViolasTypeTag,
        tag_len: u64,
        sender_ref_id: u64,
        in_script_file_name: *const c_char,
        in_script_args: &ScriptArgs,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            unsafe {
                let client = { &mut *(raw_ptr as *mut ViolasClient) };
                let script_file_name = { CStr::from_ptr(in_script_file_name).to_str().unwrap() };
                let type_tags = if tag_len == 0 {
                    vec![]
                } else {
                    let slice_tags = std::slice::from_raw_parts(v_t_tags, tag_len as usize);
                    slice_tags
                        .iter()
                        .map(|x| {
                            let modules = CStr::from_ptr((*x).module).to_str().unwrap();
                            let name = CStr::from_ptr((*x).name).to_str().unwrap();
                            make_type_tag(&AccountAddress::new((*x).address), modules, name)
                        })
                        .collect()
                };

                let script_args =
                    slice::from_raw_parts(in_script_args.data, in_script_args.len as usize);
                let mut args: Vec<&str> = vec![];

                for i in 0..script_args.len() {
                    let arg = CStr::from_ptr(script_args[i]).to_str().unwrap();
                    args.push(arg);
                }

                let script_bytecode = fs::read(script_file_name)?;

                client.execute_script(sender_ref_id, script_bytecode, type_tags, &args, true)
            }
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
                "catch panic at function 'violas_execute_script' !"
            ));
            false
        }
    }

    /// execute script with currency type tag
    #[no_mangle]
    pub extern "C" fn violas_execute_script_json(
        raw_ptr: u64,
        v_t_tags: *const ViolasTypeTag,
        tag_len: u64,
        sender_ref_id: u64,
        in_script_file_name: *const c_char,
        in_script_args: &ScriptArgs,
    ) -> bool {
        let ret = panic::catch_unwind(|| -> Result<(), Error> {
            unsafe {
                let client = { &mut *(raw_ptr as *mut ViolasClient) };
                let script_file_name = { CStr::from_ptr(in_script_file_name).to_str().unwrap() };
                let type_tags = if tag_len == 0 {
                    vec![]
                } else {
                    std::slice::from_raw_parts(v_t_tags, tag_len as usize)
                        .iter()
                        .map(|x| {
                            let modules = CStr::from_ptr((*x).module).to_str().unwrap();
                            let name = CStr::from_ptr((*x).name).to_str().unwrap();
                            make_type_tag(&AccountAddress::new((*x).address), modules, name)
                        })
                        .collect()
                };

                let script_args =
                    slice::from_raw_parts(in_script_args.data, in_script_args.len as usize);
                let mut args: Vec<&str> = vec![];

                for i in 0..script_args.len() {
                    let arg = CStr::from_ptr(script_args[i]).to_str().unwrap();
                    args.push(arg);
                }


                let script_bytecode = fs::read(script_file_name)?;

                client.execute_script(sender_ref_id, script_bytecode, type_tags, &args, true)
            }
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
                "catch panic at function 'violas_execute_script' !"
            ));
            false
        }
    }

    /// publish a new module with specialfied currency code
    #[no_mangle]
    pub extern "C" fn violas_publish_currency(
        raw_client: u64,
        in_module_name: *const c_char,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let module_name = CStr::from_ptr(in_module_name).to_str().unwrap();
                // publish currency
                match proxy.publish_currency(module_name) {
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

    /// add a new currency
    #[no_mangle]
    pub fn violas_register_currency(
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
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let data = slice::from_raw_parts(currency_code, currency_code_len as usize);
                let type_tag = make_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                    CStr::from_ptr(violas_type_tag.name).to_str().unwrap(),
                );

                //
                match proxy.register_currency(
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
                        set_last_error(format_err!(
                            "failed to register currency with error, {}",
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

    /// publish currenty for the designated dealer account
    #[no_mangle]
    pub fn violas_add_currency_for_designated_dealer(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        dd_address: &[c_uchar; 16],
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let currency = make_type_tag(
                    &AccountAddress::new(violas_type_tag.address),
                    CStr::from_ptr(violas_type_tag.module).to_str().unwrap(),
                    CStr::from_ptr(violas_type_tag.name).to_str().unwrap(),
                );
                // register currency
                match proxy.add_currency_for_designated_dealer(
                    currency,
                    AccountAddress::new(*dd_address),
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to violas add currency to designated dealer with error, {}",
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
                    "catch a panic at function 'violas_add_currency_for_designated_dealer' !'"
                ));
                false
            }
        }
    }

    /// add currency for an account
    #[no_mangle]
    pub fn violas_add_currency(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        account_index: usize,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag =
                    make_currency_tag(CStr::from_ptr(violas_type_tag.module).to_str().unwrap());
                // register currency
                match proxy.add_currency(account_index, type_tag, is_blocking) {
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

    /// mint currency for an account
    #[no_mangle]
    pub fn violas_mint_currency(
        raw_client: u64,
        violas_type_tag: &ViolasTypeTag,
        sliding_nonce: u64,
        dd_address: &[c_uchar; 16],
        amount: u64,
        tier_index: u64,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag =
                    make_currency_tag(CStr::from_ptr(violas_type_tag.module).to_str().unwrap());

                // register currency
                match proxy.mint_currency(
                    type_tag,
                    sliding_nonce,
                    AccountAddress::new(*dd_address),
                    amount,
                    tier_index,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!("failed to mint currency with error, {}", e));
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
        receiver: &[c_uchar; 16],
        amount: u64,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag =
                    make_currency_tag(CStr::from_ptr(violas_type_tag.module).to_str().unwrap());

                // register currency
                match proxy.transfer_currency(
                    sender_account_index as usize,
                    &AccountAddress::new(*receiver),
                    type_tag,
                    amount,
                    None,
                    None,
                    None,
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
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let tag =
                    make_currency_tag(CStr::from_ptr(violas_type_tag.module).to_str().unwrap());

                match proxy.get_currency_balance(tag, AccountAddress::new(*addr)) {
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

    /// get currency info
    #[no_mangle]
    pub fn violas_get_currency_info(raw_client: u64, out_currency_info: *mut *mut c_char) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);

                // register currency
                match proxy.client.get_currency_info() {
                    Ok(view) => {
                        let currency_info = serde_json::to_string(&view).unwrap();
                        *out_currency_info = CString::new(currency_info)
                            .expect("new transaction detail")
                            .into_raw();
                        true
                    }
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to get_currency_info with error, {}",
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
                    "catch a panic at function 'violas_get_currency_info' !'"
                ));
                false
            }
        }
    }

    /// Get account state
    #[no_mangle]
    pub fn violas_get_account_state(
        raw_client: u64,
        address: &[u8; 16],
        out_state: *mut *mut c_char,
        out_version: *mut u64,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);

                // register currency
                match proxy
                    .client
                    .get_account(AccountAddress::new(*address), true)
                {
                    Ok((state, version)) => {
                        *out_version = version;
                        match state {
                            Some(view) => {
                                let json_state = serde_json::to_string(&view).unwrap();
                                *out_state = CString::new(json_state)
                                    .expect("new transaction detail")
                                    .into_raw();
                                true
                            }
                            None => false,
                        }
                    }
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to get account state with error, {}",
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
                    "catch a panic at function 'violas_get_currency_info' !'"
                ));
                false
            }
        }
    }
    /// Create a testnet account
    #[no_mangle]
    pub fn violas_create_testing_account(
        raw_client: u64,
        in_type_tag: &ViolasTypeTag,
        in_auth_key: &[u8; 32],
        add_all_currencies: bool,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag =
                    make_currency_tag(CStr::from_ptr(in_type_tag.module).to_str().unwrap());

                let auth_key = AuthenticationKey::new(*in_auth_key);
                // register currency
                match proxy.create_parent_vasp_account(
                    type_tag,
                    0,
                    auth_key.derived_address(),
                    auth_key.prefix().to_vec(),
                    "user".as_bytes().to_vec(),
                    vec![],
                    vec![],
                    add_all_currencies,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to call violas_create_parent_vasp_account, {}",
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
                    "catch a panic at function 'violas_create_parent_vasp_account' !'"
                ));
                false
            }
        }
    }
    /// Create parent VASP account
    #[no_mangle]
    pub fn violas_create_parent_vasp_account(
        raw_client: u64,
        in_type_tag: &ViolasTypeTag,
        sliding_nonce: u64,
        in_auth_key: &[u8; 32],
        in_human_name: *const c_char,
        in_base_url: *const c_char,
        in_compliance_pubkey: &[u8; 32],
        add_all_currencies: bool,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag =
                    make_currency_tag(CStr::from_ptr(in_type_tag.module).to_str().unwrap());

                let auth_key = AuthenticationKey::new(*in_auth_key);
                let human_name = CStr::from_ptr(in_human_name)
                    .to_str()
                    .unwrap()
                    .as_bytes()
                    .to_owned();
                let base_url = CStr::from_ptr(in_base_url)
                    .to_str()
                    .unwrap()
                    .as_bytes()
                    .to_owned();
                let compliance_pubkey = in_compliance_pubkey.to_owned().to_vec();
                // register currency
                match proxy.create_parent_vasp_account(
                    type_tag,
                    sliding_nonce,
                    auth_key.derived_address(),
                    auth_key.prefix().to_vec(),
                    human_name,
                    base_url,
                    compliance_pubkey,
                    add_all_currencies,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to call violas_create_parent_vasp_account, {}",
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
                    "catch a panic at function 'violas_create_parent_vasp_account' !'"
                ));
                false
            }
        }
    }

    /// Create child VASP account
    #[no_mangle]
    pub fn violas_create_child_vasp_account(
        raw_client: u64,
        in_type_tag: &ViolasTypeTag,
        parent_account_index: u64,
        in_auth_key: &[u8; 32],
        add_all_currencies: bool,
        initial_balance: u64,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag = make_type_tag(
                    &AccountAddress::new(in_type_tag.address),
                    CStr::from_ptr(in_type_tag.module).to_str().unwrap(),
                    CStr::from_ptr(in_type_tag.name).to_str().unwrap(),
                );

                let auth_key = AuthenticationKey::new(*in_auth_key);

                // register currency
                match proxy.create_child_vasp_account(
                    type_tag,
                    parent_account_index,
                    auth_key.derived_address(),
                    auth_key.prefix().to_vec(),
                    add_all_currencies,
                    initial_balance,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to create child VASP account with error, {}",
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
                    "catch a panic at function 'violas_create_child_vasp_account' !'"
                ));
                false
            }
        }
    }

    /// Create child VASP account
    #[no_mangle]
    pub fn violas_create_designated_dealer_account(
        raw_client: u64,
        in_type_tag: &ViolasTypeTag,
        in_auth_key: &[u8; 32],
        nonce: u64,
        in_human_name: *const c_char,
        in_base_url: *const c_char,
        in_compliance_pubkey: &[u8; 32],
        add_all_currencies: bool,
        is_blocking: bool,
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let type_tag =
                    make_currency_tag(CStr::from_ptr(in_type_tag.module).to_str().unwrap());

                let auth_key = AuthenticationKey::new(*in_auth_key);
                let human_name = CStr::from_ptr(in_human_name)
                    .to_str()
                    .unwrap()
                    .as_bytes()
                    .to_owned();
                let base_url = CStr::from_ptr(in_base_url)
                    .to_str()
                    .unwrap()
                    .as_bytes()
                    .to_owned();
                let compliance_pubkey = in_compliance_pubkey.to_owned().to_vec();

                // register currency
                match proxy.create_designated_dealer_account(
                    type_tag,
                    nonce,
                    auth_key.derived_address(),
                    auth_key.prefix().to_vec(),
                    human_name,
                    base_url,
                    compliance_pubkey,
                    add_all_currencies,
                    is_blocking,
                ) {
                    Ok(_) => true,
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to create child VASP account with error, {}",
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
                    "catch a panic at function 'violas_create_child_vasp_account' !'"
                ));
                false
            }
        }
    }

    /// Get Exchange currencies
    #[no_mangle]
    pub fn violas_get_exchange_currencies(
        raw_client: u64,
        address: &[u8; 16],
        out_currencies: *mut *mut c_char, // output json string
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let addr = AccountAddress::new(*address);
                let tag = StructTag {
                    address: CORE_CODE_ADDRESS,
                    module: Identifier::new("Exchange").unwrap(),
                    name: Identifier::new("RegisteredCurrencies").unwrap(),
                    type_params: vec![],
                };

                let ret: Result<Option<exchange::RegisteredCurrencies>, Error> =
                    proxy.get_account_resource(&addr, &tag);

                match ret {
                    Ok(opt) => match opt {
                        Some(view) => {
                            let json_currencies = serde_json::to_string(&view).unwrap();
                            *out_currencies = CString::new(json_currencies)
                                .expect("new reserves detail")
                                .into_raw();
                            true
                        }
                        None => {
                            *out_currencies =
                                CString::new("").expect("new reserves detail").into_raw();
                            true
                        }
                    },
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to get exchagne  reserves with error, {}",
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
                    "catch a panic at function 'violas_get_account_resource' !'"
                ));
                false
            }
        }
    }

    /// Get account resource
    #[no_mangle]
    pub fn violas_get_exchange_reserves(
        raw_client: u64,
        address: &[u8; 16],
        //tag_path: ViolasTypeTag,
        out_reserves_info: *mut *mut c_char, // output json string
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let addr = AccountAddress::new(*address);
                let tag = StructTag {
                    address: CORE_CODE_ADDRESS,
                    module: Identifier::new("Exchange").unwrap(),
                    name: Identifier::new("Reserves").unwrap(),
                    type_params: vec![],
                };

                let ret: Result<Option<exchange::Reserves>, Error> =
                    proxy.get_account_resource(&addr, &tag);

                match ret {
                    Ok(opt) => match opt {
                        Some(view) => {
                            let json_reserves = serde_json::to_string(&view).unwrap();
                            *out_reserves_info = CString::new(json_reserves)
                                .expect("new reserves detail")
                                .into_raw();

                            true
                        }
                        None => false,
                    },
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to get exchagne  reserves with error, {}",
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
                    "catch a panic at function 'violas_get_account_resource' !'"
                ));
                false
            }
        }
    }

    /// Get account resource
    #[no_mangle]
    pub fn violas_get_liquidity_balance(
        raw_client: u64,
        address: &[u8; 16],
        //tag_path: ViolasTypeTag,
        out_tokens: *mut *mut c_char, // output json string
    ) -> bool {
        unsafe {
            let ret = panic::catch_unwind(|| -> bool {
                let proxy = &mut *(raw_client as *mut ViolasClient);
                let addr = AccountAddress::new(*address);
                let tag = StructTag {
                    address: CORE_CODE_ADDRESS,
                    module: Identifier::new("Exchange").unwrap(),
                    name: Identifier::new("Tokens").unwrap(),
                    type_params: vec![],
                };

                let ret: Result<Option<exchange::Tokens>, Error> =
                    proxy.get_account_resource(&addr, &tag);

                match ret {
                    Ok(opt) => match opt {
                        Some(view) => {
                            let json_reserves = serde_json::to_string(&view).unwrap();
                            *out_tokens = CString::new(json_reserves)
                                .expect("new reserves detail")
                                .into_raw();

                            true
                        }
                        None => {
                            *out_tokens = CString::new("").expect("new reserves detail").into_raw();
                            false
                        }
                    },
                    Err(e) => {
                        set_last_error(format_err!(
                            "failed to get exchagne  reserves with error, {}",
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
                    "catch a panic at function 'violas_get_account_resource' !'"
                ));
                false
            }
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }

    #[test]
    fn say_hello() {
        print!("hello");
    }
}
