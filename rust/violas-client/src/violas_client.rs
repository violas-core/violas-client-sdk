use crate::{
    diem_client::DiemClient,
    diem_client_proxy::ClientProxy,
    violas_account::CurrencyInfoViewEx, //make_currency_tag
    AccountData,                        //AccountStatus
    AccountStatus,
};
use anyhow::{bail, ensure, format_err, Result}; //ensure, Error
use diem_crypto::{
    ed25519::{Ed25519PrivateKey, Ed25519PublicKey}, //Ed25519Signature
    test_utils::KeyPair,
};
use diem_json_rpc_client::async_client::{types as jsonrpc, WaitForTransactionError};

use diem_types::{
    access_path::AccessPath,
    account_address::AccountAddress,
    account_config::{
        diem_root_address, BalanceResource, ACCOUNT_RECEIVED_EVENT_PATH, ACCOUNT_SENT_EVENT_PATH,
    },
    account_state::AccountState,
    chain_id::ChainId,
    //on_chain_config::VMPublishingOption,
    transaction::{
        authenticator::AuthenticationKey,
        helpers::{create_user_txn, TransactionSigner}, //create_unsigned_txn
        parse_transaction_argument,
        Module,
        Script,
        SignedTransaction,
        TransactionArgument,
        TransactionPayload,
    },
    waypoint::Waypoint,
};
use diem_wallet::WalletLibrary;
use move_core_types::language_storage::{StructTag, TypeTag};
use serde::de::DeserializeOwned;
use std::{
    convert::TryFrom,
    fs::{self, File},
    io::Write,
    ops::{Deref, DerefMut},
    path::Path,
    str::{self},
    time,
};
use transaction_builder;

//const CLIENT_WALLET_MNEMONIC_FILE: &str = "client.mnemonic";
const GAS_UNIT_PRICE: u64 = 0;
const MAX_GAS_AMOUNT: u64 = 1_000_000;
const TX_EXPIRATION: i64 = 100;

pub const VIOLAS_ROOT_ACCOUNT_ID: u64 = u64::MAX;
pub const VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID: u64 = VIOLAS_ROOT_ACCOUNT_ID - 1;
pub const VIOLAS_TESTNET_DD_ACCOUNT_ID: u64 = VIOLAS_ROOT_ACCOUNT_ID - 2;

///
/// struct ViolasClient
///
pub struct ViolasClient {
    diem_client_proxy: ClientProxy,
    // Account used TreasuryCompliance operations (e.g., minting)
    //pub treasury_compliance_account: Option<AccountData>,
}

impl Deref for ViolasClient {
    type Target = ClientProxy;

    fn deref(&self) -> &ClientProxy {
        &self.diem_client_proxy
    }
}

impl DerefMut for ViolasClient {
    fn deref_mut(&mut self) -> &mut ClientProxy {
        &mut self.diem_client_proxy
    }
}

impl ViolasClient {
    /// Construct a new ViolasClient.
    pub fn new(
        chain_id: ChainId,
        url: &str,
        diem_root_account_file: &str,
        tc_account_file: &str,
        testnet_designated_dealer_account_file: &str,
        sync_on_wallet_recovery: bool,
        faucet_server: Option<String>,
        mnemonic_file: Option<String>,
        waypoint: Waypoint,
    ) -> Result<Self> {
        let diem_client_proxy = ClientProxy::new(
            chain_id,
            url,
            diem_root_account_file,
            tc_account_file,
            testnet_designated_dealer_account_file,
            sync_on_wallet_recovery,
            faucet_server,
            mnemonic_file,
            waypoint,
            true,
        )?;

        let client = ViolasClient { diem_client_proxy };

        Ok(client)
    }

    /// Test JSON RPC client connection with validator.
    pub fn test_connection(&mut self) -> Result<()> {
        self.test_validator_connection()?;
        Ok(())
    }

    /// Returns the account index that should be used by user to reference this account
    pub fn create_next_account(
        &mut self,
        address: Option<AccountAddress>,
        sync_with_validator: bool,
    ) -> Result<(AccountAddress, usize)> {
        let (auth_key, child_number) = self.diem_client_proxy.wallet.new_address()?;
        let private_key = self
            .diem_client_proxy
            .wallet
            .get_private_key_by_child_num(child_number)?;

        let account_data = Self::get_account_data_from_address(
            &mut self.client,
            match address {
                Some(addr) => addr,
                None => auth_key.derived_address(),
            },
            sync_with_validator,
            Some(KeyPair::from(private_key)),
            Some(auth_key.to_vec()),
        )?;

        let addr_index = self.insert_account_data(account_data);

        Ok((addr_index.address, addr_index.index))
    }

