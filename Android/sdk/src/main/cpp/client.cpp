//
// Created by hunter on 20-1-14.
//
#include <jni.h>
#include <string>
#include <fstream>
#include <violas_sdk.hpp>
#include "client.h"

#define CLASS_METHOD(x) Java_io_violas_sdk_Client_##x

using namespace std;

std::string jstringToString(JNIEnv *env, jstring str) {

    string t;
    const char *chr = env->GetStringUTFChars(str, 0);

    t = chr;
    env->ReleaseStringUTFChars(str, chr);

    return t;
}

const std::string CLS_JNIEXCEPTION = "java/lang/Exception";

void ThrowJNIException(JNIEnv *env, const std::string &errorMsg) {
    jclass e_cls = env->FindClass(CLS_JNIEXCEPTION.c_str());
    if (e_cls == NULL) {
        std::cerr << "find class:" << CLS_JNIEXCEPTION << " error!" << std::endl;
        return;
    }

    int r = env->ThrowNew(e_cls, errorMsg.c_str());
}

extern "C"
{
/*
* Class:     io_violas_sdk_Client
* Method:    createNativeClient
* Signature: ()I
*/
JNIEXPORT jlong JNICALL CLASS_METHOD(createNativeClient_0002dWAKwML8)
        (JNIEnv *env, jobject,
         jstring host,
         jshort port,
         jstring faucetKey,
         jboolean syncWithWallet,
         jstring faucetServer,
         jstring mnemonic) {

    jlong nativeObj = 0;

    try {
        auto client = Violas::Client::create(jstringToString(env, host),
                                             (unsigned short)port,
                                             "",
                                             jstringToString(env, faucetKey),
                                             syncWithWallet,
                                             jstringToString(env, faucetServer),
                                             jstringToString(env, mnemonic));

        nativeObj = (jlong) new Violas::client_ptr(client);
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }


    return nativeObj;
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    native_test_validator_connection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_native_1test_1validator_1connection
        (JNIEnv *env, jobject, jlong nativeObj) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        client->test_validator_connection();
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCreateNextAccount
 * Signature: ()Lkotlin/Pair;
 */
JNIEXPORT jobject JNICALL Java_io_violas_sdk_Client_nativeCreateNextAccount
        (JNIEnv * env, jobject, jlong nativeObj)
{
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        auto account_info = client->create_next_account(true);
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}
} // the end of extern "C"

