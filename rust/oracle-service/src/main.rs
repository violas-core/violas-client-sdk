use anyhow::Result; //bail, ensure, format_err,
use bytes::buf::BufExt as _;
use client_proxy::violas_client::ViolasClient;
use hyper::{Client, Uri};
use hyper_tls::HttpsConnector;
use libra_types::{chain_id::ChainId, waypoint::Waypoint};
use std::{str::FromStr, string::String, time::Duration};
use structopt::StructOpt;
use tokio::task;
mod oracle;
use oracle::Oracle;

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

//
// main
//
#[tokio::main]
async fn main() -> Result<()> {
    let command = Command::from_args();
    //println!("{:?}", command);

    let create_oracle = |args: Args| -> Result<Oracle> {
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

        Oracle::new(client)
    };

    match command {
        Command::Publish(args) => {
            //let client = create_client(args)?;
            task::block_in_place(|| -> Result<()> {
                println!("Pleae input Oracle module path and file name : ");
                let mut oracle_module_file = String::new();
                std::io::stdin().read_line(&mut oracle_module_file)?;
                // remove char '\n'
                oracle_module_file.trim(); //(oracle_module_file.len()-1);

                let mut oracle = create_oracle(args)?;

                // oracle_module_file = String::from_str(
                //     "/home/hunter/Projects/work/ViolasClientSdk/move/oracle/oracle.mv",
                // )?;
                oracle.publish(oracle_module_file.as_str())
            })?;
        }
        Command::Update(args) => {
            let currency_rates = gather_exchange_rate_from_coinbase().await?;

            task::block_in_place(|| -> Result<()> {
                let mut oracle = create_oracle(args)?;

                for currency_rate in currency_rates {
                    let (currency, ex_rate) = currency_rate;
                    oracle.update_exchange_rate(currency.as_str(), ex_rate)?;
                }

                Ok(())
            })?;
        }
        Command::Test(args) => {
            //task
            task::block_in_place(|| -> Result<()> {
                let mut oracle = create_oracle(args)?;

                //curl https://api.coinbase.com/v2/exchange-rates?currency=EUR | grep GBP
                oracle.run_test_case("EUR", 100_000_000, "GBP")?;

                oracle.view_exchange_rate("BTC")?;

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
