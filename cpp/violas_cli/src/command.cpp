#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>
#include <fstream>
#include <sstream>
// cpp/include
#include <utils.h>
#include <ssl_aes.hpp>
#include <json.hpp>
#include "command.hpp"

using namespace std;

class CommandImp : public Command
{
private:
    violas::client_ptr _client;

    using Handler = std::function<void(const vector<string> &)>;
    map<std::string, Handler> _handlers;

public:
    CommandImp(violas::client_ptr client) : _client(client)
    {
        using std::placeholders::_1;

        _handlers["help"] = bind(&CommandImp::show_all_cmds, this);
        _handlers["wallet-add-account"] = bind(&CommandImp::add_account, this, _1);
        _handlers["wallet-list-account"] = bind(&CommandImp::list_accounts, this);
        _handlers["add-currency"] = bind(&CommandImp::add_currency, this, _1);
        _handlers["register-currency"] = bind(&CommandImp::register_currency, this, _1);
        _handlers["aollow-publish"] = bind(&CommandImp::allow_publish, this, _1);
        _handlers["aollow-customer-script"] = bind(&CommandImp::allow_custom_script, this, _1);
        _handlers["create-designed-dealer"] = bind(&CommandImp::create_dd_account, this, _1);
        _handlers["add-currency-for-dd"] = bind(&CommandImp::add_currency_for_dd, this, _1);
        _handlers["create-vasp"] = bind(&CommandImp::create_vasp_account, this, _1);
        _handlers["create-child-vasp"] = bind(&CommandImp::create_child_vasp_account, this, _1);
        _handlers["mint"] = bind(&CommandImp::mint_to_dd, this, _1);
        _handlers["preburn"] = bind(&CommandImp::prebrun, this, _1);
        _handlers["burn"] = bind(&CommandImp::burn, this, _1);
        _handlers["transfer"] = bind(&CommandImp::transfer, this, _1);
        _handlers["rotate-authentication-key"] = bind(&CommandImp::rotate_authentication_key, this, _1);
        _handlers["encrypt"] = bind(&CommandImp::encrypt, this, _1);
        _handlers["decrypt"] = bind(&CommandImp::decrypt, this, _1);
        _handlers["save-private-key"] = bind(&CommandImp::save_private_key, this, _1);
        _handlers["query-transaction"] = bind(&CommandImp::query_transaction, this, _1);
        _handlers["query-txn-range"] = bind(&CommandImp::query_txn_range, this, _1);
        _handlers["query-payment-events"] = bind(&CommandImp::query_payment_events, this, _1);
        _handlers["query-balances"] = bind(&CommandImp::query_balances, this, _1);
        _handlers["query-currency-events"] = bind(&CommandImp::query_currency_events, this, _1);        
        _handlers["query-account-creation-events"] = bind(&CommandImp::query_account_creation_events, this, _1);
    };

    virtual bool
    parse_cmd(std::string_view line) override
    {
        string cmd;
        vector<string> args;
        istringstream iss(line.data());

        iss >> cmd;

        string arg;
        while (iss >> arg)
            args.push_back(arg);

        auto handler = _handlers.find(cmd);
        if (handler != end(_handlers))
            handler->second(args);
        else
            return false;

        return true;
    }

    virtual std::vector<std::string> get_all_cmds() override
    {
        vector<string> cmds;

        for (const auto &handler : _handlers)
        {
            cmds.push_back(handler.first);
        }

        return cmds;
    }

protected:
    void show_all_cmds()
    {
        cout << "commands list :" << endl;

        for (const auto &cmd : get_all_cmds())
        {
            cout << "\t" << cmd << endl;
        }
    }

    void add_account(const vector<string> &args)
    {
        _client->create_next_account();
    }

    void list_accounts()
    {
        auto accounts = _client->get_all_accounts();

        for (const auto &account : accounts)
        {
            cout << "Index : " << account.index << ", "
                 << "Address : " << account.address << ", "
                 << "Authentication Key : " << account.auth_key << ", "
                 //<< "Publick Key : " << account.pub_key
                 << endl;
        }
    }

    void add_currency(const vector<string> &args)
    {
        if (args.size() < 2)
            __throw_invalid_argument("user-add-currency index currency");

        size_t account_index;

        istringstream(args[0]) >> account_index;
        string_view curency = args[1];

        _client->add_currency(account_index, curency);
    }

    void allow_publish(const vector<string> &args)
    {
        if (args.size() < 1)
            __throw_invalid_argument("commnd 'allow publish' missed parameter 'enabled' : true or false");

        bool enabled;
        istringstream(args[0]) >> std::boolalpha >> enabled;

        _client->allow_publishing_module(enabled);
    }

