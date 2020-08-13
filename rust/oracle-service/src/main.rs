use anyhow::Result; //bail, ensure, format_err,
use bytes::buf::BufExt as _;
use client_proxy::{
    violas_account::{make_currency_tag, make_struct_tag},
    violas_client::ViolasClient,
};
use hyper::{Client, Uri};
use hyper_tls::HttpsConnector;
//use libra_json_rpc::views::BytesView;
use libra_types::{
    account_config::{libra_root_address, CORE_CODE_ADDRESS},
    chain_id::ChainId,
    event::EventHandle,
    transaction::TransactionArgument,
    waypoint::Waypoint,
};
use serde::{Deserialize, Serialize};
use std::{str::FromStr, string::String, time::Duration};
use structopt::StructOpt;
use tokio::task;

#[derive(Clone, Debug, StructOpt)]
struct Args {
    /// Chain ID of the network this client is connecting to
    #[structopt(short = "c")]
    pub chain_id: u8,
    /// url
    #[structopt(short = "u")]
    pub url: String,
    /// mint key file with path
    #[structopt(short = "m")]
    pub mint_key: String,
    ///waypoint
    #[structopt(short)]
    pub waypoint: String,
}

#[derive(Debug, StructOpt)]
#[structopt(
    name = "Violas Oracle Service",
    author = "The Violas Association",
    about = "Violas Oracle client and data gathering service"
)]
enum Command {
    #[structopt(
        name = "publish",
        help = "publish Oracle Move contract to Violas blockchain."
    )]
    Publish(Args),
    /// update
    #[structopt(name = "update")]
    Update(Args),
    /// test
    #[structopt(name = "test")]
    Test(Args),
    /// service
    #[structopt(name = "service")]
    Service(Args),
}

//type Result<T> = std::result::Result<T, Box<dyn std::error::Error + Send + Sync>>;

//
// main
//
#[tokio::main]
async fn main() -> Result<()> {
    let command = Command::from_args();
    //println!("{:?}", command);

    let create_client = |args: Args| -> Result<ViolasClient> {
        let mint_key = args.mint_key.clone();
        let treasury_compliance_account_file = mint_key.clone();

        let client = ViolasClient::new(
            ChainId::new(args.chain_id),
            args.url.as_str(),
            mint_key.as_str(),
            treasury_compliance_account_file.as_str(),
            true,
            None,
            None,
            Waypoint::from_str(args.waypoint.as_str()).unwrap(),
        )?;

        Ok(client)
    };

    match command {
        Command::Publish(args) => {
            //let client = create_client(args)?;
            task::block_in_place(|| -> Result<()> {
                let mut client = create_client(args)?;
                client.publish_oracle()
            })?;
        }
        Command::Update(args) => {
            let rates = gather_exchange_rate_from_coinbase().await?;

            task::block_in_place(|| -> Result<()> {
                let mut client = create_client(args)?;

                update_oracle_exchange_rates(&mut client, rates)
            })?;
        }
        Command::Test(args) => {
            //task
            task::block_in_place(|| -> Result<()> {
                let mut client = create_client(args)?;

                //curl https://api.coinbase.com/v2/exchange-rates?currency=EUR | grep GBP
                run_test_exchange(&mut client, "EUR", 100_000_000, "GBP")?;

                oracle_view_exchange(&mut client, "BTC")?;

                Ok(())
            })?;
        }
        Command::Service(_args) => {}
    }

    Ok(())
}

async fn gather_exchange_rate_from_coinbase() -> Result<Vec<(String, f64)>> {
    let client = Client::builder()
        .pool_idle_timeout(Duration::from_secs(10))
        .build::<_, hyper::Body>(HttpsConnector::new());
    let url = Uri::from_static("https://api.coinbase.com/v2/exchange-rates?currency=USD");

    let res = client.get(url).await?;

    let body = hyper::body::aggregate(res).await?;

    let data: serde_json::Value = serde_json::from_reader(body.reader())?;

    let syms = vec!["BTC", "USD", "EUR", "GBP", "SGD"]; //"JPY", "CNY"

    let rates: Vec<(String, f64)> = syms
        .iter()
        .map(|sym| {
            (
                String::from(*sym),
                1 as f64
                    / String::from(data["data"]["rates"][sym].as_str().unwrap())
                        .parse::<f64>()
                        .unwrap(),
            )
        })
        .collect();

    Ok(rates)
}

fn update_oracle_exchange_rates(
    client: &mut ViolasClient,
    crc_code_rates: Vec<(String, f64)>,
) -> Result<()> {
    for code_rate in crc_code_rates {
        let (currency_code, rate) = code_rate;
        client.update_oracle_exchange_rate(make_currency_tag(currency_code.as_str()), rate, true)?
    }

    Ok(())
}

#[derive(Serialize, Debug, Deserialize)]
struct ExchangeRateReource {
    pub fixed_point32: u64,
    pub timestamp: u64,
    update_events: EventHandle,
}

fn oracle_view_exchange(client: &mut ViolasClient, currency_code: &str) -> Result<Option<f64>> {
    let ex_rate: Option<ExchangeRateReource> = client.get_account_resource(
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

fn run_test_exchange(
    client: &mut ViolasClient,
    currency_code1: &str,
    amount_crc1: u64,
    currency_code2: &str,
    //amount_crc2: u64,
) -> Result<()> {
    let rate1 = oracle_view_exchange(client, currency_code1)?.unwrap();
    let rate2 = oracle_view_exchange(client, currency_code2)?.unwrap();

    let amount_crc2 = (amount_crc1 as f64 * rate1 / rate2) as u64;
    println!("The exchanged amount of coin2 is {}", amount_crc2);

    client.execute_script_json(
        u64::MAX,
        "/home/hunter/Projects/work/ViolasClientSdk/move/oracle/test_exchange_rate.mv",
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
