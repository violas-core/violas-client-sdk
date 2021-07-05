#include <string_view>
#include <string>
#include <optional>
#include <variant>
#include <client.hpp>
#include "bcs_serde.hpp"
#include "json_rpc.hpp"

namespace violas
{
    const Address BANK_ADMIN_ADDRESS = Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x42, 0x41, 0x4E, 0x4B});     //BANK,00000000000000000000000042414E4B
    const Address EXCHANGE_ADMIN_ADDRESS = Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x45, 0x58, 0x43, 0x48}); //EXCH,00000000000000000000000045584348

    class Exchange
    {
    public:
        static std::shared_ptr<Exchange>
        create(client_ptr client,
               std::string_view exchange_contracts_path,
               const AddressAndIndex &admin);

        virtual ~Exchange() {}

        virtual void
        deploy_with_root_account() = 0;

        //
        //  Initialize Exchange contacts with administrator account index
        //
        virtual void
        initialize(const AddressAndIndex &admin, const Address &distributor_address) = 0;

        virtual void
        add_currency(std::string_view currency_code) = 0;

        virtual std::vector<std::string>
        get_currencies() = 0;

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
        get_reserves() = 0;

        virtual std::string
        get_liquidity_balance() = 0;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////

    class Bank
    {
    public:
        static const uint64_t MANTISSA_1_0 = std::numeric_limits<uint32_t>::max();

        static std::shared_ptr<Bank>
        create_bank(client_ptr client,
                    std::string_view bank_contracts_path);

        virtual ~Bank() {}

        virtual void
        deploy_with_root_account() = 0;

        virtual void
        initialize(const AddressAndIndex &admin) = 0;

        virtual void
        publish(size_t account_index) = 0;

        virtual void
        add_currency(std::string_view currency_code,
                     const Address &owner,
                     uint64_t collateral_factor,
                     uint64_t base_rate,
                     uint64_t rate_multiplier,
                     uint64_t rate_jump_multiplier,
                     uint64_t rate_kink) = 0;

        virtual void
        update_currency_price(std::string_view currency_code, uint64_t price) = 0;

        virtual void
        enter(size_t account_index,
              std::string_view currency_code,
              uint64_t amount) = 0;

        virtual void
        exit(size_t account_index, std::string_view currency_code, uint64_t amount) = 0;

        virtual void
        lock(size_t account_index,
             std::string_view currency_code,
             uint64_t amount) = 0;

        virtual void
        redeem(size_t account_index,
               std::string_view currency_code,
               uint64_t amount) = 0;

        virtual void
        borrow(size_t account_index,
               std::string_view currency_code,
               uint64_t amount) = 0;

        virtual void
        repay_borrow(size_t account_index,
                     std::string_view currency_code,
                     uint64_t amount) = 0;

        virtual void
        liquidate_borrow(size_t account_index,
                         std::string_view borrowed_currency_code,
                         const Address &liquidated_user,
                         uint64_t amount,
                         std::string_view liquidated_currency_code) = 0;

    }; // Bank

    using bank_ptr = std::shared_ptr<Bank>;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline BcsSerde &operator&&(BcsSerde &serde, TypeTag &tag)
    {
        return serde && tag.address && tag.module_name && tag.resource_name;
    }

    struct StructTag;
    using _TypeTag = std::variant<
        bool,
        uint8_t,
        uint64_t,
        __uint128_t,
        Address,
        std::string, // Signer,
        std::vector<std::string>,
        StructTag>;

    struct StructTag
    {
        Address address;
        std::string module;
        std::string name;
        // TODO: rename to "type_args" (or better "ty_args"?)
        std::vector<_TypeTag> type_params;

        BcsSerde &serde(BcsSerde &serde)
        {
            return serde && address && module && name && type_params;
        }
    };

    struct ModuleId
    {
        Address address;
        std::string name;

        BcsSerde &serde(BcsSerde &serde)
        {
            return serde && address && name;
        }
    };

    struct AccessPath
    {
        std::variant<ModuleId, StructTag> path;

        AccessPath(const StructTag &tag)
        {
            path = tag;
        }

        AccessPath(const ModuleId &module_id)
        {
            path = module_id;
        }

        BcsSerde &serde(BcsSerde &serde)
        {
            return serde && path;
        }
    };

    class AccountState
    {
        std::map<std::vector<uint8_t>, std::vector<uint8_t>> _resources;
        json_rpc::client_ptr _client;

    public:
        AccountState(const std::string &hex);

        AccountState(json_rpc::client_ptr client) : _client(client) {}

        template <typename T>
        std::optional<T> get_resource(Address address, StructTag tag)
        {
            get_account_state(address);

            AccessPath path(tag);

            BcsSerde serde;
            serde &&path;

            auto iter = _resources.find(serde.bytes());
            if (iter != end(_resources))
            {
                T t;

                BcsSerde serde(iter->second);

                serde &&t;

                return t;
            }
            else
                return {};
        }

    protected:
        void get_account_state(Address address);
    };

} // namespace violas