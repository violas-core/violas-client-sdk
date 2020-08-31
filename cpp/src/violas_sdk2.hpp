#include <string_view>
#include <string>
#include <client.hpp>

namespace violas
{
    class Exchange
    {
    public:
        static std::shared_ptr<Exchange>
        create(client_ptr client,
               std::string_view exchange_contracts_path);

        virtual ~Exchange() {}

        virtual void
        deploy_with_account(size_t account_index) = 0;

        //
        //  Initialize Exchange contacts with administrator account index
        //
        virtual void
        initialize(size_t admin_account_index) = 0;

        virtual void
        add_currency(std::string_view currency_code) = 0;

        virtual std::vector<std::string>
        get_currencies(const Address &address) = 0;

        struct LiquidityInfo
        {
            std::string currency_code;
            uint64_t desired_amount;
            uint64_t min_amount;
        };
        //
        //  function add_liquidity
        //  add liquidity to reserve pool
        //  this function makes the rate
        virtual void
        add_liquidity(
            size_t account_index,
            const LiquidityInfo &first,
            const LiquidityInfo &second) = 0;

        //get all liqudity info
        virtual std::string
        get_reserves(const Address &address) = 0;

        virtual std::string
        get_liquidity_balance(const Address &address) = 0;

        // remove liquidity
        virtual void
        remove_liquidity(
            size_t account_index,
            uint64_t liquidity_amount,
            std::string_view currency_code_a, uint64_t a_min_amount,
            std::string_view currency_code_b, uint64_t b_min_amount) = 0;

        // swap currency from A to B
        virtual void
        swap(size_t account_index,
             const Address &receiver,
             std::string_view currency_code_a, uint64_t amount_a,
             std::string_view currency_code_b, uint64_t b_acceptable_min_amount) = 0;

    }; // Exchange

    using exchange_ptr = std::shared_ptr<Exchange>;
}