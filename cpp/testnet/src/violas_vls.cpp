#include <iostream>
#include <map>
#include <functional>
#include <client.hpp>
#include "terminal.h"

using namespace std;
using namespace violas;

void distribute_vls(client_ptr client);

int main(int argc, const char *argv[])
{
    try
    {
        if (argc < 6)
            throw runtime_error("missing arguments. \n Usage : url mint_key mnemonic waypoint chain_id");

        uint8_t chain_id = stoi(argv[5]);
        string url = argv[1];
        string mint_key = argv[2];
        string mnemonic = argv[3];
        string waypoint = argv[4];

        auto client = Client::create(chain_id, url, mint_key, mnemonic, waypoint);

        client->test_connection();

        using handler = function<void()>;
        map<int, handler> handlers = {
            {1, [=]() { distribute_vls(client); }}

        };

        cout << "1 for deploying all currencies \n"
                //"2 for testing Account Management \n"
                //"3 for deploying Exchange Contract.\n"
                //"4 for deploying Bank Contract.\n"
                "Please input index : ";

        int index;
        cin >> index;

        client->allow_publishing_module(true);
        client->allow_custom_script();

        cout << "allow custom script and  publishing module." << endl;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << "caught an exception : " << e.what() << '\n';
    }

    return 0;
}

void distribute_vls(client_ptr client)
{
    vector<uint8_t> distribute_vls_bytecode = {161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5,
                                               7, 1, 7, 8, 28, 8, 36, 16, 0, 0, 0, 1, 0, 0, 0, 0,
                                               12, 76, 105, 98, 114, 97, 65, 99, 99, 111, 117, 110, 116, 14, 100, 105,
                                               115, 116, 114, 105, 98, 117, 116, 101, 95, 118, 108, 115, 0, 0, 0, 0,
                                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 17, 0, 2};

    client->create_next_account();
    auto accounts = client->get_all_accounts();

    try_catch([&]() {
        client->create_designated_dealer_account("Coin1", 0,
                                                 accounts[0].address, accounts[0].auth_key,
                                                 "distributer", "wwww.violas.io",
                                                 accounts[0].pub_key, true);
    });

    client->execute_script(0, distribute_vls_bytecode);
}