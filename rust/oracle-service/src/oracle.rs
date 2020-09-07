use anyhow::Result;
use client_proxy::{
    violas_account::{make_currency_tag, make_struct_tag},
    violas_client::{self, ViolasClient},
};
use libra_types::{
    account_address::AccountAddress,
    account_config::{libra_root_address, CORE_CODE_ADDRESS},
    event::EventHandle,
    transaction::{authenticator::AuthenticationKey, TransactionArgument},
};
use serde::{Deserialize, Serialize};

pub fn oracle_admin_address() -> AccountAddress {
    AccountAddress::from_hex_literal("0x4f524143")
        .expect("Parsing valid hex literal should always succeed")
}

#[derive(Serialize, Debug, Deserialize)]
struct ExchangeRateReource {
    pub fixed_point32: u64,
    pub timestamp: u64,
    update_events: EventHandle,
}

/// Oracle
pub struct Oracle {
    violas_client: ViolasClient,
}

impl Oracle {
    pub fn new(client: ViolasClient) -> Result<Self> {
        Ok(Oracle {
            violas_client: client,
        })
    }
    /// publish Oracle module mv file
    pub fn publish(&mut self, oracle_module_file_name: &str) -> Result<()> {
        self.violas_client.publish_module(
            violas_client::VIOLAS_ROOT_ACCOUNT_ID as usize,
            oracle_module_file_name,
        )?;

        self.violas_client.create_next_account(None, false)?;
        let auth_key = self.violas_client.accounts[0]
            .authentication_key
            .as_ref()
            .unwrap()
            .clone();
        let pub_key = &self.violas_client.accounts[0]
            .key_pair
            .as_ref()
            .unwrap()
            .public_key.clone();

        self.violas_client.create_designated_dealer_account(
            make_currency_tag("LBR"),
            0,
            oracle_admin_address(),
            (&auth_key[0..16]).to_vec(), //only auth key prefix is applied
            "Oracle Administrator".as_bytes().to_vec(),
            "www.violas.io".as_bytes().to_owned(),
            pub_key.to_bytes().to_vec(),
            true,
            true,
        )?;

        let mut data: [u8; 32] = [0; 32];
        data.copy_from_slice(&auth_key);

        self.violas_client
            .update_account_authentication_key(oracle_admin_address(), AuthenticationKey::new(data))
    }

    pub fn update_exchange_rate(&mut self, currency_code: &str, exchange_rate: f64) -> Result<()> {
        // let script_bytecode = fs::read(
        //     "/home/hunter/Projects/work/ViolasClientSdk/move/oracle/update_exchange_rate.mv",
        // )?;
        let script_bytecode = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 9, 7, 19, 28, 8, 47,
            16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 3, 6, 12, 3, 3, 0, 1, 9, 0, 6, 79, 114, 97, 99, 108,
            101, 20, 117, 112, 100, 97, 116, 101, 95, 101, 120, 99, 104, 97, 110, 103, 101, 95,
            114, 97, 116, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 5, 11,
            0, 10, 1, 10, 2, 56, 0, 2,
        ];
        let numerator = (exchange_rate * 1E+9_f64) as u64;
        let denominator = 1E+9 as u64;

        self.violas_client.execute_script_raw(
            violas_client::VIOLAS_ROOT_ACCOUNT_ID,
            script_bytecode,
            vec![make_currency_tag(currency_code)],
            vec![
                TransactionArgument::U64(numerator),
                TransactionArgument::U64(denominator),
            ],
        )?;

        Ok(())
    }

    ///
    pub fn run_test_case(
        &mut self,
        currency_code1: &str,
        amount_crc1: u64,
        currency_code2: &str,
        //amount_crc2: u64,
    ) -> Result<()> {
        let rate1 = self.view_exchange_rate(currency_code1)?.unwrap();
        let rate2 = self.view_exchange_rate(currency_code2)?.unwrap();
        let amount_crc2 = (amount_crc1 as f64 * rate1 / rate2) as u64;
        println!(
            "The exchanged amount of {} is {}",
            currency_code2, amount_crc2
        );

        let script_bytecode = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 7, 1, 0, 4, 2, 4, 4, 3, 8, 16, 4, 24, 4, 5, 28, 29, 7,
            57, 62, 8, 119, 16, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1, 0, 0, 3, 0, 1, 0, 1, 4, 2, 3,
            1, 1, 2, 6, 2, 7, 2, 3, 8, 0, 1, 3, 0, 2, 8, 0, 3, 2, 3, 3, 6, 3, 3, 8, 0, 8, 0, 1, 3,
            1, 9, 0, 1, 9, 1, 12, 70, 105, 120, 101, 100, 80, 111, 105, 110, 116, 51, 50, 6, 79,
            114, 97, 99, 108, 101, 10, 100, 105, 118, 105, 100, 101, 95, 117, 54, 52, 12, 109, 117,
            108, 116, 105, 112, 108, 121, 95, 117, 54, 52, 17, 103, 101, 116, 95, 101, 120, 99,
            104, 97, 110, 103, 101, 95, 114, 97, 116, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 2, 1, 1, 4, 5, 23, 56, 0, 1, 12, 4, 10, 0, 11, 4, 17, 1, 12, 2, 56, 1, 1, 12,
            5, 10, 2, 11, 5, 17, 0, 12, 3, 10, 1, 10, 3, 33, 12, 6, 11, 6, 3, 22, 6, 234, 3, 0, 0,
            0, 0, 0, 0, 39, 2,
        ];
        self.violas_client.execute_script_raw(
            u64::MAX,
            script_bytecode,
            vec![
                make_currency_tag(currency_code1),
                make_currency_tag(currency_code2),
            ],
            vec![
                TransactionArgument::U64(amount_crc1),
                TransactionArgument::U64(amount_crc2),
            ],
        )?;
        println!(
            "{}({}) -> {}({})",
            currency_code1, amount_crc1, currency_code2, amount_crc2
        );
        Ok(())
    }

    pub fn view_exchange_rate(&mut self, currency_code: &str) -> Result<Option<f64>> {
        let ex_rate: Option<ExchangeRateReource> = self.violas_client.get_account_resource(
            &libra_root_address(),
            &make_struct_tag(
                &CORE_CODE_ADDRESS,
                "Oracle",
                "ExchangeRate",
                vec![make_currency_tag(currency_code)],
            ),
        )?;
        let rate: Option<f64> = match ex_rate {
            Some(rate) => Some((rate.fixed_point32 as f64) / (0x100000000_u64 as f64)),
            None => None,
        };
        println!("{} : {}", currency_code, rate.unwrap());
        Ok(rate)
    }
}
