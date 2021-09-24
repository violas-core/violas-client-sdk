use anyhow::{bail, Result};
use diem_types::{account_address::AccountAddress, chain_id::ChainId, waypoint::Waypoint};
use hex;
use jsonrpc_http_server::jsonrpc_core::{types::Error, IoHandler, Params, Value};
use jsonrpc_http_server::ServerBuilder;
use std::str::FromStr;
use violas_client::{violas_client::ViolasClient, violas_resource::make_currency_tag};

// test from curl
// curl -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"register_account","params":[9],"id":1}' "http://localhost:50002"

pub fn run_json_rpc_server(
	url: String,
	chain_id: u8,
	mint_key: String,
	mnemonic: String,
	waypoint: String,
) {
	let mut io = IoHandler::default();

	io.add_sync_method("register_account", move |params: Params| {
		if let Params::Array(arr) = params {
			if arr[0].as_str() == None {
				return Err(Error::invalid_params(
					"The authentication key must be a string",
				));
			}

			let auth_key_hex = arr[0].as_str().unwrap();

			if auth_key_hex.len() != 64 {
				return Err(Error::invalid_params(
					"The length of authentication key string must be 64",
				));
			}

			println!("{}", auth_key_hex);

			let create_child_vasp = |hex_auth_key: String| -> Result<()> {
				let mut client: ViolasClient = ViolasClient::new(
					ChainId::new(chain_id),
					url.as_str(),
					mint_key.clone().as_str(),
					mint_key.clone().as_str(),
					mint_key.as_str(),
					true,
					None,
					Some(mnemonic.clone()),
					Waypoint::from_str(waypoint.as_str()).unwrap(),
				)?;
				client.create_next_account(None, true)?;
				client.create_next_account(None, true)?;

				let mut auth_key = hex::decode(hex_auth_key)?;
				let mut auth_key_prefix = auth_key.clone();
				auth_key_prefix.truncate(16);
				let address = auth_key.drain(16..);				

				client.create_child_vasp_account(
					make_currency_tag("VLS")?,
					1, // account index
					AccountAddress::from_bytes(&address)?,
					auth_key_prefix,
					false,
					1 * 1_000_000,
					true,
				)
			};

			let ret = create_child_vasp(auth_key_hex.to_owned());
			match ret {
				Ok(()) => {
					return Ok(Value::String("ok".to_owned()));
				}
				Err(err) => {
					return Err(Error::invalid_params_with_details("create_child_vasp", err));
				}
			}
		} else {
			Err(Error::invalid_params("parameters must be an array."))
		}
	});

	let server = ServerBuilder::new(io)
		.threads(3)
		.start_http(&"0.0.0.0:50002".parse().unwrap())
		.unwrap();

	server.wait();
}