    pub fn association_transaction_with_local_diem_root_account(
        &mut self,
        payload: TransactionPayload,
        is_blocking: bool,
    ) -> Result<()> {
        ensure!(
            self.diem_root_account.is_some(),
            "No assoc root account loaded"
        );
        let sender = self.diem_root_account.as_ref().unwrap();
        let txn = self.create_txn_to_submit(payload, sender, None, None, None)?;

        self.diem_client_proxy.client.submit_transaction(&txn)?;
        if is_blocking {
            self.wait_for_transaction(&txn)?;
        }
        Ok(())
    }

    pub fn association_transaction_with_local_testnet_dd_account(
        &mut self,
        payload: TransactionPayload,
        is_blocking: bool,
    ) -> Result<()> {
        ensure!(
            self.testnet_designated_dealer_account.is_some(),
            "No testnet Designated Dealer account loaded"
        );
        let sender = self.testnet_designated_dealer_account.as_ref().unwrap();
        let txn = self.create_txn_to_submit(payload, sender, None, None, None)?;

        self.diem_client_proxy.client.submit_transaction(&txn)?;
        if is_blocking {
            self.wait_for_transaction(&txn)?;
        }
        Ok(())
    }

    pub fn association_transaction_with_local_tc_account(
        &mut self,
        payload: TransactionPayload,
        is_blocking: bool,
    ) -> Result<()> {
        ensure!(
            self.tc_account.is_some(),
            "No treasury compliance account loaded"
        );
        //  create txn to submit
        let sender = self.tc_account.as_ref().unwrap();
        let txn = self.create_txn_to_submit(payload, sender, None, None, None)?;

        // submit txn
        let resp = self.diem_client_proxy.client.submit_transaction(&txn)?;

        // wait for txn
        if is_blocking {
            self.wait_for_transaction(&txn)?;
        }
        Ok(resp)
    }

    /// Craft a transaction to be submitted.
    fn create_txn_to_submit(
        &self,
        program: TransactionPayload,
        sender_account: &AccountData,
        max_gas_amount: Option<u64>,
        gas_unit_price: Option<u64>,
        gas_currency_code: Option<String>,
    ) -> Result<SignedTransaction> {
        let signer: Box<&dyn TransactionSigner> = match &sender_account.key_pair {
            Some(key_pair) => Box::new(key_pair),
            None => Box::new(&self.wallet),
        };
        create_user_txn(
            *signer,
            program,
            sender_account.address,
            sender_account.sequence_number,
            max_gas_amount.unwrap_or(MAX_GAS_AMOUNT),
            gas_unit_price.unwrap_or(GAS_UNIT_PRICE),
            gas_currency_code.unwrap_or_else(|| "VLS".to_string()),
            TX_EXPIRATION,
            self.chain_id,
        )
    }

    /// Get account using specific address.
    /// Sync with validator for account sequence number in case it is already created on chain.
    /// This assumes we have a very low probability of mnemonic word conflict.
    fn get_account_data_from_address(
        client: &mut DiemClient,
        address: AccountAddress,
        sync_with_validator: bool,
        key_pair: Option<KeyPair<Ed25519PrivateKey, Ed25519PublicKey>>,
        authentication_key_opt: Option<Vec<u8>>,
    ) -> Result<AccountData> {
        let (sequence_number, authentication_key, status) = if sync_with_validator {
            let ret = client.get_account(&address);
            match ret {
                Ok(resp) => match resp {
                    Some(account_view) => (
                        account_view.sequence_number,
                        Some(hex::decode(account_view.authentication_key)?),
                        AccountStatus::Persisted,
                    ),
                    None => (0, authentication_key_opt, AccountStatus::Local),
                },
                Err(e) => {
                    bail!("Failed to get account from validator, error: {:?}", e);
                }
            }
        } else {
            (0, authentication_key_opt, AccountStatus::Local)
        };
        Ok(AccountData {
            address,
            authentication_key,
            key_pair,
            sequence_number,
            status,
        })
    }

    /// Waits for the next transaction for a specific address and prints it
    pub fn wait_for_transaction(&mut self, txn: &SignedTransaction) -> Result<()> {
        //let (tx, rx) = std::sync::mpsc::channel();
        let timeout_secs = time::Duration::from_secs(15);

        let ret = self.client.wait_for_transaction(txn, timeout_secs);
        let ac_update = self.get_account_and_update(&txn.sender());

        if let Err(err) = ac_update {
            bail!("account update failed: {}", err);
        }
        match ret {
            Ok(_) => Ok(()),
            Err(WaitForTransactionError::TransactionExecutionFailed(txn)) => bail!(format_err!(
                "transaction failed to execute; status: {:?}!",
                txn.vm_status
            )),
            Err(e) => Err(anyhow::Error::new(e)),
        }
    }

