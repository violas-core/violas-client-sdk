//
// Created by hunter on 20-2-26.
//
#include <jni.h>
#include "jni_violas_sdk.h"

#define CLASS_METHOD(x) Java_io_violas_sdk_Client_##x

extern "C"
{
/*
 * Class:     io_violas_sdk_Client
 * Method:    createNativeClient_0002dWAKwML8
 * Signature: (Ljava/lang/String;SLjava/lang/String;ZLjava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_createNativeClient_0002dWAKwML8
		(JNIEnv *env, jobject obj,
		 jstring host,
		 jshort port,
		 jstring mint_key_file,
		 jboolean sync_with_wallet,
		 jstring faucet_server,
		 jstring mnemonic_file) {
	return createNativeClient(env, obj,
	                          host, port,
	                          mint_key_file,
	                          sync_with_wallet,
	                          faucet_server,
	                          mnemonic_file);
}

}
