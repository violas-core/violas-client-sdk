#include <iostream>
#include <exception>
#include <argument.hpp>
#include <console.hpp>
#include "nft.hpp"

using namespace std;
using namespace violas;

void test1();
void termiate_handle();

using handle = function<void(istringstream &params)>;
map<string, handle> create_commands(client2_ptr client, string url);

int main(int argc, char *argv[])
{
    cout << "nft-cli is getting started ..." << endl;

    std::set_terminate(termiate_handle);

    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto client = Client2::create(args.url, args.chain_id, args.mnemonic, args.mint_key);
        auto [index, address] = client->create_next_account();
        cout << "Admin address : " << bytes_to_hex(address.value) << endl;

        auto console = Console::create("NFT$ ");
        const string exit = "exit";

        console->add_completion("exit");

        auto commands = create_commands(client, args.url);
        for (auto cmd : commands)
            console->add_completion(cmd.first);

        for (auto line = trim(console->read_line());
             line != exit;
             line = trim(console->read_line()))
        {
            istringstream iss(line);
            string cmd;

            // Read a command
            iss >> cmd;

            if (auto iter = commands.find(cmd);
                iter != end(commands))
            {
                try
                {
                    iter->second(iss);
                }
                catch (const std::invalid_argument &e)
                {
                    std::cerr << "Invalid argument : " << e.what() << endl;
                }
                catch (runtime_error &e)
                {
                    std::cerr << color::RED
                              << "Runtime error : " << e.what()
                              << color::RESET << endl;
                }

                console->add_history(line);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void termiate_handle()
{
    try
    {
        std::rethrow_exception(std::current_exception());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unhandled exception : " << e.what() << endl;
    }
}

void check_istream_eof(istream &is, string_view err)
{
    if (is.eof())
    {
        throw invalid_argument(err.data());
    }
}

void check_account_index(client2_ptr client, size_t account_index)
{
    if (account_index >= client->get_all_accounts().size())
        throw invalid_argument("The account index is more than the size of all accounts.");
}

template <typename T>
T get_from_stream(istringstream &params, client2_ptr client, string_view err_info = "index or address")
{
    T addr;
    int account_index = -1;

    check_istream_eof(params, err_info);

    string temp;

    params >> temp;
    istringstream iss(temp);

    if (temp.length() == sizeof(T) * 2)
    {
        iss >> addr;
    }
    else
    {
        auto accounts = client->get_all_accounts();

        iss >> account_index;
        if (account_index >= accounts.size())
            __throw_invalid_argument("account index is out of account size.");
        else
            addr = accounts[account_index].address.value;
    }

    return addr;
}

map<string, handle> create_commands(client2_ptr client, string url)
{
    auto nft_client = make_shared<nft::Client>(client);

    return {
        {"add-account", [=](istringstream &params)
         {
             client->create_next_account();
         }},
        {"list-accounts", [=](istringstream &params)
         {
             auto accounts = client->get_all_accounts();

             cout << color::CYAN
                  << left << setw(10) << "index"
                  << left << setw(40) << "Address"
                  << left << setw(40) << "Authentication key"
                  << color::RESET << endl;

             int i = 0;
             for (auto &account : accounts)
             {
                 cout << left << setw(10) << i++
                      << left << setw(40) << account.address.value
                      << left << setw(40) << account.auth_key
                      << endl;
             }
         }},
        {"std-create-child-account", [=](istringstream &params)
         {
             Address addr;
             AuthenticationKey auth_key;

             addr = get_from_stream<Address>(params, client);
             check_istream_eof(params, "authentication key");
             params >> auth_key;

             copy(begin(auth_key) + 16, begin(auth_key) + 32, begin(addr));

             client->create_child_vasp_account(0, {addr}, auth_key, "VLS", 0, true);
         }},
        {"initialize", [=](istringstream &params)
         {
             nft_client->initialize().get();
             //  [=]() -> Task<void>
             //  {
             //      try
             //      {
             //          co_await nft_client->initialize();
             //      }
             //      catch (const std::exception &e)
             //      {
             //          std::cerr << e.what() << '\n';
             //      }
             //  }();
         }},
        {"accept", [=](istringstream &params)
         {
             size_t account_index;

             params >> account_index;

             nft_client->await_accept(account_index);
         }},
    };
}