    void allow_custom_script(const vector<string> &args)
    {
        if (args.size() < 1)
            __throw_invalid_argument("commnd 'allow publish' missed parameter 'enabled' : true or false");

        bool enabled;
        istringstream(args[0]) >> std::boolalpha >> enabled;

        _client->allow_custom_script();
    }

    void register_currency(const vector<string> &args)
    {
        if (args.size() < 1)
            __throw_invalid_argument("currency code is missing. ");

        string_view currency_code = args[0];

        cout << "You are about to register currency '" << currency_code << "' to Violas, are you sure? yes or no" << endl;

        string confirmation;
        cin >> confirmation;
        if (confirmation != "yes")
        {
            cout << "You aborted registering currency." << endl;
            return;
        }

        _client->publish_curency(currency_code);

        _client->register_currency(currency_code, 1, 2, false, 1'000'000, 100);

        cout << "You registered currency " << currency_code << " successfully." << endl;
    }

    void create_dd_account(const vector<string> &args)
    {
        if (args.size() < 5)
            __throw_invalid_argument("usage : create-dd-ex currency_code dd_address authentication_key human_name add_all_currencies(bool) ");

        string_view currency_code;
        violas::Address address;
        violas::AuthenticationKey auth_key;
        bool add_all_currencies;

        currency_code = args[0];
        istringstream(args[1]) >> address;
        istringstream(args[2]) >> auth_key;
        string_view human_name = args[3];
        istringstream(args[4]) >> std::boolalpha >> add_all_currencies;

        _client->create_designated_dealer_ex(currency_code, 0, address, auth_key, human_name, "", violas::PublicKey(), add_all_currencies);
    }

    void create_vasp_account(const vector<string> &args)
    {
        if (args.size() < 5)
            __throw_invalid_argument("usage : create_vasp_account currency_code dd_address authentication_key human_name add_all_currencies(bool) ");

        string_view currency_code;
        violas::Address address;
        violas::AuthenticationKey auth_key;
        bool add_all_currencies;

        currency_code = args[0];
        istringstream(args[1]) >> address;
        istringstream(args[2]) >> auth_key;
        string_view human_name = args[3];
        istringstream(args[4]) >> std::boolalpha >> add_all_currencies;

        _client->create_parent_vasp_account(currency_code, 0, address, auth_key, human_name, "", violas::PublicKey(), add_all_currencies);
    }

    void create_child_vasp_account(const vector<string> &args)
    {
        if (args.size() < 6)
            __throw_invalid_argument("usage : create_child_vasp_account currency_code dd_address authentication_key human_name add_all_currencies(bool) ");

        string_view currency_code;
        size_t parent_account_index;
        violas::Address address;
        violas::AuthenticationKey auth_key;
        bool add_all_currencies;
        uint64_t initial_balance = 0;

        currency_code = args[0];
        istringstream(args[1]) >> parent_account_index;
        istringstream(args[2]) >> address;
        istringstream(args[3]) >> auth_key;
        istringstream(args[4]) >> std::boolalpha >> add_all_currencies;
        istringstream(args[5]) >> initial_balance;

        _client->create_child_vasp_account(currency_code, parent_account_index, address, auth_key, add_all_currencies, initial_balance);
    }

    void add_currency_for_dd(const vector<string> &args)
    {
        if (args.size() < 2)
            __throw_invalid_argument("usage : adc-dd currency_code dd_address");

        string_view currency_code;
        violas::Address address;

        currency_code = args[0];
        istringstream(args[1]) >> address;

        _client->add_currency_for_designated_dealer(currency_code, address);
    }

    void mint_to_dd(const vector<string> &args)
    {
        if (args.size() < 4)
            __throw_invalid_argument("usage : mint currency_code dd_address amount tier_index");

        string_view currency_code;
        violas::Address address;
        double amount;
        uint64_t tier_index;

        currency_code = args[0];
        istringstream(args[1]) >> address;
        istringstream(args[2]) >> amount;
        istringstream(args[3]) >> tier_index;

        _client->mint_currency_for_designated_dealer(currency_code,
                                                     0,
                                                     address,
                                                     amount * violas::MICRO_COIN,
                                                     tier_index); // 3
    }

    void prebrun(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("prebrun aount_index currency_code amount");

        violas::Address address;
        size_t account_index;
        uint64_t amount;

        istringstream(args[0]) >> account_index;
        string_view curency_code = args[1];
        istringstream(args[2]) >> amount;

        _client->preburn(curency_code, account_index, amount, true);
    }

