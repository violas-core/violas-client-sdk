package org.libra.client;

public class ClientProxy {
    private long native_client_proxy_ptr;

    private native long nativeClientProxyNew();

    private native void nativeClientProxyDestory(long client_ptr);

    static {
        System.loadLibrary("client_proxy");
    }

    public ClientProxy() {
        native_client_proxy_ptr = nativeClientProxyNew();
    }

    public void Destroy() {
        nativeClientProxyDestory(native_client_proxy_ptr);
    }
}
