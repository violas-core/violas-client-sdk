import java.text.Format;

import io.violas.sdk.*;
import javafx.util.Pair;

public class Demo {
    public static void main(String[] argv) {
        System.out.println("Run Violas SDK Java Demo !");

        if (argv.length < 4) {
            System.out.println("Useage : Demo host port mint_key mnemonic");
            //System.out.println(String.format("argc is %d", argv.length));

            return;
        }

        try {
            String host = argv[0];
            short port = (short)Integer.parseInt(argv[1]);
            String mintKeyFileName = argv[2];
            String mnemonicFileName  = argv[3];

            Client client = new Client(host, port, mintKeyFileName, false, "", mnemonicFileName);

            client.test_validator_connection();

            testToken(client);

        } catch (Exception e) {
            System.out.println("Demo has a exception, error : " + e.getMessage());
        }
    }

    static void runViolasSdk(Client client) {
        String scripts_path = "../../cppSdk/scripts/";

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

    static void testToken(Client client) {
        String scripts_path = "../../cppSdk/scripts/";

        System.out.println("createNextAccount 5 accounts ...");
        for(int i=0; i<5; i++)
        { 
            Pair<Long, byte[]> account0 = client.createNextAccount();
        }
        
        System.out.println("getAllAccounts ...");
        Client.Account accounts[] = client.getAllAccounts();
        
        System.out.println("mint 1 coin to each account ...");
        for (Client.Account account : accounts) {            
            client.mint(account.index, 1);
            String info = String.format("the account %d's  balance is %f.", account.index, account.address, getBalance(account.index));
            System.out.println();
        }      

        //
        // test Token class
        //

        // accout 0 create a new Token(stable coin) with name Token1 by registerring his
        // address
        Token token = new Token(client, accounts[0].address, "Token1", scripts_path, "");

        // account 0 deploy a new token on Violas blockchain
        // note that the account index must be the same account as above
        // account1.getValue() used in new Token,
        // otherwise an execption will get through.
        System.out.println("account 0 deploy token  ...");
        token.deploy(0);

        // account 0 publishes(registers) token info
        token.publish(0);

        //
        token.createToken(0, accounts[1].address, "TokenA");
        token.createToken(0, accounts[2].address, "TokenB");

        // account 0 publishes(registers) token info
        token.publish(1);
        token.publish(2);
        token.publish(3);
        token.publish(4);

        // mint 1000 micro token for account 0
        // note that only account 1 can mint this token, other account call
        // method mint will cause an exception
        token.mint(0, 1, accounts[3].address, 1000);
        Pair<String, String> txn_event = client.getCommittedTxnsByAccSeq(1, client.getSequenceNumber(1)-1);
        System.out.println(txn_event.getKey());
        
        // get the balance of account 0
        long token_balance = token.getBalance(0, accounts[3].address);

        // print and check if the token_balance is 1000
        System.out.println("the balance of Stable Coin of account 0  = " + token_balance);

        // free the resource held by client, and then mustn't call any method of client
        client.close();
    }
}
