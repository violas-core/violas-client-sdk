use anyhow::Result; //bail, ensure, format_err,
use bytes::buf::BufExt as _;
use client_proxy::violas_client::ViolasClient;
use hyper::{Client, Uri};
use hyper_tls::HttpsConnector;
use libra_types::{chain_id::ChainId, waypoint::Waypoint};
use std::{path::Path, str::FromStr, string::String, time::Duration};
use structopt::StructOpt;
use tokio::{
    runtime::Runtime,
    task,
    time::{self}, 
};
mod oracle;
use chrono::prelude::*;
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
    #[structopt(short = "m", default_value = "")]
    pub mint_key: String,
    ///waypoint
    #[structopt(short)]
    pub waypoint: String,
    ///mnemonic file
    #[structopt(short = "n")]
    pub mnemonic: String,
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
//#[tokio::main]
//async fn main() -> Result<()>
fn main() -> Result<()> {
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
            Some(args.mnemonic),
            Waypoint::from_str(args.waypoint.as_str()).unwrap(),
        )?;

        Oracle::new(client)
    };

    match command {
        Command::Publish(args) => {
            let mut oracle_module_file = String::from("move/oracle/oracle.mv");
            if !Path::new(oracle_module_file.as_str()).exists() {
                println!("Pleae input module path and file name : ");
                oracle_module_file.clear();
                std::io::stdin().read_line(&mut oracle_module_file)?;
                // remove char '\n'
                oracle_module_file.remove(oracle_module_file.len() - 1);
            };

            let mut oracle = create_oracle(args)?;

            oracle.publish(oracle_module_file.as_str())?;

            println!("");
        }
        Command::Update(args) => {
            let mut rt = Runtime::new()?;

            let currency_rates =
                rt.block_on(async { gather_exchange_rate_from_coinbase().await })?;

            let mut oracle = create_oracle(args)?;

            for currency_rate in currency_rates {
                let (currency, ex_rate) = currency_rate;
                oracle.update_exchange_rate(currency.as_str(), ex_rate)?;
            }
        }
        Command::Test(args) => {
            task::block_in_place(|| -> Result<()> {
                let mut oracle = create_oracle(args)?;

                //curl https://api.coinbase.com/v2/exchange-rates?currency=EUR | grep GBP
                oracle.run_test_case("EUR", 100_000_000, "GBP")?;

                oracle.view_exchange_rate("BTC")?;

                Ok(())
            })?;
        }
        Command::Service(args) => {
            daemon()?;

            let mut rt = Runtime::new()?;

            loop {
                let currency_rates =
                    rt.block_on(async { gather_exchange_rate_from_coinbase().await })?;

                let mut oracle = create_oracle(args.clone())?;

                println!(
                    "{} : started to udpate Oracle Exchange Rates.",
                    Local::now()
                );

                for currency_rate in currency_rates {
                    let (currency, ex_rate) = currency_rate;
                    oracle.update_exchange_rate(currency.as_str(), ex_rate)?;
                    print!("{} : {}, ", currency, ex_rate);
                }

                println!(
                    "\n{} : finished udpating Oracle Exchange Rates.",
                    Local::now()
                );

                rt.block_on(async { time::delay_for(Duration::from_secs(60)).await });
            }
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

use std::fs::File;

use daemonize::Daemonize;

fn daemon() -> Result<()> {
    let stdout = File::create("/tmp/violas-oracle.out").unwrap();
    let stderr = File::create("/tmp/violas-oracle.err").unwrap();

    let daemonize = Daemonize::new()
        //.pid_file("/tmp/daemon.pid") // Every method except `new` and `start`
        .chown_pid_file(true) // is optional, see `Daemonize` documentation
        .working_directory("/tmp") // for default behaviour.
        //.user("hunter")
        //.group("daemon") // Group name
        //.group(2)        // or group id.
        .umask(0o777) // Set umask, `0o027` by default.
        .stdout(stdout) // Redirect stdout to `/tmp/daemon.out`.
        .stderr(stderr) // Redirect stderr to `/tmp/daemon.err`.
        .exit_action(|| println!("violas_oracle starts to run as a daemon."))
        .privileged_action(|| "Executed before drop privileges");

    match daemonize.start() {
        Ok(_) => println!("Success, daemonized"),
        Err(e) => eprintln!("Error, {}", e),
    }

    Ok(())
}
