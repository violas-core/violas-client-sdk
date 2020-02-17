package io.violas.sdk

class Client {
    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("violas-sdk-lib")
        }
    }

    val MICRO_LIBRO_COIN: ULong = 1000000.toULong()

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

    fun test_validator_connection(): Unit {
        native_test_validator_connection(nativeClient)
    }

    fun createNextAccount(): Pair<ULong, ByteArray> {
        return nativeCreateNextAccount(nativeClient)
    }

    class Account {
        var index: ULong = 0.toULong()
        var address: ByteArray = ByteArray(32)
        var sequenceNumber: ULong = 0.toULong()
        var status: ULong = 0.toULong()

        constructor(
            _index: ULong, _address: ByteArray, _sequenceNum: ULong, _status: ULong
        ) {

            index = _index
            address = _address
            sequenceNumber = _sequenceNum
            status = _status
        }
    }

    fun getAllAccounts(): Array<Account> {
        return nativeGetAllAccounts(nativeClient);
    }

    fun getBalance(index: ULong): Double {
        return nativeGetBalance(nativeClient, index)
    }

    fun getBalance(address: ByteArray): Double {
        return nativeGetBalance(nativeClient, address)
    }

    fun mint(index: ULong, amount: ULong): Unit {
        return nativeMint(nativeClient, index, amount, true)
    }

    fun transfer(
        accountIndex: ULong,
        receiver: ByteArray,
        amount: ULong,
        gaxUnitPrice: ULong = 0.toULong(),
        maxGasAccount: ULong = 0.toULong(),
        isBlocking: Boolean = true
    ): Unit {
        return nativeTransfer(
            nativeClient,
            accountIndex,
            receiver,
            amount,
            gaxUnitPrice,
            maxGasAccount,
            isBlocking
        )
    }

    fun compile(
        accountIndex: ULong,
        scriptFile: String,
        isModule: Boolean,
        tempDir: String = ""
    ): Unit {
        return nativeCompile(nativeClient, accountIndex, scriptFile, isModule, tempDir)
    }

    fun compile(
        address: ByteArray,
        scriptFile: String,
        isModule: Boolean,
        tempDir: String = ""
    ): Unit {
        return nativeCompile(nativeClient, address, scriptFile, isModule, tempDir)
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

    private external fun nativeCreateNextAccount(nativeClient: Long): Pair<ULong, ByteArray>

    private external fun nativeGetAllAccounts(nativeClient: Long): Array<Account>

    private external fun nativeGetBalance(nativeClient: Long, index: ULong): Double

    private external fun nativeGetBalance(nativeClient: Long, address: ByteArray): Double

    private external fun nativeMint(
        nativeClient: Long,
        accountIndex: ULong,
        amount: ULong,
        is_blocking: Boolean
    ): Unit

    private external fun nativeTransfer(
        nativeClient: Long,
        accountIndex: ULong,
        receiver: ByteArray,
        amount: ULong,
        gaxUnitPrice: ULong,
        maxGasAccount: ULong,
        block: Boolean
    ): Unit

    private external fun nativeCompile(
        nativeClient: Long,
        accountIndex: ULong,
        scriptFile: String,
        isModule: Boolean,
        tempDir: String
    ): Unit

    private external fun nativeCompile(
        nativeClient: Long,
        address: ByteArray,
        scriptFile: String,
        isModule: Boolean,
        tempDir: String
    ): Unit
}
