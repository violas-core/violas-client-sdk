#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include "command.hpp"
#include "../../testnet/src/utils.h"

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

        _handlers["rc"] = bind(&CommandImp::register_currency, this, _1);
        _handlers["ap"] = bind(&CommandImp::allow_publish, this, _1);
        _handlers["acs"] = bind(&CommandImp::allow_custom_script, this, _1);
        _handlers["create-dd"] = bind(&CommandImp::create_dd_account, this, _1);
        _handlers["adc-dd"] = bind(&CommandImp::add_currency_for_dd, this, _1);
        _handlers["create-vasp"] = bind(&CommandImp::create_vasp_account, this, _1);
        _handlers["create-child-vasp"] = bind(&CommandImp::create_vasp_account, this, _1);
        _handlers["mint"] = bind(&CommandImp::mint_to_dd, this, _1);
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

    virtual void show_all_cmd() override
    {
    }

protected:
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

        _client->publish_curency(currency_code);

        _client->register_currency(currency_code, 1, 2, false, 1'000'000, 100);
    }

    void create_dd_account(const vector<string> &args)
    {
        violas::Address address;
    }

    void create_vasp_account(const vector<string> &args)
    {
        violas::Address address;
    }

    void create_child_vasp_account(const vector<string> &args)
    {
        violas::Address address;
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

        cout << address << endl;

        _client->mint_currency_for_designated_dealer(currency_code,
                                                     0,
                                                     address,
                                                     amount * violas::MICRO_COIN,
                                                     tier_index); // 3
    }

    void prebrun(const vector<string> &args)
    {
        violas::Address address;
    }

    void brun(const vector<string> &args)
    {
        violas::Address address;
    }

    void rotate_authentication_key(const vector<string> &args)
    {
    }

    // pear to pear transaction
    void transfer(const vector<string> &args)
    {
    }

    // encrypt or decrypt mnemonic
    void crypto(const vector<string> &args)
    {
    }
};

std::shared_ptr<Command> Command::create(violas::client_ptr client)
{
    return make_shared<CommandImp>(client);
}