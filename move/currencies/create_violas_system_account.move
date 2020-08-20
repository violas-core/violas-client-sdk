script {
use 0x1::LibraAccount;

fun main<NewCurrency>(
    lr_root: &signer,
    new_account_address: address,
    auth_key: vector<u8>,    
)  {

    LibraAccount::create_violas_system_account<NewCurrency>(
        lr_root, 
        new_account_address, 
        auth_key);   
    
}

}