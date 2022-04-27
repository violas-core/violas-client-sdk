#include <iostream>
#include <string>
#include <sqlite_orm.hpp>
#include <violas_client2.hpp>
#include <sqlite3.h>
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
    const string DB_NAME = "db.sqlite";

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

    Task<std::vector<SharedTokenEvent>>
    Client::query_shared_token_events_history(Address &&address, TokenId &&token_id)
    {
        auto addr = move(address);
        auto id = move(token_id);

        struct SharedTokenHistory
        {
            uint64_t sn; // sequence number for table
            string sender;
            string receiver;
            string token_id;
            std::string message;
            uint64_t block_version;

            SharedTokenHistory(const SharedTokenEvent &event)
            {
                sn = event.sequence_number;
                sender = bytes_to_hex(event.sender);
                receiver = bytes_to_hex(event.receiver);
                token_id = bytes_to_hex(event.token_id);
                message = event.message;
                block_version = event.transaction_version;
            }
        };

        using namespace sqlite_orm;
        auto storage = make_storage("db.sqlite",
                                    make_table("shared_token_events",
                                               make_column("sn", &SharedTokenHistory::sn, unique(), primary_key()),
                                               make_column("sender", &SharedTokenHistory::sender),
                                               make_column("receiver", &SharedTokenHistory::receiver),
                                               make_column("token_id", &SharedTokenHistory::token_id),
                                               make_column("message", &SharedTokenHistory::message),
                                               make_column("block_version", &SharedTokenHistory::block_version)));

        storage.sync_schema();

        auto count = storage.count<SharedTokenHistory>();

        try
        {
            auto account_state = co_await m_violas_client->await_get_account_state({ADMIN_ADDRESS});

            if (account_state)
            {
                auto global_info = account_state->get_resource<GlobalInfo>(make_struct_tag(VIOLAS_LIB_ADDRESS, "Meta42", "GlobalInfo", {}));

                if (global_info)
                {
                    if (uint64_t event_count = global_info->shared_token_event.counter; count < event_count)
                    {
                        auto events = co_await m_violas_client->await_query_events<SharedTokenEvent>(global_info->shared_token_event, count, event_count);

                        for (const auto &event : events)
                            storage.insert(SharedTokenHistory(event));
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        make_recursive_query_shared_token(addr, id);

        co_return {};
    }

    void Client::make_recursive_query_shared_token(Address addr, TokenId token_id)
    {
        int result = 0;
        sqlite3 *db = nullptr;
        sqlite3_stmt *stmt = nullptr;

        result = sqlite3_open("db.sqlite", &db);

        string_view sql = R"(WITH RECURSIVE
                                history(sn, sender, receiver, token_id, message, block_version) AS (
                                SELECT sth.* FROM shared_token_events as sth WHERE sth.receiver='%s' AND sth.token_id='%s'
                                UNION ALL
                                SELECT ste.* FROM history AS h INNER JOIN shared_token_events AS ste ON h.sender = ste.receiver AND h.token_id=ste.token_id
                                )
                            SELECT * FROM history h)";

        const char *ptail = nullptr;
        string sql_with_args = format(sql.data(), bytes_to_hex(addr).data(), bytes_to_hex(token_id).data()).data();
        sqlite3_prepare_v3(db, sql_with_args.data(), sql.size(), 0, &stmt, &ptail);

        auto show_history = [](void *para, int n_column, char **column_value, char **column_name) -> int
        {
            size_t left_width[] = {10, 36, 36, 68, 20, 10};

            static bool initialized = false;
            if (!initialized)
            {
                cout << endl;

                for (int i = 0; i < n_column; i++)
                {
                    cout << std::left << setw(left_width[i]) << column_name[i];
                }

                cout << endl;

                initialized = true;
            }

            for (int i = 0; i < n_column; i++)
            {
                cout << std::left << setw(left_width[i]) << column_value[i];
            }
            cout << endl;

            return 0;
        };

        char *err_msg = nullptr;
        sqlite3_exec(db, sql_with_args.data(), show_history, (void *)this, &err_msg);

        if (err_msg)
            sqlite3_free(err_msg);

        if (db)
            sqlite3_close(db);
    }
}

std::ostream &operator<<(std::ostream &os, const meta42::Token &token)
{
    cout << setw(40) << bytes_to_hex(token.get_token_id()) << setw(30) << token.hdfs_path << endl;

    return os;
}