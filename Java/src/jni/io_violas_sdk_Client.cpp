#include "io_violas_sdk_Client.h"
#include <jni_violas_sdk.h>

#define CLASS_METHOD(x) Java_io_violas_sdk_Client_##x

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     io_violas_sdk_java_Client
 * Method:    createNativeClient
 * Signature: (Ljava/lang/String;SLjava/lang/String;ZLjava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong
JNICALL CLASS_METHOD(createNativeClient)(JNIEnv *env,
                                         jobject obj,
                                         jstring host,
                                         jshort port,
                                         jstring mint_key_file,
                                         jboolean sync_with_wallet,
                                         jstring faucet_server,
                                         jstring mnemonic_file) {
	return createNativeClient(env, obj,
	                          host, 
							  port,
	                          mint_key_file,
	                          sync_with_wallet,
	                          faucet_server,
	                          mnemonic_file);
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    destroyNativeClient
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_destroyNativeClient
  (JNIEnv * env, jobject obj, jlong native_client)
  {
	  destroy_native_client(env, obj, native_client);
  }

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    native_test_validator_connection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL CLASS_METHOD(native_1test_1validator_1connection)
		(JNIEnv *env, jobject obj, jlong native_client) {
	test_validator_connection(env, obj, native_client);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeCreateNextAccount
 * Signature: (J)Landroid/util/Pair;
 */
JNIEXPORT jobject JNICALL CLASS_METHOD(nativeCreateNextAccount)
		(JNIEnv *env, jobject obj, jlong native_client) {
	return create_next_account(env, obj, native_client);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeGetAllAccounts
 * Signature: (J)[Lio/violas/sdk/java/Client/Account;
 */
JNIEXPORT jobjectArray JNICALL CLASS_METHOD(nativeGetAllAccounts)
		(JNIEnv *env, jobject obj, jlong native_client) {
	return client_get_all_accounts(env, obj, native_client);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeGetBalance
 * Signature: (JJ)D
 */
JNIEXPORT jdouble JNICALL CLASS_METHOD(nativeGetBalance__JJ)
		(JNIEnv *env, jobject obj, jlong native_client, jlong account_index) {
	return client_get_balance(env, obj, native_client, account_index);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeGetBalance
 * Signature: (J[B)D
 */
JNIEXPORT jdouble JNICALL CLASS_METHOD(nativeGetBalance__J_3B)
		(JNIEnv *env, jobject obj, jlong native_client, jbyteArray address) {
	return client_get_balance(env, obj, native_client, address);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeGetSequenceNumber
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL CLASS_METHOD(nativeGetSequenceNumber)
		(JNIEnv *env, jobject obj, jlong native_client, jlong account_index) {
	return client_get_sequence_number(env, obj, native_client, account_index);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeMint
 * Signature: (JJJZ)V
 */
JNIEXPORT void JNICALL CLASS_METHOD(nativeMint)
		(JNIEnv *env, jobject obj, jlong native_client, jlong account_index, jlong amount,
		 jboolean is_blocking) {
	return client_mint(env, obj, native_client, account_index, amount, is_blocking);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeTransfer
 * Signature: (JJ[BJJJZ)V
 */
JNIEXPORT void JNICALL CLASS_METHOD(nativeTransfer)
		(JNIEnv *env, jobject obj,
		 jlong native_client, jlong account_index, jbyteArray receiver,
		 jlong amount, jlong gaxUnitPrice, jlong maxGasAccount, jboolean is_blocking) {
	return client_transfer(env, obj, native_client,
	                       account_index, receiver, amount,
	                       gaxUnitPrice, maxGasAccount, is_blocking);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeCompile
 * Signature: (JJLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
CLASS_METHOD(nativeCompile__JJLjava_lang_String_2ZLjava_lang_String_2)
		(JNIEnv * env, jobject obj, jlong native_client, jlong account_index, jstring script_file_name, jboolean is_module, jstring temp_path) {
	return jni_compile(env, obj, native_client, account_index, script_file_name, is_module, temp_path);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeCompile
 * Signature: (J[BLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
CLASS_METHOD(nativeCompile__J_3BLjava_lang_String_2ZLjava_lang_String_2)
		(JNIEnv * env, jobject obj, jlong native_client, jbyteArray address, jstring script_file_name, jboolean is_module, jstring temp_path) {
	return jni_compile(env, obj, native_client, address, script_file_name, is_module, temp_path);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativePublishModule
 * Signature: (JJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL CLASS_METHOD(nativePublishModule)
		(JNIEnv * env, jobject obj, jlong native_client, jlong account_index, jstring module_file_name) {
	return jni_publish_module(env, obj, native_client, account_index, module_file_name);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeExecuteScript
 * Signature: (JJLjava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL CLASS_METHOD(nativeExecuteScript)
		(JNIEnv * env, jobject obj, jlong native_client, jlong account_index, jstring script_file_name, jobjectArray args) {
	return jni_execute_script( env, obj, native_client, account_index, script_file_name, args);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeGetCommittedTxnsByAccSeq
 * Signature: (JJJ)Landroid/util/Pair;
 */
JNIEXPORT jobject JNICALL CLASS_METHOD(nativeGetCommittedTxnsByAccSeq)
		(JNIEnv * env, jobject obj, jlong native_client, jlong account_index, jlong sequence_number) {
	return jni_get_committed_txn_by_acc_seq(env, obj, native_client, account_index, sequence_number);
}

/*
 * Class:     io_violas_sdk_java_Client
 * Method:    nativeGetCommitedTxnbyRange
 * Signature: (JJJZ)[Landroid/util/Pair;
 */
JNIEXPORT jobjectArray JNICALL CLASS_METHOD(nativeGetCommitedTxnbyRange)
		(JNIEnv * env, jobject obj, jlong native_client, jlong start_version, jlong limit, jboolean is_fatch_event) {
	return jni_get_txn_by_range(env, obj, native_client, start_version, limit, is_fatch_event);
}

#ifdef __cplusplus
}
#endif