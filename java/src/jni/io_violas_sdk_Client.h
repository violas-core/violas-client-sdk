/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class io_violas_sdk_Client */

#ifndef _Included_io_violas_sdk_Client
#define _Included_io_violas_sdk_Client
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     io_violas_sdk_Client
 * Method:    createNativeClient
 * Signature: (Ljava/lang/String;SLjava/lang/String;ZLjava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_createNativeClient
  (JNIEnv *, jobject, jstring, jstring, jboolean, jstring, jstring, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    destroyNativeClient
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_destroyNativeClient
  (JNIEnv *, jobject, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    native_test_validator_connection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_native_1test_1validator_1connection
  (JNIEnv *, jobject, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCreateNextAccount
 * Signature: (J)Ljavafx/util/Pair;
 */
JNIEXPORT jobject JNICALL Java_io_violas_sdk_Client_nativeCreateNextAccount
  (JNIEnv *, jobject, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetAllAccounts
 * Signature: (J)[Lio/violas/sdk/Client/Account;
 */
JNIEXPORT jobjectArray JNICALL Java_io_violas_sdk_Client_nativeGetAllAccounts
  (JNIEnv *, jobject, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetBalance
 * Signature: (JJ)D
 */
JNIEXPORT jdouble JNICALL Java_io_violas_sdk_Client_nativeGetBalance__JJ
  (JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetBalance
 * Signature: (J[B)D
 */
JNIEXPORT jdouble JNICALL Java_io_violas_sdk_Client_nativeGetBalance__J_3B
  (JNIEnv *, jobject, jlong, jbyteArray);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetSequenceNumber
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_nativeGetSequenceNumber
  (JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeMint
 * Signature: (JJJZ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeMint
  (JNIEnv *, jobject, jlong, jlong, jlong, jboolean);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeTransfer
 * Signature: (JJ[BJJJZ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeTransfer
  (JNIEnv *, jobject, jlong, jlong, jbyteArray, jlong, jlong, jlong, jboolean);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCompile
 * Signature: (JJLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeCompile__JJLjava_lang_String_2ZLjava_lang_String_2
  (JNIEnv *, jobject, jlong, jlong, jstring, jboolean, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCompile
 * Signature: (J[BLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeCompile__J_3BLjava_lang_String_2ZLjava_lang_String_2
  (JNIEnv *, jobject, jlong, jbyteArray, jstring, jboolean, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativePublishModule
 * Signature: (JJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativePublishModule
  (JNIEnv *, jobject, jlong, jlong, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeExecuteScript
 * Signature: (JJLjava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeExecuteScript
  (JNIEnv *, jobject, jlong, jlong, jstring, jobjectArray);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetCommittedTxnsByAccSeq
 * Signature: (JJJ)Ljavafx/util/Pair;
 */
JNIEXPORT jobject JNICALL Java_io_violas_sdk_Client_nativeGetCommittedTxnsByAccSeq
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetCommitedTxnbyRange
 * Signature: (JJJZ)[Ljavafx/util/Pair;
 */
JNIEXPORT jobjectArray JNICALL Java_io_violas_sdk_Client_nativeGetCommitedTxnbyRange
  (JNIEnv *, jobject, jlong, jlong, jlong, jboolean);

#ifdef __cplusplus
}
#endif
#endif
