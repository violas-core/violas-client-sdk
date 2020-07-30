use cpp_build;

fn main() {
    let include_path = "src/ffi";
    cpp_build::Config::new()
        .include(include_path)
        .flag("-std=c++17")
        .build("src/ffi/mod.rs");
}
