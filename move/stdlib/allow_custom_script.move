script {

use 0x1::LibraTransactionPublishingOption;

fun main(lr_account: &signer) {    
    LibraTransactionPublishingOption::set_open_script(lr_account);
}

}