#![forbid(unsafe_code)]

use move_lang::shared::*;

pub fn compile(
    source_files: &Vec<String>,
    dependencies: Vec<String>,
    sender: Option<Address>,
    out_dir: String,
    emit_source_map: bool
) -> std::io::Result<()> {
    let (files, compiled_units) = move_lang::move_compile(&source_files, &dependencies, sender)?;
    move_lang::output_compiled_units(emit_source_map, files, compiled_units, &out_dir)
}
