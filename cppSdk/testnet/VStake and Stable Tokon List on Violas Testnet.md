# 1. Violas multi currencies info
```json
All currencies info : [
    CurrencyInfoView {
        code: "Coin1",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "Coin2",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "LBR",
        scaling_factor: 1000000,
        fractional_part: 1000,
    },
    CurrencyInfoView {
        code: "VLSUSD",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "VLSEUR",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "VLSGBP",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "VLSJPY",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "VLSSGD",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "LBRUSD",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "LBREUR",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "LBRGBP",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "LBRJPY",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "LBRSGD",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
    CurrencyInfoView {
        code: "BTCBTC",
        scaling_factor: 1000000,
        fractional_part: 100,
    },
]
```

# 如何构造TypeTag访问多币种？
     TypeTag::Struct(StructTag {
        address: 0x0,
        module: currency_code,  //如"VLSUSD", "VLSEUR", ..... "BTCBTC"
        name: "T",
        type_params: vec![],
    })

