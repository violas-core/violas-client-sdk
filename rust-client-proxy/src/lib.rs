#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}

extern crate jni;
//extern crate client;

use std::os::raw::{c_char};
use std::ffi::{CString, CStr};

  /// Expose the JNI interface for android below
#[cfg(target_os="android")]
#[allow(non_snake_case)]
pub mod android {
    
    use super::*;
    use self::jni::JNIEnv;
    use self::jni::objects::{JClass, JString};
    use self::jni::sys::{jstring};
    use client::{ client_proxy, libra_wallet };

    use client_proxy::{parse_bool, AddressAndIndex, ClientProxy};
    // use config::{config::PersistableConfig, trusted_peers::ConfigHelpers};
    // use libra_wallet::io_utils;
    // use proptest::prelude::*;
    // use tools::tempdir::TempPath;

    #[no_mangle]
    pub unsafe extern fn Java_com_mozilla_greetings_RustGreetings_greeting(env: JNIEnv, _: JClass, java_pattern: JString) -> jstring {
        // Our Java companion code might pass-in "world" as a string, hence the name.
        let world = rust_greeting(env.get_string(java_pattern).expect("invalid pattern string").as_ptr());
        // Retake pointer so that we can use it below and allow memory to be freed when it goes out of scope.
        let world_ptr = CString::from_raw(world);
        let output = env.new_string(world_ptr.to_str().unwrap()).expect("Couldn't create java string!");

        output.into_inner()
    }

    #[no_mangle]
    pub extern "system" fn Java_com_example_rustapp_HelloRust_sayHello(env: JNIEnv, _: JClass, input: JString) -> jstring {
        let input : String = env.get_string(input).expect("Couldn't get Java string").into();

        let output = env.new_string(format!("Hello, {}!", input)).expect("Couldn't create Java String!");

        output.into_inner()
    }

    #[no_mangle]
    pub extern "system" fn Java_org_libra_client_ClientProxy_nativeClientProxyNew(env: JNIEnv, cls : JClass) -> jlong
    {
        //let global_ref = env.new_global_ref(callback).unwrap();
        //let counter = client_proxy::new();

        let mut accounts = Vec::new();
        accounts.reserve(count);
        let file = TempPath::new();
        let mnemonic_path = file.path().to_str().unwrap().to_string();
        let consensus_peer_file = TempPath::new();
        let consensus_peers_path = consensus_peer_file.path();
        let (_, consensus_peers_config, _) = ConfigHelpers::gen_validator_nodes(1, None);
        consensus_peers_config.save_config(&consensus_peers_path);
        let val_set_file = consensus_peers_path.to_str().unwrap().to_string();

        // We don't need to specify host/port since the client won't be used to connect, only to
        // generate random accounts
        let mut client_proxy = ClientProxy::new(
            "", /* host */
            0,  /* port */
            &val_set_file,
            &"",
            false,
            None,
            Some(mnemonic_path),
        )
        .unwrap();

        Box::into_raw(Box::new(client_proxy)) as jlong
    }
}