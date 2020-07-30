#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <client.hpp> //rust-client-proxy/ffi/client.hpp
//#include <violas_sdk.hpp>

using namespace std;

int main(int argc, const char *argv[])
{
    using namespace violas;

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

        auto add_index = client->create_next_account();
        add_index = client->create_next_account();

        auto accounts = client->get_all_accounts();

        for (const auto & account : accounts)
        {
            cout << account.sequence_number << endl;
        }        
        
    }
    catch (const std::exception &e)
    {
        std::cerr << "cought exception : " << e.what() << '\n';
    }

    return 0;
}