//#[macro_use]
//extern crate serde_derive;

use bytes::buf::BufExt as _;
use hyper::{Client, Uri};
use hyper_tls::HttpsConnector;
use std::{string::String, time::Duration};

type Result<T> = std::result::Result<T, Box<dyn std::error::Error + Send + Sync>>;
#[tokio::main]
async fn main() -> Result<()> {
    //let client = Client::new();
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

    println!("{:?}", rates);

    Ok(())
}
