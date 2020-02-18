use anyhow::{bail, Error};
use lazy_static::lazy_static;
use libra_types::{
    access_path::{AccessPath, Accesses},
    account_address::AccountAddress,
    //account_config,
    account_state::AccountState,
    //byte_array::ByteArray,
    //event::EventHandle,
    identifier::{IdentStr, Identifier},
    language_storage::StructTag,
};
use serde::{Deserialize, Serialize};
use std::{result}; //convert::TryInto,

lazy_static! {
    //static ref COIN_MODULE_NAME: Identifier = Identifier::new("DToken").unwrap();
    //static ref COIN_STRUCT_NAME: Identifier = Identifier::new("T").unwrap();

    // Module name
    static ref ACCOUNT_MODULE_NAME: Identifier = Identifier::new("ViolasToken").unwrap();
    // Structs' name
    static ref TOKEN_STRUCT_NAME: Identifier = Identifier::new("T").unwrap();
    static ref TOKEN_ADDR_STRUCT_NAME: Identifier = Identifier::new("TokenAddr").unwrap();
    static ref INFO_STRUCT_NAME: Identifier = Identifier::new("Info").unwrap();
    static ref OWNER_DATA_STRUCT_NAME: Identifier = Identifier::new("OwnerData").unwrap();
    static ref ORDER_STRUCT_NAME: Identifier = Identifier::new("Order").unwrap();
    static ref ORDER2_STRUCT_NAME: Identifier = Identifier::new("Order2").unwrap();
}

pub fn account_module_name() -> &'static IdentStr {
    &*ACCOUNT_MODULE_NAME
}

pub fn account_struct_name() -> &'static IdentStr {
    &*TOKEN_STRUCT_NAME
}

pub fn account_struct_tag(addr: &AccountAddress) -> StructTag {
    StructTag {
        address: *addr,
        module: account_module_name().to_owned(),
        name: account_struct_name().to_owned(),
        type_params: vec![],
    }
}

/// Return the path to the Account resource. It can be used to create an AccessPath for an
/// Account resource.
pub fn account_resource_path(addr: &AccountAddress) -> Vec<u8> {
    AccessPath::resource_access_vec(&account_struct_tag(addr), &Accesses::empty())
}

#[derive(Debug, Default, Serialize, Deserialize)]
#[cfg_attr(any(test, feature = "fuzzing"), derive(Arbitrary))]
pub struct ViolasAccountResource {
    //libra_account: account_config::AccountResource,
    pub balance: u64,
}

impl ViolasAccountResource {
    #[allow(dead_code)]
    pub fn new(balance: u64) -> Self {
        ViolasAccountResource { balance }
    }

    /// Given an account map (typically from storage) retrieves the Account resource associated.
    pub fn make_from(
        path_addr: &AccountAddress,
        account_state: &AccountState,
    ) -> result::Result<Self, Error> {
        let ap = account_resource_path(path_addr);
        match account_state.get(&ap) {
            Some(bytes) => lcs::from_bytes(bytes).map_err(Into::into),
            None => bail!("No data for {:?}", ap),
        }
    }
}
