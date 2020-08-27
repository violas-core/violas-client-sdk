#include <ctime>
#include <iomanip>
//#include <chrono>
//#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <stack>
#include <iterator>
#include <functional>
#include "json.hpp"

#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#else

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
#endif

#include "violas_sdk.hpp"
#include "rust_client_proxy.hpp"

using namespace std;
using json = nlohmann::json;

#define EXCEPTION_AT \
    format(", exception at (%s:%s:%d)", __FILE__, __func__, __LINE__)

namespace LIB_NAME
{
    ostream &log(ostream &ost, const char *flag, const char *file, int line,
                 const char *func)
    {
        time_t now = time(nullptr);

        ost << flag << put_time(std::localtime(&now), "%F %T") << " (" << file << ":"
            << line << ":" << func << ") : ";

        return ost;
    }

    std::string tx_vec_data(const std::string &data)
    {
        ostringstream oss;
        oss << "b\"";

        for (auto v : data)
        {
            oss << std::setfill('0') << std::setw(2) << std::hex << (int)v;
        }

        oss << "\"";

        return oss.str();
    }

    bool is_valid_balance(uint64_t value)
    {
#if __cplusplus >= 201703L
        const uint64_t max_uint64 = numeric_limits<uint64_t>::max();
#else
        const uint64_t max_uint64 = 0xFFFFFFFFFFFFFFFF;
#endif
        return value != max_uint64;
    }

    void transform_mv_to_json(const std::string &mv_file_name,
                              const std::string &json_file_name,
                              const Address &address)
    {
        ifstream mv(mv_file_name, ios::binary);
        ofstream ofs(json_file_name);
        const uint8_t addr[] = {0x72, 0x57, 0xc2, 0x41, 0x7e, 0x4d, 0x10, 0x38, 0xe1, 0x81, 0x7c, 0x8f, 0x28, 0x3a, 0xce, 0x2e};

        if (!mv.is_open())
            throw runtime_error(format("file %s is not exist", mv_file_name.c_str()));

        mv.seekg(0, mv.end);
        int length = mv.tellg();
        vector<uint8_t> buffer(length);

        mv.seekg(0, mv.beg);
        mv.read((char *)buffer.data(), buffer.size());

        auto pos = search(begin(buffer), end(buffer), addr, end(addr));
        if (pos != end(buffer))
            copy((char *)address.data().data(), (char *)end(address.data()), pos);

        //
        //  generate the format likes {"code" : […], "args" : []}
        //
        ofs << R"({"code" : [)";

        transform(
            begin(buffer), end(buffer),
            ostream_iterator<string>(ofs),
            [](uint8_t num) -> auto {
                return to_string(num) + ",";
            });

        ofs.seekp(-1, ios_base::cur) << R"(], "args" : []})";
    }

    void replace_mv_with_addr(const std::string &mv_file_name,
                              const std::string &new_file_name,
                              const Address &address)
    {
        ifstream mv(mv_file_name, ios::binary);
        ofstream ofs(new_file_name);
        const uint8_t addr[] = {0x72, 0x57, 0xc2, 0x41, 0x7e, 0x4d, 0x10, 0x38, 0xe1, 0x81, 0x7c, 0x8f, 0x28, 0x3a, 0xce, 0x2e};

        if (!mv.is_open())
            throw runtime_error(format("file %s is not exist", mv_file_name.c_str()));

        mv.seekg(0, mv.end);
        int length = mv.tellg();
        vector<uint8_t> buffer(length);

        mv.seekg(0, mv.beg);
        mv.read((char *)buffer.data(), buffer.size());

        auto pos = begin(buffer);
        do
        {
            pos = search(pos, end(buffer), addr, end(addr));
            if (pos != end(buffer))
            {
                auto data = address.data();
                copy(begin(data), end(data), pos);
            }
        } while (pos != end(buffer));

        copy(begin(buffer), end(buffer), ostreambuf_iterator<char>(ofs));
    }

    //
    //  ClientImp
    //  the implimentation of interface Clinet
    //
    class ClientImp : virtual public Client
    {
    protected:
        void *raw_client_proxy = nullptr;

        vector<pair<uint64_t, Address>> m_accounts;

    public:
        ClientImp(uint8_t chain_id,
                  const std::string &url,
                  const std::string &mint_key_file_name,
                  bool sync_on_wallet_recovery,
                  const std::string &faucet_server,
                  const std::string &mnemonic_file_name,
                  const std::string &waypoint)
        {

            bool ret = (void *)violas_create_client(chain_id,
                                                    url.data(),
                                                    mint_key_file_name.data(),
                                                    sync_on_wallet_recovery,
                                                    faucet_server.data(),
                                                    mnemonic_file_name.data(),
                                                    waypoint.data(),
                                                    (uint64_t *)&raw_client_proxy);
            if (!ret)
                throw runtime_error(format("failed to create native Violas client, error : %s",
                                           get_last_error().c_str()));

            CLOG << "\ncreate violas client with "
                 << "\n\turl = " << url
                 << "\n\tmint_key = " << mint_key_file_name
                 << "\n\tsync_on_wallet_recovery = " << sync_on_wallet_recovery
                 << "\n\tfaucet_server = " << faucet_server
                 << "\n\tmnemonic_file = " << mnemonic_file_name
                 << endl;
        }

        virtual ~ClientImp()
        {
            libra_destory_client_proxy((uint64_t)raw_client_proxy);
            raw_client_proxy = 0;
        }

        void *get_raw_client()
        {
            return raw_client_proxy;
        }

        string get_last_error()
        {
            string error;
            char *last_error = libra_get_last_error();

            error = last_error;

            libra_free_string(last_error);

            return error;
        }

        virtual void test_validator_connection() override
        {
            bool ret = libra_test_validator_connection((uint64_t)raw_client_proxy);
            if (!ret)
                throw runtime_error("failed to test validator connection");

            CLOG << "\nsucceeded to test validator connection" << endl;
        }

        virtual std::pair<size_t, Address>
        create_next_account(bool sync_with_validator) override
        {
            Libra_Address libra_account = libra_create_next_account((uint64_t)raw_client_proxy,
                                                                    sync_with_validator);

            auto account = make_pair<>(libra_account.index, Address(libra_account.address, ADDRESS_LENGTH));

            m_accounts.push_back(account);

            return account;
        }

