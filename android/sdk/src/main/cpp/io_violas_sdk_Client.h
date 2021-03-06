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
 * Method:    createNativeClient_0002dWAKwML8
 * Signature: (Ljava/lang/String;SLjava/lang/String;ZLjava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_createNativeClient_0002dWAKwML8
  (JNIEnv *, jobject, jstring, jshort, jstring, jboolean, jstring, jstring);

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
 * Signature: (J)Lkotlin/Pair;
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
 * Method:    nativeGetBalance_0002d5AcIvR4
 * Signature: (JJ)D
 */
JNIEXPORT jdouble JNICALL Java_io_violas_sdk_Client_nativeGetBalance_0002d5AcIvR4
  (JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetBalance
 * Signature: (J[B)D
 */
JNIEXPORT jdouble JNICALL Java_io_violas_sdk_Client_nativeGetBalance
  (JNIEnv *, jobject, jlong, jbyteArray);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetSequenceNumber_0002d5AcIvR4
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_nativeGetSequenceNumber_0002d5AcIvR4
  (JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeMint_0002dqLd8ryo
 * Signature: (JJJZ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeMint_0002dqLd8ryo
  (JNIEnv *, jobject, jlong, jlong, jlong, jboolean);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeTransfer_0002d3iaSxE4
 * Signature: (JJ[BJJJZ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeTransfer_0002d3iaSxE4
  (JNIEnv *, jobject, jlong, jlong, jbyteArray, jlong, jlong, jlong, jboolean);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCompile_0002dAlou_RY
 * Signature: (JJLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeCompile_0002dAlou_1RY
  (JNIEnv *, jobject, jlong, jlong, jstring, jboolean, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCompile
 * Signature: (J[BLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeCompile
  (JNIEnv *, jobject, jlong, jbyteArray, jstring, jboolean, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativePublishModule_0002dApdV9Xc
 * Signature: (JJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativePublishModule_0002dApdV9Xc
  (JNIEnv *, jobject, jlong, jlong, jstring);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeExecuteScript_0002dP7_O3DU
 * Signature: (JJLjava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeExecuteScript_0002dP7_1O3DU
  (JNIEnv *, jobject, jlong, jlong, jstring, jobjectArray);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetCommittedTxnsByAccSeq_0002dKhFxhuQ
 * Signature: (JJJ)Lkotlin/Pair;
 */
JNIEXPORT jobject JNICALL Java_io_violas_sdk_Client_nativeGetCommittedTxnsByAccSeq_0002dKhFxhuQ
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetCommitedTxnbyRange_0002dqLd8ryo
 * Signature: (JJJZ)[Lkotlin/Pair;
 */
JNIEXPORT jobjectArray JNICALL Java_io_violas_sdk_Client_nativeGetCommitedTxnbyRange_0002dqLd8ryo
  (JNIEnv *, jobject, jlong, jlong, jlong, jboolean);

#ifdef __cplusplus
}
#endif
#endif
