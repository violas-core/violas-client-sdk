script {
use 0x1::LibraAccount;

fun recover_vls_fees_to_association(tc_account:&signer) {
    
    LibraAccount::recover_vls_fees_to_association(tc_account);

}

}