#include <iostream>
#include <string>
#include <string_view>
#include <violas_client2.hpp>

using namespace std;
using namespace violas;

using handle = function<void(istringstream &params)>;
map<string, handle> create_std_commands(client2_ptr client, string url);

int main(int argc, char* argv[])
{
    cout << "meta42-cli get started ..." << endl;

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
    Address addr;
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
    return {
        {"std-add-account", [=](istringstream &params)
         {
             client->create_next_account();
         }},
        {"std-list-accounts", [=](istringstream &params)
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

             check_istream_eof(params, "authentication key");
             params >> auth_key;

             copy(begin(auth_key) + 16, begin(auth_key) + 32, begin(addr));

             client->create_child_vasp_account(0, {addr}, auth_key, "VLS", 0, true);
         }},
    };
}