        virtual std::vector<Account> get_all_accounts() override
        {
            Accounts all_accounts = libra_get_all_accounts((uint64_t)raw_client_proxy);

            vector<Account> accounts;

            for (uint64_t i = 0; i < all_accounts.len; i++)
            {
                Account a;
                const auto &_a = all_accounts.data[i];

                a.address = Address(_a.address, ADDRESS_LENGTH);
                a.auth_key = AuthenticationKey(_a.auth_key, sizeof(_a.auth_key));
                a.index = _a.index;
                a.sequence_number = _a.sequence_number;
                a.status = _a.status;

                accounts.push_back(a);
            }

            libra_free_all_accounts_buf(all_accounts);

            return accounts;
        }

        virtual uint64_t get_balance(uint64_t account_index) override
        {
            const auto &address = m_accounts.at(account_index).second;

            return get_balance(address);
        }

        virtual uint64_t get_balance(const Address &address) override
        {
            uint64_t balance = 0.f;

            bool ret = libra_get_balance((uint64_t)raw_client_proxy,
                                         address.data().data(),
                                         &balance);
            if (!ret)
                throw runtime_error(
                    format("failed to get balance, error : %s", get_last_error().c_str()));

            return balance;
        }

        virtual uint64_t get_sequence_number(uint64_t account_index) override
        {
            auto [index, address] = m_accounts.at(account_index);

            return get_sequence_number(address);
        }

        virtual uint64_t get_sequence_number(const Address &address) override
        {
            uint64_t sequence_num = 0;

            bool ret = libra_get_sequence_number((uint64_t)raw_client_proxy,
                                                 address.data().data(),
                                                 sequence_num);
            if (!ret)
                throw runtime_error(
                    format("failed to get sequence number, error : %s", get_last_error().c_str()));

            return sequence_num;
        }

        virtual void mint_coins(uint64_t index,
                                uint64_t num_coins,
                                bool is_blocking) override
        {
            bool ret = libra_mint_coins((uint64_t)raw_client_proxy, index, num_coins, is_blocking);
            if (!ret)
            {
                throw runtime_error(
                    format("failed to mint coins, error : %s", get_last_error().c_str()));
            }
        }

        virtual std::pair<uint64_t, uint64_t>
        transfer_coins_int(uint64_t sender_account_ref_id,
                           Address receiver,
                           uint64_t num_coins,
                           uint64_t gas_unit_price = 0,
                           uint64_t max_gas_amount = 0,
                           bool is_blocking = true) override
        {
            _index_sequence index_seq;
            bool ret = libra_transfer_coins_int(
                (uint64_t)raw_client_proxy, sender_account_ref_id,
                (uint8_t *)receiver.data().data(),
                num_coins, gas_unit_price, max_gas_amount,
                is_blocking, &index_seq);
            if (!ret)
                throw runtime_error(
                    format("failed to transfer coins, error : %s", get_last_error().c_str()));

            return make_pair(index_seq.index, index_seq.sequence);
        }

        virtual void compile(Address account_address,
                             const std::string &source_file,
                             bool is_module,
                             const std::string &temp_dir) override
        {
            auto addr = account_address.to_string();
            bool ret = libra_compile((uint64_t)raw_client_proxy, addr.c_str(),
                                     source_file.c_str(), is_module, temp_dir.c_str());
            if (!ret)
            {

                throw runtime_error(format("failed to compile move script file '%s', error : %s",
                                           source_file.c_str(),
                                           get_last_error().c_str()));
            }

            CLOG << "compiled '" << source_file << "', "
                 << "is_module = " << (is_module ? "true" : "false") << endl;
        }

        virtual void compile(uint64_t account_index,
                             const string &source_file_with_path,
                             bool is_module,
                             const std::string &temp_dir) override
        {
            bool ret = libra_compile((uint64_t)raw_client_proxy,
                                     to_string(account_index).c_str(),
                                     source_file_with_path.c_str(), is_module, temp_dir.c_str());
            if (!ret)
            {

                throw runtime_error(format("failed to compile move script file '%s', error : %s",
                                           source_file_with_path.c_str(),
                                           get_last_error().c_str()));
            }

            CLOG << "compiled '" << source_file_with_path << "', "
                 << "is_module = " << (is_module ? "true" : "false") << endl;
        }

        virtual void publish_module(uint64_t account_index,
                                    const std::string &module_file) override
        {
            bool ret = violas_publish_module((uint64_t)raw_client_proxy, account_index,
                                             module_file.c_str());
            if (!ret)
            {
                auto error = format("failed to publish module file '%s', error : %s",
                                    module_file.c_str(), get_last_error().c_str());
                throw runtime_error(error);
            }

            CLOG << "published module " << module_file << endl;
        }

        virtual void
        execute_script(uint64_t account_index,
                       std::string_view script_file,
                       const std::vector<std::string> &script_args) override
        {
            ScriptArgs args;

            vector<const char *> args_array;
            for (auto &arg : script_args)
            {
                args_array.push_back(arg.c_str());
            }

            args.len = script_args.size();
            args.data = args_array.data();

            ViolasTypeTag tag = from_type_tag(TypeTag(Address(), "", ""));

            bool ret = violas_execute_script((uint64_t)raw_client_proxy,
                                             tag,
                                             account_index,
                                             script_file.data(),
                                             &args);
            if (!ret)
                throw runtime_error(
                    format("failed to execute script file '%s' for account index %d, "
                           "error : %s, "
                           "at %s",
                           script_file.data(),
                           account_index,
                           get_last_error().data(),
                           EXCEPTION_AT.c_str()));

            CLOG << format("excuted script file '%s' for account index %d",
                           script_file.data(), account_index)
                 << endl;
        }

        /// execute script with faucet account
        virtual void execute_script(const TypeTag &currency_tag,
                                    uint64_t account_index,
                                    std::string_view script_file,
                                    const std::vector<std::string> &script_args = std::vector<std::string>()) override
        {
            ScriptArgs args;

            vector<const char *> args_array;
            for (auto &arg : script_args)
            {
                args_array.push_back(arg.c_str());
            }

            args.len = script_args.size();
            args.data = args_array.data();

            ViolasTypeTag tag = from_type_tag(currency_tag);

            bool ret = violas_execute_script((uint64_t)raw_client_proxy,
                                             tag,
                                             account_index,
                                             script_file.data(),
                                             &args);
            if (!ret)
                throw runtime_error(
                    format("failed to execute script file '%s' for account faucet, "
                           "error : %s, "
                           "at %s",
                           script_file.data(),
                           get_last_error().c_str(),
                           EXCEPTION_AT.c_str()));

            CLOG << format("excuted script file '%s' for account faucet", script_file.data())
                 << endl;
        }

