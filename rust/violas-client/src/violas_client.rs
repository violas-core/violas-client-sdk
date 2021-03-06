use crate::{
    diem_client::DiemClient,
    diem_client_proxy::ClientProxy,
    violas_resource::{
        make_struct_tag, AccountOperationsCapability, CurrencyEventType, CurrencyInfoViewEx,
        ViolasStatus,
    },
    AccountData, //AccountStatus
    AccountStatus,
};
use anyhow::{bail, ensure, format_err, Result}; //ensure, Error
use diem_client::{views, WaitForTransactionError};
use diem_crypto::{
    ed25519::{Ed25519PrivateKey, Ed25519PublicKey}, //Ed25519Signature
    test_utils::KeyPair,
};

use diem_json_rpc_client::views::EventView;
use diem_transaction_builder::stdlib as transaction_builder;
use diem_types::{
    access_path::AccessPath,
    account_address::AccountAddress,
    account_config::{
        constants::allowed_currency_code_string, diem_root_address, BalanceResource,
        CORE_CODE_ADDRESS,
    },
    account_state::AccountState,
    chain_id::ChainId,
    event::EventKey,
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

//const CLIENT_WALLET_MNEMONIC_FILE: &str = "client.mnemonic";
const GAS_UNIT_PRICE: u64 = 0;
const MAX_GAS_AMOUNT: u64 = 1_000_000;
const TX_EXPIRATION: i64 = 100;

pub const VIOLAS_ROOT_ACCOUNT_ID: usize = usize::MAX;
pub const VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID: usize = VIOLAS_ROOT_ACCOUNT_ID - 1;
pub const VIOLAS_TESTNET_DD_ACCOUNT_ID: usize = VIOLAS_ROOT_ACCOUNT_ID - 2;

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
                        Some(account_view.authentication_key.into_inner().into()),
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

    /// Allow custom script
    pub fn allow_custom_script(&mut self, is_blocking: bool) -> Result<()> {
        // let script_bytes = fs::read(
        //     "move/stdlib/allow_custom_script.mv",
        // )?;
        let script_bytes = vec![
            161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 6, 7, 13, 48, 8, 61, 16, 0, 0,
            0, 1, 2, 1, 0, 1, 12, 0, 1, 6, 12, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97,
            99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116,
            105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 115, 99, 114, 105, 112,
            116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 14, 0, 17, 0, 2,
        ];

        self.execute_raw_script(
            VIOLAS_ROOT_ACCOUNT_ID,
            Script::new(script_bytes, vec![], vec![]),
            None,
            None,
            None,
            is_blocking,
        )
    }

    /// Allow publishing module
    pub fn allow_publishing_module(&mut self, open: bool, is_blocking: bool) -> Result<()> {
        // let script_bytes = fs::read(
        //     "move/stdlib/allow_publishing_module.mv",
        // )?;
        let script_bytes = vec![
            161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 8, 7, 15, 48, 8, 63, 16, 0, 0,
            0, 1, 2, 1, 0, 2, 12, 1, 0, 2, 6, 12, 1, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115,
            97, 99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112,
            116, 105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 109, 111, 100, 117,
            108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 14, 0, 10, 1, 17,
            0, 2,
        ];

        self.execute_raw_script(
            VIOLAS_ROOT_ACCOUNT_ID,
            Script::new(script_bytes, vec![], vec![TransactionArgument::Bool(open)]),
            None,
            None,
            None,
            is_blocking,
        )
    }
    ///
    /// Publish Move module    
    ///
    pub fn publish_module(&mut self, sender_ref_id: usize, module_file_name: &str) -> Result<()> {
        let module_bytes = fs::read(module_file_name)?;
        let program = TransactionPayload::Module(Module::new(module_bytes));

        // let txn = self.create_txn_to_submit(program, sender, None, None, None)?;
        // let proxy = &mut self.diem_client_proxy;

        // let resp = if sender_ref_id as u64 == u64::MAX {
        //     proxy.client.submit_transaction(&txn)
        // } else {
        //     proxy.client.submit_transaction(&txn)
        // };

        // self.wait_for_transaction(&txn)?;
        // resp
        self.execute_transaction(sender_ref_id, program, None, None, None, true)
    }

    /// Execute custom script with association account
    /// if sender ref id is u64::MAX, then execute module with association account
    pub fn execute_script(
        &mut self,
        sender_ref_id: usize,
        script_bytecode: Vec<u8>,
        tags: Vec<TypeTag>,
        args: &[&str],
        max_gas_amount: Option<u64>,
        gas_unit_price: Option<u64>,
        gas_currency_code: Option<String>,
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
            max_gas_amount,
            gas_unit_price,
            gas_currency_code,
            is_blocking,
        )
    }

    /// execute script with json format
    pub fn execute_raw_script_file(
        &mut self,
        sender_ref_id: usize,
        script_file_name: &str,
        tags: Vec<TypeTag>,
        args: Vec<TransactionArgument>,
        max_gas_amount: Option<u64>,
        gas_unit_price: Option<u64>,
        gas_currency_code: Option<String>,
        is_blocking: bool,
    ) -> Result<()> {
        let script_bytecode = fs::read(script_file_name)?;
        self.execute_raw_script_bytecode(
            sender_ref_id,
            script_bytecode,
            tags,
            args,
            max_gas_amount,
            gas_unit_price,
            gas_currency_code,
            is_blocking,
        )
    }

    /// execute script with json format
    pub fn execute_raw_script_bytecode(
        &mut self,
        sender_ref_id: usize,
        script_bytecode: Vec<u8>,
        tags: Vec<TypeTag>,
        script_arguments: Vec<TransactionArgument>,
        max_gas_amount: Option<u64>,
        gas_unit_price: Option<u64>,
        gas_currency_code: Option<String>,
        is_blocking: bool,
    ) -> Result<()> {
        let script = Script::new(script_bytecode, tags, script_arguments);
        self.execute_raw_script(
            sender_ref_id,
            script,
            max_gas_amount,
            gas_unit_price,
            gas_currency_code,
            is_blocking,
        )
    }

    /// execute script with json format
    pub fn execute_raw_script(
        &mut self,
        sender_ref_id: usize,
        script: Script,
        max_gas_amount: Option<u64>,
        gas_unit_price: Option<u64>,
        gas_currency_code: Option<String>,
        is_blocking: bool,
    ) -> Result<()> {
        self.execute_transaction(
            sender_ref_id,
            TransactionPayload::Script(script),
            max_gas_amount,
            gas_unit_price,
            gas_currency_code,
            is_blocking,
        )
    }

    pub fn execute_transaction(
        &mut self,
        sender_ref_id: usize,
        program: TransactionPayload,
        max_gas_amount: Option<u64>,
        gas_unit_price: Option<u64>,
        gas_currency_code: Option<String>,
        is_blocking: bool,
    ) -> Result<()> {
        let (sender_opt, id_name) = match sender_ref_id {
            VIOLAS_ROOT_ACCOUNT_ID => (
                self.diem_root_account.as_ref(),
                "VIOLAS_ROOT_ACCOUNT_ID".to_string(),
            ),
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID => (
                self.tc_account.as_ref(),
                "VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID".to_string(),
            ),
            VIOLAS_TESTNET_DD_ACCOUNT_ID => (
                self.testnet_designated_dealer_account.as_ref(),
                "VIOLAS_TESTNET_DD_ACCOUNT_ID".to_string(),
            ),
            _ => (self.accounts.get(sender_ref_id), sender_ref_id.to_string()),
        };

        let sender = match sender_opt {
            Some(sender) => sender,
            None => bail!("Sender's reference ID ({}) does not exist, you need load mint.key or add account by 'wallet-add-account'", id_name),
        };

        let txn = self.create_txn_to_submit(
            program,
            sender,
            max_gas_amount,
            gas_unit_price,
            gas_currency_code,
        )?;
        let proxy = &mut self.diem_client_proxy;

        proxy.client.submit_transaction(&txn)?;

        if is_blocking {
            self.wait_for_transaction(&txn)
        } else {
            let seq = txn
                .sequence_number()
                .checked_add(1)
                .ok_or_else(|| format_err!("seqnum can't reach u64::max"))?;
            proxy.update_account_seq(&txn.sender(), seq);

            Ok(())
        }
    }

    ///
    /// publish a new module with specified module name
    ///
    pub fn publish_currency(&mut self, currency_code: &str) -> Result<()> {
        if !allowed_currency_code_string(currency_code) {
            bail!("Currency code is not allowed.");
        }

        let module_name = currency_code.as_bytes().to_owned();
        let module_size: u8 = module_name.len() as u8;

        let module_byte_code = if module_size == 3 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 16, 8, 22, 16, 10, 38, 5,
                0, 0, 0, 0, 4, 0, 3, 85, 83, 68, 11, 100, 117, 109, 109, 121, 95, 102, 105, 101,
                108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 3..]; //skip VLS from current to end
            [head, &module_name[..], tail].concat()
        } else if module_size == 4 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 17, 8, 23, 16, 10, 39, 5,
                0, 0, 0, 0, 4, 0, 4, 85, 83, 68, 84, 11, 100, 117, 109, 109, 121, 95, 102, 105,
                101, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 4..]; //skip USDT from current to end
            [head, &module_name[..], tail].concat()
        } else if module_size == 5 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 18, 8, 24, 16, 10, 40, 5,
                0, 0, 0, 0, 4, 0, 5, 85, 83, 68, 69, 85, 11, 100, 117, 109, 109, 121, 95, 102, 105,
                101, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 5..]; //skip diem from current to end
            [head, &module_name[..], tail].concat()
        } else if module_size == 6 {
            let mut module_byte_code = vec![
                161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 19, 8, 25, 16, 10, 41, 5,
                0, 0, 0, 0, 4, 0, 6, 85, 83, 68, 69, 85, 82, 11, 100, 117, 109, 109, 121, 95, 102,
                105, 101, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1,
                0,
            ];

            let position = 0x1F; //module_byte_code.
            module_byte_code[position - 1] = module_size;
            let head = &module_byte_code[..position]; //from begin to index
            let tail = &module_byte_code[position + 6..]; //skip VLSUSD from current to end
            [head, &module_name[..], tail].concat()
        } else {
            bail!("The length of currency code must be between 3 and 6 character.");
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

        self.execute_transaction(
            VIOLAS_ROOT_ACCOUNT_ID,
            TransactionPayload::Module(Module::new(module_byte_code)),
            None,
            None,
            None,
            true,
        )
    }
    ///
    /// Register a new currency by Root account
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
            161, 28, 235, 11, 2, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 23, 7, 33, 46, 8,
            79, 16, 0, 0, 0, 1, 3, 1, 1, 4, 0, 2, 7, 12, 3, 3, 1, 3, 3, 10, 2, 0, 1, 9, 0, 7, 6,
            12, 3, 3, 1, 3, 3, 10, 2, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 33,
            114, 101, 103, 105, 115, 116, 101, 114, 95, 99, 117, 114, 114, 101, 110, 99, 121, 95,
            98, 121, 95, 114, 111, 111, 116, 95, 97, 99, 99, 111, 117, 110, 116, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 0, 1, 9, 14, 0, 10, 1, 10, 2, 10, 3, 10, 4, 10, 5,
            11, 6, 56, 0, 2,
        ];

        // construct register currency script
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

        self.execute_raw_script(
            VIOLAS_ROOT_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
    }

    /// add currency for the designated dealer account
    pub fn add_currency_for_designated_dealer(
        &mut self,
        currency: TypeTag,
        dd_address: AccountAddress,
        is_blocking: bool,
    ) -> Result<()> {
        // let script_bytecode = fs::read(
        //     "move/currencies/add_currency_for_designated_dealer.mv",
        // )?;

        let script_bytecode = vec![
            161, 28, 235, 11, 3, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 11, 7, 21, 47, 8,
            68, 16, 0, 0, 0, 1, 3, 1, 1, 4, 0, 2, 2, 12, 5, 0, 1, 9, 0, 2, 6, 12, 5, 11, 68, 105,
            101, 109, 65, 99, 99, 111, 117, 110, 116, 34, 97, 100, 100, 95, 99, 117, 114, 114, 101,
            110, 99, 121, 95, 102, 111, 114, 95, 100, 101, 115, 105, 103, 110, 97, 116, 101, 100,
            95, 100, 101, 97, 108, 101, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4,
            0, 1, 4, 14, 0, 10, 1, 56, 0, 2,
        ];
        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            Script::new(
                script_bytecode,
                vec![currency],
                vec![TransactionArgument::Address(dd_address)],
            ),
            None,
            None,
            None,
            is_blocking,
        )
    }

    pub fn add_currency(
        &mut self,
        account_ref_id: usize,
        type_tag: TypeTag,
        is_blocking: bool,
    ) -> Result<()> {
        self.execute_raw_script(
            account_ref_id,
            transaction_builder::encode_add_currency_to_account_script(type_tag),
            None,
            None,
            None,
            is_blocking,
        )
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
        let script = transaction_builder::encode_tiered_mint_script(
            type_tag,
            sliding_nonce,
            dd_address,
            amount,
            tier_index,
        );

        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
    }

    /// transfer currency
    pub fn transfer(
        &mut self,
        sender_account_ref_id: usize,
        receiver_address: &AccountAddress,
        currency_tag: TypeTag,
        num_coins: u64,
        gas_unit_price: Option<u64>,
        max_gas_amount: Option<u64>,
        gas_currency_tag: Option<String>,
        is_blocking: bool,
    ) -> Result<()> {
        let script = transaction_builder::encode_peer_to_peer_with_metadata_script(
            currency_tag.clone(),
            *receiver_address,
            num_coins,
            vec![],
            vec![],
        );

        self.execute_raw_script(
            sender_account_ref_id,
            script,
            max_gas_amount,
            gas_unit_price,
            gas_currency_tag,
            is_blocking,
        )
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
        sender_ref_id: usize,
        amount: u64,
        is_blocking: bool,
    ) -> Result<()> {
        self.execute_raw_script(
            sender_ref_id,
            transaction_builder::encode_preburn_script(type_tag, amount),
            None,
            None,
            None,
            is_blocking,
        )
    }

    ///  Permanently destroy the `Token`s stored in the oldest burn request under the `Preburn` resource
    pub fn burn(
        &mut self,
        type_tag: TypeTag,
        sliding_nonce: u64,
        preburn_address: AccountAddress,
        is_blocking: bool,
    ) -> Result<()> {
        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            transaction_builder::encode_burn_script(type_tag, sliding_nonce, preburn_address),
            None,
            None,
            None,
            is_blocking,
        )
    }

    pub fn canncle_burn(
        &mut self,
        type_tag: TypeTag,
        preburn_address: AccountAddress,
        is_blocking: bool,
    ) -> Result<()> {
        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            transaction_builder::encode_cancel_burn_script(type_tag, preburn_address),
            None,
            None,
            None,
            is_blocking,
        )
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
        let script = transaction_builder::encode_create_validator_account_script(
            sliding_nonce,
            new_account_address,
            auth_key_prefix,
            human_name,
        );
        self.execute_raw_script(
            VIOLAS_ROOT_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
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

        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
    }

    /// Create a child vasp account
    pub fn create_child_vasp_account(
        &mut self,
        type_tag: TypeTag,
        parent_account_index: usize,
        new_account_address: AccountAddress,
        auth_key_prefix: Vec<u8>,
        add_all_currencies: bool,
        initial_balance: u64,
        is_blocking: bool,
    ) -> Result<()> {
        let script = transaction_builder::encode_create_child_vasp_account_script(
            type_tag.clone(),
            new_account_address,
            auth_key_prefix,
            add_all_currencies,
            initial_balance,
        );

        self.execute_raw_script(parent_account_index, script, None, None, None, is_blocking)
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

        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
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
            161, 28, 235, 11, 3, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 26, 7, 43, 75, 8,
            118, 16, 0, 0, 0, 1, 1, 2, 2, 1, 0, 0, 3, 4, 1, 1, 0, 1, 3, 6, 12, 3, 5, 10, 2, 10, 2,
            1, 0, 2, 6, 12, 3, 1, 9, 0, 5, 6, 12, 5, 10, 2, 10, 2, 1, 11, 68, 105, 101, 109, 65,
            99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99,
            101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97,
            98, 111, 114, 116, 27, 99, 114, 101, 97, 116, 101, 95, 100, 101, 115, 105, 103, 110,
            97, 116, 101, 100, 95, 100, 101, 97, 108, 101, 114, 95, 101, 120, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 10, 14, 0, 10, 1, 17, 0, 14, 0, 10, 2, 11, 3,
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
            None,
            None,
            None,
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
        let script = transaction_builder::encode_peer_to_peer_with_metadata_script(
            currency_tag,
            receiver,
            amount,
            vec![],
            vec![],
        );

        self.execute_raw_script(
            VIOLAS_TESTNET_DD_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
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

        self.execute_raw_script(
            VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID,
            script,
            None,
            None,
            None,
            is_blocking,
        )
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
        account_index: usize,
        sliding_nonce: u64,
        new_auth_key: AuthenticationKey,
        is_blocking: bool,
    ) -> Result<()> {
        let script = transaction_builder::encode_rotate_authentication_key_with_nonce_script(
            sliding_nonce,
            new_auth_key.to_vec(),
        );

        self.execute_raw_script(account_index, script, None, None, None, is_blocking)
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
    ) -> Result<Option<views::AccountView>> {
        self.get_account_and_update(&address)
    }

    /// query transaction by account address and sequence number
    pub fn query_transaction_info(
        &mut self,
        address: AccountAddress,
        sequence_number: u64,
        fetching_events: bool,
    ) -> Result<Option<views::TransactionView>> {
        self.client
            .get_txn_by_acc_seq(&address, sequence_number, fetching_events)
    }

    /// query transaction by account address and sequence number
    pub fn query_transaction_by_range(
        &mut self,
        start_version: u64,
        limit: u64,
        fetching_events: bool,
    ) -> Result<Vec<views::TransactionView>> {
        self.client
            .get_txn_by_range(start_version, limit, fetching_events)
    }
    ///
    /// Query events with event key
    ///
    pub fn query_events(
        &mut self,
        event_key: &EventKey,
        start_seq_number: u64,
        limit: u64,
    ) -> Result<Vec<EventView>> {
        self.client.get_events(*event_key, start_seq_number, limit)
    }
    ///
    /// Query event view
    ///
    pub fn query_payment_events(
        &mut self,
        address: AccountAddress,
        event_type: bool,
        start_seq_number: u64,
        limit: u64,
    ) -> Result<(Vec<views::EventView>, views::AccountView)> {
        let account_view = match self.client.get_account(&address)? {
            None => bail!("No account found for address {:?}", address),
            Some(account) => account,
        };

        let event_key = match event_type {
            true => account_view.sent_events_key,
            false => account_view.received_events_key,
        };

        Ok((
            self.client.get_events(event_key, start_seq_number, limit)?,
            account_view,
        ))
    }

    pub fn query_currency_events(
        &mut self,
        currency_code: &str,
        event_type: CurrencyEventType,
        start_sn: u64,
        limit: u64,
    ) -> Result<Vec<EventView>> {
        let event_key = if let Some(crc_info) = self
            .client
            .get_currency_info()?
            .iter()
            .find(|currency| currency.code == currency_code)
        {
            match event_type {
                CurrencyEventType::Minted => crc_info.mint_events_key.clone(),
                CurrencyEventType::Burned => crc_info.burn_events_key.clone(),
                CurrencyEventType::Preburned => crc_info.preburn_events_key.clone(),
                CurrencyEventType::CancelledBurn => crc_info.cancel_burn_events_key.clone(),
                CurrencyEventType::UpdatedExchangeRate => {
                    crc_info.exchange_rate_update_events_key.clone()
                } //_ => bail!("event type is not matched."),
            }
        } else {
            bail!("didn't find currency code ");
        };

        self.client.get_events(event_key, start_sn, limit)
    }

    pub fn query_account_creation_events(
        &mut self,
        start_sn: u64,
        limit: u64,
    ) -> Result<Vec<EventView>> {
        let result: Option<AccountOperationsCapability> = self.get_account_resource(
            &diem_root_address(),
            &make_struct_tag(
                &CORE_CODE_ADDRESS,
                "DiemAccount",
                "AccountOperationsCapability",
                vec![],
            )?,
        )?;

        if let Some(ao_cap) = result {
            self.query_events(ao_cap.creation_events.key(), start_sn, limit)
        } else {
            bail!("failed to get account resource from root address.");
        }
    }

    pub fn query_vioas_status(&mut self) -> Result<ViolasStatus> {
        self.client.update_and_verify_state_proof()?;

        let result: Option<AccountOperationsCapability> = self.get_account_resource(
            &diem_root_address(),
            &make_struct_tag(
                &CORE_CODE_ADDRESS,
                "DiemAccount",
                "AccountOperationsCapability",
                vec![],
            )?,
        )?;

        let status = ViolasStatus {
            latest_version: self.client.trusted_state().version(),
            account_amount: match result {
                Some(ao_cap) => ao_cap.creation_events.count(),
                None => 0,
            },
        };

        Ok(status)
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
    // ensure!(
    //     account_vec.len() == AccountAddress::LENGTH,
    //     "The address {:?} is of invalid length. Addresses must be 16-bytes long"
    // );
    let account = AccountAddress::try_from(&account_vec[..]).map_err(|error| {
        format_err!(
            "The address {:?} is invalid, error: {:?}",
            &account_vec,
            error,
        )
    })?;
    Ok(account)
}
