script {

use 0x1::LibraTransactionPublishingOption;

fun main(account: &signer) {
    LibraTransactionPublishingOption::set_open_script(account);      
}

}