    /// submit a transaction with specified account index
    fn submit_transction_with_account(
        &mut self,
        account_ref_id: usize,
        program: Script,
        gas_unit_price: Option<u64>,
        max_gas_amount: Option<u64>,
        gas_currency_type: Option<TypeTag>,
        is_blocking: bool,
    ) -> Result<()> {
        let proxy: &mut ClientProxy = &mut self.diem_client_proxy;
        let sender = proxy
            .accounts
            .get(account_ref_id)
            .ok_or_else(|| format_err!("Unable to find sender account: {}", account_ref_id))?;

        let gas_currency_code = match gas_currency_type {
            Some(TypeTag::Struct(tag)) => Some(tag.module.into_string()),
            _ => None,
        };

        let txn = proxy.create_txn_to_submit(
            TransactionPayload::Script(program),
            sender,
            max_gas_amount, /* max_gas_amount */
            gas_unit_price, /* gas_unit_price */
            gas_currency_code,
        )?;

        proxy.client.submit_transaction(&txn)?;

        if is_blocking {
            self.wait_for_transaction(&txn)
        } else {
            Ok(())
        }

        // Ok(IndexAndSequence {
        //     account_index: AccountEntry::Index(account_ref_id),
        //     sequence_number: sender_sequence - 1,
        // })
    }

    /// Allow custom script
    pub fn allow_custom_script(&mut self, is_blocking: bool) -> Result<()> {
        // let script_bytes = fs::read(
        //     "/home/hunter/Projects/work/ViolasClientSdk/move/stdlib/allow_custom_script.mv",
        // )?;
        let script_bytes = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 4, 7, 11, 48, 8, 59, 16, 0, 0,
            0, 1, 0, 1, 0, 1, 6, 12, 0, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99, 116,
            105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105, 111,
            110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 115, 99, 114, 105, 112, 116, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 11, 0, 17, 0, 2,
        ];

