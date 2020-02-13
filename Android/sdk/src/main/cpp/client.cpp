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

    env->ThrowNew(e_cls, errorMsg.c_str());
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
                                             (unsigned short) port,
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
        (JNIEnv *env, jobject, jlong nativeObj) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        auto account_info = client->create_next_account(true);

        jclass pairClass = env->FindClass("kotlin/Pair");  //env->FindClass("javafx/util/Pair");
        jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
                                                     "(Ljava/lang/Object;Ljava/lang/Object;)V");   //"(Ljava/lang/Object;Ljava/lang/Object;)V")

        size_t length = account_info.second.size();
        jbyteArray address = env->NewByteArray(length);
        env->SetByteArrayRegion(address, 0, length, ((jbyte *) account_info.second.data()));

        jclass longClass = env->FindClass("kotlin/ULong");
        //jmethodID doubleValueOf = env->GetStaticMethodID(doubleClass, "<init>", "()V");
        jmethodID longConstruct = env->GetMethodID(longClass, "<init>", "(J)V");
        //jobject index = env->CallStaticObjectMethod(longClass, doubleValueOf, account_info.first);
        jobject index = env->NewObject(longClass, longConstruct, 0);

        // Create a new pair object
        jobject pair = env->NewObject(pairClass, pairConstructor, index, address);

        return pair;
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetAllAccounts
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL CLASS_METHOD(nativeGetAllAccounts)
        (JNIEnv *env, jobject obj, jlong nativeObj) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        auto accounts = client->get_all_accounts();

//    jclass arrayListClass = env->FindClass("kotlin/Array");  //env->FindClass("javafx/util/Pair");
//    env->GetMethodID(arrayListClass, "<init>", "()V");
//    jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
//                                                 "(Ljava/lang/Object;Ljava/lang/Object;)V");

        jclass accountClass = env->FindClass("io/violas/sdk/Client$Account");
        jmethodID init = env->GetMethodID(accountClass, "<init>", "()V");
        jmethodID constructAccount = env->GetMethodID(accountClass, "<init>", "(J[BJJ)V");
        jobject defaultAccount = env->NewObject(accountClass, init);

        jobjectArray objAccounts = env->NewObjectArray(accounts.size(), accountClass,
                                                       defaultAccount);

        for (int i = 0; i < accounts.size(); ++i) {

            size_t length = accounts[i].address.size();
            jbyteArray address = env->NewByteArray(length);
            env->SetByteArrayRegion(address, 0, length, ((jbyte *) accounts[i].address.data()));

            jobject account = env->NewObject(accountClass, constructAccount,
                                             accounts[i].index,
                                             address,
                                             accounts[i].sequence_number,
                                             accounts[i].status);
            env->SetObjectArrayElement(objAccounts, i, account);
        }

        return objAccounts;
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetBalance_0002d5AcIvR4
 * Signature: (JJ)D
 */
JNIEXPORT jdouble JNICALL Java_io_violas_sdk_Client_nativeGetBalance_0002d5AcIvR4
        (JNIEnv * env, jobject obj, jlong, jlong)
{
    uint64_t balance = 0;

    try {

    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }

    return balance;
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetBalance
 * Signature: (J[B)D
 */
JNIEXPORT jdouble JNICALL Java_io_violas_sdk_Client_nativeGetBalance
        (JNIEnv * env, jobject obj, jlong, jbyteArray)
{
    uint64_t balance = 0;

    try {

    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }

    return balance;
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeMint_0002dqLd8ryo
 * Signature: (JJJZ)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_nativeMint_0002dqLd8ryo
        (JNIEnv * env, jobject obj, jlong, jlong, jlong, jboolean)
{

}

} // the end of extern "C"

