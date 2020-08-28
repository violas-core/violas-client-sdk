script {

use 0x1::LibraTransactionPublishingOption;

fun main(account: &signer, open : bool) {           
    LibraTransactionPublishingOption::set_open_module(account, open);    
}

}