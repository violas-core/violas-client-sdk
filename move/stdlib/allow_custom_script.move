script {

use 0x1::DiemTransactionPublishingOption;

fun main(dr_account: signer) {    
    DiemTransactionPublishingOption::set_open_script(&dr_account);
}

}