        // execute script with type tag array
        virtual void
        execute_script_ex(const std::vector<TypeTag> &type_tags,
                          uint64_t sender_ref_id,
                          string_view script_file,
                          const std::vector<std::string> &script_args = std::vector<std::string>()) override
        {
            ScriptArgs args;

            vector<const char *> args_array;
            for (auto &arg : script_args)
            {
                args_array.push_back(arg.c_str());
            }

            args.len = script_args.size();
            args.data = args_array.data();

            ViolasTypeTag v_type_tags[16];
            for (size_t i = 0; i < 16 && i < type_tags.size(); i++)
                v_type_tags[i] = from_type_tag(type_tags[i]);

            bool ret = violas_execute_script_ex((uint64_t)raw_client_proxy,
                                                v_type_tags,
                                                type_tags.size(),
                                                sender_ref_id,
                                                script_file.data(),
                                                &args);
            if (!ret)
                throw runtime_error(
                    format("failed to execute script file '%s' for account %d, "
                           "error : %s, "
                           "at %s",
                           script_file.data(),
                           sender_ref_id,
                           get_last_error().c_str(),
                           EXCEPTION_AT.c_str()));
        }

        virtual std::string
        get_committed_txn_by_acc_seq(uint64_t account_index,
                                     uint64_t sequence_num,
                                     bool fetch_event) override
        {
            const auto &address = m_accounts.at(account_index).second;

            return get_committed_txn_by_acc_seq(address, sequence_num, fetch_event);
        }

        virtual std::string
        get_committed_txn_by_acc_seq(Address address, uint64_t sequence_num, bool fetch_event) override
        {
            char *out_txn = nullptr;

            bool ret = libra_get_committed_txn_by_acc_seq((uint64_t)raw_client_proxy,
                                                          address.data().data(),
                                                          sequence_num,
                                                          fetch_event,
                                                          &out_txn);
            if (!ret)
                throw runtime_error(format("failed to get committed transaction by "
                                           "account index %d and sequence number %d, "
                                           "error : %s",
                                           address.to_string().c_str(),
                                           sequence_num,
                                           get_last_error().c_str()) +
                                    EXCEPTION_AT);

            CLOG << format("get committed transaction by account index %d and sequence number %d",
                           address.to_string().c_str(), sequence_num)
                 << endl;

            string txn = out_txn;

            libra_free_string(out_txn);

            return txn;
        }

        virtual std::vector<std::pair<std::string, std::string>>
        get_txn_by_range(uint64_t start_version, uint64_t limit, bool fetch_events) override
        {
            std::vector<std::pair<std::string, std::string>> vec_txn_events;

            AllTxnEvents all_txn_events{nullptr, 0, 0};

            bool ret = libra_get_txn_by_range((uint64_t)raw_client_proxy, start_version, limit,
                                              fetch_events, &all_txn_events);
            if (!ret)
            {
                throw runtime_error(
                    format("failed to get tansaction and events by range, %d ",
                           get_last_error().c_str()) +
                    EXCEPTION_AT);
            }

            for (uint64_t i = 0; i < all_txn_events.len; i++)
            {
                auto txn_events = make_pair(all_txn_events.data[i].transaction,
                                            all_txn_events.data[i].events);
                vec_txn_events.push_back(txn_events);
            }

            libra_free_all_txn_events(&all_txn_events);

            return vec_txn_events;
        }
        //
        /// Get events by account index and event type with start sequence number and limit.
        //
        virtual std::pair<std::vector<std::string>, std::string>
        get_events(uint64_t account_index,
                   EventType type,
                   uint64_t start_seq_number,
                   uint64_t limit) override
        {
            const auto &address = m_accounts.at(account_index).second;
            return get_events(address, type, start_seq_number, limit);
        }
        //
        // Get events by account and event type with start sequence number and limit.
        //
        virtual std::pair<std::vector<std::string>, std::string>
        get_events(Address address,
                   EventType type,
                   uint64_t start_seq_number,
                   uint64_t limit) override
        {
            StrArrray out_all_events = {nullptr, 0, 0};
            char *out_last_account_event;

            bool ret = libra_get_events((uint64_t)raw_client_proxy,
                                        address.data().data(),
                                        (libra_event_type)type,
                                        start_seq_number, limit,
                                        &out_all_events,
                                        &out_last_account_event);
            if (!ret)
            {
                throw runtime_error(format("failed to get events, errror : %s ",
                                           get_last_error().c_str()));
            }

            vector<string> all_events;
            for (uint64_t i = 0; i < out_all_events.len; i++)
            {
                all_events.push_back(out_all_events.data[i]);
            }
            violas_free_str_array(&out_all_events);

            string last_account_event = out_last_account_event;
            libra_free_string(out_last_account_event);

            return make_pair<>(all_events, last_account_event);
        }

        virtual uint64_t get_account_resource_uint64(uint64_t account_index,
                                                     const Address &res_path_addr,
                                                     uint64_t token_index) override
        {
            uint64_t result = 0;
            string path_addr = "0x" + res_path_addr.to_string();

            // bool ret = libra_get_account_resource(
            //     (uint64_t)raw_client_proxy, to_string(account_index).c_str(),
            //     path_addr.c_str(), token_index, &result);
            // if (!ret)
            //     throw runtime_error(
            //         format("failed to get get resource for account index %d ", account_index) +
            //         EXCEPTION_AT);

            return result;
        }

        virtual uint64_t get_account_resource_uint64(const Address &account_addr,
                                                     const Address &res_path_addr,
                                                     uint64_t token_index) override
        {
            uint64_t result = 0;
            string path_addr = "0x" + res_path_addr.to_string();
            auto addr = account_addr.to_string();

            // bool ret = libra_get_account_resource(
            //     (uint64_t)raw_client_proxy, addr.c_str(), path_addr.c_str(), token_index, &result);
            // if (!ret)
            //     throw runtime_error(
            //         format("failed to get get resource for account address %s ", addr.c_str()) +
            //         EXCEPTION_AT);

            return result;
        }

