package io.violas.sdk;
//import android.content.res.AssetManager;

import io.violas.sdk.Client;

public class Token {
    private long nativeToken;

    public Token(Client client, byte[] publisherAddress, String tokenName, String scriptFilesPath, String tempPath) {
        nativeToken = nativeCreateTokenManager(client.getNativeClient(), publisherAddress, tokenName, scriptFilesPath,
                tempPath);
    }

    // public Token(Client client, byte[] publisherAddress, String tokenName,
    // AssetManager assetManager, String tempPath){
    // nativeToken = nativeCreateToken1(client.getNativeClient(), publisherAddress,
    // tokenName, assetManager, tempPath);
    // }

    public String name() {
        return nativeName(nativeToken);
    }

    public byte[] address() {
        return nativeAddress(nativeToken);
    }

    public void deploy(long accountIndex) {
        nativeDeploy(nativeToken, accountIndex);
    }

    public void createToken(long supervisor, byte[] owner_address, String token_name) {
        nativeCreateToken(nativeToken, supervisor, owner_address, token_name);
    }

    public void publish(long accountIndex) {
        nativePublish(nativeToken, accountIndex);
    }

    public void mint(long tokenIndex, long accountIndex, byte[] receiverAddress, long amountOfMicroCoins) {
        nativeMint(nativeToken, tokenIndex, accountIndex, receiverAddress, amountOfMicroCoins);
    }

    public void transfer(long tokenIndex, long accountIndex, byte[] receiverAddress, long amountOfMicroCoins) {
        nativeTransfer(nativeToken, tokenIndex, accountIndex, receiverAddress, amountOfMicroCoins);
    }

    public long getBalance(long tokenIndex, long accountIndex) {
        return nativeGetBalance(nativeToken, tokenIndex, accountIndex);
    }

    public long getBalance(long tokenIndex, byte[] address) {
        return nativeGetBalance(nativeToken, tokenIndex, address);
    }

    private native long nativeCreateTokenManager(long client, byte[] publisherAddress, String tokenName,
            String scriptFilesPath, String tempPath);
    // private native long nativeCreateToken1(long client, byte[] publisherAddress,
    // String tokenName, AssetManager assetManager, String tempPath);

    private native String nativeName(long nativeToken);

    private native byte[] nativeAddress(long nativeToken);

    private native void nativeDeploy(long nativeToken, long accountIndex);

    private native void nativePublish(long nativeToken, long accountIndex);

    private native void nativeCreateToken(long nativeToken, long supervisor, byte[] owner_addr, String token_name);

    private native void nativeMint(long tokenIndex, long nativeToken, long accountIndex, byte[] receiverAddress,
            long amountOfMicroCoins);

    private native void nativeTransfer(long tokenIndex, long nativeToken, long accountIndex, byte[] receiverAddress,
            long amountOfMicroCoins);

    private native long nativeGetBalance(long tokenIndex, long nativeToken, long accountIndex);

    private native long nativeGetBalance(long tokenIndex, long nativeToken, byte[] address);

}
