# dependencies 
# sudo apt install libboost-python1.71.0
#
# example 
#
try:
    import sys
    sys.path.append('../build/debug/lib')

    import libpy_violas_sdk
    from libpy_violas_sdk import *

    print("start Newnet Python sdk example ...")

    client = Client(4,
                    "http://localhost:33101",
                    "/tmp/libra/mint.key",
                    "mnemonic",
                    "0:f1a56b772ddfabe9db6249d04351bba126a1b628bf0970312e012c20f9b06035")

    client.test_connection()
    print("test connection successfully.")

    for i in range(5):
        client.create_next_account()

    accounts = client.get_all_accounts()

    for a in accounts:
        print("account index : %u, "
              "address : %s, "
              "auth_key : %s "
              "sequence_number : %d, "
              "status : %s"
              %
              (a.index,
               a.address.str,
               a.auth_key.str,
               a.sequence_number,
               a.status
               ))

    print("print all accounts.")

    if accounts[0].status == AccountStatus.Local:
        client.create_parent_vasp_account(
            "Coin1", 0, accounts[0].address, accounts[0].auth_key, "Testor", "www.violas.io", accounts[0].pub_key, True)

        client.mint_for_testnet("Coin1", accounts[0].address, 100)
        print("mint 100 Coin1 to account 0")

    currencies = ["USD", "USDT", "USDTC", "USDEUR"]
    MICRO_COIN = 1000000

    #
    #   regiester a new currency and mint 1,000,000 to account DD
    #
    for currency in currencies:
        try:
            client.publish_currency(currency)

            client.register_currency(currency,
                                    1,
                                    2,
                                    False,
                                    1000000,
                                    100)

            client.add_currency_for_designated_dealer(currency,TESTNET_DD_ADDRESS)
            
            print("deployed currency %s" % currency)            
        except BaseException as e:
            print("catch an execption while delopying a new currency : %s" % e)

        client.mint_currency_for_designated_dealer(currency,
                                                    0,
                                                    TESTNET_DD_ADDRESS,
                                                    1000000 * MICRO_COIN,
                                                    3)

        print("mint 1,000,000 %s to account DD" % currency)

except BaseException as e:
    print("catch an execption : %s" % e)
