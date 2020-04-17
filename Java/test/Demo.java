import io.violas.sdk.*;
import javafx.util.Pair;

public class Demo {
    public static void main(String[] argv) {
        System.out.println("Run Violas SDK Java Demo !");

        try {
            testToken();

        } catch (Exception e) {
            System.out.println("Demo has a exception, error : " + e.getMessage());
        }
    }

    static void runViolasSdk() {
        String scripts_path = "../../cppSdk/scripts/";

        Client client = new Client("localhost", (short) 36817, "/tmp/8a03b1bf30f48a37520b67d620ff6a14/mint.key", false,
                "", "mnemonic");

        client.test_validator_connection();

        Pair<Long, byte[]> account0 = client.createNextAccount();
        System.out.println("createNextAccount account 0");

        Pair<Long, byte[]> account1 = client.createNextAccount();
        System.out.println("createNextAccount account 1");

        System.out.println("getAllAccounts ...");
        for (Client.Account account : client.getAllAccounts()) {
            System.out.println("index=" + account.index);
        }

        client.mint((long) 0, (long) 100);

        double balance = client.getBalance(0);

        System.out.println("the account 0's balance = " + balance);

        client.mint(1, 50);

        balance = client.getBalance(account1.getValue());

        System.out.println("the account 0's balance = " + balance);

        client.transfer(0, account1.getValue(), 1 * Client.MICRO_LIBRA_COIN);

        balance = client.getBalance(account1.getValue());

        System.out.println("the account 1's balance = " + balance);

        long sequence = client.getSequenceNumber(1);

        Pair<String, String> txn_event = client.getCommittedTxnsByAccSeq(1, sequence - 1);
        System.out.println(
                String.format("the transcation = %s \n, the event = %s", txn_event.getKey(), txn_event.getValue()));

        Pair<String, String>[] txnEvents = client.getCommitedTxnByRange(100, 10, true);
        for (Pair<String, String> x : txnEvents) {
            System.out.println(String.format("the transcation = %s \n, the event = %s", x.getKey(), x.getValue()));
        }

        //
        // compile file token.mvir and generate file token.mv
        //
        client.compile(0, scripts_path + "token.mvir", true, // true, compile for move module
                "");

        // deploy module token.mv on blockchain
        client.publishModule(0, scripts_path + "token.mv");

        // compile contract publish.mvir with deployer account 0 and generate publish.mv
        client.compile(0, scripts_path + "publish.mvir", false, // false, compile for move script
                "");

        // execute the contract publish.mv to register token info for account 0
        client.executeScript(0, scripts_path + "publish.mv", new String[0]);

        // execute the contract publish.mv to register token info for account 0
        client.executeScript(1, scripts_path + "publish.mv", new String[0]);

        // free the resource held by client, and then mustn't call any method of client
        client.close();
    }

    static void testToken() {
        String scripts_path = "../../cppSdk/scripts/";

        Client client = new Client("localhost", (short) 36817, "/tmp/8a03b1bf30f48a37520b67d620ff6a14/mint.key", false,
                "", "mnemonic");

        client.test_validator_connection();

        System.out.println("createNextAccount 5 accounts ...");
        Pair<Long, byte[]> account0 = client.createNextAccount();
        Pair<Long, byte[]> account1 = client.createNextAccount();
        Pair<Long, byte[]> account2 = client.createNextAccount();
        Pair<Long, byte[]> account3 = client.createNextAccount();
        Pair<Long, byte[]> account4 = client.createNextAccount();

        for (Client.Account account : client.getAllAccounts()) {
            System.out.println("index=" + account.index);
            client.mint(account.index, 1);
        }
        System.out.println("getAllAccounts ...");

        client.mint((long) 0, (long) 100);

        double balance = client.getBalance(0);

        System.out.println("the account 0's balance = " + balance);

        client.mint(1, 100);

        //
        // test Token class
        //

        // accout 0 create a new Token(stable coin) with name Token1 by registerring his
        // address
        Token token = new Token(client, account0.getValue(), "Token1", scripts_path, "");

        // account 0 deploy a new token on Violas blockchain
        // note that the account index must be the same account as above
        // account1.getValue() used in new Token,
        // otherwise an execption will get through.
        token.deploy(0);

        // account 0 publishes(registers) token info
        token.publish(0);

        //
        token.createToken(0, account1.getValue(), "TokenA");
        token.createToken(0, account2.getValue(), "TokenB");

        // account 0 publishes(registers) token info
        token.publish(1);
        token.publish(2);
        token.publish(3);
        token.publish(4);

        // mint 1000 micro token for account 0
        // note that only account 1 can mint this token, other account call
        // method mint will cause an exception
        token.mint(0, 1, account3.getValue(), 1000);

        // get the balance of account 0
        long token_balance = token.getBalance(0, account3.getValue());

        // print and check if the token_balance is 1000
        System.out.println("the balance of Stable Coin of account 0  = " + token_balance);

        // free the resource held by client, and then mustn't call any method of client
        client.close();
    }
}
