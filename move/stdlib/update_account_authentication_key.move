script {
use 0x1::LibraAccount;

fun main(
    lr_root: &signer,
    account_address: address,
    auth_key: vector<u8>,    
)  {

    LibraAccount::update_account_authentication_key(
        lr_root, 
        account_address, 
        auth_key);   
    
}

}