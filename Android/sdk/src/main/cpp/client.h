//
// Created by hunter on 20-1-14.
//

#ifndef VIOLAS_CLIENT_H
#define VIOLAS_CLIENT_H

jlong createNativeClient(JNIEnv *env, jobject,
                         jstring host,
                         jshort port,
                         jstring faucetKey,
                         jboolean syncWithWallet,
                         jstring faucetServer,
                         jstring mnemonic);
//
// Test if the validator has been connected
//
void test_validator_connection(JNIEnv *env, jobject, jlong nativeObj);
//
// Create next account
//
jobject create_next_account(JNIEnv *env, jobject, jlong nativeObj);
//
// Get all account
//
jobjectArray  client_get_all_accounts(JNIEnv *env, jobject obj, jlong nativeObj);
//
//  Get balance by account index
//
jdouble client_get_balance(JNIEnv *env, jobject obj, jlong nativeObj, jlong index);
//
// Get balance by account address
//
jdouble client_get_balance(JNIEnv *env, jobject obj, jlong nativeObj, jbyteArray _address);
//
// Get sequence number by account index
//
jlong client_get_sequence_number(JNIEnv *env, jobject, jlong nativeObj, jlong account_index);
//
// Mint Libra coins by mint key for a account index
//
void client_mint(JNIEnv *env, jobject obj, jlong nativeObj, jlong index, jlong amount, jboolean block);
//
//  Transfer Libra coins to the address of receiver
//
void client_transfer(JNIEnv *env, jobject obj,
                     jlong nativeObj, jlong accountIndex, jbyteArray receiver, jlong amount,
                     jlong gas_unit_price, jlong max_gas_amount, jboolean is_blocking);
//
// compile move script file by account index
//
void jni_compile(JNIEnv *env, jobject, jlong nativeObj,
                 jlong account_index, jstring script_file_name, jboolean is_module, jstring temp_dir);
//
// compile move script file by account address
//
void jni_compile(JNIEnv *env, jobject obj, jlong nativeObj,
                 jbyteArray address, jstring script_file_name, jboolean is_module, jstring tempDir);
//
// Publish module into block chain
//
void jni_publish_module(JNIEnv *env, jobject obj, jlong nativeObj,
                        jlong account_index, jstring module_file);
//
// Execute script by account index
//
void jni_execute_script(JNIEnv *env, jobject, jlong nativeObj,
                        jlong account_index, jstring script_file_name, jobjectArray args);
//
//  Get committed transaction by account sequence
//
jobject jni_get_committed_txn_by_acc_seq(JNIEnv *env, jobject, jlong nativeObj,
                                         jlong account_index, jlong sequence_number);
//
//  Get
//
jobjectArray  jni_get_txn_by_range
		(JNIEnv *env, jobject, jlong nativeObj, jlong start_version, jlong limit,
		 jboolean fetch_event);

#endif //VIOLAS_CLIENT_H
