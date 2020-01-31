package io.violas.sdk

class Client {

    var nativeClient: Long = 0

    constructor(
        host: String,
        port: UShort,
        faucetKey: String,
        syncOnWallet: Boolean,
        faucetServer: String,
        mnemonicFile: String
    ) {
        nativeClient =
            createNativeClient(host, port, faucetKey, syncOnWallet, faucetServer, mnemonicFile)
    }

    public fun test_validator_connection(): Unit{
        native_test_validator_connection(nativeClient)
    }

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("violas-sdk-lib")
        }
    }

    //
    //  native fun from violas-sdk-lib.so
    //
    private external fun createNativeClient(
        host: String,
        port: UShort,
        faucetKey: String,
        syncOnWallet: Boolean,
        faucetServer: String,
        mnemonicFile: String
    ): Long

    private external fun native_test_validator_connection(nativeClient: Long): Unit
}
