script {
use 0x1::Signer;
use 0x1::Vector;
use 0x1::LibraAccount;
use 0x1::FixedPoint32;
use 0x1::VLS::{Self, VLS};
use 0x1::ViolasBank;

///
/// distribute VLS from Violas community to Bank administrator account, Exchange adminitrator account and backend(VLS-USER) account
///
fun distribute_vls_from_community(account : &signer) {
    let sender = Signer::address_of(account);    

    // retrieve all distribution receivers
    let receivers = VLS::get_receivers();
    
    // get the distribution ratio of account VLS-COMM with index 0
    let receiver = Vector::borrow(&mut receivers, 0);  
    let (addr, ratio) = VLS::unpack_receiver(*receiver);

    //Caller's address  must be the same with  account VLS-COMM's in VLS contract
    assert(sender == addr, 1000);

    // get balance and total amount of mined VLS
    let balance = LibraAccount::balance<VLS>(sender);
    let total = FixedPoint32::divide_u64(balance, ratio);
    
    let bank_distribution_ratio = FixedPoint32::create_from_rational(16, 100);
    let exchange_distribution_ratio = FixedPoint32::create_from_rational(30, 100);
    let backend_distribution_ratio = FixedPoint32::create_from_rational(15, 100);

    // 1. Distribute VLS to Bank adminitrator
    {
        if(ViolasBank::is_published(account) == false) {
	        ViolasBank::publish(account, x"00");
        };

        let distribution_amount = FixedPoint32::multiply_u64(total, bank_distribution_ratio);
        ViolasBank::set_incentive_rate(account, distribution_amount);

        // Make sure that the amount of VLS ViolasBank::set_incentive_rate extracted is distribution_amount   
        assert(LibraAccount::balance<VLS>(sender) == balance - distribution_amount, 1001);
    };
    
    // 2. Distribute VLS to Exchange adminitrator
    {
        let distribution_amount = FixedPoint32::multiply_u64(total, exchange_distribution_ratio);
        
        assert(distribution_amount > 1, 0);
    };

    let payer_withdrawal_cap = LibraAccount::extract_withdraw_capability(account);

    // Distibute VLS to Backend administrator
    {
        let distribution_amount = FixedPoint32::multiply_u64(total, backend_distribution_ratio);        

        LibraAccount::pay_from<VLS>(
            &payer_withdrawal_cap, 
            0xa5eeb37371bb939845a62bcaa1c2b41f, 
            distribution_amount, 
            x"", 
            x""
        );        
    };

    // The rest of VLS will be distributed to 0xDD00 accout 
    {
        let distribution_amount = LibraAccount::balance<VLS>(sender);        

        LibraAccount::pay_from<VLS>(
            &payer_withdrawal_cap, 
            0xDD00, 
            distribution_amount, 
            x"", 
            x""
        );
    };

    LibraAccount::restore_withdraw_capability(payer_withdrawal_cap);
}



}