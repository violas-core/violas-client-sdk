use anyhow::{bail, Result};
use chrono::{offset::TimeZone, DateTime, Local, NaiveDateTime};
use diem_types::{
    //access_path::AccessPath,
    account_address::AccountAddress,
    account_config::CORE_CODE_ADDRESS,
    event::EventHandle,
    transaction::{authenticator::AuthenticationKey, TransactionArgument},
};
use serde::{Deserialize, Serialize};
use violas_client::{
    views::EventDataView, //EventView
    violas_account::{make_currency_tag, make_struct_tag},
    violas_client::ViolasClient,
};

pub fn oracle_admin_address() -> AccountAddress {
    AccountAddress::from_hex_literal("0x4f524143") //0000000000000000000000004f524143
        .expect("Parsing valid hex literal should always succeed")
}

#[derive(Serialize, Debug, Deserialize)]
struct ExchangeRateReource {
    pub fixed_point32: u64,
    pub timestamp: u64, // micro second
    update_events: EventHandle,
}

#[derive(Serialize, Debug, Deserialize)]
struct UpdateEvent {
    pub value: u64,
    pub timestamp: u64, // micro second
    pub currency_code: Vec<u8>,
}

/// Oracle
pub struct Oracle {
    client: ViolasClient,
}

impl Oracle {
    pub fn new(client: ViolasClient) -> Result<Self> {
        Ok(Oracle { client })
    }
    /// publish Oracle module mv file
    pub fn publish(&mut self, oracle_module_file_name: &str) -> Result<()> {
        self.client.publish_module(
            violas_client::violas_client::VIOLAS_ROOT_ACCOUNT_ID as usize,
            oracle_module_file_name,
        )?;

        println!("succeded to publish Oracle moudle");

        self.client.create_next_account(None, false)?;
        let auth_key_data = self.client.accounts[0]
            .authentication_key
            .as_ref()
            .unwrap()
            .clone();
        let pub_key = &self.client.accounts[0]
            .key_pair
            .as_ref()
            .unwrap()
            .public_key
            .clone();

        let mut data: [u8; 32] = [0; 32];
        data.copy_from_slice(&auth_key_data);

        let auth_key = AuthenticationKey::new(data);

        self.client.create_designated_dealer_ex(
            make_currency_tag("VLS"),
            0,
            oracle_admin_address(),
            auth_key,
            "Oracle Administrator".as_bytes().to_vec(),
            "www.violas.io".as_bytes().to_owned(),
            pub_key.to_bytes().to_vec(),
            true,
            true,
        )?;

        println!(
            "succeded to create admin account with address {} and authentication key {:x?}",
            oracle_admin_address(),
            data
        );

        Ok(())
    }

    pub fn create_admin_account(&mut self) -> Result<()> {
        if self.client.accounts.len() == 0 {
            self.client
                .create_next_account(Some(oracle_admin_address()), true)?;
        }

        println!(
            "update Oracle exchange rate  with account address {} and authentication key {:x?}",
            oracle_admin_address(),
            self.client.accounts[0].authentication_key.as_ref().unwrap()
        );

        Ok(())
    }

    pub fn update_exchange_rate(
        &mut self,
        currency_code: &str,
        exchange_rate: f64,
        is_blocking: bool,
    ) -> Result<()> {
        // let script_bytecode = fs::read(
        //     "/home/hunter/Projects/work/ViolasClientSdk/move/oracle/update_exchange_rate.mv",
        // )?;

        // Oracle::update_exchange_rate
        let update_exchange_rate = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 9, 7, 19, 28, 8, 47,
            16, 0, 0, 0, 1, 0, 1, 1, 1, 0, 2, 3, 6, 12, 3, 3, 0, 1, 9, 0, 6, 79, 114, 97, 99, 108,
            101, 20, 117, 112, 100, 97, 116, 101, 95, 101, 120, 99, 104, 97, 110, 103, 101, 95,
            114, 97, 116, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 5, 11,
            0, 10, 1, 10, 2, 56, 0, 2,
        ];

