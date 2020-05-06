//
// Created by hunter on 20-3-2.
//
#include <jni.h>
#include <vector>
#include <fstream>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <jni_violas_sdk.h>

extern "C" {

void init_all_script(JNIEnv *env, jobject obj, jobject assetManager, const std::string & script_path ) {
	AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
	AAssetDir *top_dir = AAssetManager_openDir(mgr, ""); //open the top-level directory

	if (!top_dir)
		return;

	using namespace std;

	vector<char> buffer;

	for (const char *file_name = AAssetDir_getNextFileName(top_dir);
	     file_name != nullptr;
	     file_name = AAssetDir_getNextFileName(top_dir)) {

		AAsset *file = AAssetManager_open(mgr, file_name, AASSET_MODE_BUFFER);
		size_t length = AAsset_getLength64(file);

		buffer.resize(length);
		AAsset_read(file, buffer.data(), length);

		std::ofstream(script_path+"/" + file_name).write(buffer.data(), buffer.size());
		AAsset_close(file);
	}

	AAssetDir_close(top_dir);

}
/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeCreateToken1
 * Signature: (J[BLjava/lang/String;Landroid/content/res/AssetManager;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_java_Token_nativeCreateToken1
		(JNIEnv *env, jobject obj,
				jlong native_client,
				jbyteArray publisher_address,
				jstring token_name,
				jobject assetMgr,
				jstring temp_path) {

	auto fun = [=] (const std::string &script_path) {
		init_all_script(env, obj, assetMgr, script_path );
	};

	return JniTokenManager::jni_create_totken_manager(env, obj, native_client, publisher_address, token_name, fun, temp_path) ;
}
/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeCreateToken
 * Signature: (J[BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_java_Token_nativeCreateToken
		(JNIEnv *env, jobject obj, jlong native_client, jbyteArray publisher_address,
		 jstring token_name, jstring script_files_path, jstring temp_path) {
	return JniTokenManager::jni_create_totken_manager(env, obj, native_client, publisher_address,
	                                            token_name, script_files_path, temp_path);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeName
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_io_violas_sdk_java_Token_nativeName
		(JNIEnv *env, jobject obj, jlong native_token) {
	return JniTokenManager::jni_name(env, obj, native_token);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeAddress
 * Signature: (J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_io_violas_sdk_java_Token_nativeAddress
		(JNIEnv *env, jobject obj, jlong native_token) {
	return JniTokenManager::jni_address(env, obj, native_token);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeDeploy
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_java_Token_nativeDeploy
		(JNIEnv *env, jobject obj, jlong native_token, jlong account_index) {
	JniTokenManager::jni_deploy(env, obj, native_token, account_index);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativePublish
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_java_Token_nativePublish
		(JNIEnv *env, jobject obj, jlong native_token, jlong account_index) {
	JniTokenManager::jni_publish(env, obj, native_token, account_index);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeMint
 * Signature: (JJ[BJ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_java_Token_nativeMint
		(JNIEnv *env, jobject obj, jlong native_token, jlong account_index,
		 jbyteArray receiver_address, jlong amount_micro_coins) {
	JniTokenManager::jni_mint(env, obj, native_token, 0, account_index, receiver_address,
	                            amount_micro_coins);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeTransfer
 * Signature: (JJ[BJ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_java_Token_nativeTransfer
		(JNIEnv *env, jobject obj, jlong native_token, jlong account_index,
		 jbyteArray receiver_address, jlong amount_micro_coins) {
	JniTokenManager::jni_mint(env, obj, native_token, 0, account_index, receiver_address,
	                            amount_micro_coins);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeGetBalance
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_java_Token_nativeGetBalance__JJ
		(JNIEnv *env, jobject obj, jlong native_token, jlong account_index) {
	return JniTokenManager::jni_get_balance(env, obj, native_token, 0, account_index);
}

/*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeGetBalance
 * Signature: (J[B)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_java_Token_nativeGetBalance__J_3B
		(JNIEnv *env, jobject obj, jlong native_token, jbyteArray address) {
	return JniTokenManager::jni_get_balance(env, obj, native_token, 0, address);
}


} // end of extern "C"