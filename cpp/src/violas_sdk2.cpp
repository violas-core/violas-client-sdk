#include <vector>
#include <stack>
#include <queue>
#include <json.hpp>
#include "violas_sdk2.hpp"

using namespace std;
using json = nlohmann::json;

namespace violas
{
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
        deploy_with_account(size_t account_index) override
        {
            m_client->publish_module(account_index, _module_exdep);

            m_client->publish_module(account_index, _module_exchange);
        }

        //
        //  Initialize Exchange contacts with administrator account index
        //
        virtual void
        initialize(const AddressAndIndex &admin) override
        {
            m_admin = admin;

            //  initialize module Exchange under association account
            m_client->execute_script(m_admin.index, _script_initialize);
        }

        virtual void
        add_currency(std::string_view currency_code) override
        {
            TypeTag tag(CORE_CODE_ADDRESS, currency_code, currency_code);

            m_client->execute_script(m_admin.index, _script_add_currency, vector<TypeTag>{tag});
        }

        virtual std::vector<std::string>
        get_currencies() override
        {
            string currencies = m_client->get_exchange_currencies(m_admin.address);

            json currency_codes = json::parse(currencies);

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

            return codes;
        }

        virtual std::string
        get_reserves() override
        {
            string json_reservers_info = m_client->get_exchange_reserves(m_admin.address);

            return json_reservers_info;
        }

        virtual std::string
        get_liquidity_balance() override
        {
            string json_balances = m_client->get_liquidity_balance(m_admin.address);

            return json_balances;
        }

        virtual void
        add_liquidity(
            uint64_t account_index,
            const LiquidityInfo &first,
            const LiquidityInfo &second) override
        {
            TypeTag currency_a(CORE_CODE_ADDRESS, first.currency_code, first.currency_code);
            TypeTag currency_b(CORE_CODE_ADDRESS, second.currency_code, second.currency_code);

            m_client->execute_script(account_index,
                                     _script_add_liquidity,
                                     {currency_a, currency_b},
                                     {first.desired_amount,
                                      second.desired_amount,
                                      first.min_amount,
                                      second.min_amount});
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

            m_client->execute_script(account_index,
                                     _script_remove_liquidity,
                                     {currency_a, currency_b},
                                     {liquidity_amount,
                                      a_acceptable_min_amount,
                                      b_acceptable_min_amount});
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

            m_client->execute_script(account_index,
                                     _script_swap_currency,
                                     {currency_tag_a, currency_tag_b},
                                     {receiver, amount_a, b_acceptable_min_amount, path, VecU8()});
        }

    protected:
        vector<uint8_t>
        find_swap_path(std::string_view currency_code_a, uint64_t currency_a_amount, std::string_view currency_code_b)
        {
            auto currency_codes = get_currencies();
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
            json reserves = json::parse(get_reserves());

            for (const auto &reserve : reserves["reserves"])
            {
                //cout << reserve << endl;

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
        AddressAndIndex m_admin;
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

} // namespace violas