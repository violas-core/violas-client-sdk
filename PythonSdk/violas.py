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
    
    client.transfer(0, accounts[1].address , 1 * MICRO_LIBRA_COIN, True)

    print("account 1's balance is %f" % client.get_balance(1))

    print("Finished test job !")

except BaseException as e:
    print(e)
