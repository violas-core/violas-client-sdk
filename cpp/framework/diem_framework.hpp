#pragma once

#include "diem_types.hpp"

namespace diem_framework {


using namespace serde;
using namespace diem_types;


/// # Summary
/// Adds a zero `Currency` balance to the sending `account`. This will enable `account` to
/// send, receive, and hold `Diem::Diem<Currency>` coins. This transaction can be
/// successfully sent by any account that is allowed to hold balances
/// (e.g., VASP, Designated Dealer).
///
/// # Technical Description
/// After the successful execution of this transaction the sending account will have a
/// `DiemAccount::Balance<Currency>` resource with zero balance published under it. Only
/// accounts that can hold balances can send this transaction, the sending account cannot
/// already have a `DiemAccount::Balance<Currency>` published under it.
///
/// # Parameters
/// | Name       | Type      | Description                                                                                                                                         |
/// | ------     | ------    | -------------                                                                                                                                       |
/// | `Currency` | Type      | The Move type for the `Currency` being added to the sending account of the transaction. `Currency` must be an already-registered currency on-chain. |
/// | `account`  | `&signer` | The signer of the sending account of the transaction.                                                                                               |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                             | Description                                                                |
/// | ----------------            | --------------                           | -------------                                                              |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                  | The `Currency` is not a registered currency on-chain.                      |
/// | `Errors::INVALID_ARGUMENT`  | `DiemAccount::EROLE_CANT_STORE_BALANCE` | The sending `account`'s role does not permit balances.                     |
/// | `Errors::ALREADY_PUBLISHED` | `DiemAccount::EADD_EXISTING_CURRENCY`   | A balance for `Currency` is already published under the sending `account`. |
///
/// # Related Scripts
/// * `Script::create_child_vasp_account`
/// * `Script::create_parent_vasp_account`
/// * `Script::peer_to_peer_with_metadata`
Script encode_add_currency_to_account_script(TypeTag currency);

/// # Summary
/// Adds a zero `Currency` balance to the sending `account`. This will enable `account` to
/// send, receive, and hold `Diem::Diem<Currency>` coins. This transaction can be
/// successfully sent by any account that is allowed to hold balances
/// (e.g., VASP, Designated Dealer).
///
/// # Technical Description
/// After the successful execution of this transaction the sending account will have a
/// `DiemAccount::Balance<Currency>` resource with zero balance published under it. Only
/// accounts that can hold balances can send this transaction, the sending account cannot
/// already have a `DiemAccount::Balance<Currency>` published under it.
///
/// # Parameters
/// | Name       | Type     | Description                                                                                                                                         |
/// | ------     | ------   | -------------                                                                                                                                       |
/// | `Currency` | Type     | The Move type for the `Currency` being added to the sending account of the transaction. `Currency` must be an already-registered currency on-chain. |
/// | `account`  | `signer` | The signer of the sending account of the transaction.                                                                                               |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                             | Description                                                                |
/// | ----------------            | --------------                           | -------------                                                              |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                  | The `Currency` is not a registered currency on-chain.                      |
/// | `Errors::INVALID_ARGUMENT`  | `DiemAccount::EROLE_CANT_STORE_BALANCE` | The sending `account`'s role does not permit balances.                     |
/// | `Errors::ALREADY_PUBLISHED` | `DiemAccount::EADD_EXISTING_CURRENCY`   | A balance for `Currency` is already published under the sending `account`. |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_child_vasp_account`
/// * `AccountCreationScripts::create_parent_vasp_account`
/// * `PaymentScripts::peer_to_peer_with_metadata`
TransactionPayload encode_add_currency_to_account_script_function(TypeTag currency);

/// # Summary
/// Stores the sending accounts ability to rotate its authentication key with a designated recovery
/// account. Both the sending and recovery accounts need to belong to the same VASP and
/// both be VASP accounts. After this transaction both the sending account and the
/// specified recovery account can rotate the sender account's authentication key.
///
/// # Technical Description
/// Adds the `DiemAccount::KeyRotationCapability` for the sending account
/// (`to_recover_account`) to the `RecoveryAddress::RecoveryAddress` resource under
/// `recovery_address`. After this transaction has been executed successfully the account at
/// `recovery_address` and the `to_recover_account` may rotate the authentication key of
/// `to_recover_account` (the sender of this transaction).
///
/// The sending account of this transaction (`to_recover_account`) must not have previously given away its unique key
/// rotation capability, and must be a VASP account. The account at `recovery_address`
/// must also be a VASP account belonging to the same VASP as the `to_recover_account`.
/// Additionally the account at `recovery_address` must have already initialized itself as
/// a recovery account address using the `Script::create_recovery_address` transaction script.
///
/// The sending account's (`to_recover_account`) key rotation capability is
/// removed in this transaction and stored in the `RecoveryAddress::RecoveryAddress`
/// resource stored under the account at `recovery_address`.
///
/// # Parameters
/// | Name                 | Type      | Description                                                                                                |
/// | ------               | ------    | -------------                                                                                              |
/// | `to_recover_account` | `&signer` | The signer reference of the sending account of this transaction.                                           |
/// | `recovery_address`   | `address` | The account address where the `to_recover_account`'s `DiemAccount::KeyRotationCapability` will be stored. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                              | Description                                                                                       |
/// | ----------------           | --------------                                            | -------------                                                                                     |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `to_recover_account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.    |
/// | `Errors::NOT_PUBLISHED`    | `RecoveryAddress::ERECOVERY_ADDRESS`                      | `recovery_address` does not have a `RecoveryAddress` resource published under it.                 |
/// | `Errors::INVALID_ARGUMENT` | `RecoveryAddress::EINVALID_KEY_ROTATION_DELEGATION`       | `to_recover_account` and `recovery_address` do not belong to the same VASP.                       |
/// | `Errors::LIMIT_EXCEEDED`   | ` RecoveryAddress::EMAX_KEYS_REGISTERED`                  | `RecoveryAddress::MAX_REGISTERED_KEYS` have already been registered with this `recovery_address`. |
///
/// # Related Scripts
/// * `Script::create_recovery_address`
/// * `Script::rotate_authentication_key_with_recovery_address`
Script encode_add_recovery_rotation_capability_script(AccountAddress recovery_address);

/// # Summary
/// Stores the sending accounts ability to rotate its authentication key with a designated recovery
/// account. Both the sending and recovery accounts need to belong to the same VASP and
/// both be VASP accounts. After this transaction both the sending account and the
/// specified recovery account can rotate the sender account's authentication key.
///
/// # Technical Description
/// Adds the `DiemAccount::KeyRotationCapability` for the sending account
/// (`to_recover_account`) to the `RecoveryAddress::RecoveryAddress` resource under
/// `recovery_address`. After this transaction has been executed successfully the account at
/// `recovery_address` and the `to_recover_account` may rotate the authentication key of
/// `to_recover_account` (the sender of this transaction).
///
/// The sending account of this transaction (`to_recover_account`) must not have previously given away its unique key
/// rotation capability, and must be a VASP account. The account at `recovery_address`
/// must also be a VASP account belonging to the same VASP as the `to_recover_account`.
/// Additionally the account at `recovery_address` must have already initialized itself as
/// a recovery account address using the `AccountAdministrationScripts::create_recovery_address` transaction script.
///
/// The sending account's (`to_recover_account`) key rotation capability is
/// removed in this transaction and stored in the `RecoveryAddress::RecoveryAddress`
/// resource stored under the account at `recovery_address`.
///
/// # Parameters
/// | Name                 | Type      | Description                                                                                               |
/// | ------               | ------    | -------------                                                                                             |
/// | `to_recover_account` | `signer`  | The signer of the sending account of this transaction.                                                    |
/// | `recovery_address`   | `address` | The account address where the `to_recover_account`'s `DiemAccount::KeyRotationCapability` will be stored. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                              | Description                                                                                       |
/// | ----------------           | --------------                                            | -------------                                                                                     |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `to_recover_account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.    |
/// | `Errors::NOT_PUBLISHED`    | `RecoveryAddress::ERECOVERY_ADDRESS`                      | `recovery_address` does not have a `RecoveryAddress` resource published under it.                 |
/// | `Errors::INVALID_ARGUMENT` | `RecoveryAddress::EINVALID_KEY_ROTATION_DELEGATION`       | `to_recover_account` and `recovery_address` do not belong to the same VASP.                       |
/// | `Errors::LIMIT_EXCEEDED`   | ` RecoveryAddress::EMAX_KEYS_REGISTERED`                  | `RecoveryAddress::MAX_REGISTERED_KEYS` have already been registered with this `recovery_address`. |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::create_recovery_address`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_recovery_address`
TransactionPayload encode_add_recovery_rotation_capability_script_function(AccountAddress recovery_address);

/// # Summary
/// Adds a validator account to the validator set, and triggers a
/// reconfiguration of the system to admit the account to the validator set for the system. This
/// transaction can only be successfully called by the Diem Root account.
///
/// # Technical Description
/// This script adds the account at `validator_address` to the validator set.
/// This transaction emits a `DiemConfig::NewEpochEvent` event and triggers a
/// reconfiguration. Once the reconfiguration triggered by this script's
/// execution has been performed, the account at the `validator_address` is
/// considered to be a validator in the network.
///
/// This transaction script will fail if the `validator_address` address is already in the validator set
/// or does not have a `ValidatorConfig::ValidatorConfig` resource already published under it.
///
/// # Parameters
/// | Name                | Type         | Description                                                                                                                        |
/// | ------              | ------       | -------------                                                                                                                      |
/// | `dr_account`        | `&signer`    | The signer reference of the sending account of this transaction. Must be the Diem Root signer.                                    |
/// | `sliding_nonce`     | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                         |
/// | `validator_name`    | `vector<u8>` | ASCII-encoded human name for the validator. Must match the human name in the `ValidatorConfig::ValidatorConfig` for the validator. |
/// | `validator_address` | `address`    | The validator account address to be added to the validator set.                                                                    |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                  | Description                                                                                                                               |
/// | ----------------           | --------------                                | -------------                                                                                                                             |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                | A `SlidingNonce` resource is not published under `dr_account`.                                                                            |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.                                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                | The `sliding_nonce` is too far in the future.                                                                                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`       | The `sliding_nonce` has been previously recorded.                                                                                         |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                  | The sending account is not the Diem Root account.                                                                                        |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EDIEM_ROOT`                          | The sending account is not the Diem Root account.                                                                                        |
/// | 0                          | 0                                             | The provided `validator_name` does not match the already-recorded human name for the validator.                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemSystem::EINVALID_PROSPECTIVE_VALIDATOR` | The validator to be added does not have a `ValidatorConfig::ValidatorConfig` resource published under it, or its `config` field is empty. |
/// | `Errors::INVALID_ARGUMENT` | `DiemSystem::EALREADY_A_VALIDATOR`           | The `validator_address` account is already a registered validator.                                                                        |
/// | `Errors::INVALID_STATE`    | `DiemConfig::EINVALID_BLOCK_TIME`            | An invalid time value was encountered in reconfiguration. Unlikely to occur.                                                              |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::create_validator_operator_account`
/// * `Script::register_validator_config`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_add_validator_and_reconfigure_script(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address);

/// # Summary
/// Adds a validator account to the validator set, and triggers a
/// reconfiguration of the system to admit the account to the validator set for the system. This
/// transaction can only be successfully called by the Diem Root account.
///
/// # Technical Description
/// This script adds the account at `validator_address` to the validator set.
/// This transaction emits a `DiemConfig::NewEpochEvent` event and triggers a
/// reconfiguration. Once the reconfiguration triggered by this script's
/// execution has been performed, the account at the `validator_address` is
/// considered to be a validator in the network.
///
/// This transaction script will fail if the `validator_address` address is already in the validator set
/// or does not have a `ValidatorConfig::ValidatorConfig` resource already published under it.
///
/// # Parameters
/// | Name                | Type         | Description                                                                                                                        |
/// | ------              | ------       | -------------                                                                                                                      |
/// | `dr_account`        | `signer`     | The signer of the sending account of this transaction. Must be the Diem Root signer.                                               |
/// | `sliding_nonce`     | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                         |
/// | `validator_name`    | `vector<u8>` | ASCII-encoded human name for the validator. Must match the human name in the `ValidatorConfig::ValidatorConfig` for the validator. |
/// | `validator_address` | `address`    | The validator account address to be added to the validator set.                                                                    |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                 | Description                                                                                                                               |
/// | ----------------           | --------------                               | -------------                                                                                                                             |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`               | A `SlidingNonce` resource is not published under `dr_account`.                                                                            |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`               | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.                                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`               | The `sliding_nonce` is too far in the future.                                                                                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`      | The `sliding_nonce` has been previously recorded.                                                                                         |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                  | The sending account is not the Diem Root account.                                                                                         |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EDIEM_ROOT`                          | The sending account is not the Diem Root account.                                                                                         |
/// | 0                          | 0                                            | The provided `validator_name` does not match the already-recorded human name for the validator.                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemSystem::EINVALID_PROSPECTIVE_VALIDATOR` | The validator to be added does not have a `ValidatorConfig::ValidatorConfig` resource published under it, or its `config` field is empty. |
/// | `Errors::INVALID_ARGUMENT` | `DiemSystem::EALREADY_A_VALIDATOR`           | The `validator_address` account is already a registered validator.                                                                        |
/// | `Errors::INVALID_STATE`    | `DiemConfig::EINVALID_BLOCK_TIME`            | An invalid time value was encountered in reconfiguration. Unlikely to occur.                                                              |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemSystem::EMAX_VALIDATORS`                | The validator set is already at its maximum size. The validator could not be added.                                                       |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::register_validator_config`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_add_validator_and_reconfigure_script_function(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address);

/// # Summary
/// Add a VASP domain to parent VASP account. The transaction can only be sent by
/// the Treasury Compliance account.
///
/// # Technical Description
/// Adds a `VASPDomain::VASPDomain` to the `domains` field of the `VASPDomain::VASPDomains` resource published under
/// the account at `address`.
///
/// # Parameters
/// | Name         | Type         | Description                                                                                     |
/// | ------       | ------       | -------------                                                                                   |
/// | `tc_account` | `signer`     | The signer of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `address`    | `address`    | The `address` of the parent VASP account that will have have `domain` added to its domains.     |
/// | `domain`     | `vector<u8>` | The domain to be added.                                                                         |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                             | Description                                                                                                                            |
/// | ----------------           | --------------                           | -------------                                                                                                                          |
/// | `Errors::REQUIRES_ROLE`    | `Roles::ETREASURY_COMPLIANCE`            | The sending account is not the Treasury Compliance account.                                                                            |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`    | `tc_account` is not the Treasury Compliance account.                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `VASPDomain::EVASP_DOMAIN_MANAGER`        | The `VASPDomain::VASPDomainManager` resource is not yet published under the Treasury Compliance account.                                 |
/// | `Errors::NOT_PUBLISHED`    | `VASPDomain::EVASP_DOMAINS_NOT_PUBLISHED` | `address` does not have a `VASPDomain::VASPDomains` resource published under it.                                                         |
/// | `Errors::INVALID_ARGUMENT` | `VASPDomain::EDOMAIN_ALREADY_EXISTS`         | The `domain` already exists in the list of `VASPDomain::VASPDomain`s  in the `VASPDomain::VASPDomains` resource published under `address`. |
/// | `Errors::INVALID_ARGUMENT` | `VASPDomain::EINVALID_VASP_DOMAIN`        | The `domain` is greater in length than `VASPDomain::DOMAIN_LENGTH`.                                                                        |
TransactionPayload encode_add_vasp_domain_script_function(AccountAddress address, std::vector<uint8_t> domain);

/// # Summary
/// Burns all coins held in the preburn resource at the specified
/// preburn address and removes them from the system. The sending account must
/// be the Treasury Compliance account.
/// The account that holds the preburn resource will normally be a Designated
/// Dealer, but there are no enforced requirements that it be one.
///
/// # Technical Description
/// This transaction permanently destroys all the coins of `Token` type
/// stored in the `Diem::Preburn<Token>` resource published under the
/// `preburn_address` account address.
///
/// This transaction will only succeed if the sending `account` has a
/// `Diem::BurnCapability<Token>`, and a `Diem::Preburn<Token>` resource
/// exists under `preburn_address`, with a non-zero `to_burn` field. After the successful execution
/// of this transaction the `total_value` field in the
/// `Diem::CurrencyInfo<Token>` resource published under `0xA550C18` will be
/// decremented by the value of the `to_burn` field of the preburn resource
/// under `preburn_address` immediately before this transaction, and the
/// `to_burn` field of the preburn resource will have a zero value.
///
/// ## Events
/// The successful execution of this transaction will emit a `Diem::BurnEvent` on the event handle
/// held in the `Diem::CurrencyInfo<Token>` resource's `burn_events` published under
/// `0xA550C18`.
///
/// # Parameters
/// | Name              | Type      | Description                                                                                                                  |
/// | ------            | ------    | -------------                                                                                                                |
/// | `Token`           | Type      | The Move type for the `Token` currency being burned. `Token` must be an already-registered currency on-chain.                |
/// | `tc_account`      | `&signer` | The signer reference of the sending account of this transaction, must have a burn capability for `Token` published under it. |
/// | `sliding_nonce`   | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                   |
/// | `preburn_address` | `address` | The address where the coins to-be-burned are currently held.                                                                 |
///
/// # Common Abort Conditions
/// | Error Category                | Error Reason                            | Description                                                                                           |
/// | ----------------              | --------------                          | -------------                                                                                         |
/// | `Errors::NOT_PUBLISHED`       | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `account`.                                           |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.            |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                                         |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                                     |
/// | `Errors::REQUIRES_CAPABILITY` | `Diem::EBURN_CAPABILITY`               | The sending `account` does not have a `Diem::BurnCapability<Token>` published under it.              |
/// | `Errors::NOT_PUBLISHED`       | `Diem::EPREBURN`                       | The account at `preburn_address` does not have a `Diem::Preburn<Token>` resource published under it. |
/// | `Errors::INVALID_STATE`       | `Diem::EPREBURN_EMPTY`                 | The `Diem::Preburn<Token>` resource is empty (has a value of 0).                                     |
/// | `Errors::NOT_PUBLISHED`       | `Diem::ECURRENCY_INFO`                 | The specified `Token` is not a registered currency on-chain.                                          |
///
/// # Related Scripts
/// * `Script::burn_txn_fees`
/// * `Script::cancel_burn`
/// * `Script::preburn`
Script encode_burn_script(TypeTag token, uint64_t sliding_nonce, AccountAddress preburn_address);

/// # Summary
/// Burns the transaction fees collected in the `CoinType` currency so that the
/// Diem association may reclaim the backing coins off-chain. May only be sent
/// by the Treasury Compliance account.
///
/// # Technical Description
/// Burns the transaction fees collected in `CoinType` so that the
/// association may reclaim the backing coins. Once this transaction has executed
/// successfully all transaction fees that will have been collected in
/// `CoinType` since the last time this script was called with that specific
/// currency. Both `balance` and `preburn` fields in the
/// `TransactionFee::TransactionFee<CoinType>` resource published under the `0xB1E55ED`
/// account address will have a value of 0 after the successful execution of this script.
///
/// ## Events
/// The successful execution of this transaction will emit a `Diem::BurnEvent` on the event handle
/// held in the `Diem::CurrencyInfo<CoinType>` resource's `burn_events` published under
/// `0xA550C18`.
///
/// # Parameters
/// | Name         | Type      | Description                                                                                                                                         |
/// | ------       | ------    | -------------                                                                                                                                       |
/// | `CoinType`   | Type      | The Move type for the `CoinType` being added to the sending account of the transaction. `CoinType` must be an already-registered currency on-chain. |
/// | `tc_account` | `&signer` | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account.                                           |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                          | Description                                                 |
/// | ----------------           | --------------                        | -------------                                               |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE` | The sending account is not the Treasury Compliance account. |
/// | `Errors::NOT_PUBLISHED`    | `TransactionFee::ETRANSACTION_FEE`    | `CoinType` is not an accepted transaction fee currency.     |
/// | `Errors::INVALID_ARGUMENT` | `Diem::ECOIN`                        | The collected fees in `CoinType` are zero.                  |
///
/// # Related Scripts
/// * `Script::burn`
/// * `Script::cancel_burn`
Script encode_burn_txn_fees_script(TypeTag coin_type);

/// # Summary
/// Burns the transaction fees collected in the `CoinType` currency so that the
/// Diem association may reclaim the backing coins off-chain. May only be sent
/// by the Treasury Compliance account.
///
/// # Technical Description
/// Burns the transaction fees collected in `CoinType` so that the
/// association may reclaim the backing coins. Once this transaction has executed
/// successfully all transaction fees that will have been collected in
/// `CoinType` since the last time this script was called with that specific
/// currency. Both `balance` and `preburn` fields in the
/// `TransactionFee::TransactionFee<CoinType>` resource published under the `0xB1E55ED`
/// account address will have a value of 0 after the successful execution of this script.
///
/// # Events
/// The successful execution of this transaction will emit a `Diem::BurnEvent` on the event handle
/// held in the `Diem::CurrencyInfo<CoinType>` resource's `burn_events` published under
/// `0xA550C18`.
///
/// # Parameters
/// | Name         | Type     | Description                                                                                                                                         |
/// | ------       | ------   | -------------                                                                                                                                       |
/// | `CoinType`   | Type     | The Move type for the `CoinType` being added to the sending account of the transaction. `CoinType` must be an already-registered currency on-chain. |
/// | `tc_account` | `signer` | The signer of the sending account of this transaction. Must be the Treasury Compliance account.                                                     |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                          | Description                                                 |
/// | ----------------           | --------------                        | -------------                                               |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE` | The sending account is not the Treasury Compliance account. |
/// | `Errors::NOT_PUBLISHED`    | `TransactionFee::ETRANSACTION_FEE`    | `CoinType` is not an accepted transaction fee currency.     |
/// | `Errors::INVALID_ARGUMENT` | `Diem::ECOIN`                        | The collected fees in `CoinType` are zero.                  |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::burn_with_amount`
/// * `TreasuryComplianceScripts::cancel_burn_with_amount`
TransactionPayload encode_burn_txn_fees_script_function(TypeTag coin_type);

/// # Summary
/// Burns the coins held in a preburn resource in the preburn queue at the
/// specified preburn address, which are equal to the `amount` specified in the
/// transaction. Finds the first relevant outstanding preburn request with
/// matching amount and removes the contained coins from the system. The sending
/// account must be the Treasury Compliance account.
/// The account that holds the preburn queue resource will normally be a Designated
/// Dealer, but there are no enforced requirements that it be one.
///
/// # Technical Description
/// This transaction permanently destroys all the coins of `Token` type
/// stored in the `Diem::Preburn<Token>` resource published under the
/// `preburn_address` account address.
///
/// This transaction will only succeed if the sending `account` has a
/// `Diem::BurnCapability<Token>`, and a `Diem::Preburn<Token>` resource
/// exists under `preburn_address`, with a non-zero `to_burn` field. After the successful execution
/// of this transaction the `total_value` field in the
/// `Diem::CurrencyInfo<Token>` resource published under `0xA550C18` will be
/// decremented by the value of the `to_burn` field of the preburn resource
/// under `preburn_address` immediately before this transaction, and the
/// `to_burn` field of the preburn resource will have a zero value.
///
/// # Events
/// The successful execution of this transaction will emit a `Diem::BurnEvent` on the event handle
/// held in the `Diem::CurrencyInfo<Token>` resource's `burn_events` published under
/// `0xA550C18`.
///
/// # Parameters
/// | Name              | Type      | Description                                                                                                        |
/// | ------            | ------    | -------------                                                                                                      |
/// | `Token`           | Type      | The Move type for the `Token` currency being burned. `Token` must be an already-registered currency on-chain.      |
/// | `tc_account`      | `signer`  | The signer of the sending account of this transaction, must have a burn capability for `Token` published under it. |
/// | `sliding_nonce`   | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                         |
/// | `preburn_address` | `address` | The address where the coins to-be-burned are currently held.                                                       |
/// | `amount`          | `u64`     | The amount to be burned.                                                                                           |
///
/// # Common Abort Conditions
/// | Error Category                | Error Reason                            | Description                                                                                                                         |
/// | ----------------              | --------------                          | -------------                                                                                                                       |
/// | `Errors::NOT_PUBLISHED`       | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `account`.                                                                         |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.                                          |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                                                                       |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                                                                   |
/// | `Errors::REQUIRES_CAPABILITY` | `Diem::EBURN_CAPABILITY`                | The sending `account` does not have a `Diem::BurnCapability<Token>` published under it.                                             |
/// | `Errors::INVALID_STATE`       | `Diem::EPREBURN_NOT_FOUND`              | The `Diem::PreburnQueue<Token>` resource under `preburn_address` does not contain a preburn request with a value matching `amount`. |
/// | `Errors::NOT_PUBLISHED`       | `Diem::EPREBURN_QUEUE`                  | The account at `preburn_address` does not have a `Diem::PreburnQueue<Token>` resource published under it.                           |
/// | `Errors::NOT_PUBLISHED`       | `Diem::ECURRENCY_INFO`                  | The specified `Token` is not a registered currency on-chain.                                                                        |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::burn_txn_fees`
/// * `TreasuryComplianceScripts::cancel_burn_with_amount`
/// * `TreasuryComplianceScripts::preburn`
TransactionPayload encode_burn_with_amount_script_function(TypeTag token, uint64_t sliding_nonce, AccountAddress preburn_address, uint64_t amount);

/// # Summary
/// Cancels and returns all coins held in the preburn area under
/// `preburn_address` and returns the funds to the `preburn_address`'s balance.
/// Can only be successfully sent by an account with Treasury Compliance role.
///
/// # Technical Description
/// Cancels and returns all coins held in the `Diem::Preburn<Token>` resource under the `preburn_address` and
/// return the funds to the `preburn_address` account's `DiemAccount::Balance<Token>`.
/// The transaction must be sent by an `account` with a `Diem::BurnCapability<Token>`
/// resource published under it. The account at `preburn_address` must have a
/// `Diem::Preburn<Token>` resource published under it, and its value must be nonzero. The transaction removes
/// the entire balance held in the `Diem::Preburn<Token>` resource, and returns it back to the account's
/// `DiemAccount::Balance<Token>` under `preburn_address`. Due to this, the account at
/// `preburn_address` must already have a balance in the `Token` currency published
/// before this script is called otherwise the transaction will fail.
///
/// ## Events
/// The successful execution of this transaction will emit:
/// * A `Diem::CancelBurnEvent` on the event handle held in the `Diem::CurrencyInfo<Token>`
/// resource's `burn_events` published under `0xA550C18`.
/// * A `DiemAccount::ReceivedPaymentEvent` on the `preburn_address`'s
/// `DiemAccount::DiemAccount` `received_events` event handle with both the `payer` and `payee`
/// being `preburn_address`.
///
/// # Parameters
/// | Name              | Type      | Description                                                                                                                          |
/// | ------            | ------    | -------------                                                                                                                        |
/// | `Token`           | Type      | The Move type for the `Token` currenty that burning is being cancelled for. `Token` must be an already-registered currency on-chain. |
/// | `account`         | `&signer` | The signer reference of the sending account of this transaction, must have a burn capability for `Token` published under it.         |
/// | `preburn_address` | `address` | The address where the coins to-be-burned are currently held.                                                                         |
///
/// # Common Abort Conditions
/// | Error Category                | Error Reason                                     | Description                                                                                           |
/// | ----------------              | --------------                                   | -------------                                                                                         |
/// | `Errors::REQUIRES_CAPABILITY` | `Diem::EBURN_CAPABILITY`                        | The sending `account` does not have a `Diem::BurnCapability<Token>` published under it.              |
/// | `Errors::NOT_PUBLISHED`       | `Diem::EPREBURN`                                | The account at `preburn_address` does not have a `Diem::Preburn<Token>` resource published under it. |
/// | `Errors::NOT_PUBLISHED`       | `Diem::ECURRENCY_INFO`                          | The specified `Token` is not a registered currency on-chain.                                          |
/// | `Errors::INVALID_ARGUMENT`    | `DiemAccount::ECOIN_DEPOSIT_IS_ZERO`            | The value held in the preburn resource was zero.                                                      |
/// | `Errors::INVALID_ARGUMENT`    | `DiemAccount::EPAYEE_CANT_ACCEPT_CURRENCY_TYPE` | The account at `preburn_address` doesn't have a balance resource for `Token`.                         |
/// | `Errors::LIMIT_EXCEEDED`      | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`          | The depositing of the funds held in the prebun area would exceed the `account`'s account limits.      |
/// | `Errors::INVALID_STATE`       | `DualAttestation::EPAYEE_COMPLIANCE_KEY_NOT_SET` | The `account` does not have a compliance key set on it but dual attestion checking was performed.     |
///
/// # Related Scripts
/// * `Script::burn_txn_fees`
/// * `Script::burn`
/// * `Script::preburn`
Script encode_cancel_burn_script(TypeTag token, AccountAddress preburn_address);

/// # Summary
/// Cancels and returns the coins held in the preburn area under
/// `preburn_address`, which are equal to the `amount` specified in the transaction. Finds the first preburn
/// resource with the matching amount and returns the funds to the `preburn_address`'s balance.
/// Can only be successfully sent by an account with Treasury Compliance role.
///
/// # Technical Description
/// Cancels and returns all coins held in the `Diem::Preburn<Token>` resource under the `preburn_address` and
/// return the funds to the `preburn_address` account's `DiemAccount::Balance<Token>`.
/// The transaction must be sent by an `account` with a `Diem::BurnCapability<Token>`
/// resource published under it. The account at `preburn_address` must have a
/// `Diem::Preburn<Token>` resource published under it, and its value must be nonzero. The transaction removes
/// the entire balance held in the `Diem::Preburn<Token>` resource, and returns it back to the account's
/// `DiemAccount::Balance<Token>` under `preburn_address`. Due to this, the account at
/// `preburn_address` must already have a balance in the `Token` currency published
/// before this script is called otherwise the transaction will fail.
///
/// # Events
/// The successful execution of this transaction will emit:
/// * A `Diem::CancelBurnEvent` on the event handle held in the `Diem::CurrencyInfo<Token>`
/// resource's `burn_events` published under `0xA550C18`.
/// * A `DiemAccount::ReceivedPaymentEvent` on the `preburn_address`'s
/// `DiemAccount::DiemAccount` `received_events` event handle with both the `payer` and `payee`
/// being `preburn_address`.
///
/// # Parameters
/// | Name              | Type      | Description                                                                                                                          |
/// | ------            | ------    | -------------                                                                                                                        |
/// | `Token`           | Type      | The Move type for the `Token` currenty that burning is being cancelled for. `Token` must be an already-registered currency on-chain. |
/// | `account`         | `signer`  | The signer of the sending account of this transaction, must have a burn capability for `Token` published under it.                   |
/// | `preburn_address` | `address` | The address where the coins to-be-burned are currently held.                                                                         |
/// | `amount`          | `u64`     | The amount to be cancelled.                                                                                                          |
///
/// # Common Abort Conditions
/// | Error Category                | Error Reason                                     | Description                                                                                                                         |
/// | ----------------              | --------------                                   | -------------                                                                                                                       |
/// | `Errors::REQUIRES_CAPABILITY` | `Diem::EBURN_CAPABILITY`                         | The sending `account` does not have a `Diem::BurnCapability<Token>` published under it.                                             |
/// | `Errors::INVALID_STATE`       | `Diem::EPREBURN_NOT_FOUND`                       | The `Diem::PreburnQueue<Token>` resource under `preburn_address` does not contain a preburn request with a value matching `amount`. |
/// | `Errors::NOT_PUBLISHED`       | `Diem::EPREBURN_QUEUE`                           | The account at `preburn_address` does not have a `Diem::PreburnQueue<Token>` resource published under it.                           |
/// | `Errors::NOT_PUBLISHED`       | `Diem::ECURRENCY_INFO`                           | The specified `Token` is not a registered currency on-chain.                                                                        |
/// | `Errors::INVALID_ARGUMENT`    | `DiemAccount::EPAYEE_CANT_ACCEPT_CURRENCY_TYPE`  | The account at `preburn_address` doesn't have a balance resource for `Token`.                                                       |
/// | `Errors::LIMIT_EXCEEDED`      | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`           | The depositing of the funds held in the prebun area would exceed the `account`'s account limits.                                    |
/// | `Errors::INVALID_STATE`       | `DualAttestation::EPAYEE_COMPLIANCE_KEY_NOT_SET` | The `account` does not have a compliance key set on it but dual attestion checking was performed.                                   |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::burn_txn_fees`
/// * `TreasuryComplianceScripts::burn_with_amount`
/// * `TreasuryComplianceScripts::preburn`
TransactionPayload encode_cancel_burn_with_amount_script_function(TypeTag token, AccountAddress preburn_address, uint64_t amount);

/// # Summary
/// Creates a Child VASP account with its parent being the sending account of the transaction.
/// The sender of the transaction must be a Parent VASP account.
///
/// # Technical Description
/// Creates a `ChildVASP` account for the sender `parent_vasp` at `child_address` with a balance of
/// `child_initial_balance` in `CoinType` and an initial authentication key of
/// `auth_key_prefix | child_address`.
///
/// If `add_all_currencies` is true, the child address will have a zero balance in all available
/// currencies in the system.
///
/// The new account will be a child account of the transaction sender, which must be a
/// Parent VASP account. The child account will be recorded against the limit of
/// child accounts of the creating Parent VASP account.
///
/// ## Events
/// Successful execution with a `child_initial_balance` greater than zero will emit:
/// * A `DiemAccount::SentPaymentEvent` with the `payer` field being the Parent VASP's address,
/// and payee field being `child_address`. This is emitted on the Parent VASP's
/// `DiemAccount::DiemAccount` `sent_events` handle.
/// * A `DiemAccount::ReceivedPaymentEvent` with the  `payer` field being the Parent VASP's address,
/// and payee field being `child_address`. This is emitted on the new Child VASPS's
/// `DiemAccount::DiemAccount` `received_events` handle.
///
/// # Parameters
/// | Name                    | Type         | Description                                                                                                                                 |
/// | ------                  | ------       | -------------                                                                                                                               |
/// | `CoinType`              | Type         | The Move type for the `CoinType` that the child account should be created with. `CoinType` must be an already-registered currency on-chain. |
/// | `parent_vasp`           | `&signer`    | The signer reference of the sending account. Must be a Parent VASP account.                                                                 |
/// | `child_address`         | `address`    | Address of the to-be-created Child VASP account.                                                                                            |
/// | `auth_key_prefix`       | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.                                                    |
/// | `add_all_currencies`    | `bool`       | Whether to publish balance resources for all known currencies when the account is created.                                                  |
/// | `child_initial_balance` | `u64`        | The initial balance in `CoinType` to give the child account when it's created.                                                              |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                                             | Description                                                                              |
/// | ----------------            | --------------                                           | -------------                                                                            |
/// | `Errors::INVALID_ARGUMENT`  | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`            | The `auth_key_prefix` was not of length 32.                                              |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EPARENT_VASP`                                    | The sending account wasn't a Parent VASP account.                                        |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                                        | The `child_address` address is already taken.                                            |
/// | `Errors::LIMIT_EXCEEDED`    | `VASP::ETOO_MANY_CHILDREN`                               | The sending account has reached the maximum number of allowed child accounts.            |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                                  | The `CoinType` is not a registered currency on-chain.                                    |
/// | `Errors::INVALID_STATE`     | `DiemAccount::EWITHDRAWAL_CAPABILITY_ALREADY_EXTRACTED` | The withdrawal capability for the sending account has already been extracted.            |
/// | `Errors::NOT_PUBLISHED`     | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`              | The sending account doesn't have a balance in `CoinType`.                                |
/// | `Errors::LIMIT_EXCEEDED`    | `DiemAccount::EINSUFFICIENT_BALANCE`                    | The sending account doesn't have at least `child_initial_balance` of `CoinType` balance. |
/// | `Errors::INVALID_ARGUMENT`  | `DiemAccount::ECANNOT_CREATE_AT_VM_RESERVED`            | The `child_address` is the reserved address 0x0.                                         |
///
/// # Related Scripts
/// * `Script::create_parent_vasp_account`
/// * `Script::add_currency_to_account`
/// * `Script::rotate_authentication_key`
/// * `Script::add_recovery_rotation_capability`
/// * `Script::create_recovery_address`
Script encode_create_child_vasp_account_script(TypeTag coin_type, AccountAddress child_address, std::vector<uint8_t> auth_key_prefix, bool add_all_currencies, uint64_t child_initial_balance);

/// # Summary
/// Creates a Child VASP account with its parent being the sending account of the transaction.
/// The sender of the transaction must be a Parent VASP account.
///
/// # Technical Description
/// Creates a `ChildVASP` account for the sender `parent_vasp` at `child_address` with a balance of
/// `child_initial_balance` in `CoinType` and an initial authentication key of
/// `auth_key_prefix | child_address`. Authentication key prefixes, and how to construct them from an ed25519 public key is described
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys).
///
/// If `add_all_currencies` is true, the child address will have a zero balance in all available
/// currencies in the system.
///
/// The new account will be a child account of the transaction sender, which must be a
/// Parent VASP account. The child account will be recorded against the limit of
/// child accounts of the creating Parent VASP account.
///
/// # Events
/// Successful execution will emit:
/// * A `DiemAccount::CreateAccountEvent` with the `created` field being `child_address`,
/// and the `rold_id` field being `Roles::CHILD_VASP_ROLE_ID`. This is emitted on the
/// `DiemAccount::AccountOperationsCapability` `creation_events` handle.
///
/// Successful execution with a `child_initial_balance` greater than zero will additionaly emit:
/// * A `DiemAccount::SentPaymentEvent` with the `payee` field being `child_address`.
/// This is emitted on the Parent VASP's `DiemAccount::DiemAccount` `sent_events` handle.
/// * A `DiemAccount::ReceivedPaymentEvent` with the  `payer` field being the Parent VASP's address.
/// This is emitted on the new Child VASPS's `DiemAccount::DiemAccount` `received_events` handle.
///
/// # Parameters
/// | Name                    | Type         | Description                                                                                                                                 |
/// | ------                  | ------       | -------------                                                                                                                               |
/// | `CoinType`              | Type         | The Move type for the `CoinType` that the child account should be created with. `CoinType` must be an already-registered currency on-chain. |
/// | `parent_vasp`           | `signer`     | The reference of the sending account. Must be a Parent VASP account.                                                                        |
/// | `child_address`         | `address`    | Address of the to-be-created Child VASP account.                                                                                            |
/// | `auth_key_prefix`       | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.                                                    |
/// | `add_all_currencies`    | `bool`       | Whether to publish balance resources for all known currencies when the account is created.                                                  |
/// | `child_initial_balance` | `u64`        | The initial balance in `CoinType` to give the child account when it's created.                                                              |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                                             | Description                                                                              |
/// | ----------------            | --------------                                           | -------------                                                                            |
/// | `Errors::INVALID_ARGUMENT`  | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`            | The `auth_key_prefix` was not of length 32.                                              |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EPARENT_VASP`                                    | The sending account wasn't a Parent VASP account.                                        |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                                        | The `child_address` address is already taken.                                            |
/// | `Errors::LIMIT_EXCEEDED`    | `VASP::ETOO_MANY_CHILDREN`                               | The sending account has reached the maximum number of allowed child accounts.            |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                                  | The `CoinType` is not a registered currency on-chain.                                    |
/// | `Errors::INVALID_STATE`     | `DiemAccount::EWITHDRAWAL_CAPABILITY_ALREADY_EXTRACTED` | The withdrawal capability for the sending account has already been extracted.            |
/// | `Errors::NOT_PUBLISHED`     | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`              | The sending account doesn't have a balance in `CoinType`.                                |
/// | `Errors::LIMIT_EXCEEDED`    | `DiemAccount::EINSUFFICIENT_BALANCE`                    | The sending account doesn't have at least `child_initial_balance` of `CoinType` balance. |
/// | `Errors::INVALID_ARGUMENT`  | `DiemAccount::ECANNOT_CREATE_AT_VM_RESERVED`            | The `child_address` is the reserved address 0x0.                                         |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_parent_vasp_account`
/// * `AccountAdministrationScripts::add_currency_to_account`
/// * `AccountAdministrationScripts::rotate_authentication_key`
/// * `AccountAdministrationScripts::add_recovery_rotation_capability`
/// * `AccountAdministrationScripts::create_recovery_address`
TransactionPayload encode_create_child_vasp_account_script_function(TypeTag coin_type, AccountAddress child_address, std::vector<uint8_t> auth_key_prefix, bool add_all_currencies, uint64_t child_initial_balance);

