//
// Created by hunter on 20-1-14.
//
#include <jni.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "../include/violas_sdk2.hpp"
#include "../include/jni_violas_sdk.h"

using namespace std;
namespace fs = filesystem;

#ifdef ANDROID
const char *PAIR_CLASS_NAME = "android/util/Pair";
#else
const char *PAIR_CLASS_NAME = "javafx/util/Pair";
#endif

static std::string to_string(JNIEnv *env, jstring str)
{

	string t;
	const char *chr = env->GetStringUTFChars(str, 0);

	t = chr;
	env->ReleaseStringUTFChars(str, chr);

	return t;
}

static std::vector<string> to_string_array(JNIEnv *env, jobjectArray stringArray)
{
	vector<string> str_vector;

	int stringCount = env->GetArrayLength(stringArray);

	for (int i = 0; i < stringCount; i++)
	{
		jstring str = (jstring)(env->GetObjectArrayElement(stringArray, i));
		const char *rawString = env->GetStringUTFChars(str, 0);

		str_vector.push_back(rawString);

		env->ReleaseStringUTFChars(str, rawString);
	}

	return str_vector;
}

static const std::string CLS_JNIEXCEPTION = "java/lang/Exception";

static void ThrowJNIException(JNIEnv *env, const std::string &errorMsg)
{
	jclass e_cls = env->FindClass(CLS_JNIEXCEPTION.c_str());
	if (e_cls == NULL)
	{
		std::cerr << "find class:" << CLS_JNIEXCEPTION << " error!" << std::endl;
		return;
	}

	env->ThrowNew(e_cls, errorMsg.c_str());
}

static violas::Address to_address(JNIEnv *env, jbyteArray _address)
{

	jbyte *buffer = env->GetByteArrayElements(_address, 0);
	size_t length = env->GetArrayLength(_address);

	if (length != violas::ADDRESS_LENGTH)
		ThrowJNIException(env, "the size of address is not 16.");

	violas::Address address;
	copy(buffer, buffer + violas::ADDRESS_LENGTH, begin(address));

	return address;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  native function
////////////////////////////////////////////////////////////////////////////////////////////////////
jlong createNativeClient(JNIEnv *env, jobject,
						 jint chain_id,
						 jstring url,
						 jstring faucetKey,
						 jboolean syncWithWallet,
						 jstring faucetServer,
						 jstring mnemonic,
						 jstring waypoint)
{

	jlong nativeObj = 0;

	try
	{
		auto client = violas::Client::create((uint8_t)chain_id,
											 to_string(env, url),
											 to_string(env, faucetKey),											 									 
											 to_string(env, mnemonic),
											 to_string(env, waypoint));

		nativeObj = (jlong) new violas::client_ptr(client);
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return nativeObj;
}

//
// function:    destroy native vlient
//
void JNICALL destroy_native_client(JNIEnv *env, jobject, jlong nativeObj)
{
	auto native_client = (violas::client_ptr *)nativeObj;
	delete native_client;
}

void test_validator_connection(JNIEnv *env, jobject, jlong nativeObj)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->test_connection();
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

jobject create_next_account(JNIEnv *env, jobject, jlong nativeObj)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		auto account_info = client->create_next_account();

		jclass pairClass = env->FindClass(PAIR_CLASS_NAME);
		jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
													 "(Ljava/lang/Object;Ljava/lang/Object;)V");

		auto data = account_info.second.data();
		size_t length = data.size();
		jbyteArray address = env->NewByteArray(length);
		env->SetByteArrayRegion(address, 0, length, ((jbyte *)data.data()));

		jclass longClass = env->FindClass("java/lang/Long");
		jmethodID longConstruct = env->GetMethodID(longClass, "<init>", "(J)V");
		jobject index = env->NewObject(longClass, longConstruct, account_info.first);

		// Create a new pair object
		jobject pair = env->NewObject(pairClass, pairConstructor, index, address);

		return pair;
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return 0;
}

jobjectArray client_get_all_accounts(JNIEnv *env, jobject obj, jlong nativeObj)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		auto accounts = client->get_all_accounts();

		jclass accountClass = env->FindClass("io/violas/sdk/Client$Account");
		jmethodID init = env->GetMethodID(accountClass, "<init>", "()V");
		jmethodID constructAccount = env->GetMethodID(accountClass, "<init>",
													  "(J[BJJ)V"); //"(J[BJJ)V"
		jobject defaultAccount = env->NewObject(accountClass, init);

		jobjectArray objAccounts = env->NewObjectArray(accounts.size(), accountClass,
													   defaultAccount);

		for (size_t i = 0; i < accounts.size(); ++i)
		{

			auto data = accounts[i].address.data();
			size_t length = data.size();
			jbyteArray address = env->NewByteArray(length);
			env->SetByteArrayRegion(address, 0, length, ((jbyte *)data.data()));

			jobject account = env->NewObject(accountClass, constructAccount,
											 accounts[i].index,
											 address,
											 accounts[i].sequence_number,
											 accounts[i].status);
			env->SetObjectArrayElement(objAccounts, i, account);
		}

		return objAccounts;
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return 0;
}

