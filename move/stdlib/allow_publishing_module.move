script {

use 0x1::DiemTransactionPublishingOption;

fun main(account: &signer, open : bool) {           
    DiemTransactionPublishingOption::set_open_module(account, open);    
}

}