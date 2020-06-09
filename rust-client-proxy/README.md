# How to udpate rust code from libra client

1. git merge-file src/client_proxy.rs ../../libra/testsuite/cli/src/client_proxy.rs src/client_proxy.rs
   
   git merge-file src/libra_client.rs ../../libra/testsuite/cli/src/libra_client.rs src/libra_client.rs


2. resolve the conflicts

# Build for Windows on Ubuntu
1. Dependencies

   sudo apt install mingw-w64

   rustup target install x86_64-pc-windows-gnu

2. build

   cargo build --release --target=x86_64-pc-windows-gnu

   ln -s target/x86_64-pc-windows-gnu/release target/lib

# Fix multi currencies issue for Libra source code
## root cause
fun CurrencyInfoResource::resource_path_for constructs resource path with default Address CORE_PATH 
```
async fn currencies_info(
    service: JsonRpcService,
    request: JsonRpcRequest,
) -> Result<Vec<CurrencyInfoView>> {
    let raw_data = service.db.deref().batch_fetch_resources_by_version(
        vec![RegisteredCurrencies::CONFIG_ID.access_path()],
        request.version(),
    )?;
    ensure!(raw_data.len() == 1, "invalid storage result");
    let currencies = RegisteredCurrencies::from_bytes(&raw_data[0])?;
    let access_paths: Vec<_> = currencies
        .currency_codes()
        .iter()
        .map(|code| CurrencyInfoResource::resource_path_for(code.clone()))
        .collect();

    let mut currencies = vec![];
    for raw_data in service
        .db
        .deref()
        .batch_fetch_resources_by_version(access_paths, request.version())?
    {
        let currency_info = CurrencyInfoResource::try_from_bytes(&raw_data)?;
        currencies.push(CurrencyInfoView::from(currency_info));
    }
    Ok(currencies)
}

```
## Fix
libra/storage/storage-interface/src/lib.rs
```
access_paths
    .into_iter()
    .filter(|path| {
        let states = account_states.get(&path.address);
        if states == None {
            true
        } else {
            let blob = states.unwrap().get(&path.path);

            blob == None
        }
    })
    .map(|path| {
        Ok(account_states
            .get(&path.address)
            .ok_or_else(|| format_err!("missing account state for queried access path"))?
            .get(&path.path)
            .ok_or_else(|| format_err!("no value found in account state"))?
            .clone())
    })
    .collect()
```

# Build libra for debugging
```
debug   : cargo build --all --bins --exclude cluster-test -j4
release : cargo build --release --all --bins -j4
target/debug/libra-swarm -c /tmp/libra
``` 