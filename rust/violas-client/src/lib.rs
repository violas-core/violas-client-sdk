#![recursion_limit = "4096"]

mod diem_client;

pub mod diem_client_proxy;
pub mod ffi;
//pub mod move_compiler;
pub mod violas_client;
pub mod violas_resource;
pub use diem_json_rpc_client::views;

use diem_crypto::{
    ed25519::{Ed25519PrivateKey, Ed25519PublicKey},
    test_utils::KeyPair,
    traits::ValidCryptoMaterialStringExt,
};
use diem_types::account_address::AccountAddress;
use serde::{Deserialize, Serialize};

//extern crate cpp;

/// Struct used to store data for each created account.  We track the sequence number
/// so we can create new transactions easily
#[derive(Debug, Serialize, Deserialize, PartialEq)]
#[cfg_attr(any(test, feature = "fuzzing"), derive(Clone))]
pub struct AccountData {
    /// Address of the account.
    pub address: AccountAddress,
    /// Authentication key of the account.
    pub authentication_key: Option<Vec<u8>>,
    /// (private_key, public_key) pair if the account is not managed by wallet.
    pub key_pair: Option<KeyPair<Ed25519PrivateKey, Ed25519PublicKey>>,
    /// Latest sequence number maintained by client, it can be different from validator.
    pub sequence_number: u64,
    /// Whether the account is initialized on chain, cached local only, or status unknown.
    pub status: AccountStatus,
}

/// Enum used to represent account status.
#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub enum AccountStatus {
    /// Account exists only in local cache, it is not persisted on chain.
    Local,
    /// Account is persisted on chain.
    Persisted,
    /// Not able to check account status, probably because client is not able to talk to the
    /// validator.
    Unknown,
}

impl AccountData {
    /// Serialize account keypair if exists.
    pub fn keypair_as_string(&self) -> Option<(String, String)> {
        self.key_pair.as_ref().and_then(|key_pair| {
            let private_key_string = key_pair
                .private_key
                .to_encoded_string()
                .expect("Account private key to convertible to string!");
            let public_key_string = key_pair
                .public_key
                .to_encoded_string()
                .expect("Account public Key not convertible to string!");
            Some((private_key_string, public_key_string))
        })
    }
}