    void burn(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("brun currency_code sliding_nonce preburn_address");

        violas::Address preburn_address;
        uint64_t sliding_nonce;

        string_view curency_code = args[0];
        istringstream(args[1]) >> sliding_nonce;
        istringstream(args[2]) >> preburn_address;

        _client->burn(curency_code, sliding_nonce, preburn_address, true);
    }

    void rotate_authentication_key(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("rotate-authentication-key account_index sliding_nonce new_auth_key");

        size_t account_index;
        uint64_t sliding_nonce;
        violas::AuthenticationKey auth_key;

        istringstream(args[0]) >> account_index;
        istringstream(args[1]) >> sliding_nonce;
        istringstream(args[2]) >> auth_key;

        _client->rotate_authentication_key_with_nonce(account_index, sliding_nonce, auth_key, true);
    }

    void save_private_key(const vector<string> &args)
    {
        if (args.size() < 2)
            __throw_invalid_argument("save-private-key account_index key-file-name");

        size_t account_index;

        istringstream(args[0]) >> account_index;
        string_view file_name = args[1];

        _client->save_private_key(account_index, file_name);
    }

    // pear to pear transaction
    void transfer(const vector<string> &args)
    {
        if (args.size() < 4)
            __throw_invalid_argument("transfer account_index account_index|address currency amount gas_unit_price max_gas_amount gas_currency_code");

        size_t account_index;
        violas::Address address;
        uint64_t amount;
        string_view currency = args[2];
        uint64_t gas_unit_price = 0;
        uint64_t max_gas_amount = 1000000;
        string_view gas_currency_code = "VLS";

        istringstream(args[0]) >> account_index;
        istringstream(args[1]) >> address;
        istringstream(args[3]) >> amount;

        if (args.size() >= 5)
            istringstream(args[4]) >> gas_unit_price;

        if (args.size() >= 6)
            istringstream(args[6]) >> max_gas_amount;

        if (args.size() >= 7)
            gas_currency_code = args[6];

        _client->transfer(account_index, address, currency, amount, gas_unit_price, max_gas_amount, gas_currency_code);

        auto accounts = _client->get_all_accounts();

        cout << "use \""
             << "query-transaction " << account_index << " " << accounts[account_index].sequence_number - 1 << " true"
             << "\"to query transaction result."
             << endl;
    }

    // encrypt or decrypt mnemonic
    void encrypt(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("encrypt password input-file-name encrypted-file-name ");

        size_t account_index;

        string_view password = args[0];
        ifstream ifs(args[1]);
        ofstream ofs(args[2]);

        using ifs_iterator = istreambuf_iterator<char>;
        using ofs_iterator = ostreambuf_iterator<char>;

        aes_256_cbc_encrypt(password, ifs_iterator(ifs), ifs_iterator(), ofs_iterator(ofs));
    }

    void decrypt(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("decrypt encrypted-file decrypted-file password");

        size_t account_index;

        string_view password = args[0];
        ifstream ifs(args[1]);
        ofstream ofs(args[2]);

        using ifs_iterator = istreambuf_iterator<char>;
        using ofs_iterator = ostreambuf_iterator<char>;

        aes_256_cbc_decrypt(password, ifs_iterator(ifs), ifs_iterator(), ofs_iterator(ofs));
    }

    void query_transaction(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("query-transactions account_index|address sequence_number fectch_events(ture or false)");

        violas::Address address;
        size_t account_index;
        uint64_t seq_num;
        bool is_fectching_event;

        if (args[0].length() == 32)
            istringstream(args[0]) >> address;
        else
        {
            istringstream(args[0]) >> account_index;

            auto accounts = _client->get_all_accounts();
            address = accounts[account_index].address;
        }

        istringstream(args[1]) >> seq_num;
        istringstream(args[2]) >> std::boolalpha >> is_fectching_event;

        auto transactions = _client->query_transaction_info(address, seq_num, is_fectching_event);

        using json = nlohmann::json;
        auto txs = json::parse(transactions);

        cout << txs.dump(4) << endl;

        // cout << color::GREEN << "version : " << color::RESET << txs["version"] << endl;
        // cout << color::GREEN << "transaction : " << color::RESET << endl << txs["transaction"].dump(4) << endl;
        // cout << color::GREEN << "events : " << color::RESET << endl << txs["events"].dump(4) << endl;
        // cout << "hash : " << txs["hash"] << endl;
        // cout << "bytes : " << txs["bytes"] << endl;
        // cout << color::GREEN << "vm_status : " << color::RESET << txs["vm_status"].dump(4) << endl;
        // cout << color::GREEN << "gas_used : " << color::RESET << txs["gas_used"].dump(4) << endl;
    }

