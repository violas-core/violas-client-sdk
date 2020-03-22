package io.violas.sdk;

import javafx.util.Pair;
import java.lang.reflect.Array;

public class Client {
    static {
        System.loadLibrary("violas_sdk_jni");
    }

    private long nativeClient;

    public static long MICRO_LIBRO_COIN = 1000000;

    public Client(String host,
                  Short port,
                  String mintKey,
                  boolean syncOnWallet,
                  String faucetServer,
                  String mnomenicFile) {
        nativeClient = createNativeClient(host, port, mintKey, syncOnWallet, faucetServer, mnomenicFile);
    }

    public long getNativeClient() {
        return nativeClient;
    }
    public void test_validator_connection() {
        native_test_validator_connection(nativeClient);
    }

    public Pair<Long, byte[]> createNextAccount() {
        return nativeCreateNextAccount(nativeClient);
    }

    static public class Account {
        public long index;
        public byte[] address;
        public long sequenceNumber;
        public long status;
        
        public Account() {
            index =0;
            address = new byte[0];
            sequenceNumber = 0;
            status = 0;
        }
        
        public Account(long _index, byte[] _address, long _sequenceNum, long _status
        ) {
            index = _index;
            address = _address;
            sequenceNumber = _sequenceNum;
            status = _status;
        }
    }

    public Account[] getAllAccounts() {
        return nativeGetAllAccounts(nativeClient);
    }

    public double getBalance(long index) {
        return nativeGetBalance(nativeClient, index);
    }

    public double getBalance(byte[] address) {
        return nativeGetBalance(nativeClient, address);
    }

    public long getSequenceNumber(long accountIndex) {
        return nativeGetSequenceNumber(nativeClient, accountIndex);
    }

    public void mint(long index, long amount) {
        nativeMint(nativeClient, index, amount, true);
    }

    public void transfer(
            long accountIndex,
            byte[] receiver,
            long amount) {
        transfer(accountIndex, receiver, amount, (long) 0, (long) 0, true);
    }

    public void transfer(
            long accountIndex,
            byte[] receiver,
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

    public void compile(
            long accountIndex,
            String scriptFile,
            Boolean isModule,
            String tempDir
    ) {
        nativeCompile(nativeClient, accountIndex, scriptFile, isModule, tempDir);
    }

    public void compile(
            byte[] address,
            String scriptFile,
            Boolean isModule,
            String tempDir
    ) {
        nativeCompile(nativeClient, address, scriptFile, isModule, tempDir);
    }

    /// publish a module to Violas blockchain
    public void publishModule(long accountIndex, String moduleFile) {
        nativePublishModule(nativeClient, accountIndex, moduleFile);
    }

    /// execute a script on Violas blockchain
    public void executeScript(long accountIndex, String scriptFile, String[] args) {
        nativeExecuteScript(nativeClient, accountIndex, scriptFile, args);
    }

    /// get committed transaction and event with JSON format by account and sequence number
    public Pair<String, String> getCommittedTxnsByAccSeq(
            long accountIndex,
            long sequence
    ) {
        return nativeGetCommittedTxnsByAccSeq(nativeClient, accountIndex, sequence);
    }

    /// get commited transactions by range
    public Pair<String, String>[] getCommitedTxnByRange(
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
            short port,
            String faucetKey,
            boolean syncOnWallet,
            String faucetServer,
            String mnemonicFile
    );

    private native void native_test_validator_connection(long nativeClient);

    private native Pair<Long, byte[]> nativeCreateNextAccount(long nativeClient);

    private native Account[] nativeGetAllAccounts(long nativeClient);

    private native double nativeGetBalance(long nativeClient, long index);

    private native double nativeGetBalance(long nativeClient, byte[] address);

    private native long nativeGetSequenceNumber(long nativeClient, long accountIndex);

    private native void nativeMint(
            long nativeClient,
            long accountIndex,
            long amount,
            boolean is_blocking
    );

    private native void nativeTransfer(
            long nativeClient,
            long accountIndex,
            byte[] receiver,
            long amount,
            long gaxUnitPrice,
            long maxGasAccount,
            boolean is_blocking
    );

    private native void nativeCompile(
            long nativeClient,
            long accountIndex,
            String scriptFile,
            boolean isModule,
            String tempDir
    );

    private native void nativeCompile(
            long nativeClient,
            byte[] address,
            String scriptFile,
            boolean isModule,
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
            boolean fetchEvent
    );
}
