#[cfg(target_os = "wasm")]
pub mod wasm {
    use client::client_proxy::ClientProxy;
    use std::*;
    use wasm_bindgen::prelude::*;

    #[wasm_bindgen]
    extern "C" {
        pub fn alert(s: &str);
    }

    #[wasm_bindgen]
    pub fn greet(name: &str) {
        alert(&format!("Hello, {}!", name));
    }
}
