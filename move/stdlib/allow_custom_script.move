script {

use 0x1::DiemTransactionPublishingOption;

fun main(lr_account: &signer) {    
    DiemTransactionPublishingOption::set_open_script(lr_account);
}

}