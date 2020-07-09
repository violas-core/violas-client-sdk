use anyhow::{bail, Error, Result}; //format_err

use libra_types::{
    access_path::AccessPath, account_address::AccountAddress,
    account_config::currency_info::CurrencyInfoResource, account_config::*,
    account_state::AccountState,
};
use move_core_types::{
    identifier::{IdentStr, Identifier},
    language_storage::{StructTag, TypeTag},
};
use once_cell::sync::Lazy;
use serde::{Deserialize, Serialize};

// Module name
static ACCOUNT_MODULE_NAME: Lazy<Identifier> =
    Lazy::new(|| Identifier::new("ViolasToken").unwrap());
// Structs' name
static TOKEN_STRUCT_NAME: Lazy<Identifier> = Lazy::new(|| Identifier::new("Tokens").unwrap());

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

pub fn currency_type_tag(addr: &AccountAddress, currency_code: &str) -> TypeTag {
    TypeTag::Struct(StructTag {
        address: *addr,
        module: from_currency_code_string(currency_code).unwrap(),
        name: from_currency_code_string(currency_code).unwrap(),
        type_params: vec![],
    })
}

/// Return the path to the Account resource. It can be used to create an AccessPath for an
/// Account resource.
pub fn account_resource_path(addr: &AccountAddress) -> Vec<u8> {
    AccessPath::resource_access_vec(&account_struct_tag(addr))
}

#[derive(Clone, Default, Serialize, Debug, Deserialize)]
pub struct Token {
    pub index: u64,
    pub balance: u64,
}

impl Token {
    pub fn new(index1: u64, balance1: u64) -> Self {
        Self {
            index: index1,
            balance: balance1,
        }
    }
}
impl std::cmp::PartialEq for Token {
    fn eq(&self, other: &Self) -> bool {
        self.index == other.index
    }
}

#[derive(Default, Serialize, Deserialize)]
#[cfg_attr(any(test, feature = "fuzzing"), derive(Arbitrary))]
pub struct ViolasAccountResource {
    pub tokens: Vec<Token>,
}

impl ViolasAccountResource {
    // #[allow(dead_code)]
    // pub fn new(index : u64, balance: u64) -> Self {
    //     ViolasAccountResource { balance }
    // }

    /// Given an account map (typically from storage) retrieves the Account resource associated.
    pub fn make_from(
        path_addr: &AccountAddress,
        account_state: &AccountState,
    ) -> Result<Self, Error> {
        let ap = account_resource_path(path_addr);
        match account_state.get(&ap) {
            Some(bytes) => lcs::from_bytes(bytes).map_err(Into::into),
            None => bail!("No data for {:?}", ap),
        }
    }
}

#[derive(Clone, Serialize, Deserialize, Debug, PartialEq)]
pub struct TokenView {
    pub tokens: Vec<Token>,
}

impl TokenView {
    pub fn new() -> Self {
        Self { tokens: vec![] }
    }
}


#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct CurrencyInfoViewEx {
    total_value: u128,
    preburn_value: u64,
    pub code: String,
    pub scaling_factor: u64,
    pub fractional_part: u64,
    pub to_lbr_exchange_rate: f32,
}

impl From<CurrencyInfoResource> for CurrencyInfoViewEx {
    fn from(info: CurrencyInfoResource) -> CurrencyInfoViewEx {
        CurrencyInfoViewEx {
            total_value : info.total_value(),
            preburn_value : info.preburn_value(),
            code: info.currency_code().to_string(),
            scaling_factor: info.scaling_factor(),
            fractional_part: info.fractional_part(),
            to_lbr_exchange_rate: info.exchange_rate(),
        }
    }
}

pub mod exchange {
    use serde::{Deserialize, Serialize};
    //
    #[derive(Clone, Default, Serialize, Debug, Deserialize)]
    pub struct Token {
        index: u64,
        value: u64,
    }
    //
    #[derive(Clone, Default, Serialize, Debug, Deserialize)]
    pub struct Tokens {
        tokens: Vec<Token>,
    }
    //
    #[derive(Clone, Default, Serialize, Debug, Deserialize)]
    pub struct Reserve {
        liquidity_total_supply: u64,
        coina: Token,
        coinb: Token,
    }
    //
    #[derive(Clone, Default, Serialize, Debug, Deserialize)]
    pub struct Reserves {
        reserves: Vec<Reserve>,
    }
    //
    #[derive(Clone, Default, Serialize, Debug, Deserialize)]
    pub struct RegisteredCurrencies {
        currency_codes: Vec<Vec<u8>>,
    }
}
