use anyhow::Result;
use diem_types::{
    account_address::AccountAddress,
    account_config::{
        constants::{from_currency_code_string, type_tag_for_currency_code},
        currency_info::CurrencyInfoResource,
    },
    event::EventHandle,
};
use move_core_types::{
    identifier::Identifier,
    language_storage::{StructTag, TypeTag},
};
use serde::{Deserialize, Serialize};

/// make struct tag
pub fn make_struct_tag(
    addr: &AccountAddress,
    module: &str,
    resource: &str,
    type_params: Vec<TypeTag>,
) -> Result<StructTag> {
    Ok(StructTag {
        address: *addr,
        module: Identifier::new(module)?,
        name: Identifier::new(resource)?,
        type_params,
    })
}

/// make type tag
pub fn make_type_tag(addr: &AccountAddress, module: &str, resource: &str) -> Result<TypeTag> {
    Ok(TypeTag::Struct(make_struct_tag(
        addr,
        module,
        resource,
        vec![],
    )?))
}

/// make currency tag
pub fn make_currency_tag(currency_code: &str) -> Result<TypeTag> {
    Ok(type_tag_for_currency_code(from_currency_code_string(
        currency_code,
    )?))
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

impl From<&CurrencyInfoResource> for CurrencyInfoViewEx {
    fn from(info: &CurrencyInfoResource) -> CurrencyInfoViewEx {
        CurrencyInfoViewEx {
            total_value: info.total_value(),
            preburn_value: info.preburn_value(),
            code: info.currency_code().to_string(),
            scaling_factor: info.scaling_factor(),
            fractional_part: info.fractional_part(),
            to_lbr_exchange_rate: info.exchange_rate(),
        }
    }
}

pub enum CurrencyEventType {
    Minted = 0,
    Burned = 1,
    Preburned = 2,
    CancelledBurn = 3,
    UpdatedExchangeRate = 4,
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct ViolasStatus {
    pub latest_version: u64,
    pub account_amount: u64,
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct AccountLimitMutationCapability {
    pub dummy_field: bool,
}
///
/// Account Operations Capbility
///
#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct AccountOperationsCapability {
    pub limits_cap: AccountLimitMutationCapability,
    pub creation_events: EventHandle,
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
