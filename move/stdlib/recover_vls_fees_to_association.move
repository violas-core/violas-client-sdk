script {
use 0x1::DiemAccount;

fun recover_vls_fees_to_association(tc_account:&signer, receiver: address) {
    
    DiemAccount::recover_vls_fees_to_association(tc_account, receiver);

}

}