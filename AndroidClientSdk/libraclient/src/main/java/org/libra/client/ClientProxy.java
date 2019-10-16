package org.libra.client;

public class ClientProxy {
    private long native_client_proxy_ptr;

    private native long nativeClientProxyNew(String host,
                                             short port,
                                             String validator_set_file,
                                             String faucet_account_file,
                                             boolean sync_on_wallet_recovery,
                                             String faucet_server,
                                             String mnemonic_file);

    private native void nativeClientProxyDestory(long client_ptr);

    static {
        System.loadLibrary("client_proxy");
    }

    public ClientProxy(String host,
                       short port,
                       String validator_set_file,
                       String faucet_account_file,
                       boolean sync_on_wallet_recovery,
                       String faucet_server,
                       String mnemonic_file) {

        native_client_proxy_ptr = nativeClientProxyNew(
                host,
                port,
                validator_set_file,
                faucet_account_file,
                sync_on_wallet_recovery,
                faucet_server,
                mnemonic_file);
    }

    public void Destroy() {

        nativeClientProxyDestory(native_client_proxy_ptr);
    }
}