/// # Summary
/// Creates a Designated Dealer account with the provided information, and initializes it with
/// default mint tiers. The transaction can only be sent by the Treasury Compliance account.
///
/// # Technical Description
/// Creates an account with the Designated Dealer role at `addr` with authentication key
/// `auth_key_prefix` | `addr` and a 0 balance of type `Currency`. If `add_all_currencies` is true,
/// 0 balances for all available currencies in the system will also be added. This can only be
/// invoked by an account with the TreasuryCompliance role.
///
/// At the time of creation the account is also initialized with default mint tiers of (500_000,
/// 5000_000, 50_000_000, 500_000_000), and preburn areas for each currency that is added to the
/// account.
///
/// # Parameters
/// | Name                 | Type         | Description                                                                                                                                         |
/// | ------               | ------       | -------------                                                                                                                                       |
/// | `Currency`           | Type         | The Move type for the `Currency` that the Designated Dealer should be initialized with. `Currency` must be an already-registered currency on-chain. |
/// | `tc_account`         | `&signer`    | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account.                                           |
/// | `sliding_nonce`      | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                                          |
/// | `addr`               | `address`    | Address of the to-be-created Designated Dealer account.                                                                                             |
/// | `auth_key_prefix`    | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.                                                            |
/// | `human_name`         | `vector<u8>` | ASCII-encoded human name for the Designated Dealer.                                                                                                 |
/// | `add_all_currencies` | `bool`       | Whether to publish preburn, balance, and tier info resources for all known (SCS) currencies or just `Currency` when the account is created.         |
///