        virtual void
        enable_custom_script(bool is_enabled) override
        {
            bool ret = libra_enable_custom_script((uint64_t)raw_client_proxy, is_enabled);

            if (!ret)
                throw runtime_error(format("failed to call enable_custom_script, errror : %s ",
                                           get_last_error().c_str()));
        }

        //
        // multi currency methods
        //

        ViolasTypeTag from_type_tag(const TypeTag &type_tag)
        {
            ViolasTypeTag tag;

            copy(begin(type_tag.address.data()), end(type_tag.address.data()), tag.address);
            tag.module = type_tag.module.data();
            tag.name = type_tag.res_name.data();

            return tag;
        }

        /// pulish a currency module wiht 0x0 address
        virtual void
        publish_currency(string_view currency_code) override
        {
            bool ret = violas_publish_currency((uint64_t)raw_client_proxy, currency_code.data());
            if (!ret)
                throw runtime_error(format("failed to publish a currency module, errror : %s ",
                                           get_last_error().c_str()));
        }

        /// register a new currency to blocakchain
        virtual void
        register_currency(const TypeTag &type_tag,
                          uint64_t exchange_rate_denom,
                          uint64_t exchange_rate_num,
                          bool is_synthetic,
                          uint64_t scaling_factor,
                          uint64_t fractional_part,
                          std::string_view currency_code) override
        {
            ViolasTypeTag tag = from_type_tag(type_tag);

            bool ret = violas_register_currency((uint64_t)raw_client_proxy,
                                                tag,
                                                exchange_rate_denom,
                                                exchange_rate_num,
                                                is_synthetic,
                                                scaling_factor,
                                                fractional_part,
                                                currency_code.data(),
                                                currency_code.size());
            if (!ret)
                throw runtime_error(format("failed to add currency, errror : %s ",
                                           get_last_error().c_str()));
        }

        /// register a new currency for designated dealer
        virtual void
        add_currency_for_designated_dealer(const TypeTag &type_tag,
                                                const Address &dd_address,
                                                bool is_blocking) override
        {
            bool ret = violas_add_currency_for_designated_dealer((uint64_t)raw_client_proxy,
                                                                      from_type_tag(type_tag),
                                                                      dd_address.data().data(),
                                                                      is_blocking);

            if (!ret)
                throw runtime_error(format("failed to register currency for designated dealer, errror : %s ",
                                           get_last_error().c_str()));
        }

        /// add a currency to current account
        virtual void
        add_currency(const TypeTag &type_tag, uint64_t account_index, bool is_blocking = true) override
        {
            ViolasTypeTag tag = from_type_tag(type_tag);
            bool ret = violas_add_currency((uint64_t)raw_client_proxy, tag, account_index, is_blocking);

            if (!ret)
                throw runtime_error(format("failed to register currency, errror : %s ",
                                           get_last_error().c_str()));
        }

        /// mint currency for a designated dealer account
        virtual void
        mint_currency(const TypeTag &tag,
                      uint64_t sliding_nonce,
                      const Address &dd_address, ////address of the designated dealer account
                      uint64_t amount,
                      uint64_t tier_index,
                      bool is_blocking = true) override
        {
            bool ret = violas_mint_currency((uint64_t)raw_client_proxy,
                                            from_type_tag(tag),
                                            sliding_nonce,
                                            dd_address.data().data(),
                                            amount,
                                            tier_index,
                                            is_blocking);
            if (!ret)
                throw runtime_error(format("failed to mint currency, errror : %s ",
                                           get_last_error().c_str()));
        }

        /// transfer currency to a receiver
        virtual void
        transfer_currency(const TypeTag &_tag,
                          uint64_t sender_account_index,
                          const Address &receiver,
                          uint64_t amount,
                          bool is_blocking) override
        {
            ViolasTypeTag tag = from_type_tag(_tag);

            bool ret = violas_transfer_currency((uint64_t)raw_client_proxy,
                                                tag,
                                                sender_account_index,
                                                receiver.data().data(),
                                                amount,
                                                is_blocking);
            if (!ret)
                throw runtime_error(format("failed to transfer currency, errror : %s ",
                                           get_last_error().c_str()));
        }

        /// get balance of currency
        virtual uint64_t
        get_currency_balance(const TypeTag &currency_tag, const Address &address, bool throw_excption) override
        {
            ViolasTypeTag tag = from_type_tag(currency_tag);
            uint64_t balance = 0;

            bool ret = violas_get_currency_balance((uint64_t)raw_client_proxy,
                                                   tag,
                                                   address.data().data(),
                                                   &balance);
            if (!ret && throw_excption)
            {
                throw runtime_error(format("failed to get currency balance, errror : %s ",
                                           get_last_error().c_str()));
            }

            return balance;
        }

        // get currency info
        virtual std::string get_currency_info() override
        {
            char *currency_info = nullptr;

            bool ret = violas_get_currency_info((uint64_t)raw_client_proxy, &currency_info);
            if (!ret)
                throw runtime_error(format("failed to get currency info, errror : %s ",
                                           get_last_error().c_str()));

            string info = currency_info;
            libra_free_string(currency_info);

            return info;
        }

        // get account state
        virtual std::pair<std::string, uint64_t>
        get_account_state(const Address &addr) override
        {
            char *out_state = nullptr;
            uint64_t version = 0;

            bool ret = violas_get_account_state((uint64_t)raw_client_proxy, addr.data().data(), &out_state, &version);
            if (!ret)
                throw runtime_error(format("failed to get account state, errror : %s ",
                                           get_last_error().c_str()));
            string state = out_state;
            libra_free_string(out_state);

            return make_pair<>(state, version);
        }

        // Create a testing account
        virtual void
        create_testing_account(const TypeTag &type_tag,
                               const AuthenticationKey &auth_key,
                               bool add_all_currencies,
                               bool is_blocking = true) override
        {
            bool ret = violas_create_testing_account((uint64_t)raw_client_proxy,
                                                     from_type_tag(type_tag),
                                                     auth_key.data().data(),
                                                     add_all_currencies,
                                                     is_blocking);
            if (!ret)
                throw runtime_error(format("failed to create parent VASP account, errror : %s ",
                                           get_last_error().c_str()));
        }