    void query_txn_range(const vector<string> &args)
    {
        if (args.size() < 3)
            __throw_invalid_argument("query-txn-range start_version limit fectch_events(ture|false)");

        uint64_t start_version;
        uint64_t limit;
        bool is_fectching_events;

        istringstream(args[0]) >> start_version;

        istringstream(args[1]) >> limit;
        istringstream(args[2]) >> std::boolalpha >> is_fectching_events;

        auto transactions = _client->query_transaction_range(start_version, limit, is_fectching_events);

        using json = nlohmann::json;
        auto txs = json::parse(transactions);

        cout << txs.dump(4) << endl;
    }

    violas::Client::payment_event_type string_to_event_type(string_view str)
    {
        map<string_view, violas::Client::payment_event_type> event_types =
            {
                {"sent", violas::Client::payment_event_type::sent},
                {"received", violas::Client::payment_event_type::received},
                //{"burned", violas::Client::payment_event_type::burned},
            };

        auto iter = event_types.find(str);

        if (iter != end(event_types))
            return iter->second;
        else
            __throw_invalid_argument(fmt("\"", str, "\" is not a event type, pleas input correct event type such as \"sent, receivied, burned\"").c_str());
    }

    void query_payment_events(const vector<string> &args)
    {
        if (args.size() < 4)
            __throw_invalid_argument("query-events address event-type start limit");

        violas::Address address;
        size_t account_index;
        uint64_t start_event_sn;
        uint64_t limit;
        auto event_type = [](string_view type) -> auto
        {
            if (type == "sent")
                return violas::Client::payment_event_type::sent;
            else if (type == "received")
                return violas::Client::payment_event_type::received;

            __throw_invalid_argument(fmt("\"", type, "\" is not a payment event type, pleas input correct event type such as \"sent, receivied\"").c_str());
        }
        (args[1]);

        if (args[0].length() == 32)
            istringstream(args[0]) >> address;
        else
        {
            istringstream(args[0]) >> account_index;

            auto accounts = _client->get_all_accounts();
            address = accounts[account_index].address;
        }

        istringstream(args[2]) >> start_event_sn;
        istringstream(args[3]) >> limit;

        auto transactions = _client->query_payment_events(address, event_type, start_event_sn, limit);

        using json = nlohmann::json;
        auto txs = json::parse(transactions);

        cout << txs.dump(4) << endl;
    }

    void query_currency_events(const vector<string> &args)
    {
        if (args.size() < 4)
            __throw_invalid_argument("query-currency-events currency event-type(minted, burned, preburned, cancelled-burn, updated-exchange-rate) start limit");

        string_view currency_code = args[0];
        uint64_t start_event_sn;
        uint64_t limit;

        auto event_type = [](const string &type) -> auto
        {
            if (type == "minted")
                return violas::Client::currency_event_type::minted;
            else if (type == "burned")
                return violas::Client::currency_event_type::burned;
            else if (type == "preburned")
                return violas::Client::currency_event_type::preburned;
            else if (type == "cancelled-burn")
                return violas::Client::currency_event_type::cancelled_burn;
            else if (type == "updated_exchange_rate")
                return violas::Client::currency_event_type::updated_exchange_rate;
            else
                __throw_invalid_argument(fmt("\"", type, "\" is not a payment event type, pleas input correct event type such as \"minted, burned, preburned, cancelled-burn, updated-exchange-rate\"").c_str());
        }
        (args[1]);

        istringstream(args[2]) >> start_event_sn;
        istringstream(args[3]) >> limit;

        auto transactions = _client->query_currency_events(currency_code, event_type, start_event_sn, limit);

        using json = nlohmann::json;
        auto txs = json::parse(transactions);

        cout << txs.dump(4) << endl;
    }

    void query_account_creation_events(const vector<string> &args)
    {
        if (args.size() < 2)
            __throw_invalid_argument("usage : query_account_creation_events start limit");

        uint64_t start_event_sn;
        uint64_t limit;

        istringstream(args[0]) >> start_event_sn;
        istringstream(args[1]) >> limit;

        auto transactions = _client->query_account_creation_events(start_event_sn, limit);

        using json = nlohmann::json;
        auto txs = json::parse(transactions);

        cout << txs.dump(4) << endl;
    }

    void query_balances(const vector<string> &args)
    {
    }

    void multisign_auth()
    {
    }
};

std::shared_ptr<Command> Command::create(violas::client_ptr client)
{
    return make_shared<CommandImp>(client);
}