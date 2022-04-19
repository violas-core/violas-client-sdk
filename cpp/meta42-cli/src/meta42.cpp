#include <iostream>
#include <string>
//#include <syncstream>
#include <violas_client2.hpp>
#include "meta42.hpp"

using namespace std;
using namespace violas;

namespace meta42
{
    const Address ADMIN_ADDRESS = hex_to_array_u8<16>("458d623300e797451b3e794a45b41065");
    const string MODULE_FILE_NAME = "move/modules/00_Meta42.mv";
    const string SCRIPT_PATH = "move/scripts/";
    const string INITIALIZE_SCRIPT_FILE = SCRIPT_PATH + "meta42_initialize.mv";
    const string ACCEPT_SCRIPT_FILE = SCRIPT_PATH + "meta42_accept.mv";
    const string MINT_TOKEN_SCRIPT_FILE = SCRIPT_PATH + "meta42_mint_token.mv";
    const string SHARE_TOKEN_BY_ID_SCRIPT_FILE = SCRIPT_PATH + "meta42_share_token_by_id.mv";
    const string SHARE_TOKEN_BY_INDEX_SCRIPT_FILE = SCRIPT_PATH + "meta42_share_token_by_index.mv";

    Client::Client(violas::client2_ptr violas_client)
        : m_violas_client(violas_client)
    {
    }

    Client::~Client()
    {
    }

    void Client::initialize()
    {
        m_violas_client->allow_custom_script(true);
        m_violas_client->allow_publishing_module(true);

        m_violas_client->publish_module(ACCOUNT_ROOT_ID, "move/modules/Compare.mv");
        m_violas_client->publish_module(ACCOUNT_ROOT_ID, MODULE_FILE_NAME);
        cout << "Succeeded to publish module Meta42.mv ." << endl;

        auto accounts = m_violas_client->get_all_accounts();
        auto &admin = accounts[0];

        try
        {
            m_violas_client->create_parent_vasp_account(admin.address, admin.auth_key, "Meta42 Admin", true);
            cout << "Succeeded to create parent account." << endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to create parent account." << e.what() << '\n';
        }

        auto [sender, sn] = m_violas_client->execute_script_file(0, INITIALIZE_SCRIPT_FILE, {}, {});
        m_violas_client->check_txn_vm_status(sender, sn, "meta42::intialize");
        cout << "Succeeded to execute Meta42::initialize() ." << endl;
    }

    Task<void> Client::await_accept(size_t account_index)
    {
        try
        {
            auto [sender, sn] = co_await m_violas_client->await_execute_script(
                account_index,
                ACCEPT_SCRIPT_FILE,
                {},
                {});

            co_await m_violas_client->await_check_txn_vm_status(sender, sn, "Meta42::accept");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    Task<void>
    Client::await_mint_token(size_t account_index, std::string_view hdfs_path)
    {
        try
        {
            auto [sender, sn] = co_await m_violas_client->await_execute_script(
                account_index,
                MINT_TOKEN_SCRIPT_FILE,
                {},
                make_txn_args(hdfs_path));

            co_await m_violas_client->await_check_txn_vm_status(sender, sn, "Meta42::await_mint_token");

            cout << "succeeded to Mint token" << '\n';
        }
        catch (const std::exception &e)
        {
            std::cerr << "failed to mint token, " << e.what() << '\n';
        }
    }

    Task<void>
    Client::await_share_token(size_t account_index, Address receiver, const TokenId &token_id, std::string_view message)
    {
        try
        {
            auto [sender, sn] = co_await m_violas_client->await_execute_script(
                account_index,
                SHARE_TOKEN_BY_ID_SCRIPT_FILE,
                {},
                make_txn_args(dt::AccountAddress{receiver}, token_id, message));

            co_await m_violas_client->await_check_txn_vm_status(sender, sn, "Meta42::await_share_token");

            // auto scout = std::osyncstream{std::cout};
            cout << "succeeded to share token" << endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    Task<void>
    Client::await_share_token(size_t account_index, Address receiver, size_t token_index, std::string_view message)
    {
        try
        {
            auto [sender, sn] = co_await m_violas_client->await_execute_script(
                account_index,
                SHARE_TOKEN_BY_INDEX_SCRIPT_FILE,
                {},
                make_txn_args(dt::AccountAddress{receiver}, token_index, message));

            co_await m_violas_client->await_check_txn_vm_status(sender, sn, "Meta42::await_share_token");

            // auto scout = std::osyncstream{std::cout};
            cout << "succeeded to share token" << endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    Task<std::vector<Token>>
    Client::await_get_tokens(Address address)
    {
        try
        {
            auto account_state = co_await m_violas_client->await_get_account_state({address});

            if (account_state)
            {
                auto account_info = account_state->get_resource<AccountInfo>(make_struct_tag(VIOLAS_LIB_ADDRESS, "Meta42", "AccountInfo", {}));

                if (account_info)
                    co_return account_info->tokens;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        co_return {};
    }
}

std::ostream &operator<<(std::ostream &os, const meta42::Token &token)
{
    cout << setw(40) << bytes_to_hex(token.get_token_id()) << setw(30) << token.hdfs_path << endl;

    return os;
}