        // ViolasBank::update_price_from_oracle
        let update_price_from_oracle = vec![
            161, 28, 235, 11, 1, 0, 0, 0, 7, 1, 0, 2, 3, 2, 16, 4, 18, 2, 5, 20, 14, 7, 34, 57, 8,
            91, 16, 6, 107, 5, 0, 0, 0, 1, 0, 1, 0, 0, 2, 2, 3, 0, 0, 3, 0, 3, 1, 1, 2, 4, 1, 6,
            12, 1, 1, 2, 6, 12, 10, 2, 0, 1, 9, 0, 10, 86, 105, 111, 108, 97, 115, 66, 97, 110,
            107, 12, 105, 115, 95, 112, 117, 98, 108, 105, 115, 104, 101, 100, 7, 112, 117, 98,
            108, 105, 115, 104, 24, 117, 112, 100, 97, 116, 101, 95, 112, 114, 105, 99, 101, 95,
            102, 114, 111, 109, 95, 111, 114, 97, 99, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, 10, 2, 2, 1, 0, 1, 1, 0, 3, 12, 10, 0, 17, 0, 9, 33, 3, 6, 5, 9, 10, 0, 7,
            0, 17, 1, 11, 0, 56, 0, 2,
        ];

        let numerator = (exchange_rate * 1E+9_f64) as u64;
        let denominator = 1E+9 as u64;

        self.client.execute_raw_script_bytecode(
            0,
            update_exchange_rate,
            vec![make_currency_tag(currency_code)],
            vec![
                TransactionArgument::U64(numerator),
                TransactionArgument::U64(denominator),
            ],
            is_blocking,
        )?;

        self.client.execute_raw_script_bytecode(
            0,
            update_price_from_oracle,
            vec![make_currency_tag(currency_code)],
            vec![],
            is_blocking,
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
        self.client.execute_raw_script_bytecode(
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
            false,
        )?;
        println!(
            "{}({}) -> {}({})",
            currency_code1, amount_crc1, currency_code2, amount_crc2
        );
        Ok(())
    }

    pub fn view_exchange_rate(&mut self, currency_code: &str) -> Result<Option<f64>> {
        let ex_rate: Option<ExchangeRateReource> = self.client.get_account_resource(
            &oracle_admin_address(),
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
    /// Print the last Oracle update event
    pub fn get_last_event(&mut self, currency_code: &str) -> Result<()> {
        let sender = oracle_admin_address();

        let resource_path = make_struct_tag(
            &CORE_CODE_ADDRESS,
            "Oracle",
            "ExchangeRate",
            vec![make_currency_tag(currency_code)],
        );

        let ret: Result<Option<ExchangeRateReource>> =
            self.client.get_account_resource(&sender, &resource_path);

        let updated_event = match ret {
            Ok(Some(exchange_rate)) => exchange_rate.update_events,
            Ok(None) => bail!("exchange rate resource is none"),
            Err(e) => bail!("{}", e),
        };

        let count = if updated_event.count() >= 3 { 3 } else { 1 };

        let events =
            self.client
                .query_events(updated_event.key(), updated_event.count() - count, count)?;

        for event in events.iter() {
            let update_event: UpdateEvent = match &event.data {
                EventDataView::Unknown { raw } => {
                    let data = raw.clone().into_bytes()?;
                    bcs::from_bytes(&data)?
                }
                _ => bail!("EventDataView type was error"),
            };

            let dt = NaiveDateTime::from_timestamp((update_event.timestamp / 1_000_000) as i64, 0);
            let date_time: DateTime<Local> = Local.from_local_datetime(&dt).unwrap();

            println!(
                "{} - {} - {} ",
                String::from_utf8(update_event.currency_code)?,
                update_event.value as f64 / (0x100000000_u64 as f64),
                date_time, //dt.format("%Y-%m-%d %H:%M:%S").to_string()
            );
        }

        Ok(())
    }
}
