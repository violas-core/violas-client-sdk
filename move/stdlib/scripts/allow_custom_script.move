script {

use DiemFramework::DiemTransactionPublishingOption;

fun main(dr_account: signer) {    
    DiemTransactionPublishingOption::set_open_script(&dr_account);
}

}