        // create parent VASP account
        virtual void
        create_parent_vasp_account(const TypeTag &type_tag,
                                   const AuthenticationKey &auth_key,
                                   std::string_view human_name,
                                   std::string_view base_url,
                                   const uint8_t compliance_pubkey[32],
                                   bool add_all_currencies,
                                   bool is_blocking) override
        {
            ViolasTypeTag tag = from_type_tag(type_tag);

            bool ret = violas_create_parent_vasp_account((uint64_t)raw_client_proxy,
                                                         tag,
                                                         auth_key.data().data(),
                                                         human_name.data(),
                                                         base_url.data(),
                                                         compliance_pubkey,
                                                         add_all_currencies,
                                                         is_blocking);
            if (!ret)
                throw runtime_error(format("failed to create parent VASP account, errror : %s ",
                                           get_last_error().c_str()));
        }

        // create child vasp account
        virtual void
        create_child_vasp_account(
            const TypeTag &type_tag,
            uint64_t parent_account_index,
            const AuthenticationKey &auth_key,
            bool add_all_currencies,
            uint64_t initial_balance,
            bool is_blocking) override
        {
            ViolasTypeTag tag = from_type_tag(type_tag);

            bool ret = violas_create_child_vasp_account((uint64_t)raw_client_proxy,
                                                        tag,
                                                        parent_account_index,
                                                        auth_key.data().data(),
                                                        add_all_currencies,
                                                        initial_balance,
                                                        is_blocking);
            if (!ret)
                throw runtime_error(format("failed to create child VASP account, errror : %s ",
                                           get_last_error().c_str()));
        }

        // create designated dealder account
        virtual void
        create_designated_dealer_account(
            const TypeTag &type_tag,
            const AuthenticationKey &auth_key,
            uint64_t nonce,
            bool is_blocking) override
        {
            ViolasTypeTag tag = from_type_tag(type_tag);

            bool ret = violas_create_designated_dealer_account((uint64_t)raw_client_proxy,
                                                               tag,
                                                               auth_key.data().data(),
                                                               nonce,
                                                               is_blocking);
            if (!ret)
                throw runtime_error(format("failed to create designated dealer account, errror : %s ",
                                           get_last_error().c_str()));
        }
    };

    std::shared_ptr<Client>
    Client::create(uint8_t chain_id,
                   const std::string &url,
                   const std::string &mint_key_file_name,
                   bool sync_on_wallet_recovery,
                   const std::string &faucet_server,
                   const std::string &mnemonic_file_name,
                   const std::string &waypoint)
    {
        return make_shared<ClientImp>(chain_id,
                                      url,
                                      mint_key_file_name,
                                      sync_on_wallet_recovery,
                                      faucet_server,
                                      mnemonic_file_name,
                                      waypoint);
    }

    class TokenManagerImp : public TokenManager
    {
    public:
        TokenManagerImp(client_ptr client,
                        Address governor_addr,
                        const std::string &name,
                        const std::string &script_files_path)
            : m_libra_client(client),
              m_name(name),
              m_supervisor(governor_addr)
        {
            init_all_script(script_files_path);
        }

        TokenManagerImp(client_ptr client,
                        Address governor_addr,
                        const std::string &name,
                        function<void(const std::string &)> init_all_script_fun,
                        const std::string &temp_path)
            : m_libra_client(client),
              m_name(name),
              m_supervisor(governor_addr),
              m_temp_path(temp_path)
        {
            string governor = m_supervisor.to_string();

            m_temp_script_path = m_temp_path / m_supervisor.to_string();

            fs::remove_all(m_temp_script_path);

            fs::create_directory(m_temp_script_path);
            CLOG << m_temp_script_path.string() << endl;

            init_all_script_fun(m_temp_script_path.string());
        }

        virtual ~TokenManagerImp() {}

        virtual std::string name() override { return m_name; }

        virtual Address address() override { return m_supervisor; }

        virtual void deploy(uint64_t account_index) override
        {
            // make sure the account-index has the deploying permission
            // assert(m_libra_client->get_all_accounts()[account_index].address == m_supervisor);

            auto script_file_name = m_temp_script_path / token_module;

            try_compile(script_file_name, true);

            m_libra_client->publish_module(account_index, (script_file_name += ".mv").string());
        }

        virtual void publish(uint64_t account_index, const std::string &user_data) override
        {
            auto script_file_name = m_temp_script_path / publish_script;

            try_compile(script_file_name);

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").string(),
                                           vector<string>{tx_vec_data(user_data)});
        }

