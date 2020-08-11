script {
use 0x1::LibraAccount;
//
//  add a currency for designated dealer account 
//  note : This function is called by Treasury Compliance account 
//
fun main<Currency>(
    tc_account: &signer,
    dd_address: address,    
)  {

    LibraAccount::add_currency_for_designated_dealer<Currency>(
        tc_account,
        dd_address 
        );    
    
}

}