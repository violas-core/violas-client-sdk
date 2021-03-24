script {
use 0x1::Signer;
use 0x1::Vector;
use 0x1::Errors;
use 0x1::DiemAccount;
use 0x1::FixedPoint32;
use 0x1::VLS::{Self, VLS};
use 0x1::DiemTimestamp;
use 0x1::ViolasBank;
use 0x1::Exchange;

const E_TRANSACTION_SENDER_IS_NOT_VLS_COMM: u64 = 1000;
const E_BANK_PAYMENT_IS_INCORRECT: u64 = 1001;
const E_EXCHANGE_PAYMENT_IS_INCORRECT: u64 = 1002;
const BACKEND_ADDRESS : address = 0x585c6aa31dfb19c4af20e8e14112cb3f;

///
/// distribute VLS from Violas community to Bank administrator account, Exchange adminitrator account and backend(VLS-USER) account
///
fun distribute_vls_from_community(account : &signer, is_paying_to_bank: bool) {
    let sender = Signer::address_of(account);    

    // Mine VLS to VLS-COMM account
    DiemAccount::mine_vls();

    // retrieve all distribution receivers
    let receivers = VLS::get_receivers();
    
    // get the distribution ratio of account VLS-COMM with index 0
    let receiver = Vector::borrow(&mut receivers, 0);  
    let (addr, ratio) = VLS::unpack_receiver(*receiver);

    //Caller's address  must be the same with  account VLS-COMM's in VLS contract
    assert(sender == addr, Errors::requires_address(E_TRANSACTION_SENDER_IS_NOT_VLS_COMM));

    // get balance and total amount of mined VLS
    let balance = DiemAccount::balance<VLS>(sender);
    let total = FixedPoint32::divide_u64(balance, ratio);
    
    let bank_distribution_ratio = FixedPoint32::create_from_rational(16, 100);
    let exchange_distribution_ratio = FixedPoint32::create_from_rational(30, 100);
    let backend_distribution_ratio = FixedPoint32::create_from_rational(15, 100);

    // 1. Distribute VLS to Bank adminitrator
    if(is_paying_to_bank)   // Bank admin only receives VLS once per 24 hours
    {
        if(ViolasBank::is_published(account) == false) {
	        ViolasBank::publish(account, x"00");
        };

        let distribution_amount = FixedPoint32::multiply_u64(total, bank_distribution_ratio);
        
        balance = DiemAccount::balance<VLS>(sender);
        ViolasBank::set_incentive_rate(account, distribution_amount);

        // Make sure that the amount of VLS ViolasBank::set_incentive_rate extracted is distribution_amount   
        assert(DiemAccount::balance<VLS>(sender) == balance - distribution_amount, Errors::limit_exceeded(E_BANK_PAYMENT_IS_INCORRECT));
    };
    
    // 2. Distribute VLS to Exchange adminitrator
    {
        let distribution_amount = FixedPoint32::multiply_u64(total, exchange_distribution_ratio);
        let start_time = DiemTimestamp::now_seconds();
        let end_time = start_time + 86400; // 24hours
        
        balance = DiemAccount::balance<VLS>(sender);
        Exchange::set_next_rewards(account, distribution_amount, start_time, end_time);

        // Make sure that payment to Exchange admin is correct 
        assert(DiemAccount::balance<VLS>(sender) == balance - distribution_amount, Errors::limit_exceeded(E_EXCHANGE_PAYMENT_IS_INCORRECT));
    };

    let payer_withdrawal_cap = DiemAccount::extract_withdraw_capability(account);

    // 3. Distibute VLS to Backend administrator
    {
        let distribution_amount = FixedPoint32::multiply_u64(total, backend_distribution_ratio);        

        DiemAccount::pay_from<VLS>(
            &payer_withdrawal_cap, 
            BACKEND_ADDRESS, 
            distribution_amount, 
            x"", 
            x""
        );        
    };

    // 4.  The rest of VLS will be distributed to VLS trash accout 
    {
        let distribution_amount = DiemAccount::balance<VLS>(sender);        

        DiemAccount::pay_from<VLS>(
            &payer_withdrawal_cap, 
            0x564C5300,             //VLS Trash
            distribution_amount, 
            x"", 
            x""
        );
    };

    DiemAccount::restore_withdraw_capability(payer_withdrawal_cap);
}

} //end of script