        virtual void create_token(uint64_t account_index, Address owner, const std::string &token_data = "token") override
        {
            auto script_file_name = m_temp_script_path / create_token_script;

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").string(),
                                           vector<string>{owner.to_string(), tx_vec_data(token_data)});
        }

        virtual void mint(uint64_t token_index,
                          uint64_t account_index,
                          Address receiver,
                          uint64_t amount_micro_coin,
                          const std::string &data) override
        {
            // make sure the account-index has the minting permission
            //assert(m_libra_client->get_all_accounts()[account_index].address == m_supervisor);

            auto script_file_name = m_temp_script_path / mint_script;

            try_compile(script_file_name);

            auto args = vector<string>{to_string(token_index),
                                       receiver.to_string(),
                                       to_string(amount_micro_coin),
                                       tx_vec_data(data)};

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").string(), args);
        }

        virtual void transfer(uint64_t token_index,
                              uint64_t account_index,
                              Address receiver,
                              uint64_t amount_micro_coin,
                              const std::string &data) override
        {
            auto script_file_name = m_temp_script_path / transfer_script;

            try_compile(script_file_name);

            auto args = vector<string>{to_string(token_index),
                                       receiver.to_string(),
                                       to_string(amount_micro_coin),
                                       tx_vec_data(data)};

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").string(), args);
        }

        virtual uint64_t token_count() override
        {
            return 0;
        }

        virtual uint64_t get_account_balance(uint64_t token_index, uint64_t account_index) override
        {
            uint64_t balance = m_libra_client->get_account_resource_uint64(account_index,
                                                                           m_supervisor,
                                                                           token_index);
            return balance;
        }

        virtual uint64_t get_account_balance(uint64_t token_index, Address account_address) override
        {
            uint64_t balance = m_libra_client->get_account_resource_uint64(account_address,
                                                                           m_supervisor,
                                                                           token_index);
            return balance;
        }

    protected:
        void init_all_script(const string &script_files_path)
        {
            string governor = m_supervisor.to_string();

            if (m_temp_path.empty())
                m_temp_path = fs::temp_directory_path(); // /tmp/xxxxx

            m_temp_script_path = m_temp_path / fs::path(governor);

            fs::remove_all(m_temp_script_path);

            fs::create_directory(m_temp_script_path);
            CLOG << m_temp_script_path.string() << endl;

            for (auto &file : fs::directory_iterator(fs::path(script_files_path)))
            {
                if (file.path().extension() == ".mv")
                {
                    auto new_file = m_temp_script_path / file.path().filename().string();

#if __cplusplus >= 201703L
                    //auto option = fs::copy_options::overwrite_existing;
#else
                    //auto option = fs::copy_option::overwrite_if_exists;
#endif
                    //fs::copy_file(file.path().string(), new_file, option);

                    replace_mv_with_addr(file.path().string(), new_file.string(), m_supervisor);
                }
            }
        }

        void try_compile(fs::path script_file_name, bool is_module = false)
        {
            //auto mv(fs::path(script_file_name) += ".mv");
            //auto mvir(fs::path(script_file_name) += ".mvir");

            //if (!fs::exists(mv))
            //    m_libra_client->compile(m_supervisor, mvir.c_str(), is_module, m_temp_path.string());
        }

    private:
        client_ptr m_libra_client;
        string m_name;
        Address m_supervisor;
        string m_module;
        fs::path m_temp_path;
        fs::path m_temp_script_path;
        const string token_module = "token";
        const string publish_script = "publish";
        const string create_token_script = "create_token";
        const string mint_script = "mint";
        const string transfer_script = "transfer";
    };

    std::shared_ptr<TokenManager> TokenManager::create(client_ptr client,
                                                       Address governor_addr,
                                                       const std::string &name,
                                                       const std::string &script_files_path)
    {
        return make_shared<TokenManagerImp>(client, governor_addr, name, script_files_path);
    }

    std::shared_ptr<TokenManager> TokenManager::create(client_ptr client,
                                                       Address governor_addr,
                                                       const std::string &name,
                                                       function<void(const std::string &)> init_all_script_fun,
                                                       const std::string &temp_path)
    {
        return make_shared<TokenManagerImp>(client, governor_addr, name, init_all_script_fun, temp_path);
    }

    class ExchangeImp : public Exchange
    {
    private:
        client_ptr m_client;

    public:
        ExchangeImp(client_ptr client,
                    std::string_view exchange_contracts_path)
            : m_client(client),
              m_script_path(exchange_contracts_path)
        {
        }

        virtual void
        deploy_with_association_account() override
        {
            m_client->publish_module(ASSOCIATION_ID, _module_exdep);

            m_client->publish_module(ASSOCIATION_ID, _module_exchange);

            //  initialize module Exchange under association account
            m_client->execute_script(ASSOCIATION_ID, _script_initialize);
        }

        virtual void
        add_currency(std::string_view currency_code) override
        {
            TypeTag tag(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script(tag, ASSOCIATION_ID, _script_add_currency);
        }

        virtual std::vector<std::string>
        get_currencies(const Address &address) override
        {
            auto client_imp = dynamic_pointer_cast<ClientImp>(m_client);

            char *out_currencie_codes = nullptr;
            bool ret = violas_get_exchange_currencies((uint64_t)client_imp->get_raw_client(),
                                                      address.data().data(),
                                                      &out_currencie_codes);
            if (!ret)
                throw runtime_error(client_imp->get_last_error().c_str());

            //string temp = out_currencie_codes;
            json currency_codes = json::parse(out_currencie_codes);
            libra_free_string(out_currencie_codes);

            if (currency_codes.empty())
                throw runtime_error("The Exchange currency codes are empty");

            vector<string> codes;

            for (const auto &currency_code : currency_codes["currency_codes"])
            {
                string code;

                for (auto c : currency_code)
                    code += char(c.get<int>());

                codes.push_back(code);
            }

            cout << " the size of codes is " << codes.size() << endl;

            return codes;
        }

        virtual std::string
        get_reserves(const Address &address) override
        {
            auto client_imp = dynamic_pointer_cast<ClientImp>(m_client);
            void *raw_client = client_imp->get_raw_client();

            char *json_reservers_info = nullptr;
            bool ret = violas_get_exchange_reserves((uint64_t)raw_client,
                                                    address.data().data(),
                                                    &json_reservers_info);
            if (!ret)
                throw runtime_error(client_imp->get_last_error().c_str());

            string temp = json_reservers_info;
            libra_free_string(json_reservers_info);

            return temp;
        }

        virtual std::string
        get_liquidity_balance(const Address &address) override
        {
            auto client_imp = dynamic_pointer_cast<ClientImp>(m_client);

            char *json = nullptr;
            bool ret = violas_get_liquidity_balance((uint64_t)client_imp->get_raw_client(),
                                                    address.data().data(),
                                                    &json);
            if (!ret)
                throw runtime_error(client_imp->get_last_error().c_str());

            string temp = json;
            libra_free_string(json);

            return temp;
        }

        virtual void
        add_liquidity(
            uint64_t account_index,
            const LiquidityInfo &first,
            const LiquidityInfo &second) override
        {
            TypeTag currency_a(CORE_CODE_ADDRESS, first.currency_code, first.currency_code);
            TypeTag currency_b(CORE_CODE_ADDRESS, second.currency_code, second.currency_code);

            m_client->execute_script_ex({currency_a, currency_b},
                                        account_index,
                                        _script_add_liquidity,
                                        {to_string(first.disired_amount),
                                         to_string(second.disired_amount),
                                         to_string(first.min_amount),
                                         to_string(second.min_amount)});
        }

        // remove liquidity
        virtual void
        remove_liquidity(
            size_t account_index,
            uint64_t liquidity_amount,
            std::string_view currency_code_a, uint64_t a_acceptable_min_amount,
            std::string_view currency_code_b, uint64_t b_acceptable_min_amount) override
        {
            TypeTag currency_a(CORE_CODE_ADDRESS, currency_code_a, currency_code_a);
            TypeTag currency_b(CORE_CODE_ADDRESS, currency_code_b, currency_code_b);

            m_client->execute_script_ex({currency_a, currency_b},
                                        account_index,
                                        _script_remove_liquidity,
                                        {to_string(liquidity_amount),
                                         to_string(a_acceptable_min_amount),
                                         to_string(b_acceptable_min_amount)});
        }

        // swap currency from A to B
        virtual void
        swap(size_t account_index,
             const Address &receiver,
             string_view currency_code_a,
             uint64_t amount_a,
             string_view currency_code_b,
             uint64_t b_acceptable_min_amount) override
        {
            TypeTag currency_tag_a(CORE_CODE_ADDRESS, currency_code_a, currency_code_a);
            TypeTag currency_tag_b(CORE_CODE_ADDRESS, currency_code_b, currency_code_b);
            auto path = find_swap_path(currency_code_a, amount_a, currency_code_b);
            // path = find_swap_path("VLSUSD", 100000, "VLSSGD");
            // path = find_swap_path("VLSSGD", 100000, "VLSUSD");

            m_client->execute_script_json(_script_swap_currency,
                                          account_index,
                                          {currency_tag_a, currency_tag_b},
                                          //arguments
                                          receiver, amount_a, b_acceptable_min_amount, path, VecU8());
        }

    protected:
        vector<uint8_t>
        find_swap_path(std::string_view currency_code_a, uint64_t currency_a_amount, std::string_view currency_code_b)
        {
            auto currency_codes = get_currencies(ASSOCIATION_ADDRESS);
            size_t currency_a_index = distance(begin(currency_codes), find(begin(currency_codes), end(currency_codes), currency_code_a));
            size_t currency_b_index = distance(begin(currency_codes), find(begin(currency_codes), end(currency_codes), currency_code_b));

            using vertex = pair<size_t, uint64_t>; // index and the requested value
            using edge = tuple<size_t, uint64_t, size_t, uint64_t>;

            vertex dist_to[32] = {{0, 0}};

            map<size_t, vector<edge>> v_to_e; //vertex index maps to edges
            auto vertex_comp = [](const vertex &a, const vertex &b) { return a.second > b.second; };
            priority_queue<vertex, vector<vertex>, decltype(vertex_comp)> minpq(vertex_comp);
            auto relax = [&](const vertex &v, const edge &e) -> void {
                size_t coin_a = get<0>(e);
                uint64_t coin_a_value = get<1>(e);
                size_t coin_b = get<2>(e);
                uint64_t coin_b_value = get<3>(e);
                uint64_t available_value = v.second * coin_b_value / coin_a_value;

                if (coin_a_value >= v.second && //coin a have enought liquidity
                    dist_to[coin_b].second < available_value)
                {
                    dist_to[coin_b].first = coin_a;           //pre vertex
                    dist_to[coin_b].second = available_value; //most value

                    minpq.push(make_pair<>(coin_b, available_value));
                }
            };
            //
            //  initialize all vertex and edge
            //
            json reserves = json::parse(get_reserves(ASSOCIATION_ADDRESS));

            for (const auto &reserve : reserves["reserves"])
            {
                cout << reserve << endl;

                int coin_a_index = reserve["coina"]["index"].get<int>();
                uint64_t coin_a_value = reserve["coina"]["value"].get<uint64_t>();
                int coin_b_index = reserve["coinb"]["index"].get<int>();
                uint64_t coin_b_value = reserve["coinb"]["value"].get<double>();

                if (currency_a_index < currency_b_index)
                {
                    // forward path
                    v_to_e[coin_a_index].push_back(make_tuple(coin_a_index,
                                                              coin_a_value,
                                                              coin_b_index,
                                                              coin_b_value));
                }
                else
                {
                    // backward path
                    v_to_e[coin_b_index].push_back(make_tuple(coin_b_index,
                                                              coin_b_value,
                                                              coin_a_index,
                                                              coin_a_value));
                }
            }
            //
            //  search the most value path
            //
            minpq.push(make_pair<>(currency_a_index, currency_a_amount));

            while (!minpq.empty())
            {
                vertex v = minpq.top();
                minpq.pop();

                for (auto e : v_to_e[v.first])
                {
                    relax(v, e);
                }
            }

            stack<uint8_t> path;
            for (size_t i = currency_b_index;
                 i != currency_a_index;
                 i = dist_to[i].first)
            {
                path.push(i);
            }
            path.push(currency_a_index);

            vector<uint8_t> forward_path;
            while (!path.empty())
            {
                uint8_t v = path.top();
                path.pop();

                forward_path.push_back(v);
            }

            return forward_path;
        }

    private:
        std::string m_script_path;
        const std::string _module_exchange = m_script_path + "exchange.mv";
        const std::string _module_exdep = m_script_path + "exdep.mv";
        const std::string _script_initialize = m_script_path + "initialize.mv";
        const std::string _script_add_currency = m_script_path + "add_currency.mv";
        const std::string _script_add_liquidity = m_script_path + "add_liquidity.mv";
        const std::string _script_remove_liquidity = m_script_path + "remove_liquidity.mv";
        const std::string _script_swap_currency = m_script_path + "swap.mv";
    };

    std::shared_ptr<Exchange> Exchange::create(client_ptr client,
                                               std::string_view exchange_contracts_path)
    {
        return make_shared<ExchangeImp>(client, exchange_contracts_path);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //  BankImp
    //  Implementation for Bank interface
    /////////////////////////////////////////////////////////////////////////////////////
    class BankImp : public Bank
    {
    public:
        BankImp(client_ptr client,
                std::string_view bank_contracts_path) : m_client(client),
                                                        m_bank_path(bank_contracts_path)
        {
        }

        virtual ~BankImp() {}

        virtual void
        deploy_with_association_account() override
        {
            m_client->publish_module(ASSOCIATION_ID, _module_bank);
        }

        virtual void
        publish(size_t account_index) override
        {
            //m_client->execute_script_ex({}, account_index, _script_publish, {"b\"00\""});
            m_client->execute_script_json(_script_publish,
                                          account_index,
                                          {},
                                          VecU8({0}));
        }

        virtual void
        add_currency(std::string_view currency_code,
                     const Address &owner,
                     uint64_t collateral_factor,
                     uint64_t base_rate,
                     uint64_t rate_multiplier,
                     uint64_t rate_jump_multiplier,
                     uint64_t rate_kink) override
        {
            TypeTag type_tag(CORE_CODE_ADDRESS, currency_code, currency_code);

            //{owner.to_string(), to_string(collateral_factor), "b\"00\""}
            m_client->execute_script_json(_script_register_libra_token,
                                          ASSOCIATION_ID,
                                          {type_tag},
                                          // script arguments
                                          owner,
                                          collateral_factor,
                                          base_rate,
                                          rate_multiplier,
                                          rate_jump_multiplier,
                                          rate_kink,
                                          VecU8());
        }

        virtual void
        update_currency_price(std::string_view currency_code, uint64_t price) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_scirpt_update_price,
                                          ASSOCIATION_ID,
                                          {currency},
                                          // script arguments
                                          price);
        }

        virtual void
        enter(size_t account_index, std::string_view currency_code, uint64_t amount) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_script_enter_bank,
                                          account_index,
                                          {currency},
                                          // script arguments
                                          amount);
        }

        virtual void
        exit(size_t account_index, std::string_view currency_code, uint64_t amount) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_script_exit_bank,
                                          account_index,
                                          {currency},
                                          // script arguments
                                          amount);
        }

        virtual void
        lock(size_t account_index,
             std::string_view currency_code,
             uint64_t amount) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_script_lock,
                                          account_index,
                                          {currency},
                                          // script arguments
                                          amount, VecU8());
        }

        virtual void
        redeem(size_t account_index,
               std::string_view currency_code,
               uint64_t amount) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_script_redeem,
                                          account_index,
                                          {currency},
                                          // script arguments
                                          amount, VecU8());
        }

        virtual void
        borrow(size_t account_index,
               std::string_view currency_code,
               uint64_t amount) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_script_borrow,
                                          account_index,
                                          {currency},
                                          // script arguments
                                          amount, VecU8());
        }

        virtual void
        repay_borrow(size_t account_index,
                     std::string_view currency_code,
                     uint64_t amount) override
        {
            TypeTag currency(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script_json(_script_repay_borrow,
                                          account_index,
                                          {currency},
                                          // script arguments
                                          amount, VecU8());
        }

        virtual void
        liquidate_borrow(size_t account_index,
                         std::string_view borrowed_currency_code,
                         const Address &liquidated_user_addr,
                         uint64_t amount,
                         std::string_view liquidated_currency_code) override
        {
            TypeTag borrowed_currency(CORE_CODE_ADDRESS, borrowed_currency_code, borrowed_currency_code);
            TypeTag liquidated_currency(CORE_CODE_ADDRESS, liquidated_currency_code, liquidated_currency_code);

            m_client->execute_script_json(_script_liquidate_borrow,
                                          account_index,
                                          {borrowed_currency, liquidated_currency},
                                          // script arguments
                                          liquidated_user_addr, amount, VecU8());
        }

    private:
        client_ptr m_client;
        string m_bank_path;
        const string _module_bank = m_bank_path + "bank.mv";
        const string _script_borrow = m_bank_path + "borrow.mv";
        const string _script_create_token = m_bank_path + "create_token.mv";
        const string _script_enter_bank = m_bank_path + "enter_bank.mv";
        const string _script_exit_bank = m_bank_path + "exit_bank.mv";
        const string _script_liquidate_borrow = m_bank_path + "liquidate_borrow.mv";
        const string _script_lock = m_bank_path + "lock.mv";
        const string _script_mint = m_bank_path + "mint.mv";
        const string _script_publish = m_bank_path + "publish.mv";
        const string _script_redeem = m_bank_path + "redeem.mv";
        const string _script_register_libra_token = m_bank_path + "register_libra_token.mv";
        const string _script_repay_borrow = m_bank_path + "repay_borrow.mv";
        const string _script_update_collateral_factor = m_bank_path + "update_collateral_factor.mv";
        const string _scirpt_update_price = m_bank_path + "update_price.mv";
    }; // Bank

    std::shared_ptr<Bank>
    Bank::create_bank(client_ptr client,
                      std::string_view bank_contracts_path)
    {
        return make_shared<BankImp>(client, bank_contracts_path);
    }
} // namespace LIB_NAME

