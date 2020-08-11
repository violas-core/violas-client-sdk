#include "io_violas_sdk_Token.h"
#include <jni_violas_sdk.h>
#include <filesystem>

namespace fs = std::filesystem;

#define CLASS_METHOD(x) Java_io_violas_sdk_Token_##x

#ifdef __cplusplus
extern "C"
{
#endif
  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeCreateToken
 * Signature: (J[BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
  JNIEXPORT jlong JNICALL CLASS_METHOD(nativeCreateTokenManager)(JNIEnv *env,
                                                                 jobject obj,
                                                                 jlong native_token_manager,
                                                                 jbyteArray address,
                                                                 jstring token_name,
                                                                 jstring script_files_path,
                                                                 jstring temp_path)
  {
    return JniTokenManager::jni_create_totken_manager(env, obj, native_token_manager, address, token_name, script_files_path, temp_path);
  }

  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeName
 * Signature: (J)Ljava/lang/String;
 */
  JNIEXPORT jstring JNICALL CLASS_METHOD(nativeName)(JNIEnv *env, jobject obj, jlong native_token_manager)
  {
    return JniTokenManager::jni_name(env, obj, native_token_manager);
  }

  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeAddress
 * Signature: (J)[B
 */
  JNIEXPORT jbyteArray JNICALL CLASS_METHOD(nativeAddress)(JNIEnv *env, jobject obj, jlong native_token_manager)
  {
    return JniTokenManager::jni_address(env, obj, native_token_manager);
  }

  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeDeploy
 * Signature: (JJ)V
 */
  JNIEXPORT void JNICALL CLASS_METHOD(nativeDeploy)(JNIEnv *env, jobject obj, jlong native_token_manager, jlong account_index)
  {
    JniTokenManager::jni_deploy(env, obj, native_token_manager, account_index);
  }

  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativePublish
 * Signature: (JJ)V
 */
  JNIEXPORT void JNICALL CLASS_METHOD(nativePublish)(JNIEnv *env, jobject obj, jlong native_token_manager, jlong account_index)
  {
    JniTokenManager::jni_publish(env, obj, native_token_manager, account_index);
  }

  /*
 * Class:     io_violas_sdk_Token
 * Method:    nativeCreateToken
 * Signature: (JJ[BLjava/lang/String;)V
 */
  JNIEXPORT void JNICALL CLASS_METHOD(nativeCreateToken)(JNIEnv *env, jobject obj, jlong native_token_manager, jlong supervisor, jbyteArray owner_addr, jstring token_name)
  {
    JniTokenManager::jni_create_token(env, obj, native_token_manager, supervisor, owner_addr, token_name);
  }

  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeMint
 * Signature: (JJ[BJ)V
 */
  JNIEXPORT void JNICALL CLASS_METHOD(nativeMint)(JNIEnv *env, jobject obj,
                                                  jlong native_token_manager,
                                                  jlong token_index,
                                                  jlong account_index,
                                                  jbyteArray receiver_address, jlong amount_micro_coins)
  {
    JniTokenManager::jni_mint(env, obj,
                              native_token_manager,
                              token_index,
                              account_index,
                              receiver_address,
                              amount_micro_coins);
  }

  /*
 * Class:     io_violas_sdk_java_Token
 * Method:    nativeTransfer
 * Signature: (JJ[BJ)V
 */
  JNIEXPORT void JNICALL CLASS_METHOD(nativeTransfer)(JNIEnv *env, jobject obj,
                                                      jlong native_token_manager,
                                                      jlong token_index,
                                                      jlong account_index,
                                                      jbyteArray receiver_address,
                                                      jlong amount_micro_coins)
  {
    JniTokenManager::jni_transfer(env, obj,
                                  native_token_manager,
                                  token_index,
                                  account_index,
                                  receiver_address,
                                  amount_micro_coins);
  }

  /*
 * Class:     io_violas_sdk_Token
 * Method:    nativeGetBalance
 * Signature: (JJJ)J
 */

  JNIEXPORT jlong JNICALL CLASS_METHOD(nativeGetBalance__JJJ)(JNIEnv *env, jobject obj,
                                                              jlong native_token_manager,
                                                              jlong token_index,
                                                              jlong account_index)
  {
    return JniTokenManager::jni_get_balance(env, obj, native_token_manager, token_index, account_index);
  }

  /*
 * Class:     io_violas_sdk_Token
 * Method:    nativeGetBalance
 * Signature: (JJ[B)J
 */

  JNIEXPORT jlong JNICALL CLASS_METHOD(nativeGetBalance__JJ_3B)(JNIEnv *env, jobject obj,
                                                                jlong native_token_manager,
                                                                jlong token_index,
                                                                jbyteArray address)
  {
    return JniTokenManager::jni_get_balance(env, obj,
                                            native_token_manager,
                                            token_index,
                                            address);
  }

#ifdef __cplusplus
}
#endif