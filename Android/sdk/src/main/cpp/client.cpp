//
// Created by hunter on 20-1-14.
//
#include <jni.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <violas_sdk.hpp>
#include "client.h"

#define CLASS_METHOD(x) Java_io_violas_sdk_Client_##x

using namespace std;
namespace fs = filesystem;

std::string to_string(JNIEnv *env, jstring str) {

    string t;
    const char *chr = env->GetStringUTFChars(str, 0);

    t = chr;
    env->ReleaseStringUTFChars(str, chr);

    return t;
}

std::vector<string> to_string_array(JNIEnv *env, jobjectArray stringArray) {
    vector<string> str_vector;

    int stringCount = env->GetArrayLength(stringArray);

    for (int i = 0; i < stringCount; i++) {
        jstring str = (jstring) (env->GetObjectArrayElement(stringArray, i));
        const char *rawString = env->GetStringUTFChars(str, 0);

        str_vector.push_back(rawString);

        env->ReleaseStringUTFChars(str, rawString);
    }

    return str_vector;
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

Violas::uint256 to_address(JNIEnv *env, jbyteArray _address) {
    Violas::uint256 address;

    jbyte *buffer = env->GetByteArrayElements(_address, 0);
    size_t length = env->GetArrayLength(_address);

    if (length != address.size())
        ThrowJNIException(env, "the size of address is not 32.");

    copy(buffer, buffer + length, begin(address));

    return address;
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
        auto client = Violas::Client::create(to_string(env, host),
                                             (unsigned short) port,
                                             "",
                                             to_string(env, faucetKey),
                                             syncWithWallet,
                                             to_string(env, faucetServer),
                                             to_string(env, mnemonic));

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
JNIEXPORT jdouble JNICALL CLASS_METHOD(nativeGetBalance_0002d5AcIvR4)
        (JNIEnv *env, jobject obj, jlong nativeObj, jlong index) {
    uint64_t balance = 0;

    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        balance = client->get_balance((uint64_t) index);
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
        (JNIEnv *env, jobject obj, jlong nativeObj, jbyteArray _address) {
    uint64_t balance = 0;

    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);
        Violas::uint256 address;

        jbyte *buffer = env->GetByteArrayElements(_address, 0);
        size_t length = env->GetArrayLength(_address);

        if (length != address.size())
            ThrowJNIException(env, "the size of address is not 32.");

        copy(buffer, buffer + length, begin(address));

        balance = client->get_balance(address);
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }

    return balance;
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetSequenceNumber_0002d5AcIvR4
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_io_violas_sdk_Client_nativeGetSequenceNumber_0002d5AcIvR4
        (JNIEnv *env, jobject, jlong nativeObj, jlong account_index) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        return client->get_sequence_number(account_index);
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}
/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeMint_0002dqLd8ryo
 * Signature: (JJJZ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeMint_0002dqLd8ryo
        (JNIEnv *env, jobject obj, jlong nativeObj, jlong index, jlong amount, jboolean block) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        client->mint_coins(index, amount, block);
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}


/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeTransfer_0002d3iaSxE4
 * Signature: (JJ[BJJJZ)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeTransfer_0002d3iaSxE4
        (JNIEnv *env, jobject obj, jlong nativeObj, jlong accountIndex, jbyteArray receiver,
         jlong amount,
         jlong gas_unit_price, jlong max_gas_amount, jboolean is_blocking) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);
//        Violas::uint256 address;
//
//        jbyte *buffer = env->GetByteArrayElements(receiver, 0);
//        size_t length = env->GetArrayLength(receiver);
//
//        if (length != address.size())
//            ThrowJNIException(env, "the size of address is not 32.");
//
//        copy(buffer, buffer + length, begin(address));

        client->transfer_coins_int(accountIndex, to_address(env, receiver),
                                   amount, gas_unit_price, max_gas_amount,
                                   is_blocking);
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCompile_0002dAlou_RY
 * Signature: (JJLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL CLASS_METHOD(nativeCompile_0002dAlou_1RY)
        (JNIEnv *env, jobject, jlong nativeObj,
         jlong accountIndex, jstring scriptFile, jboolean is_module, jstring temp_dir) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        client->compile(accountIndex,
                        to_string(env, scriptFile),
                        is_module, to_string(env, temp_dir));
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeCompile
 * Signature: (J[BLjava/lang/String;ZLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeCompile
        (JNIEnv *env, jobject obj,
         jlong nativeObj,
         jbyteArray address, jstring scriptFile,
         jboolean is_module, jstring tempDir) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        client->compile(to_address(env, address),
                        to_string(env, scriptFile),
                        is_module, to_string(env, tempDir));
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativePublishModule_0002dApdV9Xc
 * Signature: (JJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativePublishModule_0002dApdV9Xc
        (JNIEnv *env, jobject obj,
         jlong nativeObj, jlong account_index, jstring module_file) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        client->publish_module(account_index,
                               to_string(env, module_file));
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeExecuteScript_0002dP7_O3DU
 * Signature: (JJLjava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_io_violas_sdk_Client_nativeExecuteScript_0002dP7_1O3DU
        (JNIEnv *env, jobject, jlong nativeObj, jlong account_index, jstring script_file_name,
         jobjectArray args) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        client->execute_script(account_index,
                               to_string(env, script_file_name),
                               to_string_array(env, args));
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetCommittedTxnsByAccSeq_0002dKhFxhuQ
 * Signature: (JJJ)Lkotlin/Pair;
 */
JNIEXPORT jobject JNICALL Java_io_violas_sdk_Client_nativeGetCommittedTxnsByAccSeq_0002dKhFxhuQ
        (JNIEnv *env, jobject, jlong nativeObj, jlong account_index, jlong sequence_number) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        auto[txn, event] = client->get_committed_txn_by_acc_seq(account_index, sequence_number);

        jclass pairClass = env->FindClass("kotlin/Pair");  //env->FindClass("javafx/util/Pair");
        jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
                                                     "(Ljava/lang/Object;Ljava/lang/Object;)V");

        // Create a new pair object
        jobject pair = env->NewObject(pairClass, pairConstructor,
                                      env->NewStringUTF(txn.c_str()),
                                      env->NewStringUTF(event.c_str()));

        return pair;
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}

/*
 * Class:     io_violas_sdk_Client
 * Method:    nativeGetCommitedTxnbyRange_0002dqLd8ryo
 * Signature: (JJJZ)[Lkotlin/Pair;
 */
JNIEXPORT jobjectArray JNICALL Java_io_violas_sdk_Client_nativeGetCommitedTxnbyRange_0002dqLd8ryo
        (JNIEnv *env, jobject, jlong nativeObj, jlong start_version, jlong limit,
         jboolean fetch_event) {
    try {
        Violas::client_ptr client = *((Violas::client_ptr *) nativeObj);

        auto txn_events = client->get_txn_by_range(start_version, limit,
                                                   fetch_event);

        jclass pairClass = env->FindClass("kotlin/Pair");  //env->FindClass("javafx/util/Pair");
        jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
                                                     "(Ljava/lang/Object;Ljava/lang/Object;)V");

        jobject defaultAccount = env->NewObject(pairClass, pairConstructor,
                                                env->NewStringUTF(""),
                                                env->NewStringUTF(""));

        jobjectArray obj_txn_events = env->NewObjectArray(txn_events.size(), pairClass,
                                                          defaultAccount);

        for (size_t i = 0; i < txn_events.size(); ++i) {
            auto[txn, event] = txn_events[i];

            jobject jobj_txn_event = env->NewObject(pairClass, pairConstructor,
                                             env->NewStringUTF(txn.c_str()),
                                             env->NewStringUTF(event.c_str()));

            env->SetObjectArrayElement(obj_txn_events, i, jobj_txn_event);
        }

        return obj_txn_events;
    }
    catch (exception &e) {
        ThrowJNIException(env, e.what());
    }
}
} // the end of extern "C"