/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::ETREASURY_COMPLIANCE`   | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::REQUIRES_ROLE`     | `Roles::ETREASURY_COMPLIANCE`           | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                 | The `Currency` is not a registered currency on-chain.                                      |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `addr` address is already taken.                                                       |
///
/// # Related Scripts
/// * `Script::tiered_mint`
/// * `Script::peer_to_peer_with_metadata`
/// * `Script::rotate_dual_attestation_info`
Script encode_create_designated_dealer_script(TypeTag currency, uint64_t sliding_nonce, AccountAddress addr, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies);

/// # Summary
/// Creates a Designated Dealer account with the provided information, and initializes it with
/// default mint tiers. The transaction can only be sent by the Treasury Compliance account.
///
/// # Technical Description
/// Creates an account with the Designated Dealer role at `addr` with authentication key
/// `auth_key_prefix` | `addr` and a 0 balance of type `Currency`. If `add_all_currencies` is true,
/// 0 balances for all available currencies in the system will also be added. This can only be
/// invoked by an account with the TreasuryCompliance role.
/// Authentication keys, prefixes, and how to construct them from an ed25519 public key are described
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys).
///
/// At the time of creation the account is also initialized with default mint tiers of (500_000,
/// 5000_000, 50_000_000, 500_000_000), and preburn areas for each currency that is added to the
/// account.
///
/// # Events
/// Successful execution will emit:
/// * A `DiemAccount::CreateAccountEvent` with the `created` field being `addr`,
/// and the `rold_id` field being `Roles::DESIGNATED_DEALER_ROLE_ID`. This is emitted on the
/// `DiemAccount::AccountOperationsCapability` `creation_events` handle.
///
/// # Parameters
/// | Name                 | Type         | Description                                                                                                                                         |
/// | ------               | ------       | -------------                                                                                                                                       |
/// | `Currency`           | Type         | The Move type for the `Currency` that the Designated Dealer should be initialized with. `Currency` must be an already-registered currency on-chain. |
/// | `tc_account`         | `signer`     | The signer of the sending account of this transaction. Must be the Treasury Compliance account.                                                     |
/// | `sliding_nonce`      | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                                          |
/// | `addr`               | `address`    | Address of the to-be-created Designated Dealer account.                                                                                             |
/// | `auth_key_prefix`    | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.                                                            |
/// | `human_name`         | `vector<u8>` | ASCII-encoded human name for the Designated Dealer.                                                                                                 |
/// | `add_all_currencies` | `bool`       | Whether to publish preburn, balance, and tier info resources for all known (SCS) currencies or just `Currency` when the account is created.         |
///

/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::ETREASURY_COMPLIANCE`   | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::REQUIRES_ROLE`     | `Roles::ETREASURY_COMPLIANCE`           | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                 | The `Currency` is not a registered currency on-chain.                                      |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `addr` address is already taken.                                                       |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::tiered_mint`
/// * `PaymentScripts::peer_to_peer_with_metadata`
/// * `AccountAdministrationScripts::rotate_dual_attestation_info`
TransactionPayload encode_create_designated_dealer_script_function(TypeTag currency, uint64_t sliding_nonce, AccountAddress addr, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies);

/// # Summary
/// Creates a Parent VASP account with the specified human name. Must be called by the Treasury Compliance account.
///
/// # Technical Description
/// Creates an account with the Parent VASP role at `address` with authentication key
/// `auth_key_prefix` | `new_account_address` and a 0 balance of type `CoinType`. If
/// `add_all_currencies` is true, 0 balances for all available currencies in the system will
/// also be added. This can only be invoked by an TreasuryCompliance account.
/// `sliding_nonce` is a unique nonce for operation, see `SlidingNonce` for details.
///
/// # Parameters
/// | Name                  | Type         | Description                                                                                                                                                    |
/// | ------                | ------       | -------------                                                                                                                                                  |
/// | `CoinType`            | Type         | The Move type for the `CoinType` currency that the Parent VASP account should be initialized with. `CoinType` must be an already-registered currency on-chain. |
/// | `tc_account`          | `&signer`    | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account.                                                      |
/// | `sliding_nonce`       | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                                                     |
/// | `new_account_address` | `address`    | Address of the to-be-created Parent VASP account.                                                                                                              |
/// | `auth_key_prefix`     | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.                                                                       |
/// | `human_name`          | `vector<u8>` | ASCII-encoded human name for the Parent VASP.                                                                                                                  |
/// | `add_all_currencies`  | `bool`       | Whether to publish balance resources for all known currencies when the account is created.                                                                     |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::ETREASURY_COMPLIANCE`   | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::REQUIRES_ROLE`     | `Roles::ETREASURY_COMPLIANCE`           | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                 | The `CoinType` is not a registered currency on-chain.                                      |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `new_account_address` address is already taken.                                        |
///
/// # Related Scripts
/// * `Script::create_child_vasp_account`
/// * `Script::add_currency_to_account`
/// * `Script::rotate_authentication_key`
/// * `Script::add_recovery_rotation_capability`
/// * `Script::create_recovery_address`
/// * `Script::rotate_dual_attestation_info`
Script encode_create_parent_vasp_account_script(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies);

/// # Summary
/// Creates a Parent VASP account with the specified human name. Must be called by the Treasury Compliance account.
///
/// # Technical Description
/// Creates an account with the Parent VASP role at `address` with authentication key
/// `auth_key_prefix` | `new_account_address` and a 0 balance of type `CoinType`. If
/// `add_all_currencies` is true, 0 balances for all available currencies in the system will
/// also be added. This can only be invoked by an TreasuryCompliance account.
/// `sliding_nonce` is a unique nonce for operation, see `SlidingNonce` for details.
/// Authentication keys, prefixes, and how to construct them from an ed25519 public key are described
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys).
///
/// # Events
/// Successful execution will emit:
/// * A `DiemAccount::CreateAccountEvent` with the `created` field being `new_account_address`,
/// and the `rold_id` field being `Roles::PARENT_VASP_ROLE_ID`. This is emitted on the
/// `DiemAccount::AccountOperationsCapability` `creation_events` handle.
///
/// # Parameters
/// | Name                  | Type         | Description                                                                                                                                                    |
/// | ------                | ------       | -------------                                                                                                                                                  |
/// | `CoinType`            | Type         | The Move type for the `CoinType` currency that the Parent VASP account should be initialized with. `CoinType` must be an already-registered currency on-chain. |
/// | `tc_account`          | `signer`     | The signer of the sending account of this transaction. Must be the Treasury Compliance account.                                                                |
/// | `sliding_nonce`       | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                                                     |
/// | `new_account_address` | `address`    | Address of the to-be-created Parent VASP account.                                                                                                              |
/// | `auth_key_prefix`     | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.                                                                       |
/// | `human_name`          | `vector<u8>` | ASCII-encoded human name for the Parent VASP.                                                                                                                  |
/// | `add_all_currencies`  | `bool`       | Whether to publish balance resources for all known currencies when the account is created.                                                                     |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::ETREASURY_COMPLIANCE`   | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::REQUIRES_ROLE`     | `Roles::ETREASURY_COMPLIANCE`           | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::NOT_PUBLISHED`     | `Diem::ECURRENCY_INFO`                 | The `CoinType` is not a registered currency on-chain.                                      |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `new_account_address` address is already taken.                                        |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_child_vasp_account`
/// * `AccountAdministrationScripts::add_currency_to_account`
/// * `AccountAdministrationScripts::rotate_authentication_key`
/// * `AccountAdministrationScripts::add_recovery_rotation_capability`
/// * `AccountAdministrationScripts::create_recovery_address`
/// * `AccountAdministrationScripts::rotate_dual_attestation_info`
TransactionPayload encode_create_parent_vasp_account_script_function(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name, bool add_all_currencies);

/// # Summary
/// Initializes the sending account as a recovery address that may be used by
/// the VASP that it belongs to. The sending account must be a VASP account.
/// Multiple recovery addresses can exist for a single VASP, but accounts in
/// each must be disjoint.
///
/// # Technical Description
/// Publishes a `RecoveryAddress::RecoveryAddress` resource under `account`. It then
/// extracts the `DiemAccount::KeyRotationCapability` for `account` and adds
/// it to the resource. After the successful execution of this transaction
/// other accounts may add their key rotation to this resource so that `account`
/// may be used as a recovery account for those accounts.
///
/// # Parameters
/// | Name      | Type      | Description                                           |
/// | ------    | ------    | -------------                                         |
/// | `account` | `&signer` | The signer of the sending account of the transaction. |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                                               | Description                                                                                   |
/// | ----------------            | --------------                                             | -------------                                                                                 |
/// | `Errors::INVALID_STATE`     | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.          |
/// | `Errors::INVALID_ARGUMENT`  | `RecoveryAddress::ENOT_A_VASP`                             | `account` is not a VASP account.                                                              |
/// | `Errors::INVALID_ARGUMENT`  | `RecoveryAddress::EKEY_ROTATION_DEPENDENCY_CYCLE`          | A key rotation recovery cycle would be created by adding `account`'s key rotation capability. |
/// | `Errors::ALREADY_PUBLISHED` | `RecoveryAddress::ERECOVERY_ADDRESS`                       | A `RecoveryAddress::RecoveryAddress` resource has already been published under `account`.     |
///
/// # Related Scripts
/// * `Script::add_recovery_rotation_capability`
/// * `Script::rotate_authentication_key_with_recovery_address`
Script encode_create_recovery_address_script();

/// # Summary
/// Initializes the sending account as a recovery address that may be used by
/// other accounts belonging to the same VASP as `account`.
/// The sending account must be a VASP account, and can be either a child or parent VASP account.
/// Multiple recovery addresses can exist for a single VASP, but accounts in
/// each must be disjoint.
///
/// # Technical Description
/// Publishes a `RecoveryAddress::RecoveryAddress` resource under `account`. It then
/// extracts the `DiemAccount::KeyRotationCapability` for `account` and adds
/// it to the resource. After the successful execution of this transaction
/// other accounts may add their key rotation to this resource so that `account`
/// may be used as a recovery account for those accounts.
///
/// # Parameters
/// | Name      | Type     | Description                                           |
/// | ------    | ------   | -------------                                         |
/// | `account` | `signer` | The signer of the sending account of the transaction. |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                                               | Description                                                                                   |
/// | ----------------            | --------------                                             | -------------                                                                                 |
/// | `Errors::INVALID_STATE`     | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.          |
/// | `Errors::INVALID_ARGUMENT`  | `RecoveryAddress::ENOT_A_VASP`                             | `account` is not a VASP account.                                                              |
/// | `Errors::INVALID_ARGUMENT`  | `RecoveryAddress::EKEY_ROTATION_DEPENDENCY_CYCLE`          | A key rotation recovery cycle would be created by adding `account`'s key rotation capability. |
/// | `Errors::ALREADY_PUBLISHED` | `RecoveryAddress::ERECOVERY_ADDRESS`                       | A `RecoveryAddress::RecoveryAddress` resource has already been published under `account`.     |
///
/// # Related Scripts
/// * `Script::add_recovery_rotation_capability`
/// * `Script::rotate_authentication_key_with_recovery_address`
TransactionPayload encode_create_recovery_address_script_function();

/// # Summary
/// Creates a Validator account. This transaction can only be sent by the Diem
/// Root account.
///
/// # Technical Description
/// Creates an account with a Validator role at `new_account_address`, with authentication key
/// `auth_key_prefix` | `new_account_address`. It publishes a
/// `ValidatorConfig::ValidatorConfig` resource with empty `config`, and
/// `operator_account` fields. The `human_name` field of the
/// `ValidatorConfig::ValidatorConfig` is set to the passed in `human_name`.
/// This script does not add the validator to the validator set or the system,
/// but only creates the account.
///
/// # Parameters
/// | Name                  | Type         | Description                                                                                     |
/// | ------                | ------       | -------------                                                                                   |
/// | `dr_account`          | `&signer`    | The signer reference of the sending account of this transaction. Must be the Diem Root signer. |
/// | `sliding_nonce`       | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                      |
/// | `new_account_address` | `address`    | Address of the to-be-created Validator account.                                                 |
/// | `auth_key_prefix`     | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.        |
/// | `human_name`          | `vector<u8>` | ASCII-encoded human name for the validator.                                                     |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `dr_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::EDIEM_ROOT`            | The sending account is not the Diem Root account.                                         |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EDIEM_ROOT`                    | The sending account is not the Diem Root account.                                         |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `new_account_address` address is already taken.                                        |
///
/// # Related Scripts
/// * `Script::add_validator_and_reconfigure`
/// * `Script::create_validator_operator_account`
/// * `Script::register_validator_config`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_create_validator_account_script(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name);

/// # Summary
/// Creates a Validator account. This transaction can only be sent by the Diem
/// Root account.
///
/// # Technical Description
/// Creates an account with a Validator role at `new_account_address`, with authentication key
/// `auth_key_prefix` | `new_account_address`. It publishes a
/// `ValidatorConfig::ValidatorConfig` resource with empty `config`, and
/// `operator_account` fields. The `human_name` field of the
/// `ValidatorConfig::ValidatorConfig` is set to the passed in `human_name`.
/// This script does not add the validator to the validator set or the system,
/// but only creates the account.
/// Authentication keys, prefixes, and how to construct them from an ed25519 public key are described
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys).
///
/// # Events
/// Successful execution will emit:
/// * A `DiemAccount::CreateAccountEvent` with the `created` field being `new_account_address`,
/// and the `rold_id` field being `Roles::VALIDATOR_ROLE_ID`. This is emitted on the
/// `DiemAccount::AccountOperationsCapability` `creation_events` handle.
///
/// # Parameters
/// | Name                  | Type         | Description                                                                              |
/// | ------                | ------       | -------------                                                                            |
/// | `dr_account`          | `signer`     | The signer of the sending account of this transaction. Must be the Diem Root signer.     |
/// | `sliding_nonce`       | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.               |
/// | `new_account_address` | `address`    | Address of the to-be-created Validator account.                                          |
/// | `auth_key_prefix`     | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account. |
/// | `human_name`          | `vector<u8>` | ASCII-encoded human name for the validator.                                              |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `dr_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::EDIEM_ROOT`            | The sending account is not the Diem Root account.                                         |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EDIEM_ROOT`                    | The sending account is not the Diem Root account.                                         |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `new_account_address` address is already taken.                                        |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::register_validator_config`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_create_validator_account_script_function(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name);

/// # Summary
/// Creates a Validator Operator account. This transaction can only be sent by the Diem
/// Root account.
///
/// # Technical Description
/// Creates an account with a Validator Operator role at `new_account_address`, with authentication key
/// `auth_key_prefix` | `new_account_address`. It publishes a
/// `ValidatorOperatorConfig::ValidatorOperatorConfig` resource with the specified `human_name`.
/// This script does not assign the validator operator to any validator accounts but only creates the account.
///
/// # Parameters
/// | Name                  | Type         | Description                                                                                     |
/// | ------                | ------       | -------------                                                                                   |
/// | `dr_account`          | `&signer`    | The signer reference of the sending account of this transaction. Must be the Diem Root signer. |
/// | `sliding_nonce`       | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                      |
/// | `new_account_address` | `address`    | Address of the to-be-created Validator account.                                                 |
/// | `auth_key_prefix`     | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account.        |
/// | `human_name`          | `vector<u8>` | ASCII-encoded human name for the validator.                                                     |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `dr_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::EDIEM_ROOT`            | The sending account is not the Diem Root account.                                         |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EDIEM_ROOT`                    | The sending account is not the Diem Root account.                                         |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `new_account_address` address is already taken.                                        |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::add_validator_and_reconfigure`
/// * `Script::register_validator_config`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_create_validator_operator_account_script(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name);

/// # Summary
/// Creates a Validator Operator account. This transaction can only be sent by the Diem
/// Root account.
///
/// # Technical Description
/// Creates an account with a Validator Operator role at `new_account_address`, with authentication key
/// `auth_key_prefix` | `new_account_address`. It publishes a
/// `ValidatorOperatorConfig::ValidatorOperatorConfig` resource with the specified `human_name`.
/// This script does not assign the validator operator to any validator accounts but only creates the account.
/// Authentication key prefixes, and how to construct them from an ed25519 public key are described
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys).
///
/// # Events
/// Successful execution will emit:
/// * A `DiemAccount::CreateAccountEvent` with the `created` field being `new_account_address`,
/// and the `rold_id` field being `Roles::VALIDATOR_OPERATOR_ROLE_ID`. This is emitted on the
/// `DiemAccount::AccountOperationsCapability` `creation_events` handle.
///
/// # Parameters
/// | Name                  | Type         | Description                                                                              |
/// | ------                | ------       | -------------                                                                            |
/// | `dr_account`          | `signer`     | The signer of the sending account of this transaction. Must be the Diem Root signer.     |
/// | `sliding_nonce`       | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.               |
/// | `new_account_address` | `address`    | Address of the to-be-created Validator account.                                          |
/// | `auth_key_prefix`     | `vector<u8>` | The authentication key prefix that will be used initially for the newly created account. |
/// | `human_name`          | `vector<u8>` | ASCII-encoded human name for the validator.                                              |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                            | Description                                                                                |
/// | ----------------            | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`     | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `dr_account`.                             |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT`  | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS`  | `CoreAddresses::EDIEM_ROOT`            | The sending account is not the Diem Root account.                                         |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EDIEM_ROOT`                    | The sending account is not the Diem Root account.                                         |
/// | `Errors::ALREADY_PUBLISHED` | `Roles::EROLE_ID`                       | The `new_account_address` address is already taken.                                        |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::register_validator_config`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_create_validator_operator_account_script_function(uint64_t sliding_nonce, AccountAddress new_account_address, std::vector<uint8_t> auth_key_prefix, std::vector<uint8_t> human_name);

