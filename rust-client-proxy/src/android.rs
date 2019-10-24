/// Expose the JNI interface for android below
#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    //use std::ffi::{CStr, CString};
    //use std::os::raw::c_char;
    use std::*;

    use client::client_proxy::ClientProxy;
    use jni::objects::{JClass, JString};
    use jni::sys::*;
    use jni::JNIEnv;
    // use config::{config::PersistableConfig, trusted_peers::ConfigHelpers};
    // use libra_wallet::io_utils;
    // use proptest::prelude::*;
    // use tools::tempdir::TempPath;

    // #[no_mangle]
    // pub unsafe extern fn Java_com_mozilla_greetings_RustGreetings_greeting(env: JNIEnv, _: JClass, java_pattern: JString) -> jstring {
    //     // Our Java companion code might pass-in "world" as a string, hence the name.
    //     let world = rust_greeting(env.get_string(java_pattern).expect("invalid pattern string").as_ptr());
    //     // Retake pointer so that we can use it below and allow memory to be freed when it goes out of scope.
    //     let world_ptr = CString::from_raw(world);
    //     let output = env.new_string(world_ptr.to_str().unwrap()).expect("Couldn't create java string!");

    //     output.into_inner()
    // }

    #[no_mangle]
    pub extern "system" fn Java_com_example_rustapp_HelloRust_sayHello(
        env: JNIEnv,
        _: JClass,
        input: JString,
    ) -> jstring {
        let input: String = env
            .get_string(input)
            .expect("Couldn't get Java string")
            .into();

        let output = env
            .new_string(format!("Hello, {}!", input))
            .expect("Couldn't create Java String!");

        output.into_inner()
    }

    /*
     * Class:     org_libra_client_ClientProxy
     * Method:    nativeClientProxyNew
     * Signature: ()J
     */
    #[no_mangle]
    pub unsafe extern "system" fn Java_org_libra_client_ClientProxy_nativeClientProxyNew(
        env: JNIEnv,
        _: JClass,
        jhost: JString,
        jport: jshort,
        jvalidator_set_file: JString,
        jfaucet_account_file: JString,
        jsync_on_wallet_recovery: jboolean,
        jfaucet_server: JString,
        jmnemonic_file: JString,
    ) -> jlong {
        //
        //  new ClientProxy without panic
        //
        let ret = panic::catch_unwind(|| {
            //
            // initlize all variables
            //
            let host: String = env
                .get_string(jhost)
                .expect("Couldn't get Java string : host")
                .into();
            let port = jport as u16;
            let validator_set_file: String = env
                .get_string(jvalidator_set_file)
                .expect("Counln't get Java string : validator_set_file")
                .into();
            let faucet_account_file: String = env
                .get_string(jfaucet_account_file)
                .expect("Counln't get Java string : faucet_account_file")
                .into();
            let sync_on_wallet_recovery = jsync_on_wallet_recovery != 0;
            let faucet_server: Option<String> = Some(
                env.get_string(jfaucet_server)
                    .expect("Counln't get Java string : faucet_server")
                    .into(),
            );
            let mnemonic_file: Option<String> = Some(
                env.get_string(jmnemonic_file)
                    .expect("Counln't get Java string : mnemonic_file ")
                    .into(),
            );
            //
            //  new ClientProxy
            //
            ClientProxy::new(
                &host,
                port,
                &validator_set_file,
                &faucet_account_file,
                sync_on_wallet_recovery,
                faucet_server,
                mnemonic_file,
            )
            .unwrap()
        })
        .ok();
        //
        //  Check the result and then return a raw pointer
        //
        let raw_ptr = match ret {
            Some(value) => Box::into_raw(Box::new(value as ClientProxy)) as jlong,
            None => {
                println!("failed to new client proxy ");
                //let exception: JClass = env.find_class("java/lang/Exception");

                0
            }
        };

        raw_ptr
    }

    /*
     * Class:     org_libra_client_ClientProxy
     * Method:    nativeClientProxyDestory
     * Signature: (J)V
     */
    #[no_mangle]
    pub unsafe extern "system" fn Java_org_libra_client_ClientProxy_nativeClientProxyDestory(
        _: JNIEnv,
        _: JClass,
        raw_ptr: jlong,
    ) {
        if (raw_ptr != 0) {
            let _proxy = Box::from_raw(raw_ptr as *mut ClientProxy);
        }
    }
}
