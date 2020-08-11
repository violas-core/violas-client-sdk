//#[macro_use]
//extern crate serde_derive;

use bytes::buf::BufExt as _;
use client_proxy::{violas_account::make_currency_tag, violas_client::ViolasClient};
use hyper::{Client, Uri};
use hyper_tls::HttpsConnector;
use libra_types::{chain_id::ChainId, waypoint::Waypoint};
use std::{str::FromStr, string::String, time::Duration};
use structopt::StructOpt;

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
    #[structopt(name = "update")]
    Update(Args),
    #[structopt(name = "service")]
    Service(Args),
}

type Result<T> = std::result::Result<T, Box<dyn std::error::Error + Send + Sync>>;

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
            let client = create_client(args)?;
            publish_oracle(client).await?;
        }
        Command::Update(args) => {
            let ret = gather_exchange_rate_from_coinbase().await;
            match ret {
                Ok(rates) => {
                    //println!("{:?}", rates.await);
                    let client = create_client(args)?;
                    update_oracle_exchange_rates(client, rates).await?;
                }
                Err(e) => {
                    println! {"failed to gather data from coinbase, {}", e};
                }
            }
        }
        Command::Service(args) => {
            let _client = create_client(args);
        }
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

    let syms = vec!["BTC", "EUR", "GBP", "SGD", "JPY", "CNY"];

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

async fn update_oracle_exchange_rates(
    mut client: ViolasClient,
    crc_code_rates: Vec<(String, f64)>,
) -> Result<()> {
    for code_rate in crc_code_rates {
        let (currency_code, rate) = code_rate;
        client.update_oracle_exchange_rate(
            make_currency_tag(currency_code.as_str()),
            rate,
            true,
        )?;
    }

    Ok(())
}

async fn publish_oracle(mut client: ViolasClient) -> Result<()> {
    client.publish_oracle()?;
    Ok(())
}
