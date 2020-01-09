try:
    import violas;
    from violas import *;

    client = Client("13.68.141.242",
                           40001,
                           "consensus_peers.config.toml",
                           "faucet_keys",
                           False,
                           "",
                           "mnemonic")

    client.test_validator_connection()

    for i in range(5):
        client.create_next_account(True)

    accounts = client.get_all_accounts()

    for a in accounts:
        print("account index : %d, "
              "address : %s, "
              "sequence_number : %d, "
              "status : %d" %
              (a.index, uint256_to_string(a.address), a.sequence_number, a.status))

    print("account 0's balance is %f" % client.get_balance(0))
    
    MICRO_LIBRA_COIN = 1000000
    
    client.transfer(0, accounts[1].address , int(0.1 * MICRO_LIBRA_COIN), True)
    client.transfer(0, accounts[2].address , int(0.1 * MICRO_LIBRA_COIN), True)

    print("account 1's balance is %f" % client.get_balance(1))

    token = Token(client, accounts[1].address, "TEST", "../cppSdk/scripts")
    print("Token name is %s" % token.name())

    token.deploy( 1 )

    token.publish( 1 )

    token.mint( 1, accounts[1].address, 1 * MICRO_LIBRA_COIN)

    balance = token.get_account_balance(1)
    print("account 1's balance is %d" % balance)

    token.publish( 2 )
    
    token.transfer(1, accounts[2].address, 1000)
    print("account 1's balance is %d" % token.get_account_balance(2))

    print("Finished test job !")

except BaseException as e:
    print("catch an execption : %s" % e)