        match self.diem_root_account {
            Some(_) => self.association_transaction_with_local_diem_root_account(
                TransactionPayload::Script(Script::new(script_bytes, vec![], vec![])),
                is_blocking,
            ),
            None => unimplemented!(),
        }
    }

    /// Allow publishing module
    pub fn allow_publishing_module(&mut self, open: bool, is_blocking: bool) -> Result<()> {
        // let script_bytes = fs::read(
        //     "/home/hunter/Projects/work/ViolasClientSdk/move/stdlib/allow_publishing_module.mv",
        // )?;
        let script_bytes = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 5, 7, 12, 48, 8, 60, 16, 0, 0,
            0, 1, 0, 1, 0, 2, 6, 12, 1, 0, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99,
            116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105,
            111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 109, 111, 100, 117, 108, 101,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 11, 0, 10, 1, 17, 0, 2,
        ];

        match self.diem_root_account {
            Some(_) => self.association_transaction_with_local_diem_root_account(
                TransactionPayload::Script(Script::new(
                    script_bytes,
                    vec![],
                    vec![TransactionArgument::Bool(open)],
                )),
                is_blocking,
            ),
            None => unimplemented!(),
        }
    }
    /// Publish Move module
    /// if sender ref id is u64::MAX, then publish module with association account
    ///
    pub fn publish_module(&mut self, sender_ref_id: usize, module_file_name: &str) -> Result<()> {
        let sender = if sender_ref_id as u64 == u64::MAX {
            if self.diem_root_account.is_none() {
                bail!("No faucet account loaded");
            }
            self.diem_root_account.as_ref().unwrap()
        } else {
            self.accounts.get(sender_ref_id as usize).unwrap()
        };

        let module_bytes = fs::read(module_file_name)?;
        let program = TransactionPayload::Module(Module::new(module_bytes));

        let txn = self.create_txn_to_submit(program, sender, None, None, None)?;
        let proxy = &mut self.diem_client_proxy;

        let resp = if sender_ref_id as u64 == u64::MAX {
            proxy.client.submit_transaction(&txn)
        } else {
            proxy.client.submit_transaction(&txn)
        };

        self.wait_for_transaction(&txn)?;
        resp
    }

    /// Execute custom script with association account
    /// if sender ref id is u64::MAX, then execute module with association account
    pub fn execute_script(
        &mut self,
        sender_ref_id: u64,
        script_bytecode: Vec<u8>,
        tags: Vec<TypeTag>,
        args: &[&str],
        is_blocking: bool,
    ) -> Result<()> {
        let arguments: Vec<_> = args[0..]
            .iter()
            .filter_map(|arg| parse_transaction_argument_for_client(arg).ok())
            .collect();

        self.execute_raw_script_bytecode(
            sender_ref_id,
            script_bytecode,
            tags,
            arguments,
            is_blocking,
        )
    }

    /// execute script with json format
    pub fn execute_raw_script_file(
        &mut self,
        sender_ref_id: u64,
        script_file_name: &str,
        tags: Vec<TypeTag>,
        args: Vec<TransactionArgument>,
        is_blocking: bool,
    ) -> Result<()> {
        let script_bytecode = fs::read(script_file_name)?;
        self.execute_raw_script_bytecode(sender_ref_id, script_bytecode, tags, args, is_blocking)
    }

    /// execute script with json format
    pub fn execute_raw_script_bytecode(
        &mut self,
        sender_ref_id: u64,
        script_bytecode: Vec<u8>,
        tags: Vec<TypeTag>,
        script_arguments: Vec<TransactionArgument>,
        is_blocking: bool,
    ) -> Result<()> {
        let script = Script::new(script_bytecode, tags, script_arguments);
        self.execute_raw_script(sender_ref_id, script, is_blocking)
    }

    /// execute script with json format
    pub fn execute_raw_script(
        &mut self,
        sender_ref_id: u64,
        script: Script,
        is_blocking: bool,
    ) -> Result<()> {
        let sender_opt = match sender_ref_id {
            VIOLAS_ROOT_ACCOUNT_ID => self.diem_root_account.as_ref(),
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID => self.tc_account.as_ref(),
            VIOLAS_TESTNET_DD_ACCOUNT_ID => self.testnet_designated_dealer_account.as_ref(),
            _ => self.accounts.get(sender_ref_id as usize),
        };
        let sender = match sender_opt {
            Some(sender) => sender,
            None => bail!("sener reference ID does not exists."),
        };

        let txn = self.create_txn_to_submit(
            TransactionPayload::Script(script),
            sender,
            None,
            None,
            None,
        )?;
        let proxy = &mut self.diem_client_proxy;

        proxy.client.submit_transaction(&txn)?;

        if is_blocking {
            self.wait_for_transaction(&txn)
        } else {
            Ok(())
        }
    }

    ///
    /// publish a new module with specified module name
    ///
    pub fn publish_currency(&mut self, currency_code: &str) -> Result<()> {
        let module_name = currency_code.as_bytes().to_owned();
        let module_size: u8 = module_name.len() as u8;

        let module_byte_code = if module_size == 3 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 16, 8, 22, 16, 10, 38, 5,
                0, 0, 0, 0, 2, 0, 3, 86, 76, 83, 11, 100, 117, 109, 109, 121, 95, 102, 105, 101,
                108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 3..]; //skip VLS from current to end
            [head, &module_name[..], tail].concat()
        } else if module_size == 4 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 17, 8, 23, 16, 10, 39, 5,
                0, 0, 0, 0, 2, 0, 4, 85, 83, 68, 84, 11, 100, 117, 109, 109, 121, 95, 102, 105,
                101, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 4..]; //skip USDT from current to end
            [head, &module_name[..], tail].concat()
        } else if module_size == 5 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 18, 8, 24, 16, 10, 40, 5,
                0, 0, 0, 0, 2, 0, 5, 76, 73, 66, 82, 65, 11, 100, 117, 109, 109, 121, 95, 102, 105,
                101, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 5..]; //skip diem from current to end
            [head, &module_name[..], tail].concat()
        } else if module_size == 6 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 19, 8, 25, 16, 10, 41, 5,
                0, 0, 0, 0, 2, 0, 6, 86, 76, 83, 85, 83, 68, 11, 100, 117, 109, 109, 121, 95, 102,
                105, 101, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1,
                0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 6..]; //skip VLSUSD from current to end
            [head, &module_name[..], tail].concat()
        } else {
            bail!("The length of module name must be between 3 and 6 character.");
        };

        // let module_byte_code = {
        //     let code = "
        //     module VLS {
        //         struct VLS { }
        //     }
        //     "
        //     .to_string();
        //     //.replace("VLS", currency_code);

        //     let compiler = Compiler {
        //         address: diem_types::account_config::CORE_CODE_ADDRESS,
        //         extra_deps: vec![],
        //         ..Compiler::default()
        //     };
        //     compiler.into_module_blob("file_name", code.as_str())?
        // };
        match self.diem_root_account {
            Some(_) => self.association_transaction_with_local_diem_root_account(
                TransactionPayload::Module(Module::new(module_byte_code)),
                true,
            ),
            None => unimplemented!(),
        }
    }
    ///
    /// add a new currency with association account
    ///
    pub fn register_currency(
        &mut self,
        type_tag: TypeTag,
        exchange_rate_denom: u64,
        exchange_rate_num: u64,
        is_synthetic: bool,
        scaling_factor: u64,
        fractional_part: u64,
        currency_code: Vec<u8>,
        is_blocking: bool,
    ) -> Result<()> {
        let script_bytecode = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 14, 7, 24, 46, 8,
            70, 16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 7, 6, 12, 3, 3, 1, 3, 3, 10, 2, 0, 1, 9, 0, 11,
            68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 33, 114, 101, 103, 105, 115, 116,
            101, 114, 95, 99, 117, 114, 114, 101, 110, 99, 121, 95, 119, 105, 116, 104, 95, 116,
            99, 95, 97, 99, 99, 111, 117, 110, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 0, 1, 9, 11, 0, 10, 1, 10, 2, 10, 3, 10, 4, 10, 5, 11, 6, 56, 0, 2,
        ];

        // costruct register currency script
        let script = Script::new(
            script_bytecode,
            vec![type_tag],
            vec![
                TransactionArgument::U64(exchange_rate_denom),
                TransactionArgument::U64(exchange_rate_num),
                TransactionArgument::Bool(is_synthetic),
                TransactionArgument::U64(scaling_factor),
                TransactionArgument::U64(fractional_part),
                TransactionArgument::U8Vector(currency_code),
            ],
        );

        match self.diem_root_account {
            Some(_) => self.association_transaction_with_local_diem_root_account(
                TransactionPayload::Script(script),
                is_blocking,
            ),
            None => unimplemented!(),
        }
    }

    /// add currency for the designated dealer account
    pub fn add_currency_for_designated_dealer(
        &mut self,
        currency: TypeTag,
        dd_address: AccountAddress,
        is_blocking: bool,
    ) -> Result<()> {
        // let script_bytecode = fs::read(
        //     "/home/hunter/Projects/work/ViolasClientSdk/cppSdk/move/currencies/add_currency_for_designated_dealer.mv",
        // )?;

        let script_bytecode = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 8, 7, 18, 47, 8, 65,
            16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 2, 6, 12, 5, 0, 1, 9, 0, 11, 68, 105, 101, 109, 65,
            99, 99, 111, 117, 110, 116, 34, 97, 100, 100, 95, 99, 117, 114, 114, 101, 110, 99, 121,
            95, 102, 111, 114, 95, 100, 101, 115, 105, 103, 110, 97, 116, 101, 100, 95, 100, 101,
            97, 108, 101, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 4, 11,
            0, 10, 1, 56, 0, 2,
        ];

        // costruct register_currency_for_designated_dealer script
        let script = Script::new(
            script_bytecode,
            vec![currency],
            vec![TransactionArgument::Address(dd_address)],
        );

        match &self.diem_root_account {
            Some(_) => {
                // add currency for testnet dd account
                self.association_transaction_with_local_tc_account(
                    TransactionPayload::Script(script),
                    is_blocking,
                )
            }
            None => unimplemented!(),
        }
    }

    pub fn add_currency(
        &mut self,
        account_ref_id: usize,
        type_tag: TypeTag,
        is_blocking: bool,
    ) -> Result<()> {
        if account_ref_id == usize::MAX {
            match &self.diem_root_account {
                Some(_) => {
                    let script =
                        transaction_builder::encode_add_currency_to_account_script(type_tag);
                    // add currency for testnet dd account
                    self.association_transaction_with_local_testnet_dd_account(
                        TransactionPayload::Script(script),
                        is_blocking,
                    )
                }
                None => unimplemented!(),
            }
        } else {
            self.submit_transction_with_account(
                account_ref_id,
                transaction_builder::encode_add_currency_to_account_script(type_tag),
                None,
                None,
                None,
                is_blocking,
            )
        }
    }

    /// mint currency with specified type tag
    pub fn mint_currency(
        &mut self,
        type_tag: TypeTag,
        sliding_nonce: u64,
        dd_address: AccountAddress,
        amount: u64,
        tier_index: u64,
        is_blocking: bool,
    ) -> Result<()> {
        let module = match type_tag.clone() {
            TypeTag::Struct(tag) => tag.module.into_string(),
            _ => String::from(""),
        };

        let script = if module == "LBR" {
            //transaction_builder::encode_mint_lbr_script(amount)
            bail!("LBR is not allowed");
        } else {
            transaction_builder::encode_tiered_mint_script(
                type_tag,
                sliding_nonce,
                dd_address,
                amount,
                tier_index,
            )
        };

        match &self.diem_root_account {
            Some(_faucet) => self.association_transaction_with_local_tc_account(
                TransactionPayload::Script(script),
                is_blocking,
            ),
            None => unimplemented!(),
        }
    }

    /// transfer currency
    pub fn transfer_currency(
        &mut self,
        sender_account_ref_id: usize,
        receiver_address: &AccountAddress,
        currency_tag: TypeTag,
        num_coins: u64,
        gas_unit_price: Option<u64>,
        max_gas_amount: Option<u64>,
        gas_currency_tag: Option<TypeTag>,
        is_blocking: bool,
    ) -> Result<()> {
        let program = transaction_builder::encode_peer_to_peer_with_metadata_script(
            currency_tag.clone(),
            *receiver_address,
            num_coins,
            vec![],
            vec![],
        );

        self.submit_transction_with_account(
            sender_account_ref_id,
            program,
            gas_unit_price,
            max_gas_amount,
            gas_currency_tag,
            is_blocking,
        )?;

        Ok(())
    }

    /// get balance of currency for account
    pub fn get_currency_balance(
        &mut self,
        currency_tag: TypeTag,
        address: AccountAddress,
    ) -> Result<u64> {
        if let (Some(blob), _) = self.client.get_account_state_blob(&address)? {
            let account_state = AccountState::try_from(&blob)?;
            let res_path = BalanceResource::access_path_for(currency_tag);

            match account_state.get(&res_path) {
                Some(bytes) => {
                    let bal_res: BalanceResource = bcs::from_bytes(bytes).unwrap();
                    Ok(bal_res.coin())
                }
                None => bail!("No data for {:?}", address),
            }
        } else {
            bail!("No data for {:?}", address);
        }
    }

    ///get currency info
    pub fn get_all_currency_info(&mut self) -> Result<Vec<CurrencyInfoViewEx>> {
        if let (Some(blob), _) = self.client.get_account_state_blob(&diem_root_address())? {
            let account_state = AccountState::try_from(&blob)?;
            Ok(account_state
                .get_registered_currency_info_resources()?
                .iter()
                .map(|info| CurrencyInfoViewEx::from(info))
                .collect())
        } else {
            bail!("get account state blob error for diem root address ");
        }
    }

    /// Preburn `amount` `Token(type_tag)`s from `account`.
    /// This will only succeed if `account` has already registerred preburner resource.
    pub fn preburn(
        &mut self,
        type_tag: TypeTag,
        amount: u64,
        account_ref_id: u64,
        is_blocking: bool,
    ) -> Result<()> {
        if account_ref_id == u64::MAX {
            match &self.diem_root_account {
                Some(_) => {
                    let script = transaction_builder::encode_preburn_script(type_tag, amount);
                    self.association_transaction_with_local_diem_root_account(
                        TransactionPayload::Script(script),
                        is_blocking,
                    )
                }
                None => unimplemented!(),
            }
        } else {
            self.submit_transction_with_account(
                account_ref_id as usize,
                transaction_builder::encode_preburn_script(type_tag.clone(), amount),
                None,
                None,
                Some(type_tag),
                is_blocking,
            )
        }
    }

    ///  Permanently destroy the `Token`s stored in the oldest burn request under the `Preburn` resource
    pub fn burn(
        &mut self,
        type_tag: TypeTag,
        account_ref_id: u64,
        sliding_nonce: u64,
        preburn_address: AccountAddress,
        is_blocking: bool,
    ) -> Result<()> {
        if account_ref_id == u64::MAX {
            match &self.diem_root_account {
                Some(_) => {
                    let script = transaction_builder::encode_burn_script(
                        type_tag,
                        sliding_nonce,
                        preburn_address,
                    );
                    self.association_transaction_with_local_diem_root_account(
                        TransactionPayload::Script(script),
                        is_blocking,
                    )
                }
                None => unimplemented!(),
            }
        } else {
            self.submit_transction_with_account(
                account_ref_id as usize,
                transaction_builder::encode_burn_script(
                    type_tag.clone(),
                    sliding_nonce,
                    preburn_address,
                ),
                None,
                None,
                Some(type_tag),
                is_blocking,
            )
        }
    }

    /// Create a testnet account
    pub fn create_validator_account(
        self: &mut Self,
        sliding_nonce: u64,
        new_account_address: AccountAddress,
        auth_key_prefix: Vec<u8>,
        human_name: Vec<u8>,
        is_blocking: bool,
    ) -> Result<()> {
        match &self.diem_root_account {
            Some(_) => {
                let script = transaction_builder::encode_create_validator_account_script(
                    sliding_nonce,
                    new_account_address,
                    auth_key_prefix,
                    human_name,
                );
                self.association_transaction_with_local_diem_root_account(
                    TransactionPayload::Script(script),
                    is_blocking,
                )
            }
            None => unimplemented!(),
        }
    }
    /// Create a parent vasp account
    pub fn create_parent_vasp_account(
        &mut self,
        type_tag: TypeTag,
        sliding_nonce: u64,
        new_account_address: AccountAddress,
        auth_key_prefix: Vec<u8>,
        human_name: Vec<u8>,
        _base_url: Vec<u8>,
        _compliance_public_key: Vec<u8>,
        add_all_currencies: bool,
        is_blocking: bool,
    ) -> Result<()> {
        match &self.tc_account {
            Some(_) => {
                let script = transaction_builder::encode_create_parent_vasp_account_script(
                    type_tag,
                    sliding_nonce,
                    new_account_address,
                    auth_key_prefix,
                    human_name,
                    //base_url,
                    //compliance_public_key,
                    add_all_currencies,
                );
                self.association_transaction_with_local_tc_account(
                    TransactionPayload::Script(script),
                    is_blocking,
                )
            }
            None => unimplemented!(),
        }
    }

    /// Create a child vasp account
    pub fn create_child_vasp_account(
        &mut self,
        type_tag: TypeTag,
        parent_account_index: u64,
        new_account_address: AccountAddress,
        auth_key_prefix: Vec<u8>,
        add_all_currencies: bool,
        initial_balance: u64,
        is_blocking: bool,
    ) -> Result<()> {
        // match &self.diem_root_account {
        //     Some(_) => {}
        //     None => unimplemented!(),
        // }

        let script = transaction_builder::encode_create_child_vasp_account_script(
            type_tag.clone(),
            new_account_address,
            auth_key_prefix,
            add_all_currencies,
            initial_balance,
        );

        self.submit_transction_with_account(
            parent_account_index as usize,
            script,
            None,
            None,
            Some(type_tag),
            is_blocking,
        )
    }

    /// Create a designed dealer vasp account
    pub fn create_designated_dealer_account(
        &mut self,
        type_tag: TypeTag,
        nonce: u64,
        new_account_address: AccountAddress,
        auth_key_prefix: Vec<u8>,
        human_name: Vec<u8>,
        _base_url: Vec<u8>,
        _compliance_public_key: Vec<u8>,
        add_all_currencies: bool,
        is_blocking: bool,
    ) -> Result<()> {
        match &self.tc_account {
            Some(_) => {
                let script = transaction_builder::encode_create_designated_dealer_script(
                    type_tag,
                    nonce,
                    new_account_address,
                    auth_key_prefix,
                    human_name,
                    //base_url,
                    //compliance_public_key,
                    add_all_currencies,
                );
                self.association_transaction_with_local_tc_account(
                    TransactionPayload::Script(script),
                    is_blocking,
                )
            }
            None => unimplemented!(),
        }
    }

    /// Create a designed dealer vasp account
    pub fn create_designated_dealer_ex(
        &mut self,
        type_tag: TypeTag,
        nonce: u64,
        new_account_address: AccountAddress,
        auth_key: AuthenticationKey,
        human_name: Vec<u8>,
        _base_url: Vec<u8>,
        _compliance_public_key: Vec<u8>,
        add_all_currencies: bool,
        is_blocking: bool,
    ) -> Result<()> {
        let scripte_bytecode = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 27, 7, 44, 75, 8,
            119, 16, 0, 0, 0, 1, 1, 2, 0, 1, 0, 0, 3, 2, 1, 1, 1, 1, 4, 2, 6, 12, 3, 0, 5, 6, 12,
            5, 10, 2, 10, 2, 1, 6, 6, 12, 3, 5, 10, 2, 10, 2, 1, 1, 9, 0, 11, 68, 105, 101, 109,
            65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99,
            101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97,
            98, 111, 114, 116, 27, 99, 114, 101, 97, 116, 101, 95, 100, 101, 115, 105, 103, 110,
            97, 116, 101, 100, 95, 100, 101, 97, 108, 101, 114, 95, 101, 120, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 1, 10, 10, 0, 10, 1, 17, 0, 11, 0, 10, 2, 11, 3,
            11, 4, 10, 5, 56, 0, 2,
        ];

        self.execute_raw_script_bytecode(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            scripte_bytecode,
            vec![type_tag],
            vec![
                TransactionArgument::U64(nonce),
                TransactionArgument::Address(new_account_address),
                TransactionArgument::U8Vector(auth_key.to_vec()),
                TransactionArgument::U8Vector(human_name),
                TransactionArgument::Bool(add_all_currencies),
            ],
            is_blocking,
        )
    }

    //// Get account resource
    pub fn get_account_resource<T: DeserializeOwned>(
        &mut self,
        address: &AccountAddress,
        tag_path: &StructTag,
    ) -> Result<Option<T>> {
        if let (Some(blob), _) = self.client.get_account_state_blob(&address)? {
            let account_state = AccountState::try_from(&blob)?;
            let access_path = AccessPath::resource_access_vec(tag_path.clone());

            let resource = account_state.get_resource_impl(&access_path.to_vec())?;
            Ok(resource)
        } else {
            Ok(None)
        }
    }

    /// excute testnet_mint script
    pub fn mint_for_testnet(
        &mut self,
        currency_tag: TypeTag,
        receiver: AccountAddress,
        amount: u64,
        is_blocking: bool,
    ) -> Result<()> {
        match &self.diem_root_account {
            Some(_) => {
                let script = transaction_builder::encode_peer_to_peer_with_metadata_script(
                    currency_tag,
                    receiver,
                    amount,
                    vec![],
                    vec![],
                );

                self.association_transaction_with_local_testnet_dd_account(
                    TransactionPayload::Script(script),
                    is_blocking,
                )
            }
            None => unimplemented!(),
        }
    }

    ///
    /// Update dual attestation limit
    ///
    pub fn update_dual_attestation_limit(
        &mut self,
        sliding_nonce: u64,
        new_micro_lbr_limit: u64,
        is_blocking: bool,
    ) -> Result<()> {
        let script = transaction_builder::encode_update_dual_attestation_limit_script(
            sliding_nonce,
            new_micro_lbr_limit,
        );

        match self.testnet_designated_dealer_account {
            Some(_) => self.association_transaction_with_local_tc_account(
                TransactionPayload::Script(script),
                is_blocking,
            ),
            None => unimplemented!(),
        }
    }

    ///
    ///  Recover wallet accounts from file
    ///
    pub fn recover_wallet_accounts(&mut self, file_path_str: &str) -> Result<()> {
        let file_path = Path::new(file_path_str);
        if file_path.exists() == true {
            self.wallet = WalletLibrary::recover(file_path)?;
            self.recover_accounts_in_wallet()?;
            Ok(())
        } else {
            bail!("file \"{}\" does not exist", file_path_str)
        }
    }

    ///
    /// Rotate authentication key wiht nonce
    ///
    pub fn rotate_authentication_key_with_nonce(
        &mut self,
        account_index: u64,
        sliding_nonce: u64,
        new_auth_key: AuthenticationKey,
        is_blocking: bool,
    ) -> Result<()> {
        let script = transaction_builder::encode_rotate_authentication_key_with_nonce_script(
            sliding_nonce,
            new_auth_key.to_vec(),
        );

        self.execute_raw_script(account_index, script, is_blocking)
    }
    ///
    /// Save private key
    ///
    pub fn save_private_key(&self, account_index: usize, path_file_str: &str) -> Result<()> {
        // let private_key: Ed25519PrivateKey = lcs::from_bytes(
        //     self.accounts[account_index]
        //         .key_pair
        //         .as_ref()
        //         .unwrap()
        //         .private_key
        //         .to_bytes()
        //         .as_ref(),
        // )?;

        //generate_key::save_key(private_key, Path::new(path_file_str));

        let output_file_path = Path::new(path_file_str);
        if output_file_path.exists() && !output_file_path.is_file() {
            bail!("Specified output file path is a directory");
        }

        let encoded = bcs::to_bytes(
            &self.accounts[account_index]
                .key_pair
                .as_ref()
                .unwrap()
                .private_key,
        )?;

        let mut file = File::create(output_file_path)?;
        file.write_all(&encoded)?;

        Ok(())
    }
    /// Query account info
    pub fn query_account_info(
        &mut self,
        address: AccountAddress,
    ) -> Result<Option<jsonrpc::Account>> {
        let account = self.client.get_account(&address)?;

        Ok(account)
    }

    /// query transaction by account address and sequence number
    pub fn query_transaction_info(
        &mut self,
        address: AccountAddress,
        sequence_number: u64,
        fetching_events: bool,
    ) -> Result<Option<jsonrpc::Transaction>> {
        self.client
            .get_txn_by_acc_seq(&address, sequence_number, fetching_events)
    }

    /// query transaction by account address and sequence number
    pub fn query_transaction_by_range(
        &mut self,
        start_version: u64,
        limit: u64,
        fetching_events: bool,
    ) -> Result<Vec<jsonrpc::Transaction>> {
        self.client
            .get_txn_by_range(start_version, limit, fetching_events)
    }

    /// Query event view
    pub fn query_events(
        &mut self,
        address: AccountAddress,
        event_type: bool,
        start_seq_number: u64,
        limit: u64,
    ) -> Result<(Vec<jsonrpc::Event>, jsonrpc::Account)> {
        let path = match event_type {
            true => ACCOUNT_SENT_EVENT_PATH.to_vec(),
            false => ACCOUNT_RECEIVED_EVENT_PATH.to_vec(),
        };

        let access_path = AccessPath::new(address, path);

        self.client
            .get_events_by_access_path(access_path, start_seq_number, limit)
    }

    /// Query events ex
    pub fn query_events_ex(
        &mut self,
        access_path: AccessPath,
        start_seq_number: u64,
        limit: u64,
    ) -> Result<(Vec<jsonrpc::Event>, jsonrpc::Account)> {
        self.client
            .get_events_by_access_path(access_path, start_seq_number, limit)
    }
}

fn parse_transaction_argument_for_client(s: &str) -> Result<TransactionArgument> {
    if is_address(s) {
        let account_address = address_from_strings(s)?;
        return Ok(TransactionArgument::Address(account_address));
    }
    parse_transaction_argument(s)
}

/// Check whether the input string is a valid diem address.
pub fn is_address(data: &str) -> bool {
    hex::decode(data).map_or(false, |vec| vec.len() == AccountAddress::LENGTH)
}

/// Check whether the input string is a valid diem authentication key.
pub fn is_authentication_key(data: &str) -> bool {
    hex::decode(data).map_or(false, |vec| vec.len() == AuthenticationKey::LENGTH)
}

fn address_from_strings(data: &str) -> Result<AccountAddress> {
    let account_vec: Vec<u8> = hex::decode(data.parse::<String>()?)?;
    ensure!(
        account_vec.len() == AccountAddress::LENGTH,
        "The address {:?} is of invalid length. Addresses must be 16-bytes long"
    );
    let account = AccountAddress::try_from(&account_vec[..]).map_err(|error| {
        format_err!(
            "The address {:?} is invalid, error: {:?}",
            &account_vec,
            error,
        )
    })?;
    Ok(account)
}
