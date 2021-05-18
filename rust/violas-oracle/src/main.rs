use anyhow::Result; //bail, ensure, format_err,
use bytes::buf::BufExt as _;
use diem_types::{chain_id::ChainId, waypoint::Waypoint};
use hyper::{Client, Uri};
use hyper_tls::HttpsConnector;
use std::{path::Path, str::FromStr, string::String, time::Duration}; //io::stdin,
use structopt::StructOpt;
use tokio::{
    runtime::Runtime,
    task,
    time::{self},
};
use violas_client::violas_client::ViolasClient;
mod oracle;
use chrono::prelude::*;
use hyper_timeout::TimeoutConnector;
use oracle::Oracle;

const ALL_CURRENCIES_CODE: [&str; 15] = [
    "BTC", "WBTC", "REN", "USDC", "BUSD", "DAI", "WETH", "UNI", "SUSHI", "LINK", "COMP", "AAVE",
    "BNB", "WFIL", "USDT",
    //, "USD", "EUR", "GBP", "SGD", "JPY", "CNY"]
];
const PREFIX_CURRENCY: &str = "V";

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

    /// service
    #[structopt(name = "view-events")]
    View(Args),
}

//
// main
//
//#[tokio::main]
//async fn main() -> Result<()>
fn main() -> Result<()> {
    let command = Command::from_args();
    process_command(command)?;

    Ok(())
}

fn create_oracle(args: Args) -> Result<Oracle> {
    let mint_key = args.mint_key.clone();
    let treasury_compliance_account_file = mint_key.clone();

    let client = ViolasClient::new(
        ChainId::new(args.chain_id),
        args.url.as_str(),
        mint_key.as_str(),
        mint_key.as_str(),
        treasury_compliance_account_file.as_str(),
        true,
        None,
        Some(args.mnemonic),
        Waypoint::from_str(args.waypoint.as_str()).unwrap(),
    )?;

    Oracle::new(client)
}

fn process_command(command: Command) -> Result<()> {
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

            let mut oracle = create_oracle(args.clone())?;
            oracle.create_admin_account()?;

            for currency_rate in currency_rates {
                let (currency, ex_rate) = currency_rate;

                let ret = oracle.update_exchange_rate(
                    (String::from(PREFIX_CURRENCY) + &currency).as_str(),
                    ex_rate,
                    false,
                );
                match ret {
                    Ok(_) => {
                        print!("{} : {}, ", currency, ex_rate);
                    }
                    Err(e) => {
                        println!("failed to update exchange rate, error : {}", e);
                    }
                }
            }

            println!("");
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

            let mut oracle = create_oracle(args.clone())?;

            oracle.create_admin_account()?;

            loop {
                println!("{} : started to loop.", Local::now());

                let ret = rt.block_on(async { gather_exchange_rate_from_coinbase().await });

                let currency_rates = match ret {
                    Ok(rates) => rates,
                    Err(e) => {
                        eprintln!(
                            "{} : failed to gather exchange rate from coinbase, error : {}",
                            Local::now(),
                            e
                        );
                        continue;
                    }
                };

                println!(
                    "{} : started to udpate Oracle Exchange Rates, {:?}",
                    Local::now(),
                    currency_rates,
                );

                // Update all currencies Violas supported
                for currency_rate in currency_rates {
                    let (currency, ex_rate) = currency_rate;

                    let ret = oracle.update_exchange_rate(
                        (String::from(PREFIX_CURRENCY) + &currency).as_str(),
                        ex_rate,
                        false,
                    );

                    match ret {
                        Ok(_) => {
                            print!("{} : {}, ", currency, ex_rate);
                        }
                        Err(e) => {
                            eprintln!(
                                "{} : failed to update exchange rate for {}, error : {}",
                                Local::now(),
                                currency,
                                e
                            );
                        }
                    }

                    println!("");
                }
                //
                // Update USDT
                //
                let usdt = "VUSDT";
                let rate = 1.0;

                let ret = oracle.update_exchange_rate(usdt, rate, true);
                match ret {
                    Ok(_) => {
                        print!("{} : {}, ", usdt, rate);
                    }
                    Err(e) => {
                        eprintln!(
                            "{} : failed to update exchange rate, error : {}",
                            Local::now(),
                            e
                        );
                        match create_oracle(args.clone()) {
                            Ok(ret) => oracle = ret,
                            Err(e) => println!("failed to create oracle client, {}.", e),
                        }
                        oracle.create_admin_account()?;
                    }
                }

                println!(
                    "\n{} : finished udpating Oracle Exchange Rates.",
                    Local::now()
                );

                rt.block_on(async { time::delay_for(Duration::from_secs(60)).await });

                println!("{} : end to loop.", Local::now());
            }

            //println!("{} : finished running service.", Local::now());
        }
        Command::View(args) => {
            task::block_in_place(|| -> Result<()> {
                let mut oracle = create_oracle(args)?;

                for currency_code in ALL_CURRENCIES_CODE.iter() {
                    let mut currency = String::from(PREFIX_CURRENCY);
                    currency.push_str(currency_code);

                    let _ret = oracle.get_last_event(currency.as_str());
                }

                Ok(())
            })?;
        }
    }

    Ok(())
}

async fn gather_exchange_rate_from_coinbase() -> Result<Vec<(String, f64)>> {
    let https = HttpsConnector::new();

    let mut connector = TimeoutConnector::new(https);
    connector.set_connect_timeout(Some(Duration::from_secs(10)));
    connector.set_read_timeout(Some(Duration::from_secs(10)));
    connector.set_write_timeout(Some(Duration::from_secs(10)));

    let client = Client::builder()
        .pool_idle_timeout(Duration::from_secs(10))
        .build::<_, hyper::Body>(connector);
    let url = Uri::from_static("https://api.coinbase.com/v2/exchange-rates?currency=USD");

    let res = client.get(url).await?;

    let body = hyper::body::aggregate(res).await?;

    let data: serde_json::Value = serde_json::from_reader(body.reader())?;

    //let syms = vec!["BTC", "USD", "EUR", "GBP", "SGD"]; //"JPY", "CNY"

    let rates: Vec<(String, f64)> = ALL_CURRENCIES_CODE
        .iter()
        .filter(|sym| data["data"]["rates"][sym].as_str().is_some())
        .map(|sym| {
            let rate = data["data"]["rates"][sym].as_str().unwrap();

            (
                String::from(*sym),
                1 as f64 / String::from(rate).parse::<f64>().unwrap(),
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
