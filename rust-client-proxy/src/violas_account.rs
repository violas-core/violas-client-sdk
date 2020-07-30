use libra_types::{
    account_address::AccountAddress, account_config::currency_info::CurrencyInfoResource,
    account_config::*,
};
use move_core_types::language_storage::{StructTag, TypeTag};
use serde::{Deserialize, Serialize};

// #[derive(Default, Serialize, Deserialize)]
// pub struct ViolasAccountResource {
//     pub tokens: Vec<Token>,
// }

// impl ViolasAccountResource {
//     // #[allow(dead_code)]
//     // pub fn new(index : u64, balance: u64) -> Self {
//     //     ViolasAccountResource { balance }
//     // }

//     /// Given an account map (typically from storage) retrieves the Account resource associated.
//     pub fn make_from(
//         path_addr: &AccountAddress,
//         account_state: &AccountState,
//     ) -> Result<Self, Error> {
//         let ap = account_resource_path(path_addr);
//         match account_state.get(&ap) {
//             Some(bytes) => lcs::from_bytes(bytes).map_err(Into::into),
//             None => bail!("No data for {:?}", ap),
//         }
//     }
// }

pub fn currency_type_tag(addr: &AccountAddress, currency_code: &str) -> TypeTag {
    TypeTag::Struct(StructTag {
        address: *addr,
        module: from_currency_code_string(currency_code).unwrap(),
        name: from_currency_code_string(currency_code).unwrap(),
        type_params: vec![],
    })
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
            total_value: info.total_value(),
            preburn_value: info.preburn_value(),
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
