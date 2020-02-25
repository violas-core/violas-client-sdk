package io.violas.sdk.java;

import android.net.wifi.WifiManager;
import android.util.Pair;

import java.lang.reflect.Array;

public class Client {
    private long nativeClient;

    Client(String host, Short port, String mintKey, boolean syncOnWallet, String faucetServer, String mnomenicFile) {
        nativeClient = createNativeClient(host, port, mintKey, syncOnWallet, faucetServer, mnomenicFile);
    }

    void test_validator_connection() {
        native_test_validator_connection(nativeClient);
    }

    Pair<Long, Byte[]> createNextAccount() {
        return  nativeCreateNextAccount(nativeClient);
    }

    class Account {
        long index;
        Byte[] address;
        long sequenceNumber;
        long status;

        Account(long _index, Byte[] _address, long _sequenceNum, long _status) {

            index = _index;
            address = _address;
            sequenceNumber = _sequenceNum;
            status = _status;
        }
    }

    Account[] getAllAccounts() {
        return nativeGetAllAccounts(nativeClient);
    }

    Double getBalance(long index) {
        return nativeGetBalance(nativeClient, index);
    }

    Double getBalance(Byte[] address) {
        return nativeGetBalance(nativeClient, address);
    }

    long getSequenceNumber(long accountIndex) {
        return nativeGetSequenceNumber(nativeClient, accountIndex);
    }

    void mint(long index, long amount) {
        nativeMint(nativeClient, index, amount, true);
    }

    void transfer(
            long accountIndex,
            Byte[] receiver,
            long amount) {
        transfer(accountIndex, receiver, amount, (long)0, (long)0, true);
    }

    void transfer(
            long accountIndex,
            Byte[] receiver,
            long amount,
            long gaxUnitPrice,
            long maxGasAccount,
            Boolean isBlocking
    ) {
        nativeTransfer(
                nativeClient,
                accountIndex,
                receiver,
                amount,
                gaxUnitPrice,
                maxGasAccount,
                isBlocking);
    }

    void compile(
            long accountIndex,
            String scriptFile,
            Boolean isModule,
            String tempDir
    ) {
        nativeCompile(nativeClient, accountIndex, scriptFile, isModule, tempDir);
    }

    void compile(
            Byte[] address,
            String scriptFile,
            Boolean isModule,
            String tempDir
    ) {
        nativeCompile(nativeClient, address, scriptFile, isModule, tempDir);
    }

    /// publish a module to Violas blockchain
    void publishModule(long accountIndex, String moduleFile) {
        nativePublishModule(nativeClient, accountIndex, moduleFile);
    }

    /// execute a script on Violas blockchain
    void executeScript(long accountIndex, String scriptFile, String[] args) {
        nativeExecuteScript(nativeClient, accountIndex, scriptFile, args);
    }

    /// get committed transaction and event with JSON format by account and sequence number
    Pair<String, String> getCommittedTxnsByAccSeq(
            long accountIndex,
            long sequence
    ) {
        return nativeGetCommittedTxnsByAccSeq(nativeClient, accountIndex, sequence);
    }

    /// get commited transactions by range
    Pair<String, String>[] getCommitedTxnByRange(
            long start_version,
            long limit,
            Boolean fetchEvent
    ) {
        return nativeGetCommitedTxnbyRange(nativeClient, start_version, limit, fetchEvent);
    }

    //
    //  native fun from violas-sdk-lib.so
    //
    private native long createNativeClient(
            String host,
            Short port,
            String faucetKey,
            Boolean syncOnWallet,
            String faucetServer,
            String mnemonicFile
    );

    private native void native_test_validator_connection(long nativeClient);

    private native Pair<Long, Byte[]> nativeCreateNextAccount(long nativeClient);

    private native Account[] nativeGetAllAccounts(long nativeClient);

    private native Double nativeGetBalance(long nativeClient, long index);

    private native Double nativeGetBalance(long nativeClient, Byte[] address);

    private native long nativeGetSequenceNumber(long nativeClient, long accountIndex);

    private native void nativeMint(
            long nativeClient,
            long accountIndex,
            long amount,
            Boolean is_blocking
    );

    private native void nativeTransfer(
            long nativeClient,
            long accountIndex,
            Byte[] receiver,
            long amount,
            long gaxUnitPrice,
            long maxGasAccount,
            Boolean is_blocking
    );

    private native void nativeCompile(
            long nativeClient,
            long accountIndex,
            String scriptFile,
            Boolean isModule,
            String tempDir
    );

    private native void nativeCompile(
            long nativeClient,
            Byte[] address,
            String scriptFile,
            Boolean isModule,
            String tempDir
    );

    /// publish a module to Violas blockchain
    private native void nativePublishModule(
            long nativeClient,
            long accountIndex,
            String moduleFileName
    );

    /// execute a script on Violas blockchain
    private native void nativeExecuteScript(
            long nativeClient,
            long accountIndex,
            String scriptFile,
            String[] args
    );

    /// get committed transaction and event with JSON format by account and sequence number
    private native Pair<String, String> nativeGetCommittedTxnsByAccSeq(
            long nativeClient,
            long accountIndex,
            long sequence
    );

    /// get commited transactions by range
    private native Pair<String, String>[] nativeGetCommitedTxnbyRange(
            long nativeClient,
            long start_version,
            long limit,
            Boolean fetchEvent
    );
}
