script {

use DiemFramework::DiemTransactionPublishingOption;

fun main(dr_account: signer, open : bool) {           
    DiemTransactionPublishingOption::set_open_module(&dr_account, open);    
}

}