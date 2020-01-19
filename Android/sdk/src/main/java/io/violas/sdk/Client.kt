package io.violas.sdk

class Client {

    var nativeClient : Long = 0

    constructor(
        host: String,
        port: UShort,
        faucetKey: String,
        syncOnWallet: Boolean,
        faucetServer: String,
        mnemonicFile: String
    ) {
        nativeClient = createNativeClient(host, port, faucetKey, syncOnWallet, faucetServer, mnemonicFile)
    }

    external fun stringFromJNI(): String

    external fun createNativeClient(
        host: String,
        port: UShort,
        faucetKey: String,
        syncOnWallet: Boolean,
        faucetServer: String,
        mnemonicFile: String
    ): Long

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("violas-sdk-lib")
        }
    }
}