jdouble client_get_balance(JNIEnv *env, jobject obj, jlong nativeObj, jlong index)
{
	uint64_t balance = 0;

	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		balance = client->get_balance((uint64_t)index);
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return balance;
}

jdouble client_get_balance(JNIEnv *env, jobject obj, jlong nativeObj, jbyteArray _address)
{
	uint64_t balance = 0;

	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);
		//violas::Address address;

		//jbyte *buffer = env->GetByteArrayElements(_address, 0);
		size_t length = env->GetArrayLength(_address);

		if (length != violas::Address::length)
			ThrowJNIException(env, "the size of address is not 32.");

		//copy(buffer, buffer + length, begin(address));

		balance = client->get_balance(to_address(env, _address));
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return balance;
}

jlong client_get_sequence_number(JNIEnv *env, jobject, jlong nativeObj, jlong account_index)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		return client->get_sequence_number(account_index);
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return 0;
}

void client_mint(JNIEnv *env, jobject obj, jlong nativeObj, jlong index, jlong amount, jboolean block)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->mint_coins(index, amount, block);
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

void client_transfer(JNIEnv *env, jobject obj,
					 jlong nativeObj, jlong accountIndex, jbyteArray receiver, jlong amount,
					 jlong gas_unit_price, jlong max_gas_amount, jboolean is_blocking)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->transfer_coins_int(accountIndex, to_address(env, receiver),
								   amount, gas_unit_price, max_gas_amount,
								   is_blocking);
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

void jni_compile(JNIEnv *env, jobject, jlong nativeObj,
				 jlong account_index, jstring script_file_name, jboolean is_module,
				 jstring temp_dir)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->compile(account_index,
						to_string(env, script_file_name),
						is_module,
						to_string(env, temp_dir));
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

void jni_compile(JNIEnv *env, jobject obj, jlong nativeObj,
				 jbyteArray address, jstring scriptFile,
				 jboolean is_module, jstring tempDir)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->compile(to_address(env, address),
						to_string(env, scriptFile),
						is_module, to_string(env, tempDir));
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

void jni_publish_module(JNIEnv *env, jobject obj, jlong nativeObj,
						jlong account_index, jstring module_file)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->publish_module(account_index,
							   to_string(env, module_file));
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

void jni_execute_script(JNIEnv *env, jobject, jlong nativeObj,
						jlong account_index, jstring script_file_name, jobjectArray args)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		client->execute_script(account_index,
							   to_string(env, script_file_name),
							   to_string_array(env, args));
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}
}

jobject jni_get_committed_txn_by_acc_seq(JNIEnv *env, jobject, jlong nativeObj,
										 jlong account_index, jlong sequence_number)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		auto txn = client->get_committed_txn_by_acc_seq(account_index, sequence_number);

		jclass pairClass = env->FindClass(PAIR_CLASS_NAME);
		jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
													 "(Ljava/lang/Object;Ljava/lang/Object;)V");

		// Create a new pair object
		jobject pair = env->NewObject(pairClass, pairConstructor,
									  env->NewStringUTF(txn.c_str()),
									  env->NewStringUTF(""));

		return pair;
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return 0;
}

jobjectArray jni_get_txn_by_range(JNIEnv *env, jobject, jlong nativeObj, jlong start_version, jlong limit,
								  jboolean fetch_event)
{
	try
	{
		violas::client_ptr client = *((violas::client_ptr *)nativeObj);

		auto txn_events = client->get_txn_by_range(start_version, limit,
												   fetch_event);

		jclass pairClass = env->FindClass(PAIR_CLASS_NAME);
		jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>",
													 "(Ljava/lang/Object;Ljava/lang/Object;)V");

		jobject defaultAccount = env->NewObject(pairClass, pairConstructor,
												env->NewStringUTF(""),
												env->NewStringUTF(""));

		jobjectArray obj_txn_events = env->NewObjectArray(txn_events.size(), pairClass,
														  defaultAccount);

		for (size_t i = 0; i < txn_events.size(); ++i)
		{
			auto [txn, event] = txn_events[i];

			jobject jobj_txn_event = env->NewObject(pairClass, pairConstructor,
													env->NewStringUTF(txn.c_str()),
													env->NewStringUTF(event.c_str()));

			env->SetObjectArrayElement(obj_txn_events, i, jobj_txn_event);
		}

		return obj_txn_events;
	}
	catch (exception &e)
	{
		ThrowJNIException(env, e.what());
	}

	return 0;
}