#ifdef PYTHON
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ClientImp_transfer_overloads, ClientImp::transfer_coins_int, 3, 6)

BOOST_PYTHON_MODULE(violas)
{
    using namespace Violas;

    class_<std::pair<ulong, uint256>>("IntPair")
        .def_readwrite("first", &std::pair<ulong, uint256>::first)
        .def_readwrite("second", &std::pair<ulong, uint256>::second);

    class_<std::array<unsigned char, 32ul>>("uint256");
    class_<std::pair<ulong, ulong>>("UlongPair");

    def("uint256_to_string", uint256_to_string);
    def("uint256_from_string", uint256_from_string);

    class_<Client::Account>("Account")
        .add_property("index", &Client::Account::index)
        .add_property("address", &Client::Account::address)
        .add_property("sequence_number", &Client::Account::sequence_number)
        .add_property("status", &Client::Account::status);

    using Accounts = std::vector<Client::Account>;
    class_<Accounts>("Accounts")
        .def(vector_indexing_suite<Accounts>());

    void (ClientImp::*compile1)(uint64_t, const string &, bool) = &ClientImp::compile;

    class_<ClientImp, std::shared_ptr<ClientImp>>("Client", init<string, uint16_t, string, string, bool, string, string>())
        .def("test_validator_connection", &ClientImp::test_validator_connection)
        .def("create_next_account", &ClientImp::create_next_account)
        .def("get_all_accounts", &ClientImp::get_all_accounts)
        .def("get_balance", (double (ClientImp::*)(uint64_t index)) & ClientImp::get_balance)
        .def("get_sequence_number", &ClientImp::get_sequence_number)
        .def("mint_coins", &ClientImp::mint_coins)
        .def("transfer", &ClientImp::transfer_coins_int, ClientImp_transfer_overloads())
        .def("compile", compile1)
        .def("publish_module", &ClientImp::publish_module)
        .def("execute_script", &ClientImp::execute_script)
        .def("get_committed_txn_by_acc_seq", &ClientImp::get_committed_txn_by_acc_seq)
        .def("get_txn_by_range", &ClientImp::get_txn_by_range);

    uint64_t (TokenImp::*get_account_balance1)(uint64_t) = &TokenImp::get_account_balance;
    uint64_t (TokenImp::*get_account_balance2)(uint256) = &TokenImp::get_account_balance;

    class_<TokenImp, std::shared_ptr<TokenImp>>("Token", init<client_ptr, uint256, string, string>())
        .def("name", &TokenImp::name)
        .def("address", &TokenImp::address)
        .def("deploy", &TokenImp::deploy)
        .def("mint", &TokenImp::mint)
        .def("publish", &TokenImp::publish)
        .def("transfer", &TokenImp::transfer)
        .def("get_account_balance", get_account_balance1)
        .def("get_account_balance", get_account_balance2);

    implicitly_convertible<std::shared_ptr<ClientImp>, std::shared_ptr<Client>>();
}

#endif
