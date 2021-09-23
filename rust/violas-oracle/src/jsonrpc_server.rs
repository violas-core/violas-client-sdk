use anyhow::Result;
use diem_types::{account_address::AccountAddress, chain_id::ChainId, waypoint::Waypoint};
use hex;
use jsonrpc_http_server::jsonrpc_core::{types::Error, IoHandler, Params, Value};
use jsonrpc_http_server::ServerBuilder;
use std::str::FromStr;
use violas_client::{violas_client::ViolasClient, violas_resource::make_currency_tag};

// test from curl
// curl -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"register_account","params":[9],"id":1}' "http://localhost:3030"

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
			println!("{}", arr[0]);

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

				let auth_key = hex::decode(hex_auth_key)?;
				client.create_child_vasp_account(
					make_currency_tag("VLS")?,
					1, // account index
					AccountAddress::from_bytes(&auth_key[16..32])?,
					auth_key,
					false,
					1 * 1_000_000,
					true,
				)
			};

			let ret = create_child_vasp(arr[0].clone().as_str().expect("").to_owned());
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