namespace JniTokenManager
{
	uint64_t jni_create_totken_manager(JNIEnv *env, jobject obj,
									   jlong native_client,
									   jbyteArray publisher_address,
									   jstring token_name,
									   jstring script_files_path,
									   jstring temp_path)
	{
		jlong native_token = 0;

		try
		{
			violas::client_ptr client = *((violas::client_ptr *)native_client);

			auto token = violas::TokenManager::create(client,
													  to_address(env, publisher_address),
													  to_string(env, token_name),
													  to_string(env, script_files_path));

			native_token = (jlong) new violas::token_manager_ptr(token);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}

		return native_token;
	}

	uint64_t jni_create_totken_manager(JNIEnv *env, jobject obj,
									   jlong native_client,
									   jbyteArray publisher_address,
									   jstring token_name,
									   function<void(const std::string &)> const &init_all_script_fun,
									   jstring temp_path)
	{
		jlong native_token = 0;

		try
		{
			violas::client_ptr client = *((violas::client_ptr *)native_client);

			auto token = violas::TokenManager::create(client,
													  to_address(env, publisher_address),
													  to_string(env, token_name),
													  init_all_script_fun,
													  to_string(env, temp_path));

			native_token = (jlong) new violas::token_manager_ptr(token);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}

		return native_token;
	}

	jstring jni_name(JNIEnv *env, jobject,
					 long native_token_manager)
	{
		try
		{
			violas::token_manager_ptr token = *((violas::token_manager_ptr *)((intptr_t)native_token_manager));

			return env->NewStringUTF(token->name().c_str());
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}

		return 0;
	}

	jbyteArray jni_address(JNIEnv *env, jobject,
						   long native_token)
	{
		try
		{
			auto token = *((violas::token_manager_ptr *)(intptr_t)native_token);

			auto address = token->address().data();

			size_t length = address.size();
			jbyteArray jaddress = env->NewByteArray(length);
			env->SetByteArrayRegion(jaddress, 0, length, ((jbyte *)address.data()));

			return jaddress;
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}

		return 0;
	}

	void jni_deploy(JNIEnv *env, jobject, jlong native_token, jlong account_index)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token);

			token_manager->deploy(account_index);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}
	}

	void jni_create_token(JNIEnv *env, jobject obj,
						  jlong native_token_manager,
						  jlong supervisor_account_index,
						  jbyteArray ownor_address,
						  jstring token_name)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token_manager);

			token_manager->create_token(supervisor_account_index, to_address(env, ownor_address), to_string(env, token_name));
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}
	}

	void jni_publish(JNIEnv *env, jobject, jlong native_token_manager, jlong account_index)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token_manager);

			token_manager->publish(account_index);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}
	}

	void jni_mint(JNIEnv *env, jobject obj,
				  jlong native_token_manager,
				  jlong token_index,
				  jlong account_index,
				  jbyteArray receiver_address,
				  jlong amount_micro_coins)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token_manager);

			token_manager->mint(token_index,
								account_index,
								to_address(env, receiver_address),
								amount_micro_coins);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}
	}

	void jni_transfer(JNIEnv *env, jobject,
					  jlong native_token_manager,
					  jlong token_index,
					  jlong account_index,
					  jbyteArray receiver_address,
					  jlong amount_micro_coins)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token_manager);

			token_manager->transfer(token_index,
									account_index,
									to_address(env, receiver_address),
									amount_micro_coins);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}
	}

	jlong jni_get_balance(JNIEnv *env, jobject,
						  jlong native_token_manager,
						  jlong token_index,
						  jlong account_index)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token_manager);

			return (jlong)token_manager->get_account_balance(token_index, account_index);
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}

		return 0;
	}

	jlong jni_get_balance(JNIEnv *env, jobject,
						  jlong native_token_manager,
						  jlong token_index,
						  jbyteArray address)
	{
		try
		{
			auto token_manager = *((violas::token_manager_ptr *)native_token_manager);

			return (jlong)token_manager->get_account_balance(token_index, to_address(env, address));
		}
		catch (exception &e)
		{
			ThrowJNIException(env, e.what());
		}

		return 0;
	}
} // namespace JniTokenManager