try:
    import violas

    client = violas.Client("13.68.141.242",
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
              (a.index, violas.uint256_to_string(a.address), a.sequence_number, a.status))

    print("Finished test job !")

except BaseException as e:
    print(e)