/// # Summary
/// Publishes a `VASPDomain::VASPDomains` resource under a parent VASP account.
/// The sending account must be a parent VASP account.
///
/// # Technical Description
/// Publishes a `VASPDomain::VASPDomains` resource under `account`.
/// The The `VASPDomain::VASPDomains` resource's `domains` field is a vector
/// of VASPDomain, and will be empty on at the end of processing this transaction.
///
/// # Parameters
/// | Name      | Type     | Description                                           |
/// | ------    | ------   | -------------                                         |
/// | `account` | `signer` | The signer of the sending account of the transaction. |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason              | Description                                                                    |
/// | ----------------            | --------------            | -------------                                                                  |
/// | `Errors::ALREADY_PUBLISHED` | `VASPDomain::EVASP_DOMAINS` | A `VASPDomain::VASPDomains` resource has already been published under `account`. |
/// | `Errors::REQUIRES_ROLE`     | `Roles::EPARENT_VASP`     | The sending `account` was not a parent VASP account.                           |
TransactionPayload encode_create_vasp_domains_script_function();

/// # Summary
/// Freezes the account at `address`. The sending account of this transaction
/// must be the Treasury Compliance account. The account being frozen cannot be
/// the Diem Root or Treasury Compliance account. After the successful
/// execution of this transaction no transactions may be sent from the frozen
/// account, and the frozen account may not send or receive coins.
///
/// # Technical Description
/// Sets the `AccountFreezing::FreezingBit` to `true` and emits a
/// `AccountFreezing::FreezeAccountEvent`. The transaction sender must be the
/// Treasury Compliance account, but the account at `to_freeze_account` must
/// not be either `0xA550C18` (the Diem Root address), or `0xB1E55ED` (the
/// Treasury Compliance address). Note that this is a per-account property
/// e.g., freezing a Parent VASP will not effect the status any of its child
/// accounts and vice versa.
///

/// ## Events
/// Successful execution of this transaction will emit a `AccountFreezing::FreezeAccountEvent` on
/// the `freeze_event_handle` held in the `AccountFreezing::FreezeEventsHolder` resource published
/// under `0xA550C18` with the `frozen_address` being the `to_freeze_account`.
///
/// # Parameters
/// | Name                | Type      | Description                                                                                               |
/// | ------              | ------    | -------------                                                                                             |
/// | `tc_account`        | `&signer` | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `sliding_nonce`     | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                |
/// | `to_freeze_account` | `address` | The account address to be frozen.                                                                         |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                 | Description                                                                                |
/// | ----------------           | --------------                               | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`               | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`               | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`               | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`      | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`        | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::REQUIRES_ROLE`    | `Roles::ETREASURY_COMPLIANCE`                | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::INVALID_ARGUMENT` | `AccountFreezing::ECANNOT_FREEZE_TC`         | `to_freeze_account` was the Treasury Compliance account (`0xB1E55ED`).                     |
/// | `Errors::INVALID_ARGUMENT` | `AccountFreezing::ECANNOT_FREEZE_DIEM_ROOT` | `to_freeze_account` was the Diem Root account (`0xA550C18`).                              |
///
/// # Related Scripts
/// * `Script::unfreeze_account`
Script encode_freeze_account_script(uint64_t sliding_nonce, AccountAddress to_freeze_account);

/// # Summary
/// Freezes the account at `address`. The sending account of this transaction
/// must be the Treasury Compliance account. The account being frozen cannot be
/// the Diem Root or Treasury Compliance account. After the successful
/// execution of this transaction no transactions may be sent from the frozen
/// account, and the frozen account may not send or receive coins.
///
/// # Technical Description
/// Sets the `AccountFreezing::FreezingBit` to `true` and emits a
/// `AccountFreezing::FreezeAccountEvent`. The transaction sender must be the
/// Treasury Compliance account, but the account at `to_freeze_account` must
/// not be either `0xA550C18` (the Diem Root address), or `0xB1E55ED` (the
/// Treasury Compliance address). Note that this is a per-account property
/// e.g., freezing a Parent VASP will not effect the status any of its child
/// accounts and vice versa.
///

/// # Events
/// Successful execution of this transaction will emit a `AccountFreezing::FreezeAccountEvent` on
/// the `freeze_event_handle` held in the `AccountFreezing::FreezeEventsHolder` resource published
/// under `0xA550C18` with the `frozen_address` being the `to_freeze_account`.
///
/// # Parameters
/// | Name                | Type      | Description                                                                                     |
/// | ------              | ------    | -------------                                                                                   |
/// | `tc_account`        | `signer`  | The signer of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `sliding_nonce`     | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                      |
/// | `to_freeze_account` | `address` | The account address to be frozen.                                                               |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                 | Description                                                                                |
/// | ----------------           | --------------                               | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`               | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`               | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`               | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`      | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`        | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::REQUIRES_ROLE`    | `Roles::ETREASURY_COMPLIANCE`                | The sending account is not the Treasury Compliance account.                                |
/// | `Errors::INVALID_ARGUMENT` | `AccountFreezing::ECANNOT_FREEZE_TC`         | `to_freeze_account` was the Treasury Compliance account (`0xB1E55ED`).                     |
/// | `Errors::INVALID_ARGUMENT` | `AccountFreezing::ECANNOT_FREEZE_DIEM_ROOT` | `to_freeze_account` was the Diem Root account (`0xA550C18`).                              |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::unfreeze_account`
TransactionPayload encode_freeze_account_script_function(uint64_t sliding_nonce, AccountAddress to_freeze_account);

/// # Summary
/// Initializes the Diem consensus config that is stored on-chain.  This
/// transaction can only be sent from the Diem Root account.
///
/// # Technical Description
/// Initializes the `DiemConsensusConfig` on-chain config to empty and allows future updates from DiemRoot via
/// `update_diem_consensus_config`. This doesn't emit a `DiemConfig::NewEpochEvent`.
///
/// # Parameters
/// | Name            | Type      | Description                                                                |
/// | ------          | ------    | -------------                                                              |
/// | `account`       | `signer` | Signer of the sending account. Must be the Diem Root account.               |
/// | `sliding_nonce` | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                  | Description                                                                                |
/// | ----------------           | --------------                                | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`       | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                   | `account` is not the Diem Root account.                                                    |
TransactionPayload encode_initialize_diem_consensus_config_script_function(uint64_t sliding_nonce);

/// # Summary
/// Transfers a given number of coins in a specified currency from one account to another by multi-agent transaction.
/// Transfers over a specified amount defined on-chain that are between two different VASPs, or
/// other accounts that have opted-in will be subject to on-chain checks to ensure the receiver has
/// agreed to receive the coins.  This transaction can be sent by any account that can hold a
/// balance, and to any account that can hold a balance. Both accounts must hold balances in the
/// currency being transacted.
///
/// # Technical Description
///
/// Transfers `amount` coins of type `Currency` from `payer` to `payee` with (optional) associated
/// `metadata`.
/// Dual attestation is not applied to this script as payee is also a signer of the transaction.
/// Standardized `metadata` BCS format can be found in `diem_types::transaction::metadata::Metadata`.
///
/// # Events
/// Successful execution of this script emits two events:
/// * A `DiemAccount::SentPaymentEvent` on `payer`'s `DiemAccount::DiemAccount` `sent_events` handle; and
/// * A `DiemAccount::ReceivedPaymentEvent` on `payee`'s `DiemAccount::DiemAccount` `received_events` handle.
///
/// # Parameters
/// | Name                 | Type         | Description                                                                                                                  |
/// | ------               | ------       | -------------                                                                                                                |
/// | `Currency`           | Type         | The Move type for the `Currency` being sent in this transaction. `Currency` must be an already-registered currency on-chain. |
/// | `payer`              | `signer`     | The signer of the sending account that coins are being transferred from.                                                     |
/// | `payee`              | `signer`     | The signer of the receiving account that the coins are being transferred to.                                                 |
/// | `metadata`           | `vector<u8>` | Optional metadata about this payment.                                                                                        |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                     | Description                                                                                                                         |
/// | ----------------           | --------------                                   | -------------                                                                                                                       |
/// | `Errors::NOT_PUBLISHED`    | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`       | `payer` doesn't hold a balance in `Currency`.                                                                                       |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EINSUFFICIENT_BALANCE`             | `amount` is greater than `payer`'s balance in `Currency`.                                                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::ECOIN_DEPOSIT_IS_ZERO`             | `amount` is zero.                                                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `DiemAccount::EPAYEE_DOES_NOT_EXIST`             | No account exists at the `payee` address.                                                                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EPAYEE_CANT_ACCEPT_CURRENCY_TYPE`  | An account exists at `payee`, but it does not accept payments in `Currency`.                                                        |
/// | `Errors::INVALID_STATE`    | `AccountFreezing::EACCOUNT_FROZEN`               | The `payee` account is frozen.                                                                                                      |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EWITHDRAWAL_EXCEEDS_LIMITS`        | `payer` has exceeded its daily withdrawal limits for the backing coins of XDX.                                                      |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`           | `payee` has exceeded its daily deposit limits for XDX.                                                                              |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_child_vasp_account`
/// * `AccountCreationScripts::create_parent_vasp_account`
/// * `AccountAdministrationScripts::add_currency_to_account`
/// * `PaymentScripts::peer_to_peer_with_metadata`
TransactionPayload encode_peer_to_peer_by_signers_script_function(TypeTag currency, uint64_t amount, std::vector<uint8_t> metadata);

/// # Summary
/// Transfers a given number of coins in a specified currency from one account to another.
/// Transfers over a specified amount defined on-chain that are between two different VASPs, or
/// other accounts that have opted-in will be subject to on-chain checks to ensure the receiver has
/// agreed to receive the coins.  This transaction can be sent by any account that can hold a
/// balance, and to any account that can hold a balance. Both accounts must hold balances in the
/// currency being transacted.
///
/// # Technical Description
///
/// Transfers `amount` coins of type `Currency` from `payer` to `payee` with (optional) associated
/// `metadata` and an (optional) `metadata_signature` on the message
/// `metadata` | `Signer::address_of(payer)` | `amount` | `DualAttestation::DOMAIN_SEPARATOR`.
/// The `metadata` and `metadata_signature` parameters are only required if `amount` >=
/// `DualAttestation::get_cur_microdiem_limit` XDX and `payer` and `payee` are distinct VASPs.
/// However, a transaction sender can opt in to dual attestation even when it is not required
/// (e.g., a DesignatedDealer -> VASP payment) by providing a non-empty `metadata_signature`.
/// Standardized `metadata` BCS format can be found in `diem_types::transaction::metadata::Metadata`.
///
/// ## Events
/// Successful execution of this script emits two events:
/// * A `DiemAccount::SentPaymentEvent` on `payer`'s `DiemAccount::DiemAccount` `sent_events` handle; and
/// * A `DiemAccount::ReceivedPaymentEvent` on `payee`'s `DiemAccount::DiemAccount` `received_events` handle.
///
/// # Parameters
/// | Name                 | Type         | Description                                                                                                                  |
/// | ------               | ------       | -------------                                                                                                                |
/// | `Currency`           | Type         | The Move type for the `Currency` being sent in this transaction. `Currency` must be an already-registered currency on-chain. |
/// | `payer`              | `&signer`    | The signer reference of the sending account that coins are being transferred from.                                           |
/// | `payee`              | `address`    | The address of the account the coins are being transferred to.                                                               |
/// | `metadata`           | `vector<u8>` | Optional metadata about this payment.                                                                                        |
/// | `metadata_signature` | `vector<u8>` | Optional signature over `metadata` and payment information. See                                                              |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                     | Description                                                                                                                         |
/// | ----------------           | --------------                                   | -------------                                                                                                                       |
/// | `Errors::NOT_PUBLISHED`    | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`      | `payer` doesn't hold a balance in `Currency`.                                                                                       |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EINSUFFICIENT_BALANCE`            | `amount` is greater than `payer`'s balance in `Currency`.                                                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::ECOIN_DEPOSIT_IS_ZERO`            | `amount` is zero.                                                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `DiemAccount::EPAYEE_DOES_NOT_EXIST`            | No account exists at the `payee` address.                                                                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EPAYEE_CANT_ACCEPT_CURRENCY_TYPE` | An account exists at `payee`, but it does not accept payments in `Currency`.                                                        |
/// | `Errors::INVALID_STATE`    | `AccountFreezing::EACCOUNT_FROZEN`               | The `payee` account is frozen.                                                                                                      |
/// | `Errors::INVALID_ARGUMENT` | `DualAttestation::EMALFORMED_METADATA_SIGNATURE` | `metadata_signature` is not 64 bytes.                                                                                               |
/// | `Errors::INVALID_ARGUMENT` | `DualAttestation::EINVALID_METADATA_SIGNATURE`   | `metadata_signature` does not verify on the against the `payee'`s `DualAttestation::Credential` `compliance_public_key` public key. |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EWITHDRAWAL_EXCEEDS_LIMITS`       | `payer` has exceeded its daily withdrawal limits for the backing coins of XDX.                                                      |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`          | `payee` has exceeded its daily deposit limits for XDX.                                                                              |
///
/// # Related Scripts
/// * `Script::create_child_vasp_account`
/// * `Script::create_parent_vasp_account`
/// * `Script::add_currency_to_account`
Script encode_peer_to_peer_with_metadata_script(TypeTag currency, AccountAddress payee, uint64_t amount, std::vector<uint8_t> metadata, std::vector<uint8_t> metadata_signature);

/// # Summary
/// Transfers a given number of coins in a specified currency from one account to another.
/// Transfers over a specified amount defined on-chain that are between two different VASPs, or
/// other accounts that have opted-in will be subject to on-chain checks to ensure the receiver has
/// agreed to receive the coins.  This transaction can be sent by any account that can hold a
/// balance, and to any account that can hold a balance. Both accounts must hold balances in the
/// currency being transacted.
///
/// # Technical Description
///
/// Transfers `amount` coins of type `Currency` from `payer` to `payee` with (optional) associated
/// `metadata` and an (optional) `metadata_signature` on the message of the form
/// `metadata` | `Signer::address_of(payer)` | `amount` | `DualAttestation::DOMAIN_SEPARATOR`, that
/// has been signed by the `payee`'s private key associated with the `compliance_public_key` held in
/// the `payee`'s `DualAttestation::Credential`. Both the `Signer::address_of(payer)` and `amount` fields
/// in the `metadata_signature` must be BCS-encoded bytes, and `|` denotes concatenation.
/// The `metadata` and `metadata_signature` parameters are only required if `amount` >=
/// `DualAttestation::get_cur_microdiem_limit` XDX and `payer` and `payee` are distinct VASPs.
/// However, a transaction sender can opt in to dual attestation even when it is not required
/// (e.g., a DesignatedDealer -> VASP payment) by providing a non-empty `metadata_signature`.
/// Standardized `metadata` BCS format can be found in `diem_types::transaction::metadata::Metadata`.
///
/// # Events
/// Successful execution of this script emits two events:
/// * A `DiemAccount::SentPaymentEvent` on `payer`'s `DiemAccount::DiemAccount` `sent_events` handle; and
/// * A `DiemAccount::ReceivedPaymentEvent` on `payee`'s `DiemAccount::DiemAccount` `received_events` handle.
///
/// # Parameters
/// | Name                 | Type         | Description                                                                                                                  |
/// | ------               | ------       | -------------                                                                                                                |
/// | `Currency`           | Type         | The Move type for the `Currency` being sent in this transaction. `Currency` must be an already-registered currency on-chain. |
/// | `payer`              | `signer`     | The signer of the sending account that coins are being transferred from.                                                     |
/// | `payee`              | `address`    | The address of the account the coins are being transferred to.                                                               |
/// | `metadata`           | `vector<u8>` | Optional metadata about this payment.                                                                                        |
/// | `metadata_signature` | `vector<u8>` | Optional signature over `metadata` and payment information. See                                                              |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                     | Description                                                                                                                         |
/// | ----------------           | --------------                                   | -------------                                                                                                                       |
/// | `Errors::NOT_PUBLISHED`    | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`       | `payer` doesn't hold a balance in `Currency`.                                                                                       |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EINSUFFICIENT_BALANCE`             | `amount` is greater than `payer`'s balance in `Currency`.                                                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::ECOIN_DEPOSIT_IS_ZERO`             | `amount` is zero.                                                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `DiemAccount::EPAYEE_DOES_NOT_EXIST`             | No account exists at the `payee` address.                                                                                           |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EPAYEE_CANT_ACCEPT_CURRENCY_TYPE`  | An account exists at `payee`, but it does not accept payments in `Currency`.                                                        |
/// | `Errors::INVALID_STATE`    | `AccountFreezing::EACCOUNT_FROZEN`               | The `payee` account is frozen.                                                                                                      |
/// | `Errors::INVALID_ARGUMENT` | `DualAttestation::EMALFORMED_METADATA_SIGNATURE` | `metadata_signature` is not 64 bytes.                                                                                               |
/// | `Errors::INVALID_ARGUMENT` | `DualAttestation::EINVALID_METADATA_SIGNATURE`   | `metadata_signature` does not verify on the against the `payee'`s `DualAttestation::Credential` `compliance_public_key` public key. |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EWITHDRAWAL_EXCEEDS_LIMITS`        | `payer` has exceeded its daily withdrawal limits for the backing coins of XDX.                                                      |
/// | `Errors::LIMIT_EXCEEDED`   | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`           | `payee` has exceeded its daily deposit limits for XDX.                                                                              |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_child_vasp_account`
/// * `AccountCreationScripts::create_parent_vasp_account`
/// * `AccountAdministrationScripts::add_currency_to_account`
/// * `PaymentScripts::peer_to_peer_by_signers`
TransactionPayload encode_peer_to_peer_with_metadata_script_function(TypeTag currency, AccountAddress payee, uint64_t amount, std::vector<uint8_t> metadata, std::vector<uint8_t> metadata_signature);

/// # Summary
/// Moves a specified number of coins in a given currency from the account's
/// balance to its preburn area after which the coins may be burned. This
/// transaction may be sent by any account that holds a balance and preburn area
/// in the specified currency.
///
/// # Technical Description
/// Moves the specified `amount` of coins in `Token` currency from the sending `account`'s
/// `DiemAccount::Balance<Token>` to the `Diem::Preburn<Token>` published under the same
/// `account`. `account` must have both of these resources published under it at the start of this
/// transaction in order for it to execute successfully.
///
/// ## Events
/// Successful execution of this script emits two events:
/// * `DiemAccount::SentPaymentEvent ` on `account`'s `DiemAccount::DiemAccount` `sent_events`
/// handle with the `payee` and `payer` fields being `account`'s address; and
/// * A `Diem::PreburnEvent` with `Token`'s currency code on the
/// `Diem::CurrencyInfo<Token`'s `preburn_events` handle for `Token` and with
/// `preburn_address` set to `account`'s address.
///
/// # Parameters
/// | Name      | Type      | Description                                                                                                                      |
/// | ------    | ------    | -------------                                                                                                                    |
/// | `Token`   | Type      | The Move type for the `Token` currency being moved to the preburn area. `Token` must be an already-registered currency on-chain. |
/// | `account` | `&signer` | The signer reference of the sending account.                                                                                     |
/// | `amount`  | `u64`     | The amount in `Token` to be moved to the preburn area.                                                                           |
///
/// # Common Abort Conditions
/// | Error Category           | Error Reason                                             | Description                                                                             |
/// | ----------------         | --------------                                           | -------------                                                                           |
/// | `Errors::NOT_PUBLISHED`  | `Diem::ECURRENCY_INFO`                                  | The `Token` is not a registered currency on-chain.                                      |
/// | `Errors::INVALID_STATE`  | `DiemAccount::EWITHDRAWAL_CAPABILITY_ALREADY_EXTRACTED` | The withdrawal capability for `account` has already been extracted.                     |
/// | `Errors::LIMIT_EXCEEDED` | `DiemAccount::EINSUFFICIENT_BALANCE`                    | `amount` is greater than `payer`'s balance in `Token`.                                  |
/// | `Errors::NOT_PUBLISHED`  | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`              | `account` doesn't hold a balance in `Token`.                                            |
/// | `Errors::NOT_PUBLISHED`  | `Diem::EPREBURN`                                        | `account` doesn't have a `Diem::Preburn<Token>` resource published under it.           |
/// | `Errors::INVALID_STATE`  | `Diem::EPREBURN_OCCUPIED`                               | The `value` field in the `Diem::Preburn<Token>` resource under the sender is non-zero. |
/// | `Errors::NOT_PUBLISHED`  | `Roles::EROLE_ID`                                        | The `account` did not have a role assigned to it.                                       |
/// | `Errors::REQUIRES_ROLE`  | `Roles::EDESIGNATED_DEALER`                              | The `account` did not have the role of DesignatedDealer.                                |
///
/// # Related Scripts
/// * `Script::cancel_burn`
/// * `Script::burn`
/// * `Script::burn_txn_fees`
Script encode_preburn_script(TypeTag token, uint64_t amount);

