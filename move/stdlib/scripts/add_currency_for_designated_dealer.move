script {
use DiemFramework::DiemAccount;
//
//  add a currency for designated dealer account 
//  note : This function is called by Treasury Compliance account 
//
fun add_currency_for_designated_dealer<CoinType: store>(
    tc_account: signer,
    dd_address: address,    
)  {

    DiemAccount::add_currency_for_designated_dealer<CoinType>(
        &tc_account,
        dd_address 
        );        
}

}

script {
use DiemFramework::DesignatedDealer;
//
//  The designated dealer account adds a currency for himself 
//  note : This function is called by DD, and needs the signature of TC account 
//
fun dd_add_currency<CoinType: store>(
    dd_account: signer,
    tc_account: signer,    
)  {
    DesignatedDealer::add_currency<CoinType>(&dd_account, &tc_account);    
}

}