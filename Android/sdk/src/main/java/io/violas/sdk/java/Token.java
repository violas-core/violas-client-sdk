package io.violas.sdk.java;
import io.violas.sdk.java.Client;

public class Token {
    private long nativeToken;

    public Token(Client client, byte[] publisherAddress, String tokenName, String scriptFilesPath, String tempPath){
        nativeToken = nativeCreateToken(client.getNativeClient(), publisherAddress, tokenName, scriptFilesPath, tempPath);
    }

    public String name() {
        return nativeName(nativeToken);
    }

    public byte[] address() {
        return nativeAddress(nativeToken);
    }

    public  void deploy(long accountIndex) {
        nativeDeploy(nativeToken, accountIndex);
    }

    public  void publish(long accountIndex) {
        nativePublish(nativeToken, accountIndex);
    }

    public void mint(long accountIndex, byte[] receiverAddress, long amountOfMicroCoins) {
        nativeMint(nativeToken, accountIndex, receiverAddress, amountOfMicroCoins);
    }

    public void transfer(long accountIndex, byte[] receiverAddress, long amountOfMicroCoins) {
        nativeTransfer(nativeToken, accountIndex, receiverAddress, amountOfMicroCoins);
    }

    public long getBalance(long accountIndex) {
        return nativeGetBalance(nativeToken, accountIndex);
    }

    public long getBalance(byte[] address) {
        return nativeGetBalance(nativeToken, address);
    }

    private native long nativeCreateToken(long client, byte[] publisherAddress, String tokenName, String scriptFilesPath, String tempPath);
    private native String nativeName(long nativeToken);
    private native byte[] nativeAddress(long nativeToken);
    private native void nativeDeploy(long nativeToken, long accountIndex);
    private native void nativePublish(long nativeToken, long accountIndex);
    private native void nativeMint(long nativeToken, long accountIndex, byte[] receiverAddress, long amountOfMicroCoins);
    private native void nativeTransfer(long nativeToken, long accountIndex, byte[] receiverAddress, long amountOfMicroCoins);
    private native long nativeGetBalance(long nativeToken, long accountIndex);
    private native long nativeGetBalance(long nativeToken, byte[] address);

}