/// # Summary
/// Moves a specified number of coins in a given currency from the account's
/// balance to its preburn area after which the coins may be burned. This
/// transaction may be sent by any account that holds a balance and preburn area
/// in the specified currency.
///
/// # Technical Description
/// Moves the specified `amount` of coins in `Token` currency from the sending `account`'s
/// `DiemAccount::Balance<Token>` to the `Diem::Preburn<Token>` published under the same
/// `account`. `account` must have both of these resources published under it at the start of this
/// transaction in order for it to execute successfully.
///
/// # Events
/// Successful execution of this script emits two events:
/// * `DiemAccount::SentPaymentEvent ` on `account`'s `DiemAccount::DiemAccount` `sent_events`
/// handle with the `payee` and `payer` fields being `account`'s address; and
/// * A `Diem::PreburnEvent` with `Token`'s currency code on the
/// `Diem::CurrencyInfo<Token`'s `preburn_events` handle for `Token` and with
/// `preburn_address` set to `account`'s address.
///
/// # Parameters
/// | Name      | Type     | Description                                                                                                                      |
/// | ------    | ------   | -------------                                                                                                                    |
/// | `Token`   | Type     | The Move type for the `Token` currency being moved to the preburn area. `Token` must be an already-registered currency on-chain. |
/// | `account` | `signer` | The signer of the sending account.                                                                                               |
/// | `amount`  | `u64`    | The amount in `Token` to be moved to the preburn area.                                                                           |
///
/// # Common Abort Conditions
/// | Error Category           | Error Reason                                             | Description                                                                             |
/// | ----------------         | --------------                                           | -------------                                                                           |
/// | `Errors::NOT_PUBLISHED`  | `Diem::ECURRENCY_INFO`                                  | The `Token` is not a registered currency on-chain.                                      |
/// | `Errors::INVALID_STATE`  | `DiemAccount::EWITHDRAWAL_CAPABILITY_ALREADY_EXTRACTED` | The withdrawal capability for `account` has already been extracted.                     |
/// | `Errors::LIMIT_EXCEEDED` | `DiemAccount::EINSUFFICIENT_BALANCE`                    | `amount` is greater than `payer`'s balance in `Token`.                                  |
/// | `Errors::NOT_PUBLISHED`  | `DiemAccount::EPAYER_DOESNT_HOLD_CURRENCY`              | `account` doesn't hold a balance in `Token`.                                            |
/// | `Errors::NOT_PUBLISHED`  | `Diem::EPREBURN`                                        | `account` doesn't have a `Diem::Preburn<Token>` resource published under it.           |
/// | `Errors::INVALID_STATE`  | `Diem::EPREBURN_OCCUPIED`                               | The `value` field in the `Diem::Preburn<Token>` resource under the sender is non-zero. |
/// | `Errors::NOT_PUBLISHED`  | `Roles::EROLE_ID`                                        | The `account` did not have a role assigned to it.                                       |
/// | `Errors::REQUIRES_ROLE`  | `Roles::EDESIGNATED_DEALER`                              | The `account` did not have the role of DesignatedDealer.                                |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::cancel_burn_with_amount`
/// * `TreasuryComplianceScripts::burn_with_amount`
/// * `TreasuryComplianceScripts::burn_txn_fees`
TransactionPayload encode_preburn_script_function(TypeTag token, uint64_t amount);

/// # Summary
/// Rotates the authentication key of the sending account to the
/// newly-specified public key and publishes a new shared authentication key
/// under the sender's account. Any account can send this transaction.
///
/// # Technical Description
/// Rotates the authentication key of the sending account to `public_key`,
/// and publishes a `SharedEd25519PublicKey::SharedEd25519PublicKey` resource
/// containing the 32-byte ed25519 `public_key` and the `DiemAccount::KeyRotationCapability` for
/// `account` under `account`.
///
/// # Parameters
/// | Name         | Type         | Description                                                                               |
/// | ------       | ------       | -------------                                                                             |
/// | `account`    | `&signer`    | The signer reference of the sending account of the transaction.                           |
/// | `public_key` | `vector<u8>` | 32-byte Ed25519 public key for `account`' authentication key to be rotated to and stored. |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                                               | Description                                                                                         |
/// | ----------------            | --------------                                             | -------------                                                                                       |
/// | `Errors::INVALID_STATE`     | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability` resource.       |
/// | `Errors::ALREADY_PUBLISHED` | `SharedEd25519PublicKey::ESHARED_KEY`                      | The `SharedEd25519PublicKey::SharedEd25519PublicKey` resource is already published under `account`. |
/// | `Errors::INVALID_ARGUMENT`  | `SharedEd25519PublicKey::EMALFORMED_PUBLIC_KEY`            | `public_key` is an invalid ed25519 public key.                                                      |
///
/// # Related Scripts
/// * `Script::rotate_shared_ed25519_public_key`
Script encode_publish_shared_ed25519_public_key_script(std::vector<uint8_t> public_key);

/// # Summary
/// Rotates the authentication key of the sending account to the newly-specified ed25519 public key and
/// publishes a new shared authentication key derived from that public key under the sender's account.
/// Any account can send this transaction.
///
/// # Technical Description
/// Rotates the authentication key of the sending account to the
/// [authentication key derived from `public_key`](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys)
/// and publishes a `SharedEd25519PublicKey::SharedEd25519PublicKey` resource
/// containing the 32-byte ed25519 `public_key` and the `DiemAccount::KeyRotationCapability` for
/// `account` under `account`.
///
/// # Parameters
/// | Name         | Type         | Description                                                                                        |
/// | ------       | ------       | -------------                                                                                      |
/// | `account`    | `signer`     | The signer of the sending account of the transaction.                                              |
/// | `public_key` | `vector<u8>` | A valid 32-byte Ed25519 public key for `account`'s authentication key to be rotated to and stored. |
///
/// # Common Abort Conditions
/// | Error Category              | Error Reason                                               | Description                                                                                         |
/// | ----------------            | --------------                                             | -------------                                                                                       |
/// | `Errors::INVALID_STATE`     | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability` resource.       |
/// | `Errors::ALREADY_PUBLISHED` | `SharedEd25519PublicKey::ESHARED_KEY`                      | The `SharedEd25519PublicKey::SharedEd25519PublicKey` resource is already published under `account`. |
/// | `Errors::INVALID_ARGUMENT`  | `SharedEd25519PublicKey::EMALFORMED_PUBLIC_KEY`            | `public_key` is an invalid ed25519 public key.                                                      |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::rotate_shared_ed25519_public_key`
TransactionPayload encode_publish_shared_ed25519_public_key_script_function(std::vector<uint8_t> public_key);

/// # Summary
/// Updates a validator's configuration. This does not reconfigure the system and will not update
/// the configuration in the validator set that is seen by other validators in the network. Can
/// only be successfully sent by a Validator Operator account that is already registered with a
/// validator.
///
/// # Technical Description
/// This updates the fields with corresponding names held in the `ValidatorConfig::ValidatorConfig`
/// config resource held under `validator_account`. It does not emit a `DiemConfig::NewEpochEvent`
/// so the copy of this config held in the validator set will not be updated, and the changes are
/// only "locally" under the `validator_account` account address.
///
/// # Parameters
/// | Name                          | Type         | Description                                                                                                                  |
/// | ------                        | ------       | -------------                                                                                                                |
/// | `validator_operator_account`  | `&signer`    | Signer reference of the sending account. Must be the registered validator operator for the validator at `validator_address`. |
/// | `validator_account`           | `address`    | The address of the validator's `ValidatorConfig::ValidatorConfig` resource being updated.                                    |
/// | `consensus_pubkey`            | `vector<u8>` | New Ed25519 public key to be used in the updated `ValidatorConfig::ValidatorConfig`.                                         |
/// | `validator_network_addresses` | `vector<u8>` | New set of `validator_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.                       |
/// | `fullnode_network_addresses`  | `vector<u8>` | New set of `fullnode_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.                        |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                   | Description                                                                                           |
/// | ----------------           | --------------                                 | -------------                                                                                         |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`           | `validator_address` does not have a `ValidatorConfig::ValidatorConfig` resource published under it.   |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_TRANSACTION_SENDER` | `validator_operator_account` is not the registered operator for the validator at `validator_address`. |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_CONSENSUS_KEY`      | `consensus_pubkey` is not a valid ed25519 public key.                                                 |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::create_validator_operator_account`
/// * `Script::add_validator_and_reconfigure`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_register_validator_config_script(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses);

/// # Summary
/// Updates a validator's configuration. This does not reconfigure the system and will not update
/// the configuration in the validator set that is seen by other validators in the network. Can
/// only be successfully sent by a Validator Operator account that is already registered with a
/// validator.
///
/// # Technical Description
/// This updates the fields with corresponding names held in the `ValidatorConfig::ValidatorConfig`
/// config resource held under `validator_account`. It does not emit a `DiemConfig::NewEpochEvent`
/// so the copy of this config held in the validator set will not be updated, and the changes are
/// only "locally" under the `validator_account` account address.
///
/// # Parameters
/// | Name                          | Type         | Description                                                                                                        |
/// | ------                        | ------       | -------------                                                                                                      |
/// | `validator_operator_account`  | `signer`     | Signer of the sending account. Must be the registered validator operator for the validator at `validator_address`. |
/// | `validator_account`           | `address`    | The address of the validator's `ValidatorConfig::ValidatorConfig` resource being updated.                          |
/// | `consensus_pubkey`            | `vector<u8>` | New Ed25519 public key to be used in the updated `ValidatorConfig::ValidatorConfig`.                               |
/// | `validator_network_addresses` | `vector<u8>` | New set of `validator_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.             |
/// | `fullnode_network_addresses`  | `vector<u8>` | New set of `fullnode_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.              |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                   | Description                                                                                           |
/// | ----------------           | --------------                                 | -------------                                                                                         |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`           | `validator_address` does not have a `ValidatorConfig::ValidatorConfig` resource published under it.   |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_TRANSACTION_SENDER` | `validator_operator_account` is not the registered operator for the validator at `validator_address`. |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_CONSENSUS_KEY`      | `consensus_pubkey` is not a valid ed25519 public key.                                                 |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_register_validator_config_script_function(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses);

/// # Summary
/// This script removes a validator account from the validator set, and triggers a reconfiguration
/// of the system to remove the validator from the system. This transaction can only be
/// successfully called by the Diem Root account.
///
/// # Technical Description
/// This script removes the account at `validator_address` from the validator set. This transaction
/// emits a `DiemConfig::NewEpochEvent` event. Once the reconfiguration triggered by this event
/// has been performed, the account at `validator_address` is no longer considered to be a
/// validator in the network. This transaction will fail if the validator at `validator_address`
/// is not in the validator set.
///
/// # Parameters
/// | Name                | Type         | Description                                                                                                                        |
/// | ------              | ------       | -------------                                                                                                                      |
/// | `dr_account`        | `&signer`    | The signer reference of the sending account of this transaction. Must be the Diem Root signer.                                    |
/// | `sliding_nonce`     | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                         |
/// | `validator_name`    | `vector<u8>` | ASCII-encoded human name for the validator. Must match the human name in the `ValidatorConfig::ValidatorConfig` for the validator. |
/// | `validator_address` | `address`    | The validator account address to be removed from the validator set.                                                                |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                     |
/// | ----------------           | --------------                          | -------------                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `dr_account`.                                  |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.      |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                               |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | The sending account is not the Diem Root account or Treasury Compliance account                |
/// | 0                          | 0                                       | The provided `validator_name` does not match the already-recorded human name for the validator. |
/// | `Errors::INVALID_ARGUMENT` | `DiemSystem::ENOT_AN_ACTIVE_VALIDATOR` | The validator to be removed is not in the validator set.                                        |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`            | The sending account is not the Diem Root account.                                              |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EDIEM_ROOT`                    | The sending account is not the Diem Root account.                                              |
/// | `Errors::INVALID_STATE`    | `DiemConfig::EINVALID_BLOCK_TIME`      | An invalid time value was encountered in reconfiguration. Unlikely to occur.                    |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::create_validator_operator_account`
/// * `Script::register_validator_config`
/// * `Script::add_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_remove_validator_and_reconfigure_script(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address);

/// # Summary
/// This script removes a validator account from the validator set, and triggers a reconfiguration
/// of the system to remove the validator from the system. This transaction can only be
/// successfully called by the Diem Root account.
///
/// # Technical Description
/// This script removes the account at `validator_address` from the validator set. This transaction
/// emits a `DiemConfig::NewEpochEvent` event. Once the reconfiguration triggered by this event
/// has been performed, the account at `validator_address` is no longer considered to be a
/// validator in the network. This transaction will fail if the validator at `validator_address`
/// is not in the validator set.
///
/// # Parameters
/// | Name                | Type         | Description                                                                                                                        |
/// | ------              | ------       | -------------                                                                                                                      |
/// | `dr_account`        | `signer`     | The signer of the sending account of this transaction. Must be the Diem Root signer.                                               |
/// | `sliding_nonce`     | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                                         |
/// | `validator_name`    | `vector<u8>` | ASCII-encoded human name for the validator. Must match the human name in the `ValidatorConfig::ValidatorConfig` for the validator. |
/// | `validator_address` | `address`    | The validator account address to be removed from the validator set.                                                                |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                     |
/// | ----------------           | --------------                          | -------------                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `dr_account`.                                  |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.      |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                               |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | The sending account is not the Diem Root account or Treasury Compliance account                |
/// | 0                          | 0                                       | The provided `validator_name` does not match the already-recorded human name for the validator. |
/// | `Errors::INVALID_ARGUMENT` | `DiemSystem::ENOT_AN_ACTIVE_VALIDATOR` | The validator to be removed is not in the validator set.                                        |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`            | The sending account is not the Diem Root account.                                              |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EDIEM_ROOT`                    | The sending account is not the Diem Root account.                                              |
/// | `Errors::INVALID_STATE`    | `DiemConfig::EINVALID_BLOCK_TIME`      | An invalid time value was encountered in reconfiguration. Unlikely to occur.                    |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::register_validator_config`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_remove_validator_and_reconfigure_script_function(uint64_t sliding_nonce, std::vector<uint8_t> validator_name, AccountAddress validator_address);

/// # Summary
/// Remove a VASP domain from parent VASP account. The transaction can only be sent by
/// the Treasury Compliance account.
///
/// # Technical Description
/// Removes a `VASPDomain::VASPDomain` from the `domains` field of the `VASPDomain::VASPDomains` resource published under
/// account with `address`.
///
/// # Parameters
/// | Name         | Type         | Description                                                                                     |
/// | ------       | ------       | -------------                                                                                   |
/// | `tc_account` | `signer`     | The signer of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `address`    | `address`    | The `address` of parent VASP account that will update its domains.                              |
/// | `domain`     | `vector<u8>` | The domain name.                                                                                |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                             | Description                                                                                                                            |
/// | ----------------           | --------------                           | -------------                                                                                                                          |
/// | `Errors::REQUIRES_ROLE`    | `Roles::ETREASURY_COMPLIANCE`            | The sending account is not the Treasury Compliance account.                                                                            |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`    | `tc_account` is not the Treasury Compliance account.                                                                                   |
/// | `Errors::NOT_PUBLISHED`    | `VASPDomain::EVASP_DOMAIN_MANAGER`        | The `VASPDomain::VASPDomainManager` resource is not yet published under the Treasury Compliance account.                                 |
/// | `Errors::NOT_PUBLISHED`    | `VASPDomain::EVASP_DOMAINS_NOT_PUBLISHED` | `address` does not have a `VASPDomain::VASPDomains` resource published under it.                                                         |
/// | `Errors::INVALID_ARGUMENT` | `VASPDomain::EINVALID_VASP_DOMAIN`        | The `domain` is greater in length than `VASPDomain::DOMAIN_LENGTH`.                                                                        |
/// | `Errors::INVALID_ARGUMENT` | `VASPDomain::EVASP_DOMAIN_NOT_FOUND`              | The `domain` does not exist in the list of `VASPDomain::VASPDomain`s  in the `VASPDomain::VASPDomains` resource published under `address`. |
TransactionPayload encode_remove_vasp_domain_script_function(AccountAddress address, std::vector<uint8_t> domain);

