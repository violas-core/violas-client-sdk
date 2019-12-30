//
// export modules
//
#[cfg(target_os = "android")]
pub mod android;

#[cfg(target_os = "ios")]
pub mod ios;

#[cfg(target_os = "wasm")]
pub mod wasm;

//#[cfg(target_arch = "x86_64")]
pub mod x86_64;

pub(crate) mod compiler_proxy;
pub(crate) mod violas_account;

//
// test code
//

//use client::client_proxy::*;
use client::client_proxy::ClientProxy;
//use client::{client_proxy::ClientProxy, commands::*};
use std::error::Error;
use std::*;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
        println!("run it_works");
    }

    use super::new_client_proxy;
    use super::*;
    #[test]
    fn check_new() {
        let ret = panic::catch_unwind(|| new_client_proxy().unwrap()).ok();
        //ClientProxy;
        let proxy;
        match ret {
            Some(value) => proxy = value,
            None => println!("failed to new client proxy "),
        };
        //assert!(_proxy);
    }
}

pub fn new_client_proxy() -> Result<ClientProxy, Box<dyn Error>> {
    let host = "localhost";
    let port: u16 = 8000;
    let validator_set_file = "../../libra/scripts/cli/consensus_peers.config.toml";
    let faucet_account_file = "";
    let sync_on_wallet_recovery = false;
    let faucet_server = None;
    let mnemonic_file = None;

    // new a Client Proxy
    let client = ClientProxy::new(
        host,
        port,
        validator_set_file,
        faucet_account_file,
        sync_on_wallet_recovery,
        faucet_server,
        mnemonic_file,
    )?;

    Ok(client)
}
