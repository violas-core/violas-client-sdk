script {
use 0x1::DiemAccount;
//
//  add a currency for designated dealer account 
//  note : This function is called by Treasury Compliance account 
//
fun add_currency_for_designated_dealer<CoinType>(
    tc_account: &signer,
    dd_address: address,    
)  {

    DiemAccount::add_currency_for_designated_dealer<CoinType>(
        tc_account,
        dd_address 
        );    
    
}

}