/// # Summary
/// Rotates the transaction sender's authentication key to the supplied new authentication key. May
/// be sent by any account.
///
/// # Technical Description
/// Rotate the `account`'s `DiemAccount::DiemAccount` `authentication_key` field to `new_key`.
/// `new_key` must be a valid ed25519 public key, and `account` must not have previously delegated
/// its `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name      | Type         | Description                                                 |
/// | ------    | ------       | -------------                                               |
/// | `account` | `&signer`    | Signer reference of the sending account of the transaction. |
/// | `new_key` | `vector<u8>` | New ed25519 public key to be used for `account`.            |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                               | Description                                                                              |
/// | ----------------           | --------------                                             | -------------                                                                            |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.     |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`              | `new_key` was an invalid length.                                                         |
///
/// # Related Scripts
/// * `Script::rotate_authentication_key_with_nonce`
/// * `Script::rotate_authentication_key_with_nonce_admin`
/// * `Script::rotate_authentication_key_with_recovery_address`
Script encode_rotate_authentication_key_script(std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the `account`'s authentication key to the supplied new authentication key. May be sent by any account.
///
/// # Technical Description
/// Rotate the `account`'s `DiemAccount::DiemAccount` `authentication_key`
/// field to `new_key`. `new_key` must be a valid authentication key that
/// corresponds to an ed25519 public key as described [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys),
/// and `account` must not have previously delegated its `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name      | Type         | Description                                       |
/// | ------    | ------       | -------------                                     |
/// | `account` | `signer`     | Signer of the sending account of the transaction. |
/// | `new_key` | `vector<u8>` | New authentication key to be used for `account`.  |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                              | Description                                                                         |
/// | ----------------           | --------------                                            | -------------                                                                       |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`. |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`              | `new_key` was an invalid length.                                                    |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::rotate_authentication_key_with_nonce`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_nonce_admin`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_recovery_address`
TransactionPayload encode_rotate_authentication_key_script_function(std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the sender's authentication key to the supplied new authentication key. May be sent by
/// any account that has a sliding nonce resource published under it (usually this is Treasury
/// Compliance or Diem Root accounts).
///
/// # Technical Description
/// Rotates the `account`'s `DiemAccount::DiemAccount` `authentication_key` field to `new_key`.
/// `new_key` must be a valid ed25519 public key, and `account` must not have previously delegated
/// its `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name            | Type         | Description                                                                |
/// | ------          | ------       | -------------                                                              |
/// | `account`       | `&signer`    | Signer reference of the sending account of the transaction.                |
/// | `sliding_nonce` | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction. |
/// | `new_key`       | `vector<u8>` | New ed25519 public key to be used for `account`.                           |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                               | Description                                                                                |
/// | ----------------           | --------------                                             | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                             | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                             | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                             | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`                    | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.       |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`              | `new_key` was an invalid length.                                                           |
///
/// # Related Scripts
/// * `Script::rotate_authentication_key`
/// * `Script::rotate_authentication_key_with_nonce_admin`
/// * `Script::rotate_authentication_key_with_recovery_address`
Script encode_rotate_authentication_key_with_nonce_script(uint64_t sliding_nonce, std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the sender's authentication key to the supplied new authentication key. May be sent by
/// any account that has a sliding nonce resource published under it (usually this is Treasury
/// Compliance or Diem Root accounts).
///
/// # Technical Description
/// Rotates the `account`'s `DiemAccount::DiemAccount` `authentication_key`
/// field to `new_key`. `new_key` must be a valid authentication key that
/// corresponds to an ed25519 public key as described [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys),
/// and `account` must not have previously delegated its `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name            | Type         | Description                                                                |
/// | ------          | ------       | -------------                                                              |
/// | `account`       | `signer`     | Signer of the sending account of the transaction.                          |
/// | `sliding_nonce` | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction. |
/// | `new_key`       | `vector<u8>` | New authentication key to be used for `account`.                           |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                               | Description                                                                                |
/// | ----------------           | --------------                                             | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                             | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                             | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                             | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`                    | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.       |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`              | `new_key` was an invalid length.                                                           |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::rotate_authentication_key`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_nonce_admin`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_recovery_address`
TransactionPayload encode_rotate_authentication_key_with_nonce_script_function(uint64_t sliding_nonce, std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the specified account's authentication key to the supplied new authentication key. May
/// only be sent by the Diem Root account as a write set transaction.
///
/// # Technical Description
/// Rotate the `account`'s `DiemAccount::DiemAccount` `authentication_key` field to `new_key`.
/// `new_key` must be a valid ed25519 public key, and `account` must not have previously delegated
/// its `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name            | Type         | Description                                                                                                  |
/// | ------          | ------       | -------------                                                                                                |
/// | `dr_account`    | `&signer`    | The signer reference of the sending account of the write set transaction. May only be the Diem Root signer. |
/// | `account`       | `&signer`    | Signer reference of account specified in the `execute_as` field of the write set transaction.                |
/// | `sliding_nonce` | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction for Diem Root.                    |
/// | `new_key`       | `vector<u8>` | New ed25519 public key to be used for `account`.                                                             |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                               | Description                                                                                                |
/// | ----------------           | --------------                                             | -------------                                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                             | A `SlidingNonce` resource is not published under `dr_account`.                                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                             | The `sliding_nonce` in `dr_account` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                             | The `sliding_nonce` in `dr_account` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`                    | The `sliding_nonce` in` dr_account` has been previously recorded.                                          |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.                       |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`              | `new_key` was an invalid length.                                                                           |
///
/// # Related Scripts
/// * `Script::rotate_authentication_key`
/// * `Script::rotate_authentication_key_with_nonce`
/// * `Script::rotate_authentication_key_with_recovery_address`
Script encode_rotate_authentication_key_with_nonce_admin_script(uint64_t sliding_nonce, std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the specified account's authentication key to the supplied new authentication key. May
/// only be sent by the Diem Root account as a write set transaction.
///
/// # Technical Description
/// Rotate the `account`'s `DiemAccount::DiemAccount` `authentication_key` field to `new_key`.
/// `new_key` must be a valid authentication key that corresponds to an ed25519
/// public key as described [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys),
/// and `account` must not have previously delegated its `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name            | Type         | Description                                                                                       |
/// | ------          | ------       | -------------                                                                                     |
/// | `dr_account`    | `signer`     | The signer of the sending account of the write set transaction. May only be the Diem Root signer. |
/// | `account`       | `signer`     | Signer of account specified in the `execute_as` field of the write set transaction.               |
/// | `sliding_nonce` | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction for Diem Root.          |
/// | `new_key`       | `vector<u8>` | New authentication key to be used for `account`.                                                  |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                              | Description                                                                                                |
/// | ----------------           | --------------                                            | -------------                                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                            | A `SlidingNonce` resource is not published under `dr_account`.                                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                            | The `sliding_nonce` in `dr_account` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                            | The `sliding_nonce` in `dr_account` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`                   | The `sliding_nonce` in` dr_account` has been previously recorded.                                          |
/// | `Errors::INVALID_STATE`    | `DiemAccount::EKEY_ROTATION_CAPABILITY_ALREADY_EXTRACTED` | `account` has already delegated/extracted its `DiemAccount::KeyRotationCapability`.                        |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY`              | `new_key` was an invalid length.                                                                           |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::rotate_authentication_key`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_nonce`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_recovery_address`
TransactionPayload encode_rotate_authentication_key_with_nonce_admin_script_function(uint64_t sliding_nonce, std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the authentication key of a specified account that is part of a recovery address to a
/// new authentication key. Only used for accounts that are part of a recovery address (see
/// `Script::add_recovery_rotation_capability` for account restrictions).
///
/// # Technical Description
/// Rotates the authentication key of the `to_recover` account to `new_key` using the
/// `DiemAccount::KeyRotationCapability` stored in the `RecoveryAddress::RecoveryAddress` resource
/// published under `recovery_address`. This transaction can be sent either by the `to_recover`
/// account, or by the account where the `RecoveryAddress::RecoveryAddress` resource is published
/// that contains `to_recover`'s `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name               | Type         | Description                                                                                                                    |
/// | ------             | ------       | -------------                                                                                                                  |
/// | `account`          | `&signer`    | Signer reference of the sending account of the transaction.                                                                    |
/// | `recovery_address` | `address`    | Address where `RecoveryAddress::RecoveryAddress` that holds `to_recover`'s `DiemAccount::KeyRotationCapability` is published. |
/// | `to_recover`       | `address`    | The address of the account whose authentication key will be updated.                                                           |
/// | `new_key`          | `vector<u8>` | New ed25519 public key to be used for the account at the `to_recover` address.                                                 |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                  | Description                                                                                                                                          |
/// | ----------------           | --------------                                | -------------                                                                                                                                        |
/// | `Errors::NOT_PUBLISHED`    | `RecoveryAddress::ERECOVERY_ADDRESS`          | `recovery_address` does not have a `RecoveryAddress::RecoveryAddress` resource published under it.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `RecoveryAddress::ECANNOT_ROTATE_KEY`         | The address of `account` is not `recovery_address` or `to_recover`.                                                                                  |
/// | `Errors::INVALID_ARGUMENT` | `RecoveryAddress::EACCOUNT_NOT_RECOVERABLE`   | `to_recover`'s `DiemAccount::KeyRotationCapability`  is not in the `RecoveryAddress::RecoveryAddress`  resource published under `recovery_address`. |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY` | `new_key` was an invalid length.                                                                                                                     |
///
/// # Related Scripts
/// * `Script::rotate_authentication_key`
/// * `Script::rotate_authentication_key_with_nonce`
/// * `Script::rotate_authentication_key_with_nonce_admin`
Script encode_rotate_authentication_key_with_recovery_address_script(AccountAddress recovery_address, AccountAddress to_recover, std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the authentication key of a specified account that is part of a recovery address to a
/// new authentication key. Only used for accounts that are part of a recovery address (see
/// `AccountAdministrationScripts::add_recovery_rotation_capability` for account restrictions).
///
/// # Technical Description
/// Rotates the authentication key of the `to_recover` account to `new_key` using the
/// `DiemAccount::KeyRotationCapability` stored in the `RecoveryAddress::RecoveryAddress` resource
/// published under `recovery_address`. `new_key` must be a valide authentication key as described
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys).
/// This transaction can be sent either by the `to_recover` account, or by the account where the
/// `RecoveryAddress::RecoveryAddress` resource is published that contains `to_recover`'s `DiemAccount::KeyRotationCapability`.
///
/// # Parameters
/// | Name               | Type         | Description                                                                                                                   |
/// | ------             | ------       | -------------                                                                                                                 |
/// | `account`          | `signer`     | Signer of the sending account of the transaction.                                                                             |
/// | `recovery_address` | `address`    | Address where `RecoveryAddress::RecoveryAddress` that holds `to_recover`'s `DiemAccount::KeyRotationCapability` is published. |
/// | `to_recover`       | `address`    | The address of the account whose authentication key will be updated.                                                          |
/// | `new_key`          | `vector<u8>` | New authentication key to be used for the account at the `to_recover` address.                                                |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                 | Description                                                                                                                                         |
/// | ----------------           | --------------                               | -------------                                                                                                                                       |
/// | `Errors::NOT_PUBLISHED`    | `RecoveryAddress::ERECOVERY_ADDRESS`         | `recovery_address` does not have a `RecoveryAddress::RecoveryAddress` resource published under it.                                                  |
/// | `Errors::INVALID_ARGUMENT` | `RecoveryAddress::ECANNOT_ROTATE_KEY`        | The address of `account` is not `recovery_address` or `to_recover`.                                                                                 |
/// | `Errors::INVALID_ARGUMENT` | `RecoveryAddress::EACCOUNT_NOT_RECOVERABLE`  | `to_recover`'s `DiemAccount::KeyRotationCapability`  is not in the `RecoveryAddress::RecoveryAddress`  resource published under `recovery_address`. |
/// | `Errors::INVALID_ARGUMENT` | `DiemAccount::EMALFORMED_AUTHENTICATION_KEY` | `new_key` was an invalid length.                                                                                                                    |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::rotate_authentication_key`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_nonce`
/// * `AccountAdministrationScripts::rotate_authentication_key_with_nonce_admin`
TransactionPayload encode_rotate_authentication_key_with_recovery_address_script_function(AccountAddress recovery_address, AccountAddress to_recover, std::vector<uint8_t> new_key);

/// # Summary
/// Updates the url used for off-chain communication, and the public key used to verify dual
/// attestation on-chain. Transaction can be sent by any account that has dual attestation
/// information published under it. In practice the only such accounts are Designated Dealers and
/// Parent VASPs.
///
/// # Technical Description
/// Updates the `base_url` and `compliance_public_key` fields of the `DualAttestation::Credential`
/// resource published under `account`. The `new_key` must be a valid ed25519 public key.
///
/// ## Events
/// Successful execution of this transaction emits two events:
/// * A `DualAttestation::ComplianceKeyRotationEvent` containing the new compliance public key, and
/// the blockchain time at which the key was updated emitted on the `DualAttestation::Credential`
/// `compliance_key_rotation_events` handle published under `account`; and
/// * A `DualAttestation::BaseUrlRotationEvent` containing the new base url to be used for
/// off-chain communication, and the blockchain time at which the url was updated emitted on the
/// `DualAttestation::Credential` `base_url_rotation_events` handle published under `account`.
///
/// # Parameters
/// | Name      | Type         | Description                                                               |
/// | ------    | ------       | -------------                                                             |
/// | `account` | `&signer`    | Signer reference of the sending account of the transaction.               |
/// | `new_url` | `vector<u8>` | ASCII-encoded url to be used for off-chain communication with `account`.  |
/// | `new_key` | `vector<u8>` | New ed25519 public key to be used for on-chain dual attestation checking. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                           | Description                                                                |
/// | ----------------           | --------------                         | -------------                                                              |
/// | `Errors::NOT_PUBLISHED`    | `DualAttestation::ECREDENTIAL`         | A `DualAttestation::Credential` resource is not published under `account`. |
/// | `Errors::INVALID_ARGUMENT` | `DualAttestation::EINVALID_PUBLIC_KEY` | `new_key` is not a valid ed25519 public key.                               |
///
/// # Related Scripts
/// * `Script::create_parent_vasp_account`
/// * `Script::create_designated_dealer`
/// * `Script::rotate_dual_attestation_info`
Script encode_rotate_dual_attestation_info_script(std::vector<uint8_t> new_url, std::vector<uint8_t> new_key);

/// # Summary
/// Updates the url used for off-chain communication, and the public key used to verify dual
/// attestation on-chain. Transaction can be sent by any account that has dual attestation
/// information published under it. In practice the only such accounts are Designated Dealers and
/// Parent VASPs.
///
/// # Technical Description
/// Updates the `base_url` and `compliance_public_key` fields of the `DualAttestation::Credential`
/// resource published under `account`. The `new_key` must be a valid ed25519 public key.
///
/// # Events
/// Successful execution of this transaction emits two events:
/// * A `DualAttestation::ComplianceKeyRotationEvent` containing the new compliance public key, and
/// the blockchain time at which the key was updated emitted on the `DualAttestation::Credential`
/// `compliance_key_rotation_events` handle published under `account`; and
/// * A `DualAttestation::BaseUrlRotationEvent` containing the new base url to be used for
/// off-chain communication, and the blockchain time at which the url was updated emitted on the
/// `DualAttestation::Credential` `base_url_rotation_events` handle published under `account`.
///
/// # Parameters
/// | Name      | Type         | Description                                                               |
/// | ------    | ------       | -------------                                                             |
/// | `account` | `signer`     | Signer of the sending account of the transaction.                         |
/// | `new_url` | `vector<u8>` | ASCII-encoded url to be used for off-chain communication with `account`.  |
/// | `new_key` | `vector<u8>` | New ed25519 public key to be used for on-chain dual attestation checking. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                           | Description                                                                |
/// | ----------------           | --------------                         | -------------                                                              |
/// | `Errors::NOT_PUBLISHED`    | `DualAttestation::ECREDENTIAL`         | A `DualAttestation::Credential` resource is not published under `account`. |
/// | `Errors::INVALID_ARGUMENT` | `DualAttestation::EINVALID_PUBLIC_KEY` | `new_key` is not a valid ed25519 public key.                               |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_parent_vasp_account`
/// * `AccountCreationScripts::create_designated_dealer`
/// * `AccountAdministrationScripts::rotate_dual_attestation_info`
TransactionPayload encode_rotate_dual_attestation_info_script_function(std::vector<uint8_t> new_url, std::vector<uint8_t> new_key);

/// # Summary
/// Rotates the authentication key in a `SharedEd25519PublicKey`. This transaction can be sent by
/// any account that has previously published a shared ed25519 public key using
/// `Script::publish_shared_ed25519_public_key`.
///
/// # Technical Description
/// This first rotates the public key stored in `account`'s
/// `SharedEd25519PublicKey::SharedEd25519PublicKey` resource to `public_key`, after which it
/// rotates the authentication key using the capability stored in `account`'s
/// `SharedEd25519PublicKey::SharedEd25519PublicKey` to a new value derived from `public_key`
///
/// # Parameters
/// | Name         | Type         | Description                                                     |
/// | ------       | ------       | -------------                                                   |
/// | `account`    | `&signer`    | The signer reference of the sending account of the transaction. |
/// | `public_key` | `vector<u8>` | 32-byte Ed25519 public key.                                     |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                    | Description                                                                                   |
/// | ----------------           | --------------                                  | -------------                                                                                 |
/// | `Errors::NOT_PUBLISHED`    | `SharedEd25519PublicKey::ESHARED_KEY`           | A `SharedEd25519PublicKey::SharedEd25519PublicKey` resource is not published under `account`. |
/// | `Errors::INVALID_ARGUMENT` | `SharedEd25519PublicKey::EMALFORMED_PUBLIC_KEY` | `public_key` is an invalid ed25519 public key.                                                |
///
/// # Related Scripts
/// * `Script::publish_shared_ed25519_public_key`
Script encode_rotate_shared_ed25519_public_key_script(std::vector<uint8_t> public_key);

/// # Summary
/// Rotates the authentication key in a `SharedEd25519PublicKey`. This transaction can be sent by
/// any account that has previously published a shared ed25519 public key using
/// `AccountAdministrationScripts::publish_shared_ed25519_public_key`.
///
/// # Technical Description
/// `public_key` must be a valid ed25519 public key.  This transaction first rotates the public key stored in `account`'s
/// `SharedEd25519PublicKey::SharedEd25519PublicKey` resource to `public_key`, after which it
/// rotates the `account`'s authentication key to the new authentication key derived from `public_key` as defined
/// [here](https://developers.diem.com/docs/core/accounts/#addresses-authentication-keys-and-cryptographic-keys)
/// using the `DiemAccount::KeyRotationCapability` stored in `account`'s `SharedEd25519PublicKey::SharedEd25519PublicKey`.
///
/// # Parameters
/// | Name         | Type         | Description                                           |
/// | ------       | ------       | -------------                                         |
/// | `account`    | `signer`     | The signer of the sending account of the transaction. |
/// | `public_key` | `vector<u8>` | 32-byte Ed25519 public key.                           |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                    | Description                                                                                   |
/// | ----------------           | --------------                                  | -------------                                                                                 |
/// | `Errors::NOT_PUBLISHED`    | `SharedEd25519PublicKey::ESHARED_KEY`           | A `SharedEd25519PublicKey::SharedEd25519PublicKey` resource is not published under `account`. |
/// | `Errors::INVALID_ARGUMENT` | `SharedEd25519PublicKey::EMALFORMED_PUBLIC_KEY` | `public_key` is an invalid ed25519 public key.                                                |
///
/// # Related Scripts
/// * `AccountAdministrationScripts::publish_shared_ed25519_public_key`
TransactionPayload encode_rotate_shared_ed25519_public_key_script_function(std::vector<uint8_t> public_key);

/// # Summary
/// Updates the gas constants stored on chain and used by the VM for gas
/// metering. This transaction can only be sent from the Diem Root account.
///
/// # Technical Description
/// Updates the on-chain config holding the `DiemVMConfig` and emits a
/// `DiemConfig::NewEpochEvent` to trigger a reconfiguration of the system.
///
/// # Parameters
/// | Name                                | Type     | Description                                                                                            |
/// | ------                              | ------   | -------------                                                                                          |
/// | `account`                           | `signer` | Signer of the sending account. Must be the Diem Root account.                                          |
/// | `sliding_nonce`                     | `u64`    | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                             |
/// | `global_memory_per_byte_cost`       | `u64`    | The new cost to read global memory per-byte to be used for gas metering.                               |
/// | `global_memory_per_byte_write_cost` | `u64`    | The new cost to write global memory per-byte to be used for gas metering.                              |
/// | `min_transaction_gas_units`         | `u64`    | The new flat minimum amount of gas required for any transaction.                                       |
/// | `large_transaction_cutoff`          | `u64`    | The new size over which an additional charge will be assessed for each additional byte.                |
/// | `intrinsic_gas_per_byte`            | `u64`    | The new number of units of gas that to be charged per-byte over the new `large_transaction_cutoff`.    |
/// | `maximum_number_of_gas_units`       | `u64`    | The new maximum number of gas units that can be set in a transaction.                                  |
/// | `min_price_per_gas_unit`            | `u64`    | The new minimum gas price that can be set for a transaction.                                           |
/// | `max_price_per_gas_unit`            | `u64`    | The new maximum gas price that can be set for a transaction.                                           |
/// | `max_transaction_size_in_bytes`     | `u64`    | The new maximum size of a transaction that can be processed.                                           |
/// | `gas_unit_scaling_factor`           | `u64`    | The new scaling factor to use when scaling between external and internal gas units.                    |
/// | `default_account_size`              | `u64`    | The new default account size to use when assessing final costs for reads and writes to global storage. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                | Description                                                                                |
/// | ----------------           | --------------                              | -------------                                                                              |
/// | `Errors::INVALID_ARGUMENT` | `DiemVMConfig::EGAS_CONSTANT_INCONSISTENCY` | The provided gas constants are inconsistent.                                               |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`              | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`              | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`              | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`     | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                 | `account` is not the Diem Root account.                                                    |
TransactionPayload encode_set_gas_constants_script_function(uint64_t sliding_nonce, uint64_t global_memory_per_byte_cost, uint64_t global_memory_per_byte_write_cost, uint64_t min_transaction_gas_units, uint64_t large_transaction_cutoff, uint64_t intrinsic_gas_per_byte, uint64_t maximum_number_of_gas_units, uint64_t min_price_per_gas_unit, uint64_t max_price_per_gas_unit, uint64_t max_transaction_size_in_bytes, uint64_t gas_unit_scaling_factor, uint64_t default_account_size);

/// # Summary
/// Updates a validator's configuration, and triggers a reconfiguration of the system to update the
/// validator set with this new validator configuration.  Can only be successfully sent by a
/// Validator Operator account that is already registered with a validator.
///
/// # Technical Description
/// This updates the fields with corresponding names held in the `ValidatorConfig::ValidatorConfig`
/// config resource held under `validator_account`. It then emits a `DiemConfig::NewEpochEvent` to
/// trigger a reconfiguration of the system.  This reconfiguration will update the validator set
/// on-chain with the updated `ValidatorConfig::ValidatorConfig`.
///
/// # Parameters
/// | Name                          | Type         | Description                                                                                                                  |
/// | ------                        | ------       | -------------                                                                                                                |
/// | `validator_operator_account`  | `&signer`    | Signer reference of the sending account. Must be the registered validator operator for the validator at `validator_address`. |
/// | `validator_account`           | `address`    | The address of the validator's `ValidatorConfig::ValidatorConfig` resource being updated.                                    |
/// | `consensus_pubkey`            | `vector<u8>` | New Ed25519 public key to be used in the updated `ValidatorConfig::ValidatorConfig`.                                         |
/// | `validator_network_addresses` | `vector<u8>` | New set of `validator_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.                       |
/// | `fullnode_network_addresses`  | `vector<u8>` | New set of `fullnode_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.                        |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                   | Description                                                                                           |
/// | ----------------           | --------------                                 | -------------                                                                                         |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`           | `validator_address` does not have a `ValidatorConfig::ValidatorConfig` resource published under it.   |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EVALIDATOR_OPERATOR`                   | `validator_operator_account` does not have a Validator Operator role.                                 |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_TRANSACTION_SENDER` | `validator_operator_account` is not the registered operator for the validator at `validator_address`. |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_CONSENSUS_KEY`      | `consensus_pubkey` is not a valid ed25519 public key.                                                 |
/// | `Errors::INVALID_STATE`    | `DiemConfig::EINVALID_BLOCK_TIME`             | An invalid time value was encountered in reconfiguration. Unlikely to occur.                          |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::create_validator_operator_account`
/// * `Script::add_validator_and_reconfigure`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::register_validator_config`
Script encode_set_validator_config_and_reconfigure_script(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses);

/// # Summary
/// Updates a validator's configuration, and triggers a reconfiguration of the system to update the
/// validator set with this new validator configuration.  Can only be successfully sent by a
/// Validator Operator account that is already registered with a validator.
///
/// # Technical Description
/// This updates the fields with corresponding names held in the `ValidatorConfig::ValidatorConfig`
/// config resource held under `validator_account`. It then emits a `DiemConfig::NewEpochEvent` to
/// trigger a reconfiguration of the system.  This reconfiguration will update the validator set
/// on-chain with the updated `ValidatorConfig::ValidatorConfig`.
///
/// # Parameters
/// | Name                          | Type         | Description                                                                                                        |
/// | ------                        | ------       | -------------                                                                                                      |
/// | `validator_operator_account`  | `signer`     | Signer of the sending account. Must be the registered validator operator for the validator at `validator_address`. |
/// | `validator_account`           | `address`    | The address of the validator's `ValidatorConfig::ValidatorConfig` resource being updated.                          |
/// | `consensus_pubkey`            | `vector<u8>` | New Ed25519 public key to be used in the updated `ValidatorConfig::ValidatorConfig`.                               |
/// | `validator_network_addresses` | `vector<u8>` | New set of `validator_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.             |
/// | `fullnode_network_addresses`  | `vector<u8>` | New set of `fullnode_network_addresses` to be used in the updated `ValidatorConfig::ValidatorConfig`.              |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                   | Description                                                                                           |
/// | ----------------           | --------------                                 | -------------                                                                                         |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`           | `validator_address` does not have a `ValidatorConfig::ValidatorConfig` resource published under it.   |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EVALIDATOR_OPERATOR`                   | `validator_operator_account` does not have a Validator Operator role.                                 |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_TRANSACTION_SENDER` | `validator_operator_account` is not the registered operator for the validator at `validator_address`. |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::EINVALID_CONSENSUS_KEY`      | `consensus_pubkey` is not a valid ed25519 public key.                                                 |
/// | `Errors::INVALID_STATE`    | `DiemConfig::EINVALID_BLOCK_TIME`             | An invalid time value was encountered in reconfiguration. Unlikely to occur.                          |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::register_validator_config`
TransactionPayload encode_set_validator_config_and_reconfigure_script_function(AccountAddress validator_account, std::vector<uint8_t> consensus_pubkey, std::vector<uint8_t> validator_network_addresses, std::vector<uint8_t> fullnode_network_addresses);

/// # Summary
/// Sets the validator operator for a validator in the validator's configuration resource "locally"
/// and does not reconfigure the system. Changes from this transaction will not picked up by the
/// system until a reconfiguration of the system is triggered. May only be sent by an account with
/// Validator role.
///
/// # Technical Description
/// Sets the account at `operator_account` address and with the specified `human_name` as an
/// operator for the sending validator account. The account at `operator_account` address must have
/// a Validator Operator role and have a `ValidatorOperatorConfig::ValidatorOperatorConfig`
/// resource published under it. The sending `account` must be a Validator and have a
/// `ValidatorConfig::ValidatorConfig` resource published under it. This script does not emit a
/// `DiemConfig::NewEpochEvent` and no reconfiguration of the system is initiated by this script.
///
/// # Parameters
/// | Name               | Type         | Description                                                                                  |
/// | ------             | ------       | -------------                                                                                |
/// | `account`          | `&signer`    | The signer reference of the sending account of the transaction.                              |
/// | `operator_name`    | `vector<u8>` | Validator operator's human name.                                                             |
/// | `operator_account` | `address`    | Address of the validator operator account to be added as the `account` validator's operator. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                          | Description                                                                                                                                                  |
/// | ----------------           | --------------                                        | -------------                                                                                                                                                |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorOperatorConfig::EVALIDATOR_OPERATOR_CONFIG` | The `ValidatorOperatorConfig::ValidatorOperatorConfig` resource is not published under `operator_account`.                                                   |
/// | 0                          | 0                                                     | The `human_name` field of the `ValidatorOperatorConfig::ValidatorOperatorConfig` resource under `operator_account` does not match the provided `human_name`. |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EVALIDATOR`                                   | `account` does not have a Validator account role.                                                                                                            |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::ENOT_A_VALIDATOR_OPERATOR`          | The account at `operator_account` does not have a `ValidatorOperatorConfig::ValidatorOperatorConfig` resource.                                               |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`                  | A `ValidatorConfig::ValidatorConfig` is not published under `account`.                                                                                       |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::create_validator_operator_account`
/// * `Script::register_validator_config`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::add_validator_and_reconfigure`
/// * `Script::set_validator_operator_with_nonce_admin`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_set_validator_operator_script(std::vector<uint8_t> operator_name, AccountAddress operator_account);

/// # Summary
/// Sets the validator operator for a validator in the validator's configuration resource "locally"
/// and does not reconfigure the system. Changes from this transaction will not picked up by the
/// system until a reconfiguration of the system is triggered. May only be sent by an account with
/// Validator role.
///
/// # Technical Description
/// Sets the account at `operator_account` address and with the specified `human_name` as an
/// operator for the sending validator account. The account at `operator_account` address must have
/// a Validator Operator role and have a `ValidatorOperatorConfig::ValidatorOperatorConfig`
/// resource published under it. The sending `account` must be a Validator and have a
/// `ValidatorConfig::ValidatorConfig` resource published under it. This script does not emit a
/// `DiemConfig::NewEpochEvent` and no reconfiguration of the system is initiated by this script.
///
/// # Parameters
/// | Name               | Type         | Description                                                                                  |
/// | ------             | ------       | -------------                                                                                |
/// | `account`          | `signer`     | The signer of the sending account of the transaction.                                        |
/// | `operator_name`    | `vector<u8>` | Validator operator's human name.                                                             |
/// | `operator_account` | `address`    | Address of the validator operator account to be added as the `account` validator's operator. |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                          | Description                                                                                                                                                  |
/// | ----------------           | --------------                                        | -------------                                                                                                                                                |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorOperatorConfig::EVALIDATOR_OPERATOR_CONFIG` | The `ValidatorOperatorConfig::ValidatorOperatorConfig` resource is not published under `operator_account`.                                                   |
/// | 0                          | 0                                                     | The `human_name` field of the `ValidatorOperatorConfig::ValidatorOperatorConfig` resource under `operator_account` does not match the provided `human_name`. |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EVALIDATOR`                                   | `account` does not have a Validator account role.                                                                                                            |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::ENOT_A_VALIDATOR_OPERATOR`          | The account at `operator_account` does not have a `ValidatorOperatorConfig::ValidatorOperatorConfig` resource.                                               |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`                  | A `ValidatorConfig::ValidatorConfig` is not published under `account`.                                                                                       |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::register_validator_config`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator_with_nonce_admin`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_set_validator_operator_script_function(std::vector<uint8_t> operator_name, AccountAddress operator_account);

/// # Summary
/// Sets the validator operator for a validator in the validator's configuration resource "locally"
/// and does not reconfigure the system. Changes from this transaction will not picked up by the
/// system until a reconfiguration of the system is triggered. May only be sent by the Diem Root
/// account as a write set transaction.
///
/// # Technical Description
/// Sets the account at `operator_account` address and with the specified `human_name` as an
/// operator for the validator `account`. The account at `operator_account` address must have a
/// Validator Operator role and have a `ValidatorOperatorConfig::ValidatorOperatorConfig` resource
/// published under it. The account represented by the `account` signer must be a Validator and
/// have a `ValidatorConfig::ValidatorConfig` resource published under it. No reconfiguration of
/// the system is initiated by this script.
///
/// # Parameters
/// | Name               | Type         | Description                                                                                                  |
/// | ------             | ------       | -------------                                                                                                |
/// | `dr_account`       | `&signer`    | The signer reference of the sending account of the write set transaction. May only be the Diem Root signer. |
/// | `account`          | `&signer`    | Signer reference of account specified in the `execute_as` field of the write set transaction.                |
/// | `sliding_nonce`    | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction for Diem Root.                    |
/// | `operator_name`    | `vector<u8>` | Validator operator's human name.                                                                             |
/// | `operator_account` | `address`    | Address of the validator operator account to be added as the `account` validator's operator.                 |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                          | Description                                                                                                                                                  |
/// | ----------------           | --------------                                        | -------------                                                                                                                                                |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                        | A `SlidingNonce` resource is not published under `dr_account`.                                                                                               |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                        | The `sliding_nonce` in `dr_account` is too old and it's impossible to determine if it's duplicated or not.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                        | The `sliding_nonce` in `dr_account` is too far in the future.                                                                                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`               | The `sliding_nonce` in` dr_account` has been previously recorded.                                                                                            |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                        | The sending account is not the Diem Root account or Treasury Compliance account                                                                             |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorOperatorConfig::EVALIDATOR_OPERATOR_CONFIG` | The `ValidatorOperatorConfig::ValidatorOperatorConfig` resource is not published under `operator_account`.                                                   |
/// | 0                          | 0                                                     | The `human_name` field of the `ValidatorOperatorConfig::ValidatorOperatorConfig` resource under `operator_account` does not match the provided `human_name`. |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EVALIDATOR`                                   | `account` does not have a Validator account role.                                                                                                            |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::ENOT_A_VALIDATOR_OPERATOR`          | The account at `operator_account` does not have a `ValidatorOperatorConfig::ValidatorOperatorConfig` resource.                                               |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`                  | A `ValidatorConfig::ValidatorConfig` is not published under `account`.                                                                                       |
///
/// # Related Scripts
/// * `Script::create_validator_account`
/// * `Script::create_validator_operator_account`
/// * `Script::register_validator_config`
/// * `Script::remove_validator_and_reconfigure`
/// * `Script::add_validator_and_reconfigure`
/// * `Script::set_validator_operator`
/// * `Script::set_validator_config_and_reconfigure`
Script encode_set_validator_operator_with_nonce_admin_script(uint64_t sliding_nonce, std::vector<uint8_t> operator_name, AccountAddress operator_account);

/// # Summary
/// Sets the validator operator for a validator in the validator's configuration resource "locally"
/// and does not reconfigure the system. Changes from this transaction will not picked up by the
/// system until a reconfiguration of the system is triggered. May only be sent by the Diem Root
/// account as a write set transaction.
///
/// # Technical Description
/// Sets the account at `operator_account` address and with the specified `human_name` as an
/// operator for the validator `account`. The account at `operator_account` address must have a
/// Validator Operator role and have a `ValidatorOperatorConfig::ValidatorOperatorConfig` resource
/// published under it. The account represented by the `account` signer must be a Validator and
/// have a `ValidatorConfig::ValidatorConfig` resource published under it. No reconfiguration of
/// the system is initiated by this script.
///
/// # Parameters
/// | Name               | Type         | Description                                                                                   |
/// | ------             | ------       | -------------                                                                                 |
/// | `dr_account`       | `signer`     | Signer of the sending account of the write set transaction. May only be the Diem Root signer. |
/// | `account`          | `signer`     | Signer of account specified in the `execute_as` field of the write set transaction.           |
/// | `sliding_nonce`    | `u64`        | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction for Diem Root.      |
/// | `operator_name`    | `vector<u8>` | Validator operator's human name.                                                              |
/// | `operator_account` | `address`    | Address of the validator operator account to be added as the `account` validator's operator.  |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                          | Description                                                                                                                                                  |
/// | ----------------           | --------------                                        | -------------                                                                                                                                                |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                        | A `SlidingNonce` resource is not published under `dr_account`.                                                                                               |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                        | The `sliding_nonce` in `dr_account` is too old and it's impossible to determine if it's duplicated or not.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                        | The `sliding_nonce` in `dr_account` is too far in the future.                                                                                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`               | The `sliding_nonce` in` dr_account` has been previously recorded.                                                                                            |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                        | The sending account is not the Diem Root account or Treasury Compliance account                                                                             |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorOperatorConfig::EVALIDATOR_OPERATOR_CONFIG` | The `ValidatorOperatorConfig::ValidatorOperatorConfig` resource is not published under `operator_account`.                                                   |
/// | 0                          | 0                                                     | The `human_name` field of the `ValidatorOperatorConfig::ValidatorOperatorConfig` resource under `operator_account` does not match the provided `human_name`. |
/// | `Errors::REQUIRES_ROLE`    | `Roles::EVALIDATOR`                                   | `account` does not have a Validator account role.                                                                                                            |
/// | `Errors::INVALID_ARGUMENT` | `ValidatorConfig::ENOT_A_VALIDATOR_OPERATOR`          | The account at `operator_account` does not have a `ValidatorOperatorConfig::ValidatorOperatorConfig` resource.                                               |
/// | `Errors::NOT_PUBLISHED`    | `ValidatorConfig::EVALIDATOR_CONFIG`                  | A `ValidatorConfig::ValidatorConfig` is not published under `account`.                                                                                       |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_validator_account`
/// * `AccountCreationScripts::create_validator_operator_account`
/// * `ValidatorAdministrationScripts::register_validator_config`
/// * `ValidatorAdministrationScripts::remove_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::add_validator_and_reconfigure`
/// * `ValidatorAdministrationScripts::set_validator_operator`
/// * `ValidatorAdministrationScripts::set_validator_config_and_reconfigure`
TransactionPayload encode_set_validator_operator_with_nonce_admin_script_function(uint64_t sliding_nonce, std::vector<uint8_t> operator_name, AccountAddress operator_account);

/// # Summary
/// Mints a specified number of coins in a currency to a Designated Dealer. The sending account
/// must be the Treasury Compliance account, and coins can only be minted to a Designated Dealer
/// account.
///
/// # Technical Description
/// Mints `mint_amount` of coins in the `CoinType` currency to Designated Dealer account at
/// `designated_dealer_address`. The `tier_index` parameter specifies which tier should be used to
/// check verify the off-chain approval policy, and is based in part on the on-chain tier values
/// for the specific Designated Dealer, and the number of `CoinType` coins that have been minted to
/// the dealer over the past 24 hours. Every Designated Dealer has 4 tiers for each currency that
/// they support. The sending `tc_account` must be the Treasury Compliance account, and the
/// receiver an authorized Designated Dealer account.
///
/// ## Events
/// Successful execution of the transaction will emit two events:
/// * A `Diem::MintEvent` with the amount and currency code minted is emitted on the
/// `mint_event_handle` in the stored `Diem::CurrencyInfo<CoinType>` resource stored under
/// `0xA550C18`; and
/// * A `DesignatedDealer::ReceivedMintEvent` with the amount, currency code, and Designated
/// Dealer's address is emitted on the `mint_event_handle` in the stored `DesignatedDealer::Dealer`
/// resource published under the `designated_dealer_address`.
///
/// # Parameters
/// | Name                        | Type      | Description                                                                                                |
/// | ------                      | ------    | -------------                                                                                              |
/// | `CoinType`                  | Type      | The Move type for the `CoinType` being minted. `CoinType` must be an already-registered currency on-chain. |
/// | `tc_account`                | `&signer` | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account.  |
/// | `sliding_nonce`             | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                 |
/// | `designated_dealer_address` | `address` | The address of the Designated Dealer account being minted to.                                              |
/// | `mint_amount`               | `u64`     | The number of coins to be minted.                                                                          |
/// | `tier_index`                | `u64`     | The mint tier index to use for the Designated Dealer account.                                              |
///
/// # Common Abort Conditions
/// | Error Category                | Error Reason                                 | Description                                                                                                                  |
/// | ----------------              | --------------                               | -------------                                                                                                                |
/// | `Errors::NOT_PUBLISHED`       | `SlidingNonce::ESLIDING_NONCE`               | A `SlidingNonce` resource is not published under `tc_account`.                                                               |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_OLD`               | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.                                   |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_NEW`               | The `sliding_nonce` is too far in the future.                                                                                |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_ALREADY_RECORDED`      | The `sliding_nonce` has been previously recorded.                                                                            |
/// | `Errors::REQUIRES_ADDRESS`    | `CoreAddresses::ETREASURY_COMPLIANCE`        | `tc_account` is not the Treasury Compliance account.                                                                         |
/// | `Errors::REQUIRES_ROLE`       | `Roles::ETREASURY_COMPLIANCE`                | `tc_account` is not the Treasury Compliance account.                                                                         |
/// | `Errors::INVALID_ARGUMENT`    | `DesignatedDealer::EINVALID_MINT_AMOUNT`     | `mint_amount` is zero.                                                                                                       |
/// | `Errors::NOT_PUBLISHED`       | `DesignatedDealer::EDEALER`                  | `DesignatedDealer::Dealer` or `DesignatedDealer::TierInfo<CoinType>` resource does not exist at `designated_dealer_address`. |
/// | `Errors::INVALID_ARGUMENT`    | `DesignatedDealer::EINVALID_TIER_INDEX`      | The `tier_index` is out of bounds.                                                                                           |
/// | `Errors::INVALID_ARGUMENT`    | `DesignatedDealer::EINVALID_AMOUNT_FOR_TIER` | `mint_amount` exceeds the maximum allowed amount for `tier_index`.                                                           |
/// | `Errors::REQUIRES_CAPABILITY` | `Diem::EMINT_CAPABILITY`                    | `tc_account` does not have a `Diem::MintCapability<CoinType>` resource published under it.                                  |
/// | `Errors::INVALID_STATE`       | `Diem::EMINTING_NOT_ALLOWED`                | Minting is not currently allowed for `CoinType` coins.                                                                       |
/// | `Errors::LIMIT_EXCEEDED`      | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`      | The depositing of the funds would exceed the `account`'s account limits.                                                     |
///
/// # Related Scripts
/// * `Script::create_designated_dealer`
/// * `Script::peer_to_peer_with_metadata`
/// * `Script::rotate_dual_attestation_info`
Script encode_tiered_mint_script(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress designated_dealer_address, uint64_t mint_amount, uint64_t tier_index);

/// # Summary
/// Mints a specified number of coins in a currency to a Designated Dealer. The sending account
/// must be the Treasury Compliance account, and coins can only be minted to a Designated Dealer
/// account.
///
/// # Technical Description
/// Mints `mint_amount` of coins in the `CoinType` currency to Designated Dealer account at
/// `designated_dealer_address`. The `tier_index` parameter specifies which tier should be used to
/// check verify the off-chain approval policy, and is based in part on the on-chain tier values
/// for the specific Designated Dealer, and the number of `CoinType` coins that have been minted to
/// the dealer over the past 24 hours. Every Designated Dealer has 4 tiers for each currency that
/// they support. The sending `tc_account` must be the Treasury Compliance account, and the
/// receiver an authorized Designated Dealer account.
///
/// # Events
/// Successful execution of the transaction will emit two events:
/// * A `Diem::MintEvent` with the amount and currency code minted is emitted on the
/// `mint_event_handle` in the stored `Diem::CurrencyInfo<CoinType>` resource stored under
/// `0xA550C18`; and
/// * A `DesignatedDealer::ReceivedMintEvent` with the amount, currency code, and Designated
/// Dealer's address is emitted on the `mint_event_handle` in the stored `DesignatedDealer::Dealer`
/// resource published under the `designated_dealer_address`.
///
/// # Parameters
/// | Name                        | Type      | Description                                                                                                |
/// | ------                      | ------    | -------------                                                                                              |
/// | `CoinType`                  | Type      | The Move type for the `CoinType` being minted. `CoinType` must be an already-registered currency on-chain. |
/// | `tc_account`                | `signer`  | The signer of the sending account of this transaction. Must be the Treasury Compliance account.            |
/// | `sliding_nonce`             | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                 |
/// | `designated_dealer_address` | `address` | The address of the Designated Dealer account being minted to.                                              |
/// | `mint_amount`               | `u64`     | The number of coins to be minted.                                                                          |
/// | `tier_index`                | `u64`     | [Deprecated] The mint tier index to use for the Designated Dealer account. Will be ignored                 |
///
/// # Common Abort Conditions
/// | Error Category                | Error Reason                                 | Description                                                                                                                  |
/// | ----------------              | --------------                               | -------------                                                                                                                |
/// | `Errors::NOT_PUBLISHED`       | `SlidingNonce::ESLIDING_NONCE`               | A `SlidingNonce` resource is not published under `tc_account`.                                                               |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_OLD`               | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not.                                   |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_TOO_NEW`               | The `sliding_nonce` is too far in the future.                                                                                |
/// | `Errors::INVALID_ARGUMENT`    | `SlidingNonce::ENONCE_ALREADY_RECORDED`      | The `sliding_nonce` has been previously recorded.                                                                            |
/// | `Errors::REQUIRES_ADDRESS`    | `CoreAddresses::ETREASURY_COMPLIANCE`        | `tc_account` is not the Treasury Compliance account.                                                                         |
/// | `Errors::REQUIRES_ROLE`       | `Roles::ETREASURY_COMPLIANCE`                | `tc_account` is not the Treasury Compliance account.                                                                         |
/// | `Errors::INVALID_ARGUMENT`    | `DesignatedDealer::EINVALID_MINT_AMOUNT`     | `mint_amount` is zero.                                                                                                       |
/// | `Errors::NOT_PUBLISHED`       | `DesignatedDealer::EDEALER`                  | `DesignatedDealer::Dealer` or `DesignatedDealer::TierInfo<CoinType>` resource does not exist at `designated_dealer_address`. |
/// | `Errors::REQUIRES_CAPABILITY` | `Diem::EMINT_CAPABILITY`                    | `tc_account` does not have a `Diem::MintCapability<CoinType>` resource published under it.                                  |
/// | `Errors::INVALID_STATE`       | `Diem::EMINTING_NOT_ALLOWED`                | Minting is not currently allowed for `CoinType` coins.                                                                       |
/// | `Errors::LIMIT_EXCEEDED`      | `DiemAccount::EDEPOSIT_EXCEEDS_LIMITS`      | The depositing of the funds would exceed the `account`'s account limits.                                                     |
///
/// # Related Scripts
/// * `AccountCreationScripts::create_designated_dealer`
/// * `PaymentScripts::peer_to_peer_with_metadata`
/// * `AccountAdministrationScripts::rotate_dual_attestation_info`
TransactionPayload encode_tiered_mint_script_function(TypeTag coin_type, uint64_t sliding_nonce, AccountAddress designated_dealer_address, uint64_t mint_amount, uint64_t tier_index);

/// # Summary
/// Unfreezes the account at `address`. The sending account of this transaction must be the
/// Treasury Compliance account. After the successful execution of this transaction transactions
/// may be sent from the previously frozen account, and coins may be sent and received.
///
/// # Technical Description
/// Sets the `AccountFreezing::FreezingBit` to `false` and emits a
/// `AccountFreezing::UnFreezeAccountEvent`. The transaction sender must be the Treasury Compliance
/// account. Note that this is a per-account property so unfreezing a Parent VASP will not effect
/// the status any of its child accounts and vice versa.
///
/// ## Events
/// Successful execution of this script will emit a `AccountFreezing::UnFreezeAccountEvent` with
/// the `unfrozen_address` set the `to_unfreeze_account`'s address.
///
/// # Parameters
/// | Name                  | Type      | Description                                                                                               |
/// | ------                | ------    | -------------                                                                                             |
/// | `tc_account`          | `&signer` | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `sliding_nonce`       | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                |
/// | `to_unfreeze_account` | `address` | The account address to be frozen.                                                                         |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                |
/// | ----------------           | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`   | The sending account is not the Treasury Compliance account.                                |
///
/// # Related Scripts
/// * `Script::freeze_account`
Script encode_unfreeze_account_script(uint64_t sliding_nonce, AccountAddress to_unfreeze_account);

/// # Summary
/// Unfreezes the account at `address`. The sending account of this transaction must be the
/// Treasury Compliance account. After the successful execution of this transaction transactions
/// may be sent from the previously frozen account, and coins may be sent and received.
///
/// # Technical Description
/// Sets the `AccountFreezing::FreezingBit` to `false` and emits a
/// `AccountFreezing::UnFreezeAccountEvent`. The transaction sender must be the Treasury Compliance
/// account. Note that this is a per-account property so unfreezing a Parent VASP will not effect
/// the status any of its child accounts and vice versa.
///
/// # Events
/// Successful execution of this script will emit a `AccountFreezing::UnFreezeAccountEvent` with
/// the `unfrozen_address` set the `to_unfreeze_account`'s address.
///
/// # Parameters
/// | Name                  | Type      | Description                                                                                     |
/// | ------                | ------    | -------------                                                                                   |
/// | `tc_account`          | `signer`  | The signer of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `sliding_nonce`       | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                      |
/// | `to_unfreeze_account` | `address` | The account address to be frozen.                                                               |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                |
/// | ----------------           | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`   | The sending account is not the Treasury Compliance account.                                |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::freeze_account`
TransactionPayload encode_unfreeze_account_script_function(uint64_t sliding_nonce, AccountAddress to_unfreeze_account);

/// # Summary
/// Updates the Diem consensus config that is stored on-chain and is used by the Consensus.  This
/// transaction can only be sent from the Diem Root account.
///
/// # Technical Description
/// Updates the `DiemConsensusConfig` on-chain config and emits a `DiemConfig::NewEpochEvent` to trigger
/// a reconfiguration of the system.
///
/// # Parameters
/// | Name            | Type          | Description                                                                |
/// | ------          | ------        | -------------                                                              |
/// | `account`       | `signer`      | Signer of the sending account. Must be the Diem Root account.              |
/// | `sliding_nonce` | `u64`         | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction. |
/// | `config`        | `vector<u8>`  | The serialized bytes of consensus config.                                  |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                  | Description                                                                                |
/// | ----------------           | --------------                                | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`       | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                   | `account` is not the Diem Root account.                                                    |
TransactionPayload encode_update_diem_consensus_config_script_function(uint64_t sliding_nonce, std::vector<uint8_t> config);

/// # Summary
/// Updates the Diem major version that is stored on-chain and is used by the VM.  This
/// transaction can only be sent from the Diem Root account.
///
/// # Technical Description
/// Updates the `DiemVersion` on-chain config and emits a `DiemConfig::NewEpochEvent` to trigger
/// a reconfiguration of the system. The `major` version that is passed in must be strictly greater
/// than the current major version held on-chain. The VM reads this information and can use it to
/// preserve backwards compatibility with previous major versions of the VM.
///
/// # Parameters
/// | Name            | Type      | Description                                                                |
/// | ------          | ------    | -------------                                                              |
/// | `account`       | `&signer` | Signer reference of the sending account. Must be the Diem Root account.   |
/// | `sliding_nonce` | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction. |
/// | `major`         | `u64`     | The `major` version of the VM to be used from this transaction on.         |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                  | Description                                                                                |
/// | ----------------           | --------------                                | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`       | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                  | `account` is not the Diem Root account.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `DiemVersion::EINVALID_MAJOR_VERSION_NUMBER` | `major` is less-than or equal to the current major version stored on-chain.                |
Script encode_update_diem_version_script(uint64_t sliding_nonce, uint64_t major);

/// # Summary
/// Updates the Diem major version that is stored on-chain and is used by the VM.  This
/// transaction can only be sent from the Diem Root account.
///
/// # Technical Description
/// Updates the `DiemVersion` on-chain config and emits a `DiemConfig::NewEpochEvent` to trigger
/// a reconfiguration of the system. The `major` version that is passed in must be strictly greater
/// than the current major version held on-chain. The VM reads this information and can use it to
/// preserve backwards compatibility with previous major versions of the VM.
///
/// # Parameters
/// | Name            | Type     | Description                                                                |
/// | ------          | ------   | -------------                                                              |
/// | `account`       | `signer` | Signer of the sending account. Must be the Diem Root account.              |
/// | `sliding_nonce` | `u64`    | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction. |
/// | `major`         | `u64`    | The `major` version of the VM to be used from this transaction on.         |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                                  | Description                                                                                |
/// | ----------------           | --------------                                | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`                | A `SlidingNonce` resource is not published under `account`.                                |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`                | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`                | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED`       | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::EDIEM_ROOT`                   | `account` is not the Diem Root account.                                                    |
/// | `Errors::INVALID_ARGUMENT` | `DiemVersion::EINVALID_MAJOR_VERSION_NUMBER`  | `major` is less-than or equal to the current major version stored on-chain.                |
TransactionPayload encode_update_diem_version_script_function(uint64_t sliding_nonce, uint64_t major);

/// # Summary
/// Update the dual attestation limit on-chain. Defined in terms of micro-XDX.  The transaction can
/// only be sent by the Treasury Compliance account.  After this transaction all inter-VASP
/// payments over this limit must be checked for dual attestation.
///
/// # Technical Description
/// Updates the `micro_xdx_limit` field of the `DualAttestation::Limit` resource published under
/// `0xA550C18`. The amount is set in micro-XDX.
///
/// # Parameters
/// | Name                  | Type      | Description                                                                                               |
/// | ------                | ------    | -------------                                                                                             |
/// | `tc_account`          | `&signer` | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `sliding_nonce`       | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                                |
/// | `new_micro_xdx_limit` | `u64`     | The new dual attestation limit to be used on-chain.                                                       |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                |
/// | ----------------           | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`   | `tc_account` is not the Treasury Compliance account.                                       |
///
/// # Related Scripts
/// * `Script::update_exchange_rate`
/// * `Script::update_minting_ability`
Script encode_update_dual_attestation_limit_script(uint64_t sliding_nonce, uint64_t new_micro_xdx_limit);

/// # Summary
/// Update the dual attestation limit on-chain. Defined in terms of micro-XDX.  The transaction can
/// only be sent by the Treasury Compliance account.  After this transaction all inter-VASP
/// payments over this limit must be checked for dual attestation.
///
/// # Technical Description
/// Updates the `micro_xdx_limit` field of the `DualAttestation::Limit` resource published under
/// `0xA550C18`. The amount is set in micro-XDX.
///
/// # Parameters
/// | Name                  | Type     | Description                                                                                     |
/// | ------                | ------   | -------------                                                                                   |
/// | `tc_account`          | `signer` | The signer of the sending account of this transaction. Must be the Treasury Compliance account. |
/// | `sliding_nonce`       | `u64`    | The `sliding_nonce` (see: `SlidingNonce`) to be used for this transaction.                      |
/// | `new_micro_xdx_limit` | `u64`    | The new dual attestation limit to be used on-chain.                                             |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                |
/// | ----------------           | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`   | `tc_account` is not the Treasury Compliance account.                                       |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::update_exchange_rate`
/// * `TreasuryComplianceScripts::update_minting_ability`
TransactionPayload encode_update_dual_attestation_limit_script_function(uint64_t sliding_nonce, uint64_t new_micro_xdx_limit);

/// # Summary
/// Update the rough on-chain exchange rate between a specified currency and XDX (as a conversion
/// to micro-XDX). The transaction can only be sent by the Treasury Compliance account. After this
/// transaction the updated exchange rate will be used for normalization of gas prices, and for
/// dual attestation checking.
///
/// # Technical Description
/// Updates the on-chain exchange rate from the given `Currency` to micro-XDX.  The exchange rate
/// is given by `new_exchange_rate_numerator/new_exchange_rate_denominator`.
///
/// # Parameters
/// | Name                            | Type      | Description                                                                                                                        |
/// | ------                          | ------    | -------------                                                                                                                      |
/// | `Currency`                      | Type      | The Move type for the `Currency` whose exchange rate is being updated. `Currency` must be an already-registered currency on-chain. |
/// | `tc_account`                    | `&signer` | The signer reference of the sending account of this transaction. Must be the Treasury Compliance account.                          |
/// | `sliding_nonce`                 | `u64`     | The `sliding_nonce` (see: `SlidingNonce`) to be used for the transaction.                                                          |
/// | `new_exchange_rate_numerator`   | `u64`     | The numerator for the new to micro-XDX exchange rate for `Currency`.                                                               |
/// | `new_exchange_rate_denominator` | `u64`     | The denominator for the new to micro-XDX exchange rate for `Currency`.                                                             |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                |
/// | ----------------           | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`   | `tc_account` is not the Treasury Compliance account.                                       |
/// | `Errors::REQUIRES_ROLE`    | `Roles::ETREASURY_COMPLIANCE`           | `tc_account` is not the Treasury Compliance account.                                       |
/// | `Errors::INVALID_ARGUMENT` | `FixedPoint32::EDENOMINATOR`            | `new_exchange_rate_denominator` is zero.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `FixedPoint32::ERATIO_OUT_OF_RANGE`     | The quotient is unrepresentable as a `FixedPoint32`.                                       |
/// | `Errors::LIMIT_EXCEEDED`   | `FixedPoint32::ERATIO_OUT_OF_RANGE`     | The quotient is unrepresentable as a `FixedPoint32`.                                       |
///
/// # Related Scripts
/// * `Script::update_dual_attestation_limit`
/// * `Script::update_minting_ability`
Script encode_update_exchange_rate_script(TypeTag currency, uint64_t sliding_nonce, uint64_t new_exchange_rate_numerator, uint64_t new_exchange_rate_denominator);

/// # Summary
/// Update the rough on-chain exchange rate between a specified currency and XDX (as a conversion
/// to micro-XDX). The transaction can only be sent by the Treasury Compliance account. After this
/// transaction the updated exchange rate will be used for normalization of gas prices, and for
/// dual attestation checking.
///
/// # Technical Description
/// Updates the on-chain exchange rate from the given `Currency` to micro-XDX.  The exchange rate
/// is given by `new_exchange_rate_numerator/new_exchange_rate_denominator`.
///
/// # Parameters
/// | Name                            | Type     | Description                                                                                                                        |
/// | ------                          | ------   | -------------                                                                                                                      |
/// | `Currency`                      | Type     | The Move type for the `Currency` whose exchange rate is being updated. `Currency` must be an already-registered currency on-chain. |
/// | `tc_account`                    | `signer` | The signer of the sending account of this transaction. Must be the Treasury Compliance account.                                    |
/// | `sliding_nonce`                 | `u64`    | The `sliding_nonce` (see: `SlidingNonce`) to be used for the transaction.                                                          |
/// | `new_exchange_rate_numerator`   | `u64`    | The numerator for the new to micro-XDX exchange rate for `Currency`.                                                               |
/// | `new_exchange_rate_denominator` | `u64`    | The denominator for the new to micro-XDX exchange rate for `Currency`.                                                             |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                            | Description                                                                                |
/// | ----------------           | --------------                          | -------------                                                                              |
/// | `Errors::NOT_PUBLISHED`    | `SlidingNonce::ESLIDING_NONCE`          | A `SlidingNonce` resource is not published under `tc_account`.                             |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_OLD`          | The `sliding_nonce` is too old and it's impossible to determine if it's duplicated or not. |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_TOO_NEW`          | The `sliding_nonce` is too far in the future.                                              |
/// | `Errors::INVALID_ARGUMENT` | `SlidingNonce::ENONCE_ALREADY_RECORDED` | The `sliding_nonce` has been previously recorded.                                          |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE`   | `tc_account` is not the Treasury Compliance account.                                       |
/// | `Errors::REQUIRES_ROLE`    | `Roles::ETREASURY_COMPLIANCE`           | `tc_account` is not the Treasury Compliance account.                                       |
/// | `Errors::INVALID_ARGUMENT` | `FixedPoint32::EDENOMINATOR`            | `new_exchange_rate_denominator` is zero.                                                   |
/// | `Errors::INVALID_ARGUMENT` | `FixedPoint32::ERATIO_OUT_OF_RANGE`     | The quotient is unrepresentable as a `FixedPoint32`.                                       |
/// | `Errors::LIMIT_EXCEEDED`   | `FixedPoint32::ERATIO_OUT_OF_RANGE`     | The quotient is unrepresentable as a `FixedPoint32`.                                       |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::update_dual_attestation_limit`
/// * `TreasuryComplianceScripts::update_minting_ability`
TransactionPayload encode_update_exchange_rate_script_function(TypeTag currency, uint64_t sliding_nonce, uint64_t new_exchange_rate_numerator, uint64_t new_exchange_rate_denominator);

/// # Summary
/// Script to allow or disallow minting of new coins in a specified currency.  This transaction can
/// only be sent by the Treasury Compliance account.  Turning minting off for a currency will have
/// no effect on coins already in circulation, and coins may still be removed from the system.
///
/// # Technical Description
/// This transaction sets the `can_mint` field of the `Diem::CurrencyInfo<Currency>` resource
/// published under `0xA550C18` to the value of `allow_minting`. Minting of coins if allowed if
/// this field is set to `true` and minting of new coins in `Currency` is disallowed otherwise.
/// This transaction needs to be sent by the Treasury Compliance account.
///
/// # Parameters
/// | Name            | Type      | Description                                                                                                                          |
/// | ------          | ------    | -------------                                                                                                                        |
/// | `Currency`      | Type      | The Move type for the `Currency` whose minting ability is being updated. `Currency` must be an already-registered currency on-chain. |
/// | `account`       | `&signer` | Signer reference of the sending account. Must be the Diem Root account.                                                             |
/// | `allow_minting` | `bool`    | Whether to allow minting of new coins in `Currency`.                                                                                 |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                          | Description                                          |
/// | ----------------           | --------------                        | -------------                                        |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE` | `tc_account` is not the Treasury Compliance account. |
/// | `Errors::NOT_PUBLISHED`    | `Diem::ECURRENCY_INFO`               | `Currency` is not a registered currency on-chain.    |
///
/// # Related Scripts
/// * `Script::update_dual_attestation_limit`
/// * `Script::update_exchange_rate`
Script encode_update_minting_ability_script(TypeTag currency, bool allow_minting);

/// # Summary
/// Script to allow or disallow minting of new coins in a specified currency.  This transaction can
/// only be sent by the Treasury Compliance account.  Turning minting off for a currency will have
/// no effect on coins already in circulation, and coins may still be removed from the system.
///
/// # Technical Description
/// This transaction sets the `can_mint` field of the `Diem::CurrencyInfo<Currency>` resource
/// published under `0xA550C18` to the value of `allow_minting`. Minting of coins if allowed if
/// this field is set to `true` and minting of new coins in `Currency` is disallowed otherwise.
/// This transaction needs to be sent by the Treasury Compliance account.
///
/// # Parameters
/// | Name            | Type     | Description                                                                                                                          |
/// | ------          | ------   | -------------                                                                                                                        |
/// | `Currency`      | Type     | The Move type for the `Currency` whose minting ability is being updated. `Currency` must be an already-registered currency on-chain. |
/// | `account`       | `signer` | Signer of the sending account. Must be the Diem Root account.                                                                        |
/// | `allow_minting` | `bool`   | Whether to allow minting of new coins in `Currency`.                                                                                 |
///
/// # Common Abort Conditions
/// | Error Category             | Error Reason                          | Description                                          |
/// | ----------------           | --------------                        | -------------                                        |
/// | `Errors::REQUIRES_ADDRESS` | `CoreAddresses::ETREASURY_COMPLIANCE` | `tc_account` is not the Treasury Compliance account. |
/// | `Errors::NOT_PUBLISHED`    | `Diem::ECURRENCY_INFO`               | `Currency` is not a registered currency on-chain.    |
///
/// # Related Scripts
/// * `TreasuryComplianceScripts::update_dual_attestation_limit`
/// * `TreasuryComplianceScripts::update_exchange_rate`
TransactionPayload encode_update_minting_ability_script_function(TypeTag currency, bool allow_minting);

} // end of namespace diem_framework
