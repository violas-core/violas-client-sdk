try:
    import violas;

    client = violas.Client("18.220.66.235", 
                            40001,  
                            "violas_consensus_peers.config.toml",
                            "temp_faucet_keys",
                            False,
                            "faucet.testnet.libra.org", 
                            "mnemonic")
    
    client.test_validator_connection()

    for i in range(5):    
        a, b = client.create_next_account(True)

    print("Finished test job !")

except BaseException as e:
    print(e)



