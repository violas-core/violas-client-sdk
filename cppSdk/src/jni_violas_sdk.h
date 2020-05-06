//
// Created by hunter on 20-1-14.
//

#ifndef VIOLAS_JNI_VIOLAS_SDK_H
#define VIOLAS_JNI_VIOLAS_SDK_H

#include <functional>

//
//	Create native clinet
//
jlong createNativeClient(JNIEnv *env, jobject,
						 jstring url,						 
						 jstring faucetKey,
						 jboolean syncWithWallet,
						 jstring faucetServer,
						 jstring mnemonic,
						 jstring waypoint);

//
// function:    destroy native vlient
//
void JNICALL destroy_native_client(JNIEnv *env, jobject, jlong nativeObj);

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
jobjectArray client_get_all_accounts(JNIEnv *env, jobject obj, jlong nativeObj);

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
				 jlong account_index, jstring script_file_name, jboolean is_module,
				 jstring temp_dir);

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
jobjectArray jni_get_txn_by_range(JNIEnv *env, jobject, jlong nativeObj, jlong start_version, jlong limit,
								  jboolean fetch_event);

namespace JniTokenManager
{
uint64_t jni_create_totken_manager(JNIEnv *env, jobject,
								   jlong native_client, jbyteArray publisher_address,
								   jstring token_name, jstring script_files_path, jstring temp_path);

uint64_t jni_create_totken_manager(JNIEnv *env,
								   jobject obj,
								   jlong native_client,
								   jbyteArray publisher_address,
								   jstring token_name,
								   std::function<void(const std::string &)> const &init_all_script_fun,
								   jstring temp_path);

jstring jni_name(JNIEnv *env, jobject,
				 long naitve_token_manager);

jbyteArray jni_address(JNIEnv *env, jobject,
					   long native_token_manager);

void jni_deploy(JNIEnv *env, jobject, jlong native_token_manager, jlong account_index);

void jni_create_token(JNIEnv *env, jobject obj,
					  jlong native_token_manager,
					  jlong supervisor_account_index,
					  jbyteArray ownor_address,
					  jstring token_name);

void jni_publish(JNIEnv *env, jobject, jlong native_token_manager, jlong account_index);

void jni_mint(JNIEnv *env, jobject obj,
			  jlong native_token_manager,
			  jlong token_index,
			  jlong account_index,
			  jbyteArray receiver_address,
			  jlong amount_micro_coins);

void jni_transfer(JNIEnv *env, jobject obj,
				  jlong native_token_manager,
				  jlong token_index,
				  jlong account_index,
				  jbyteArray receiver_address,
				  jlong amount_micro_coins);

jlong jni_get_balance(JNIEnv *env, jobject obj,
					  jlong native_token_manager,
					  jlong token_index,
					  jlong account_index);

jlong jni_get_balance(JNIEnv *env, jobject obj,
					  jlong native_token_manager,
					  jlong token_index,
					  jbyteArray address);
} // namespace JniTokenManager
#endif //VIOLAS_JNI_VIOLAS_SDK_H
