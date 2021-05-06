script {
    use 0x1::Exchange;

    fun swap<Token1: store, Token2: store>(
        sender : signer,
        token1_input_amount : u64,
        token2_output_min_amount : u64
        ) {

        Exchange::swap<Token1, Token2>(&sender, token1_input_amount, token2_output_min_amount);
    }
}

script {
    use 0x1::Exchange;

    fun swap3<Token1: store, Token2: store, Token3: store>(
        sender : signer,
        token1_input_amount : u64,
        token3_output_min_amount : u64
        ) {
        
        let token2_output_amount = Exchange::swap<Token1, Token2>(&sender, token1_input_amount, 0);
        Exchange::swap<Token2, Token3>(&sender, token2_output_amount, token3_output_min_amount);
    }
}

script {
    use 0x1::Exchange;

    fun swap4<Token1: store, Token2: store, Token3: store, Token4: store>(
        sender : signer,
        token1_input_amount : u64,
        token4_output_min_amount : u64
        ) {

        let token2_output_amount = Exchange::swap<Token1, Token2>(&sender, token1_input_amount, 0);
        let token3_output_amount = Exchange::swap<Token2, Token3>(&sender, token2_output_amount, 0);
        Exchange::swap<Token2, Token3>(&sender, token3_output_amount, token4_output_min_amount);
    }
}