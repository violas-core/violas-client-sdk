#include <iostream>
#include <string>
#include <string_view>
#include <violas_client2.hpp>
#include <utils.hpp>
#include <argument.hpp>
#include <console.hpp>
#include "meta42.hpp"

using namespace std;
using namespace violas;

using handle = function<void(istringstream &params)>;
map<string, handle> create_std_commands(client2_ptr client, string url);

int main(int argc, char *argv[])
{
    cout << "meta42-cli is getting started ..." << endl;

    std::set_unexpected([]()
                        {
        try
        {
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << endl;
        } });

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

        auto commands = create_std_commands(client, args.url);
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

/**
 * @brief Create a cli commands object
 *
 * @param client
 * @param url
 * @return map<string, handle>
 */
map<string, handle> create_std_commands(client2_ptr client, string url)
{
    auto meta42_client = make_shared<meta42::Client>(client);

    return {
        {"cli-add-account", [=](istringstream &params)
         {
             client->create_next_account();
         }},
        {"cli-list-accounts", [=](istringstream &params)
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
             meta42_client->initialize();
         }},
        {"accept", [=](istringstream &params)
         {
             size_t account_index;

             params >> account_index;

             meta42_client->await_accept(account_index);
         }},
        {"mint-token", [=](istringstream &params)
         {
             size_t account_index = 0;
             string hdfs_path;

             check_istream_eof(params, "account_index hdfs-path");
             params >> account_index;
             params >> hdfs_path;

             meta42_client->await_mint_token(account_index, hdfs_path);
         }},
        {"share-token-by-id", [=](istringstream &params)
         {
             size_t account_index = 0;
             Address receiver;
             meta42::TokenId token_id;
             string message;

             check_istream_eof(params, "account_index receiver_address token_id");

             params >> account_index;
             receiver = get_from_stream<Address>(params, client);
             params >> token_id >> message;

             meta42_client->await_share_token(account_index, receiver, token_id, message);
         }

        },
        {"share-token-by-index", [=](istringstream &params)
         {
             size_t account_index = 0;
             Address receiver;
             size_t token_index = 0;
             string message;

             check_istream_eof(params, "account_index receiver_address token_id");

             params >> account_index;
             receiver = get_from_stream<Address>(params, client);
             params >> token_index >> message;

             meta42_client->await_share_token(account_index, receiver, token_index, message);
         }

        },
        {"get-tokens", [=](istringstream &params)
         {
             Address addr = get_from_stream<Address>(params, client);

             [=]() -> Task<void>
             {
                 auto tokens = co_await meta42_client->await_get_tokens(addr);

                 cout << endl
                      << color::CYAN
                      << setw(70) << std::left << "Token Id"
                      << setw(40) << std::left << "HDFS Path"
                      << color::RESET
                      << endl;

                 for (auto &token : tokens)
                 {
                     cout << setw(70) << std::left << bytes_to_hex(token.get_token_id())
                          << setw(40) << std::left << token.hdfs_path << endl;
                 }
             }();
         }

        },
        {"get-mint-events", [=](istringstream &params)
         {
             //
         }

        },
        {"get-sharing-tokens", [=](istringstream &params)
         {
             //
         }

        },
        {"get-received-tokens", [=](istringstream &params)
         {
             //
         }

        },
    };
}