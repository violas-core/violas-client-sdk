#pragma once

#include "serde.hpp"
#include "bcs.hpp"

namespace diem_types {

    struct CoinTradeMetadataV0 {
        std::vector<std::string> trade_ids;

        friend bool operator==(const CoinTradeMetadataV0&, const CoinTradeMetadataV0&);
        std::vector<uint8_t> bcsSerialize() const;
        static CoinTradeMetadataV0 bcsDeserialize(std::vector<uint8_t>);
    };

    struct CoinTradeMetadata {

        struct CoinTradeMetadataV0 {
            diem_types::CoinTradeMetadataV0 value;

            friend bool operator==(const CoinTradeMetadataV0&, const CoinTradeMetadataV0&);
            std::vector<uint8_t> bcsSerialize() const;
            static CoinTradeMetadataV0 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<CoinTradeMetadataV0> value;

        friend bool operator==(const CoinTradeMetadata&, const CoinTradeMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static CoinTradeMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct GeneralMetadataV0 {
        std::optional<std::vector<uint8_t>> to_subaddress;
        std::optional<std::vector<uint8_t>> from_subaddress;
        std::optional<uint64_t> referenced_event;

        friend bool operator==(const GeneralMetadataV0&, const GeneralMetadataV0&);
        std::vector<uint8_t> bcsSerialize() const;
        static GeneralMetadataV0 bcsDeserialize(std::vector<uint8_t>);
    };

    struct GeneralMetadata {

        struct GeneralMetadataVersion0 {
            diem_types::GeneralMetadataV0 value;

            friend bool operator==(const GeneralMetadataVersion0&, const GeneralMetadataVersion0&);
            std::vector<uint8_t> bcsSerialize() const;
            static GeneralMetadataVersion0 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<GeneralMetadataVersion0> value;

        friend bool operator==(const GeneralMetadata&, const GeneralMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static GeneralMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct PaymentMetadataV0 {
        std::array<uint8_t, 16> reference_id;

        friend bool operator==(const PaymentMetadataV0&, const PaymentMetadataV0&);
        std::vector<uint8_t> bcsSerialize() const;
        static PaymentMetadataV0 bcsDeserialize(std::vector<uint8_t>);
    };

    struct PaymentMetadata {

        struct PaymentMetadataVersion0 {
            diem_types::PaymentMetadataV0 value;

            friend bool operator==(const PaymentMetadataVersion0&, const PaymentMetadataVersion0&);
            std::vector<uint8_t> bcsSerialize() const;
            static PaymentMetadataVersion0 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<PaymentMetadataVersion0> value;

        friend bool operator==(const PaymentMetadata&, const PaymentMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static PaymentMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct RefundReason {

        struct OtherReason {
            friend bool operator==(const OtherReason&, const OtherReason&);
            std::vector<uint8_t> bcsSerialize() const;
            static OtherReason bcsDeserialize(std::vector<uint8_t>);
        };

        struct InvalidSubaddress {
            friend bool operator==(const InvalidSubaddress&, const InvalidSubaddress&);
            std::vector<uint8_t> bcsSerialize() const;
            static InvalidSubaddress bcsDeserialize(std::vector<uint8_t>);
        };

        struct UserInitiatedPartialRefund {
            friend bool operator==(const UserInitiatedPartialRefund&, const UserInitiatedPartialRefund&);
            std::vector<uint8_t> bcsSerialize() const;
            static UserInitiatedPartialRefund bcsDeserialize(std::vector<uint8_t>);
        };

        struct UserInitiatedFullRefund {
            friend bool operator==(const UserInitiatedFullRefund&, const UserInitiatedFullRefund&);
            std::vector<uint8_t> bcsSerialize() const;
            static UserInitiatedFullRefund bcsDeserialize(std::vector<uint8_t>);
        };

        struct InvalidReferenceId {
            friend bool operator==(const InvalidReferenceId&, const InvalidReferenceId&);
            std::vector<uint8_t> bcsSerialize() const;
            static InvalidReferenceId bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<OtherReason, InvalidSubaddress, UserInitiatedPartialRefund, UserInitiatedFullRefund, InvalidReferenceId> value;

        friend bool operator==(const RefundReason&, const RefundReason&);
        std::vector<uint8_t> bcsSerialize() const;
        static RefundReason bcsDeserialize(std::vector<uint8_t>);
    };

    struct RefundMetadataV0 {
        uint64_t transaction_version;
        diem_types::RefundReason reason;

        friend bool operator==(const RefundMetadataV0&, const RefundMetadataV0&);
        std::vector<uint8_t> bcsSerialize() const;
        static RefundMetadataV0 bcsDeserialize(std::vector<uint8_t>);
    };

    struct RefundMetadata {

        struct RefundMetadataV0 {
            diem_types::RefundMetadataV0 value;

            friend bool operator==(const RefundMetadataV0&, const RefundMetadataV0&);
            std::vector<uint8_t> bcsSerialize() const;
            static RefundMetadataV0 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<RefundMetadataV0> value;

        friend bool operator==(const RefundMetadata&, const RefundMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static RefundMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct TravelRuleMetadataV0 {
        std::optional<std::string> off_chain_reference_id;

        friend bool operator==(const TravelRuleMetadataV0&, const TravelRuleMetadataV0&);
        std::vector<uint8_t> bcsSerialize() const;
        static TravelRuleMetadataV0 bcsDeserialize(std::vector<uint8_t>);
    };

    struct TravelRuleMetadata {

        struct TravelRuleMetadataVersion0 {
            diem_types::TravelRuleMetadataV0 value;

            friend bool operator==(const TravelRuleMetadataVersion0&, const TravelRuleMetadataVersion0&);
            std::vector<uint8_t> bcsSerialize() const;
            static TravelRuleMetadataVersion0 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<TravelRuleMetadataVersion0> value;

        friend bool operator==(const TravelRuleMetadata&, const TravelRuleMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static TravelRuleMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct UnstructuredBytesMetadata {
        std::optional<std::vector<uint8_t>> metadata;

        friend bool operator==(const UnstructuredBytesMetadata&, const UnstructuredBytesMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static UnstructuredBytesMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct Metadata {

        struct Undefined {
            friend bool operator==(const Undefined&, const Undefined&);
            std::vector<uint8_t> bcsSerialize() const;
            static Undefined bcsDeserialize(std::vector<uint8_t>);
        };

        struct GeneralMetadata {
            diem_types::GeneralMetadata value;

            friend bool operator==(const GeneralMetadata&, const GeneralMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static GeneralMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        struct TravelRuleMetadata {
            diem_types::TravelRuleMetadata value;

            friend bool operator==(const TravelRuleMetadata&, const TravelRuleMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static TravelRuleMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        struct UnstructuredBytesMetadata {
            diem_types::UnstructuredBytesMetadata value;

            friend bool operator==(const UnstructuredBytesMetadata&, const UnstructuredBytesMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static UnstructuredBytesMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        struct RefundMetadata {
            diem_types::RefundMetadata value;

            friend bool operator==(const RefundMetadata&, const RefundMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static RefundMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        struct CoinTradeMetadata {
            diem_types::CoinTradeMetadata value;

            friend bool operator==(const CoinTradeMetadata&, const CoinTradeMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static CoinTradeMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        struct PaymentMetadata {
            diem_types::PaymentMetadata value;

            friend bool operator==(const PaymentMetadata&, const PaymentMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static PaymentMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<Undefined, GeneralMetadata, TravelRuleMetadata, UnstructuredBytesMetadata, RefundMetadata, CoinTradeMetadata, PaymentMetadata> value;

        friend bool operator==(const Metadata&, const Metadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static Metadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct AccountAddress {
        std::array<uint8_t, 16> value;

        friend bool operator==(const AccountAddress&, const AccountAddress&);
        std::vector<uint8_t> bcsSerialize() const;
        static AccountAddress bcsDeserialize(std::vector<uint8_t>);
    };

    struct Ed25519PublicKey {
        std::vector<uint8_t> value;

        friend bool operator==(const Ed25519PublicKey&, const Ed25519PublicKey&);
        std::vector<uint8_t> bcsSerialize() const;
        static Ed25519PublicKey bcsDeserialize(std::vector<uint8_t>);
    };

    struct Ed25519Signature {
        std::vector<uint8_t> value;

        friend bool operator==(const Ed25519Signature&, const Ed25519Signature&);
        std::vector<uint8_t> bcsSerialize() const;
        static Ed25519Signature bcsDeserialize(std::vector<uint8_t>);
    };

    struct MultiEd25519PublicKey {
        std::vector<uint8_t> value;

        friend bool operator==(const MultiEd25519PublicKey&, const MultiEd25519PublicKey&);
        std::vector<uint8_t> bcsSerialize() const;
        static MultiEd25519PublicKey bcsDeserialize(std::vector<uint8_t>);
    };

    struct MultiEd25519Signature {
        std::vector<uint8_t> value;

        friend bool operator==(const MultiEd25519Signature&, const MultiEd25519Signature&);
        std::vector<uint8_t> bcsSerialize() const;
        static MultiEd25519Signature bcsDeserialize(std::vector<uint8_t>);
    };

    struct AccountAuthenticator {

        struct Ed25519 {
            diem_types::Ed25519PublicKey public_key;
            diem_types::Ed25519Signature signature;

            friend bool operator==(const Ed25519&, const Ed25519&);
            std::vector<uint8_t> bcsSerialize() const;
            static Ed25519 bcsDeserialize(std::vector<uint8_t>);
        };

        struct MultiEd25519 {
            diem_types::MultiEd25519PublicKey public_key;
            diem_types::MultiEd25519Signature signature;

            friend bool operator==(const MultiEd25519&, const MultiEd25519&);
            std::vector<uint8_t> bcsSerialize() const;
            static MultiEd25519 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<Ed25519, MultiEd25519> value;

        friend bool operator==(const AccountAuthenticator&, const AccountAuthenticator&);
        std::vector<uint8_t> bcsSerialize() const;
        static AccountAuthenticator bcsDeserialize(std::vector<uint8_t>);
    };

    struct TransactionAuthenticator {

        struct Ed25519 {
            diem_types::Ed25519PublicKey public_key;
            diem_types::Ed25519Signature signature;

            friend bool operator==(const Ed25519&, const Ed25519&);
            std::vector<uint8_t> bcsSerialize() const;
            static Ed25519 bcsDeserialize(std::vector<uint8_t>);
        };

        struct MultiEd25519 {
            diem_types::MultiEd25519PublicKey public_key;
            diem_types::MultiEd25519Signature signature;

            friend bool operator==(const MultiEd25519&, const MultiEd25519&);
            std::vector<uint8_t> bcsSerialize() const;
            static MultiEd25519 bcsDeserialize(std::vector<uint8_t>);
        };

        struct MultiAgent {
            diem_types::AccountAuthenticator sender;
            std::vector<diem_types::AccountAddress> secondary_signer_addresses;
            std::vector<diem_types::AccountAuthenticator> secondary_signers;

            friend bool operator==(const MultiAgent&, const MultiAgent&);
            std::vector<uint8_t> bcsSerialize() const;
            static MultiAgent bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<Ed25519, MultiEd25519, MultiAgent> value;

        friend bool operator==(const TransactionAuthenticator&, const TransactionAuthenticator&);
        std::vector<uint8_t> bcsSerialize() const;
        static TransactionAuthenticator bcsDeserialize(std::vector<uint8_t>);
    };

    struct Module {
        std::vector<uint8_t> code;

        friend bool operator==(const Module&, const Module&);
        std::vector<uint8_t> bcsSerialize() const;
        static Module bcsDeserialize(std::vector<uint8_t>);
    };

    struct TransactionArgument {

        struct U8 {
            uint8_t value;

            friend bool operator==(const U8&, const U8&);
            std::vector<uint8_t> bcsSerialize() const;
            static U8 bcsDeserialize(std::vector<uint8_t>);
        };

        struct U64 {
            uint64_t value;

            friend bool operator==(const U64&, const U64&);
            std::vector<uint8_t> bcsSerialize() const;
            static U64 bcsDeserialize(std::vector<uint8_t>);
        };

        struct U128 {
            serde::uint128_t value;

            friend bool operator==(const U128&, const U128&);
            std::vector<uint8_t> bcsSerialize() const;
            static U128 bcsDeserialize(std::vector<uint8_t>);
        };

        struct Address {
            diem_types::AccountAddress value;

            friend bool operator==(const Address&, const Address&);
            std::vector<uint8_t> bcsSerialize() const;
            static Address bcsDeserialize(std::vector<uint8_t>);
        };

        struct U8Vector {
            std::vector<uint8_t> value;

            friend bool operator==(const U8Vector&, const U8Vector&);
            std::vector<uint8_t> bcsSerialize() const;
            static U8Vector bcsDeserialize(std::vector<uint8_t>);
        };

        struct Bool {
            bool value;

            friend bool operator==(const Bool&, const Bool&);
            std::vector<uint8_t> bcsSerialize() const;
            static Bool bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<U8, U64, U128, Address, U8Vector, Bool> value;

        friend bool operator==(const TransactionArgument&, const TransactionArgument&);
        std::vector<uint8_t> bcsSerialize() const;
        static TransactionArgument bcsDeserialize(std::vector<uint8_t>);
    };

    struct Identifier {
        std::string value;

        friend bool operator==(const Identifier&, const Identifier&);
        std::vector<uint8_t> bcsSerialize() const;
        static Identifier bcsDeserialize(std::vector<uint8_t>);
    };

    struct TypeTag;

    struct StructTag {
        diem_types::AccountAddress address;
        diem_types::Identifier module;
        diem_types::Identifier name;
        std::vector<diem_types::TypeTag> type_params;

        friend bool operator==(const StructTag&, const StructTag&);
        std::vector<uint8_t> bcsSerialize() const;
        static StructTag bcsDeserialize(std::vector<uint8_t>);
    };

    struct TypeTag {

        struct Bool {
            friend bool operator==(const Bool&, const Bool&);
            std::vector<uint8_t> bcsSerialize() const;
            static Bool bcsDeserialize(std::vector<uint8_t>);
        };

        struct U8 {
            friend bool operator==(const U8&, const U8&);
            std::vector<uint8_t> bcsSerialize() const;
            static U8 bcsDeserialize(std::vector<uint8_t>);
        };

        struct U64 {
            friend bool operator==(const U64&, const U64&);
            std::vector<uint8_t> bcsSerialize() const;
            static U64 bcsDeserialize(std::vector<uint8_t>);
        };

        struct U128 {
            friend bool operator==(const U128&, const U128&);
            std::vector<uint8_t> bcsSerialize() const;
            static U128 bcsDeserialize(std::vector<uint8_t>);
        };

        struct Address {
            friend bool operator==(const Address&, const Address&);
            std::vector<uint8_t> bcsSerialize() const;
            static Address bcsDeserialize(std::vector<uint8_t>);
        };

        struct Signer {
            friend bool operator==(const Signer&, const Signer&);
            std::vector<uint8_t> bcsSerialize() const;
            static Signer bcsDeserialize(std::vector<uint8_t>);
        };

        struct Vector {
            serde::value_ptr<diem_types::TypeTag> value;

            friend bool operator==(const Vector&, const Vector&);
            std::vector<uint8_t> bcsSerialize() const;
            static Vector bcsDeserialize(std::vector<uint8_t>);
        };

        struct Struct {
            diem_types::StructTag value;

            friend bool operator==(const Struct&, const Struct&);
            std::vector<uint8_t> bcsSerialize() const;
            static Struct bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<Bool, U8, U64, U128, Address, Signer, Vector, Struct> value;

        friend bool operator==(const TypeTag&, const TypeTag&);
        std::vector<uint8_t> bcsSerialize() const;
        static TypeTag bcsDeserialize(std::vector<uint8_t>);
    };

    struct Script {
        std::vector<uint8_t> code;
        std::vector<diem_types::TypeTag> ty_args;
        std::vector<diem_types::TransactionArgument> args;

        friend bool operator==(const Script&, const Script&);
        std::vector<uint8_t> bcsSerialize() const;
        static Script bcsDeserialize(std::vector<uint8_t>);
    };

    struct ModuleId {
        diem_types::AccountAddress address;
        diem_types::Identifier name;

        friend bool operator==(const ModuleId&, const ModuleId&);
        std::vector<uint8_t> bcsSerialize() const;
        static ModuleId bcsDeserialize(std::vector<uint8_t>);
    };

    struct ScriptFunction {
        diem_types::ModuleId module;
        diem_types::Identifier function;
        std::vector<diem_types::TypeTag> ty_args;
        std::vector<std::vector<uint8_t>> args;

        friend bool operator==(const ScriptFunction&, const ScriptFunction&);
        std::vector<uint8_t> bcsSerialize() const;
        static ScriptFunction bcsDeserialize(std::vector<uint8_t>);
    };

    struct EventKey {
        std::vector<uint8_t> value;

        friend bool operator==(const EventKey&, const EventKey&);
        std::vector<uint8_t> bcsSerialize() const;
        static EventKey bcsDeserialize(std::vector<uint8_t>);
    };

    struct ContractEventV0 {
        diem_types::EventKey key;
        uint64_t sequence_number;
        diem_types::TypeTag type_tag;
        std::vector<uint8_t> event_data;

        friend bool operator==(const ContractEventV0&, const ContractEventV0&);
        std::vector<uint8_t> bcsSerialize() const;
        static ContractEventV0 bcsDeserialize(std::vector<uint8_t>);
    };

    struct ContractEvent {

        struct V0 {
            diem_types::ContractEventV0 value;

            friend bool operator==(const V0&, const V0&);
            std::vector<uint8_t> bcsSerialize() const;
            static V0 bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<V0> value;

        friend bool operator==(const ContractEvent&, const ContractEvent&);
        std::vector<uint8_t> bcsSerialize() const;
        static ContractEvent bcsDeserialize(std::vector<uint8_t>);
    };

    struct AccessPath {
        diem_types::AccountAddress address;
        std::vector<uint8_t> path;

        friend bool operator==(const AccessPath&, const AccessPath&);
        std::vector<uint8_t> bcsSerialize() const;
        static AccessPath bcsDeserialize(std::vector<uint8_t>);
    };

    struct WriteOp {

        struct Deletion {
            friend bool operator==(const Deletion&, const Deletion&);
            std::vector<uint8_t> bcsSerialize() const;
            static Deletion bcsDeserialize(std::vector<uint8_t>);
        };

        struct Value {
            std::vector<uint8_t> value;

            friend bool operator==(const Value&, const Value&);
            std::vector<uint8_t> bcsSerialize() const;
            static Value bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<Deletion, Value> value;

        friend bool operator==(const WriteOp&, const WriteOp&);
        std::vector<uint8_t> bcsSerialize() const;
        static WriteOp bcsDeserialize(std::vector<uint8_t>);
    };

    struct WriteSetMut {
        std::vector<std::tuple<diem_types::AccessPath, diem_types::WriteOp>> write_set;

        friend bool operator==(const WriteSetMut&, const WriteSetMut&);
        std::vector<uint8_t> bcsSerialize() const;
        static WriteSetMut bcsDeserialize(std::vector<uint8_t>);
    };

    struct WriteSet {
        diem_types::WriteSetMut value;

        friend bool operator==(const WriteSet&, const WriteSet&);
        std::vector<uint8_t> bcsSerialize() const;
        static WriteSet bcsDeserialize(std::vector<uint8_t>);
    };

    struct ChangeSet {
        diem_types::WriteSet write_set;
        std::vector<diem_types::ContractEvent> events;

        friend bool operator==(const ChangeSet&, const ChangeSet&);
        std::vector<uint8_t> bcsSerialize() const;
        static ChangeSet bcsDeserialize(std::vector<uint8_t>);
    };

    struct WriteSetPayload {

        struct Direct {
            diem_types::ChangeSet value;

            friend bool operator==(const Direct&, const Direct&);
            std::vector<uint8_t> bcsSerialize() const;
            static Direct bcsDeserialize(std::vector<uint8_t>);
        };

        struct Script {
            diem_types::AccountAddress execute_as;
            diem_types::Script script;

            friend bool operator==(const Script&, const Script&);
            std::vector<uint8_t> bcsSerialize() const;
            static Script bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<Direct, Script> value;

        friend bool operator==(const WriteSetPayload&, const WriteSetPayload&);
        std::vector<uint8_t> bcsSerialize() const;
        static WriteSetPayload bcsDeserialize(std::vector<uint8_t>);
    };

    struct TransactionPayload {

        struct WriteSet {
            diem_types::WriteSetPayload value;

            friend bool operator==(const WriteSet&, const WriteSet&);
            std::vector<uint8_t> bcsSerialize() const;
            static WriteSet bcsDeserialize(std::vector<uint8_t>);
        };

        struct Script {
            diem_types::Script value;

            friend bool operator==(const Script&, const Script&);
            std::vector<uint8_t> bcsSerialize() const;
            static Script bcsDeserialize(std::vector<uint8_t>);
        };

        struct Module {
            diem_types::Module value;

            friend bool operator==(const Module&, const Module&);
            std::vector<uint8_t> bcsSerialize() const;
            static Module bcsDeserialize(std::vector<uint8_t>);
        };

        struct ScriptFunction {
            diem_types::ScriptFunction value;

            friend bool operator==(const ScriptFunction&, const ScriptFunction&);
            std::vector<uint8_t> bcsSerialize() const;
            static ScriptFunction bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<WriteSet, Script, Module, ScriptFunction> value;

        friend bool operator==(const TransactionPayload&, const TransactionPayload&);
        std::vector<uint8_t> bcsSerialize() const;
        static TransactionPayload bcsDeserialize(std::vector<uint8_t>);
    };

    struct ChainId {
        uint8_t value;

        friend bool operator==(const ChainId&, const ChainId&);
        std::vector<uint8_t> bcsSerialize() const;
        static ChainId bcsDeserialize(std::vector<uint8_t>);
    };

    struct RawTransaction {
        diem_types::AccountAddress sender;
        uint64_t sequence_number;
        diem_types::TransactionPayload payload;
        uint64_t max_gas_amount;
        uint64_t gas_unit_price;
        std::string gas_currency_code;
        uint64_t expiration_timestamp_secs;
        diem_types::ChainId chain_id;

        friend bool operator==(const RawTransaction&, const RawTransaction&);
        std::vector<uint8_t> bcsSerialize() const;
        static RawTransaction bcsDeserialize(std::vector<uint8_t>);
    };

    struct HashValue {
        std::vector<uint8_t> value;

        friend bool operator==(const HashValue&, const HashValue&);
        std::vector<uint8_t> bcsSerialize() const;
        static HashValue bcsDeserialize(std::vector<uint8_t>);
    };

    struct BlockMetadata {
        diem_types::HashValue id;
        uint64_t round;
        uint64_t timestamp_usecs;
        std::vector<diem_types::AccountAddress> previous_block_votes;
        diem_types::AccountAddress proposer;

        friend bool operator==(const BlockMetadata&, const BlockMetadata&);
        std::vector<uint8_t> bcsSerialize() const;
        static BlockMetadata bcsDeserialize(std::vector<uint8_t>);
    };

    struct SignedTransaction {
        diem_types::RawTransaction raw_txn;
        diem_types::TransactionAuthenticator authenticator;

        friend bool operator==(const SignedTransaction&, const SignedTransaction&);
        std::vector<uint8_t> bcsSerialize() const;
        static SignedTransaction bcsDeserialize(std::vector<uint8_t>);
    };

    struct Transaction {

        struct UserTransaction {
            diem_types::SignedTransaction value;

            friend bool operator==(const UserTransaction&, const UserTransaction&);
            std::vector<uint8_t> bcsSerialize() const;
            static UserTransaction bcsDeserialize(std::vector<uint8_t>);
        };

        struct GenesisTransaction {
            diem_types::WriteSetPayload value;

            friend bool operator==(const GenesisTransaction&, const GenesisTransaction&);
            std::vector<uint8_t> bcsSerialize() const;
            static GenesisTransaction bcsDeserialize(std::vector<uint8_t>);
        };

        struct BlockMetadata {
            diem_types::BlockMetadata value;

            friend bool operator==(const BlockMetadata&, const BlockMetadata&);
            std::vector<uint8_t> bcsSerialize() const;
            static BlockMetadata bcsDeserialize(std::vector<uint8_t>);
        };

        std::variant<UserTransaction, GenesisTransaction, BlockMetadata> value;

        friend bool operator==(const Transaction&, const Transaction&);
        std::vector<uint8_t> bcsSerialize() const;
        static Transaction bcsDeserialize(std::vector<uint8_t>);
    };

} // end of namespace diem_types


namespace diem_types {

    inline bool operator==(const AccessPath &lhs, const AccessPath &rhs) {
        if (!(lhs.address == rhs.address)) { return false; }
        if (!(lhs.path == rhs.path)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> AccessPath::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<AccessPath>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline AccessPath AccessPath::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<AccessPath>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::AccessPath>::serialize(const diem_types::AccessPath &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.address)>::serialize(obj.address, serializer);
    serde::Serializable<decltype(obj.path)>::serialize(obj.path, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::AccessPath serde::Deserializable<diem_types::AccessPath>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::AccessPath obj;
    obj.address = serde::Deserializable<decltype(obj.address)>::deserialize(deserializer);
    obj.path = serde::Deserializable<decltype(obj.path)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const AccountAddress &lhs, const AccountAddress &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> AccountAddress::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<AccountAddress>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline AccountAddress AccountAddress::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<AccountAddress>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::AccountAddress>::serialize(const diem_types::AccountAddress &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::AccountAddress serde::Deserializable<diem_types::AccountAddress>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::AccountAddress obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const AccountAuthenticator &lhs, const AccountAuthenticator &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> AccountAuthenticator::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<AccountAuthenticator>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline AccountAuthenticator AccountAuthenticator::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<AccountAuthenticator>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::AccountAuthenticator>::serialize(const diem_types::AccountAuthenticator &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::AccountAuthenticator serde::Deserializable<diem_types::AccountAuthenticator>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::AccountAuthenticator obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const AccountAuthenticator::Ed25519 &lhs, const AccountAuthenticator::Ed25519 &rhs) {
        if (!(lhs.public_key == rhs.public_key)) { return false; }
        if (!(lhs.signature == rhs.signature)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> AccountAuthenticator::Ed25519::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<AccountAuthenticator::Ed25519>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline AccountAuthenticator::Ed25519 AccountAuthenticator::Ed25519::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<AccountAuthenticator::Ed25519>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::AccountAuthenticator::Ed25519>::serialize(const diem_types::AccountAuthenticator::Ed25519 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.public_key)>::serialize(obj.public_key, serializer);
    serde::Serializable<decltype(obj.signature)>::serialize(obj.signature, serializer);
}

template <>
template <typename Deserializer>
diem_types::AccountAuthenticator::Ed25519 serde::Deserializable<diem_types::AccountAuthenticator::Ed25519>::deserialize(Deserializer &deserializer) {
    diem_types::AccountAuthenticator::Ed25519 obj;
    obj.public_key = serde::Deserializable<decltype(obj.public_key)>::deserialize(deserializer);
    obj.signature = serde::Deserializable<decltype(obj.signature)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const AccountAuthenticator::MultiEd25519 &lhs, const AccountAuthenticator::MultiEd25519 &rhs) {
        if (!(lhs.public_key == rhs.public_key)) { return false; }
        if (!(lhs.signature == rhs.signature)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> AccountAuthenticator::MultiEd25519::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<AccountAuthenticator::MultiEd25519>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline AccountAuthenticator::MultiEd25519 AccountAuthenticator::MultiEd25519::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<AccountAuthenticator::MultiEd25519>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::AccountAuthenticator::MultiEd25519>::serialize(const diem_types::AccountAuthenticator::MultiEd25519 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.public_key)>::serialize(obj.public_key, serializer);
    serde::Serializable<decltype(obj.signature)>::serialize(obj.signature, serializer);
}

template <>
template <typename Deserializer>
diem_types::AccountAuthenticator::MultiEd25519 serde::Deserializable<diem_types::AccountAuthenticator::MultiEd25519>::deserialize(Deserializer &deserializer) {
    diem_types::AccountAuthenticator::MultiEd25519 obj;
    obj.public_key = serde::Deserializable<decltype(obj.public_key)>::deserialize(deserializer);
    obj.signature = serde::Deserializable<decltype(obj.signature)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const BlockMetadata &lhs, const BlockMetadata &rhs) {
        if (!(lhs.id == rhs.id)) { return false; }
        if (!(lhs.round == rhs.round)) { return false; }
        if (!(lhs.timestamp_usecs == rhs.timestamp_usecs)) { return false; }
        if (!(lhs.previous_block_votes == rhs.previous_block_votes)) { return false; }
        if (!(lhs.proposer == rhs.proposer)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> BlockMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<BlockMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline BlockMetadata BlockMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<BlockMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::BlockMetadata>::serialize(const diem_types::BlockMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.id)>::serialize(obj.id, serializer);
    serde::Serializable<decltype(obj.round)>::serialize(obj.round, serializer);
    serde::Serializable<decltype(obj.timestamp_usecs)>::serialize(obj.timestamp_usecs, serializer);
    serde::Serializable<decltype(obj.previous_block_votes)>::serialize(obj.previous_block_votes, serializer);
    serde::Serializable<decltype(obj.proposer)>::serialize(obj.proposer, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::BlockMetadata serde::Deserializable<diem_types::BlockMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::BlockMetadata obj;
    obj.id = serde::Deserializable<decltype(obj.id)>::deserialize(deserializer);
    obj.round = serde::Deserializable<decltype(obj.round)>::deserialize(deserializer);
    obj.timestamp_usecs = serde::Deserializable<decltype(obj.timestamp_usecs)>::deserialize(deserializer);
    obj.previous_block_votes = serde::Deserializable<decltype(obj.previous_block_votes)>::deserialize(deserializer);
    obj.proposer = serde::Deserializable<decltype(obj.proposer)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const ChainId &lhs, const ChainId &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ChainId::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ChainId>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ChainId ChainId::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ChainId>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ChainId>::serialize(const diem_types::ChainId &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::ChainId serde::Deserializable<diem_types::ChainId>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::ChainId obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const ChangeSet &lhs, const ChangeSet &rhs) {
        if (!(lhs.write_set == rhs.write_set)) { return false; }
        if (!(lhs.events == rhs.events)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ChangeSet::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ChangeSet>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ChangeSet ChangeSet::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ChangeSet>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ChangeSet>::serialize(const diem_types::ChangeSet &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.write_set)>::serialize(obj.write_set, serializer);
    serde::Serializable<decltype(obj.events)>::serialize(obj.events, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::ChangeSet serde::Deserializable<diem_types::ChangeSet>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::ChangeSet obj;
    obj.write_set = serde::Deserializable<decltype(obj.write_set)>::deserialize(deserializer);
    obj.events = serde::Deserializable<decltype(obj.events)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const CoinTradeMetadata &lhs, const CoinTradeMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> CoinTradeMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<CoinTradeMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline CoinTradeMetadata CoinTradeMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<CoinTradeMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::CoinTradeMetadata>::serialize(const diem_types::CoinTradeMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::CoinTradeMetadata serde::Deserializable<diem_types::CoinTradeMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::CoinTradeMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const CoinTradeMetadata::CoinTradeMetadataV0 &lhs, const CoinTradeMetadata::CoinTradeMetadataV0 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> CoinTradeMetadata::CoinTradeMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<CoinTradeMetadata::CoinTradeMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline CoinTradeMetadata::CoinTradeMetadataV0 CoinTradeMetadata::CoinTradeMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<CoinTradeMetadata::CoinTradeMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::CoinTradeMetadata::CoinTradeMetadataV0>::serialize(const diem_types::CoinTradeMetadata::CoinTradeMetadataV0 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::CoinTradeMetadata::CoinTradeMetadataV0 serde::Deserializable<diem_types::CoinTradeMetadata::CoinTradeMetadataV0>::deserialize(Deserializer &deserializer) {
    diem_types::CoinTradeMetadata::CoinTradeMetadataV0 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const CoinTradeMetadataV0 &lhs, const CoinTradeMetadataV0 &rhs) {
        if (!(lhs.trade_ids == rhs.trade_ids)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> CoinTradeMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<CoinTradeMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline CoinTradeMetadataV0 CoinTradeMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<CoinTradeMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::CoinTradeMetadataV0>::serialize(const diem_types::CoinTradeMetadataV0 &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.trade_ids)>::serialize(obj.trade_ids, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::CoinTradeMetadataV0 serde::Deserializable<diem_types::CoinTradeMetadataV0>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::CoinTradeMetadataV0 obj;
    obj.trade_ids = serde::Deserializable<decltype(obj.trade_ids)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const ContractEvent &lhs, const ContractEvent &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ContractEvent::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ContractEvent>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ContractEvent ContractEvent::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ContractEvent>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ContractEvent>::serialize(const diem_types::ContractEvent &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::ContractEvent serde::Deserializable<diem_types::ContractEvent>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::ContractEvent obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const ContractEvent::V0 &lhs, const ContractEvent::V0 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ContractEvent::V0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ContractEvent::V0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ContractEvent::V0 ContractEvent::V0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ContractEvent::V0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ContractEvent::V0>::serialize(const diem_types::ContractEvent::V0 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::ContractEvent::V0 serde::Deserializable<diem_types::ContractEvent::V0>::deserialize(Deserializer &deserializer) {
    diem_types::ContractEvent::V0 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const ContractEventV0 &lhs, const ContractEventV0 &rhs) {
        if (!(lhs.key == rhs.key)) { return false; }
        if (!(lhs.sequence_number == rhs.sequence_number)) { return false; }
        if (!(lhs.type_tag == rhs.type_tag)) { return false; }
        if (!(lhs.event_data == rhs.event_data)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ContractEventV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ContractEventV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ContractEventV0 ContractEventV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ContractEventV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ContractEventV0>::serialize(const diem_types::ContractEventV0 &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.key)>::serialize(obj.key, serializer);
    serde::Serializable<decltype(obj.sequence_number)>::serialize(obj.sequence_number, serializer);
    serde::Serializable<decltype(obj.type_tag)>::serialize(obj.type_tag, serializer);
    serde::Serializable<decltype(obj.event_data)>::serialize(obj.event_data, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::ContractEventV0 serde::Deserializable<diem_types::ContractEventV0>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::ContractEventV0 obj;
    obj.key = serde::Deserializable<decltype(obj.key)>::deserialize(deserializer);
    obj.sequence_number = serde::Deserializable<decltype(obj.sequence_number)>::deserialize(deserializer);
    obj.type_tag = serde::Deserializable<decltype(obj.type_tag)>::deserialize(deserializer);
    obj.event_data = serde::Deserializable<decltype(obj.event_data)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Ed25519PublicKey &lhs, const Ed25519PublicKey &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Ed25519PublicKey::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Ed25519PublicKey>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Ed25519PublicKey Ed25519PublicKey::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Ed25519PublicKey>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Ed25519PublicKey>::serialize(const diem_types::Ed25519PublicKey &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Ed25519PublicKey serde::Deserializable<diem_types::Ed25519PublicKey>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Ed25519PublicKey obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Ed25519Signature &lhs, const Ed25519Signature &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Ed25519Signature::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Ed25519Signature>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Ed25519Signature Ed25519Signature::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Ed25519Signature>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Ed25519Signature>::serialize(const diem_types::Ed25519Signature &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Ed25519Signature serde::Deserializable<diem_types::Ed25519Signature>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Ed25519Signature obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const EventKey &lhs, const EventKey &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> EventKey::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<EventKey>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline EventKey EventKey::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<EventKey>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::EventKey>::serialize(const diem_types::EventKey &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::EventKey serde::Deserializable<diem_types::EventKey>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::EventKey obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const GeneralMetadata &lhs, const GeneralMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> GeneralMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<GeneralMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline GeneralMetadata GeneralMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<GeneralMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::GeneralMetadata>::serialize(const diem_types::GeneralMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::GeneralMetadata serde::Deserializable<diem_types::GeneralMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::GeneralMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const GeneralMetadata::GeneralMetadataVersion0 &lhs, const GeneralMetadata::GeneralMetadataVersion0 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> GeneralMetadata::GeneralMetadataVersion0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<GeneralMetadata::GeneralMetadataVersion0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline GeneralMetadata::GeneralMetadataVersion0 GeneralMetadata::GeneralMetadataVersion0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<GeneralMetadata::GeneralMetadataVersion0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::GeneralMetadata::GeneralMetadataVersion0>::serialize(const diem_types::GeneralMetadata::GeneralMetadataVersion0 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::GeneralMetadata::GeneralMetadataVersion0 serde::Deserializable<diem_types::GeneralMetadata::GeneralMetadataVersion0>::deserialize(Deserializer &deserializer) {
    diem_types::GeneralMetadata::GeneralMetadataVersion0 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const GeneralMetadataV0 &lhs, const GeneralMetadataV0 &rhs) {
        if (!(lhs.to_subaddress == rhs.to_subaddress)) { return false; }
        if (!(lhs.from_subaddress == rhs.from_subaddress)) { return false; }
        if (!(lhs.referenced_event == rhs.referenced_event)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> GeneralMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<GeneralMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline GeneralMetadataV0 GeneralMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<GeneralMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::GeneralMetadataV0>::serialize(const diem_types::GeneralMetadataV0 &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.to_subaddress)>::serialize(obj.to_subaddress, serializer);
    serde::Serializable<decltype(obj.from_subaddress)>::serialize(obj.from_subaddress, serializer);
    serde::Serializable<decltype(obj.referenced_event)>::serialize(obj.referenced_event, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::GeneralMetadataV0 serde::Deserializable<diem_types::GeneralMetadataV0>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::GeneralMetadataV0 obj;
    obj.to_subaddress = serde::Deserializable<decltype(obj.to_subaddress)>::deserialize(deserializer);
    obj.from_subaddress = serde::Deserializable<decltype(obj.from_subaddress)>::deserialize(deserializer);
    obj.referenced_event = serde::Deserializable<decltype(obj.referenced_event)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const HashValue &lhs, const HashValue &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> HashValue::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<HashValue>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline HashValue HashValue::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<HashValue>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::HashValue>::serialize(const diem_types::HashValue &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::HashValue serde::Deserializable<diem_types::HashValue>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::HashValue obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Identifier &lhs, const Identifier &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Identifier::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Identifier>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Identifier Identifier::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Identifier>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Identifier>::serialize(const diem_types::Identifier &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Identifier serde::Deserializable<diem_types::Identifier>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Identifier obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata &lhs, const Metadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata Metadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata>::serialize(const diem_types::Metadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Metadata serde::Deserializable<diem_types::Metadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Metadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::Undefined &lhs, const Metadata::Undefined &rhs) {
        return true;
    }

    inline std::vector<uint8_t> Metadata::Undefined::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::Undefined>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::Undefined Metadata::Undefined::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::Undefined>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::Undefined>::serialize(const diem_types::Metadata::Undefined &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::Metadata::Undefined serde::Deserializable<diem_types::Metadata::Undefined>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::Undefined obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::GeneralMetadata &lhs, const Metadata::GeneralMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::GeneralMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::GeneralMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::GeneralMetadata Metadata::GeneralMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::GeneralMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::GeneralMetadata>::serialize(const diem_types::Metadata::GeneralMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Metadata::GeneralMetadata serde::Deserializable<diem_types::Metadata::GeneralMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::GeneralMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::TravelRuleMetadata &lhs, const Metadata::TravelRuleMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::TravelRuleMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::TravelRuleMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::TravelRuleMetadata Metadata::TravelRuleMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::TravelRuleMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::TravelRuleMetadata>::serialize(const diem_types::Metadata::TravelRuleMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Metadata::TravelRuleMetadata serde::Deserializable<diem_types::Metadata::TravelRuleMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::TravelRuleMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::UnstructuredBytesMetadata &lhs, const Metadata::UnstructuredBytesMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::UnstructuredBytesMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::UnstructuredBytesMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::UnstructuredBytesMetadata Metadata::UnstructuredBytesMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::UnstructuredBytesMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::UnstructuredBytesMetadata>::serialize(const diem_types::Metadata::UnstructuredBytesMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Metadata::UnstructuredBytesMetadata serde::Deserializable<diem_types::Metadata::UnstructuredBytesMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::UnstructuredBytesMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::RefundMetadata &lhs, const Metadata::RefundMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::RefundMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::RefundMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::RefundMetadata Metadata::RefundMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::RefundMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::RefundMetadata>::serialize(const diem_types::Metadata::RefundMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Metadata::RefundMetadata serde::Deserializable<diem_types::Metadata::RefundMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::RefundMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::CoinTradeMetadata &lhs, const Metadata::CoinTradeMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::CoinTradeMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::CoinTradeMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::CoinTradeMetadata Metadata::CoinTradeMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::CoinTradeMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::CoinTradeMetadata>::serialize(const diem_types::Metadata::CoinTradeMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Metadata::CoinTradeMetadata serde::Deserializable<diem_types::Metadata::CoinTradeMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::CoinTradeMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Metadata::PaymentMetadata &lhs, const Metadata::PaymentMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Metadata::PaymentMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Metadata::PaymentMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Metadata::PaymentMetadata Metadata::PaymentMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Metadata::PaymentMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Metadata::PaymentMetadata>::serialize(const diem_types::Metadata::PaymentMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Metadata::PaymentMetadata serde::Deserializable<diem_types::Metadata::PaymentMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Metadata::PaymentMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Module &lhs, const Module &rhs) {
        if (!(lhs.code == rhs.code)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Module::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Module>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Module Module::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Module>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Module>::serialize(const diem_types::Module &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.code)>::serialize(obj.code, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Module serde::Deserializable<diem_types::Module>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Module obj;
    obj.code = serde::Deserializable<decltype(obj.code)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const ModuleId &lhs, const ModuleId &rhs) {
        if (!(lhs.address == rhs.address)) { return false; }
        if (!(lhs.name == rhs.name)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ModuleId::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ModuleId>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ModuleId ModuleId::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ModuleId>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ModuleId>::serialize(const diem_types::ModuleId &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.address)>::serialize(obj.address, serializer);
    serde::Serializable<decltype(obj.name)>::serialize(obj.name, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::ModuleId serde::Deserializable<diem_types::ModuleId>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::ModuleId obj;
    obj.address = serde::Deserializable<decltype(obj.address)>::deserialize(deserializer);
    obj.name = serde::Deserializable<decltype(obj.name)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const MultiEd25519PublicKey &lhs, const MultiEd25519PublicKey &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> MultiEd25519PublicKey::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<MultiEd25519PublicKey>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline MultiEd25519PublicKey MultiEd25519PublicKey::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<MultiEd25519PublicKey>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::MultiEd25519PublicKey>::serialize(const diem_types::MultiEd25519PublicKey &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::MultiEd25519PublicKey serde::Deserializable<diem_types::MultiEd25519PublicKey>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::MultiEd25519PublicKey obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const MultiEd25519Signature &lhs, const MultiEd25519Signature &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> MultiEd25519Signature::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<MultiEd25519Signature>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline MultiEd25519Signature MultiEd25519Signature::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<MultiEd25519Signature>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::MultiEd25519Signature>::serialize(const diem_types::MultiEd25519Signature &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::MultiEd25519Signature serde::Deserializable<diem_types::MultiEd25519Signature>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::MultiEd25519Signature obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const PaymentMetadata &lhs, const PaymentMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> PaymentMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<PaymentMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline PaymentMetadata PaymentMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<PaymentMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::PaymentMetadata>::serialize(const diem_types::PaymentMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::PaymentMetadata serde::Deserializable<diem_types::PaymentMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::PaymentMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const PaymentMetadata::PaymentMetadataVersion0 &lhs, const PaymentMetadata::PaymentMetadataVersion0 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> PaymentMetadata::PaymentMetadataVersion0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<PaymentMetadata::PaymentMetadataVersion0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline PaymentMetadata::PaymentMetadataVersion0 PaymentMetadata::PaymentMetadataVersion0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<PaymentMetadata::PaymentMetadataVersion0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::PaymentMetadata::PaymentMetadataVersion0>::serialize(const diem_types::PaymentMetadata::PaymentMetadataVersion0 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::PaymentMetadata::PaymentMetadataVersion0 serde::Deserializable<diem_types::PaymentMetadata::PaymentMetadataVersion0>::deserialize(Deserializer &deserializer) {
    diem_types::PaymentMetadata::PaymentMetadataVersion0 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const PaymentMetadataV0 &lhs, const PaymentMetadataV0 &rhs) {
        if (!(lhs.reference_id == rhs.reference_id)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> PaymentMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<PaymentMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline PaymentMetadataV0 PaymentMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<PaymentMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::PaymentMetadataV0>::serialize(const diem_types::PaymentMetadataV0 &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.reference_id)>::serialize(obj.reference_id, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::PaymentMetadataV0 serde::Deserializable<diem_types::PaymentMetadataV0>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::PaymentMetadataV0 obj;
    obj.reference_id = serde::Deserializable<decltype(obj.reference_id)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const RawTransaction &lhs, const RawTransaction &rhs) {
        if (!(lhs.sender == rhs.sender)) { return false; }
        if (!(lhs.sequence_number == rhs.sequence_number)) { return false; }
        if (!(lhs.payload == rhs.payload)) { return false; }
        if (!(lhs.max_gas_amount == rhs.max_gas_amount)) { return false; }
        if (!(lhs.gas_unit_price == rhs.gas_unit_price)) { return false; }
        if (!(lhs.gas_currency_code == rhs.gas_currency_code)) { return false; }
        if (!(lhs.expiration_timestamp_secs == rhs.expiration_timestamp_secs)) { return false; }
        if (!(lhs.chain_id == rhs.chain_id)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> RawTransaction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RawTransaction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RawTransaction RawTransaction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RawTransaction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RawTransaction>::serialize(const diem_types::RawTransaction &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.sender)>::serialize(obj.sender, serializer);
    serde::Serializable<decltype(obj.sequence_number)>::serialize(obj.sequence_number, serializer);
    serde::Serializable<decltype(obj.payload)>::serialize(obj.payload, serializer);
    serde::Serializable<decltype(obj.max_gas_amount)>::serialize(obj.max_gas_amount, serializer);
    serde::Serializable<decltype(obj.gas_unit_price)>::serialize(obj.gas_unit_price, serializer);
    serde::Serializable<decltype(obj.gas_currency_code)>::serialize(obj.gas_currency_code, serializer);
    serde::Serializable<decltype(obj.expiration_timestamp_secs)>::serialize(obj.expiration_timestamp_secs, serializer);
    serde::Serializable<decltype(obj.chain_id)>::serialize(obj.chain_id, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::RawTransaction serde::Deserializable<diem_types::RawTransaction>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::RawTransaction obj;
    obj.sender = serde::Deserializable<decltype(obj.sender)>::deserialize(deserializer);
    obj.sequence_number = serde::Deserializable<decltype(obj.sequence_number)>::deserialize(deserializer);
    obj.payload = serde::Deserializable<decltype(obj.payload)>::deserialize(deserializer);
    obj.max_gas_amount = serde::Deserializable<decltype(obj.max_gas_amount)>::deserialize(deserializer);
    obj.gas_unit_price = serde::Deserializable<decltype(obj.gas_unit_price)>::deserialize(deserializer);
    obj.gas_currency_code = serde::Deserializable<decltype(obj.gas_currency_code)>::deserialize(deserializer);
    obj.expiration_timestamp_secs = serde::Deserializable<decltype(obj.expiration_timestamp_secs)>::deserialize(deserializer);
    obj.chain_id = serde::Deserializable<decltype(obj.chain_id)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundMetadata &lhs, const RefundMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> RefundMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundMetadata RefundMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundMetadata>::serialize(const diem_types::RefundMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::RefundMetadata serde::Deserializable<diem_types::RefundMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::RefundMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundMetadata::RefundMetadataV0 &lhs, const RefundMetadata::RefundMetadataV0 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> RefundMetadata::RefundMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundMetadata::RefundMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundMetadata::RefundMetadataV0 RefundMetadata::RefundMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundMetadata::RefundMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundMetadata::RefundMetadataV0>::serialize(const diem_types::RefundMetadata::RefundMetadataV0 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::RefundMetadata::RefundMetadataV0 serde::Deserializable<diem_types::RefundMetadata::RefundMetadataV0>::deserialize(Deserializer &deserializer) {
    diem_types::RefundMetadata::RefundMetadataV0 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundMetadataV0 &lhs, const RefundMetadataV0 &rhs) {
        if (!(lhs.transaction_version == rhs.transaction_version)) { return false; }
        if (!(lhs.reason == rhs.reason)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> RefundMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundMetadataV0 RefundMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundMetadataV0>::serialize(const diem_types::RefundMetadataV0 &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.transaction_version)>::serialize(obj.transaction_version, serializer);
    serde::Serializable<decltype(obj.reason)>::serialize(obj.reason, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::RefundMetadataV0 serde::Deserializable<diem_types::RefundMetadataV0>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::RefundMetadataV0 obj;
    obj.transaction_version = serde::Deserializable<decltype(obj.transaction_version)>::deserialize(deserializer);
    obj.reason = serde::Deserializable<decltype(obj.reason)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundReason &lhs, const RefundReason &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> RefundReason::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundReason>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundReason RefundReason::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundReason>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundReason>::serialize(const diem_types::RefundReason &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::RefundReason serde::Deserializable<diem_types::RefundReason>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::RefundReason obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundReason::OtherReason &lhs, const RefundReason::OtherReason &rhs) {
        return true;
    }

    inline std::vector<uint8_t> RefundReason::OtherReason::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundReason::OtherReason>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundReason::OtherReason RefundReason::OtherReason::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundReason::OtherReason>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundReason::OtherReason>::serialize(const diem_types::RefundReason::OtherReason &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::RefundReason::OtherReason serde::Deserializable<diem_types::RefundReason::OtherReason>::deserialize(Deserializer &deserializer) {
    diem_types::RefundReason::OtherReason obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundReason::InvalidSubaddress &lhs, const RefundReason::InvalidSubaddress &rhs) {
        return true;
    }

    inline std::vector<uint8_t> RefundReason::InvalidSubaddress::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundReason::InvalidSubaddress>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundReason::InvalidSubaddress RefundReason::InvalidSubaddress::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundReason::InvalidSubaddress>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundReason::InvalidSubaddress>::serialize(const diem_types::RefundReason::InvalidSubaddress &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::RefundReason::InvalidSubaddress serde::Deserializable<diem_types::RefundReason::InvalidSubaddress>::deserialize(Deserializer &deserializer) {
    diem_types::RefundReason::InvalidSubaddress obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundReason::UserInitiatedPartialRefund &lhs, const RefundReason::UserInitiatedPartialRefund &rhs) {
        return true;
    }

    inline std::vector<uint8_t> RefundReason::UserInitiatedPartialRefund::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundReason::UserInitiatedPartialRefund>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundReason::UserInitiatedPartialRefund RefundReason::UserInitiatedPartialRefund::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundReason::UserInitiatedPartialRefund>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundReason::UserInitiatedPartialRefund>::serialize(const diem_types::RefundReason::UserInitiatedPartialRefund &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::RefundReason::UserInitiatedPartialRefund serde::Deserializable<diem_types::RefundReason::UserInitiatedPartialRefund>::deserialize(Deserializer &deserializer) {
    diem_types::RefundReason::UserInitiatedPartialRefund obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundReason::UserInitiatedFullRefund &lhs, const RefundReason::UserInitiatedFullRefund &rhs) {
        return true;
    }

    inline std::vector<uint8_t> RefundReason::UserInitiatedFullRefund::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundReason::UserInitiatedFullRefund>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundReason::UserInitiatedFullRefund RefundReason::UserInitiatedFullRefund::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundReason::UserInitiatedFullRefund>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundReason::UserInitiatedFullRefund>::serialize(const diem_types::RefundReason::UserInitiatedFullRefund &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::RefundReason::UserInitiatedFullRefund serde::Deserializable<diem_types::RefundReason::UserInitiatedFullRefund>::deserialize(Deserializer &deserializer) {
    diem_types::RefundReason::UserInitiatedFullRefund obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const RefundReason::InvalidReferenceId &lhs, const RefundReason::InvalidReferenceId &rhs) {
        return true;
    }

    inline std::vector<uint8_t> RefundReason::InvalidReferenceId::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<RefundReason::InvalidReferenceId>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline RefundReason::InvalidReferenceId RefundReason::InvalidReferenceId::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<RefundReason::InvalidReferenceId>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::RefundReason::InvalidReferenceId>::serialize(const diem_types::RefundReason::InvalidReferenceId &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::RefundReason::InvalidReferenceId serde::Deserializable<diem_types::RefundReason::InvalidReferenceId>::deserialize(Deserializer &deserializer) {
    diem_types::RefundReason::InvalidReferenceId obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const Script &lhs, const Script &rhs) {
        if (!(lhs.code == rhs.code)) { return false; }
        if (!(lhs.ty_args == rhs.ty_args)) { return false; }
        if (!(lhs.args == rhs.args)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Script::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Script>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Script Script::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Script>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Script>::serialize(const diem_types::Script &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.code)>::serialize(obj.code, serializer);
    serde::Serializable<decltype(obj.ty_args)>::serialize(obj.ty_args, serializer);
    serde::Serializable<decltype(obj.args)>::serialize(obj.args, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Script serde::Deserializable<diem_types::Script>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Script obj;
    obj.code = serde::Deserializable<decltype(obj.code)>::deserialize(deserializer);
    obj.ty_args = serde::Deserializable<decltype(obj.ty_args)>::deserialize(deserializer);
    obj.args = serde::Deserializable<decltype(obj.args)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const ScriptFunction &lhs, const ScriptFunction &rhs) {
        if (!(lhs.module == rhs.module)) { return false; }
        if (!(lhs.function == rhs.function)) { return false; }
        if (!(lhs.ty_args == rhs.ty_args)) { return false; }
        if (!(lhs.args == rhs.args)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> ScriptFunction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<ScriptFunction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline ScriptFunction ScriptFunction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<ScriptFunction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::ScriptFunction>::serialize(const diem_types::ScriptFunction &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.module)>::serialize(obj.module, serializer);
    serde::Serializable<decltype(obj.function)>::serialize(obj.function, serializer);
    serde::Serializable<decltype(obj.ty_args)>::serialize(obj.ty_args, serializer);
    serde::Serializable<decltype(obj.args)>::serialize(obj.args, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::ScriptFunction serde::Deserializable<diem_types::ScriptFunction>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::ScriptFunction obj;
    obj.module = serde::Deserializable<decltype(obj.module)>::deserialize(deserializer);
    obj.function = serde::Deserializable<decltype(obj.function)>::deserialize(deserializer);
    obj.ty_args = serde::Deserializable<decltype(obj.ty_args)>::deserialize(deserializer);
    obj.args = serde::Deserializable<decltype(obj.args)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const SignedTransaction &lhs, const SignedTransaction &rhs) {
        if (!(lhs.raw_txn == rhs.raw_txn)) { return false; }
        if (!(lhs.authenticator == rhs.authenticator)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> SignedTransaction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<SignedTransaction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline SignedTransaction SignedTransaction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<SignedTransaction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::SignedTransaction>::serialize(const diem_types::SignedTransaction &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.raw_txn)>::serialize(obj.raw_txn, serializer);
    serde::Serializable<decltype(obj.authenticator)>::serialize(obj.authenticator, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::SignedTransaction serde::Deserializable<diem_types::SignedTransaction>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::SignedTransaction obj;
    obj.raw_txn = serde::Deserializable<decltype(obj.raw_txn)>::deserialize(deserializer);
    obj.authenticator = serde::Deserializable<decltype(obj.authenticator)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const StructTag &lhs, const StructTag &rhs) {
        if (!(lhs.address == rhs.address)) { return false; }
        if (!(lhs.module == rhs.module)) { return false; }
        if (!(lhs.name == rhs.name)) { return false; }
        if (!(lhs.type_params == rhs.type_params)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> StructTag::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<StructTag>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline StructTag StructTag::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<StructTag>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::StructTag>::serialize(const diem_types::StructTag &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.address)>::serialize(obj.address, serializer);
    serde::Serializable<decltype(obj.module)>::serialize(obj.module, serializer);
    serde::Serializable<decltype(obj.name)>::serialize(obj.name, serializer);
    serde::Serializable<decltype(obj.type_params)>::serialize(obj.type_params, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::StructTag serde::Deserializable<diem_types::StructTag>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::StructTag obj;
    obj.address = serde::Deserializable<decltype(obj.address)>::deserialize(deserializer);
    obj.module = serde::Deserializable<decltype(obj.module)>::deserialize(deserializer);
    obj.name = serde::Deserializable<decltype(obj.name)>::deserialize(deserializer);
    obj.type_params = serde::Deserializable<decltype(obj.type_params)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Transaction &lhs, const Transaction &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Transaction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Transaction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Transaction Transaction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Transaction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Transaction>::serialize(const diem_types::Transaction &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::Transaction serde::Deserializable<diem_types::Transaction>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::Transaction obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const Transaction::UserTransaction &lhs, const Transaction::UserTransaction &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Transaction::UserTransaction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Transaction::UserTransaction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Transaction::UserTransaction Transaction::UserTransaction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Transaction::UserTransaction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Transaction::UserTransaction>::serialize(const diem_types::Transaction::UserTransaction &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Transaction::UserTransaction serde::Deserializable<diem_types::Transaction::UserTransaction>::deserialize(Deserializer &deserializer) {
    diem_types::Transaction::UserTransaction obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Transaction::GenesisTransaction &lhs, const Transaction::GenesisTransaction &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Transaction::GenesisTransaction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Transaction::GenesisTransaction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Transaction::GenesisTransaction Transaction::GenesisTransaction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Transaction::GenesisTransaction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Transaction::GenesisTransaction>::serialize(const diem_types::Transaction::GenesisTransaction &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Transaction::GenesisTransaction serde::Deserializable<diem_types::Transaction::GenesisTransaction>::deserialize(Deserializer &deserializer) {
    diem_types::Transaction::GenesisTransaction obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const Transaction::BlockMetadata &lhs, const Transaction::BlockMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> Transaction::BlockMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<Transaction::BlockMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline Transaction::BlockMetadata Transaction::BlockMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<Transaction::BlockMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::Transaction::BlockMetadata>::serialize(const diem_types::Transaction::BlockMetadata &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::Transaction::BlockMetadata serde::Deserializable<diem_types::Transaction::BlockMetadata>::deserialize(Deserializer &deserializer) {
    diem_types::Transaction::BlockMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument &lhs, const TransactionArgument &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument TransactionArgument::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument>::serialize(const diem_types::TransactionArgument &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument serde::Deserializable<diem_types::TransactionArgument>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::TransactionArgument obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument::U8 &lhs, const TransactionArgument::U8 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::U8::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument::U8>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument::U8 TransactionArgument::U8::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument::U8>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument::U8>::serialize(const diem_types::TransactionArgument::U8 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument::U8 serde::Deserializable<diem_types::TransactionArgument::U8>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionArgument::U8 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument::U64 &lhs, const TransactionArgument::U64 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::U64::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument::U64>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument::U64 TransactionArgument::U64::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument::U64>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument::U64>::serialize(const diem_types::TransactionArgument::U64 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument::U64 serde::Deserializable<diem_types::TransactionArgument::U64>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionArgument::U64 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument::U128 &lhs, const TransactionArgument::U128 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::U128::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument::U128>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument::U128 TransactionArgument::U128::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument::U128>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument::U128>::serialize(const diem_types::TransactionArgument::U128 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument::U128 serde::Deserializable<diem_types::TransactionArgument::U128>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionArgument::U128 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument::Address &lhs, const TransactionArgument::Address &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::Address::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument::Address>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument::Address TransactionArgument::Address::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument::Address>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument::Address>::serialize(const diem_types::TransactionArgument::Address &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument::Address serde::Deserializable<diem_types::TransactionArgument::Address>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionArgument::Address obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument::U8Vector &lhs, const TransactionArgument::U8Vector &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::U8Vector::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument::U8Vector>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument::U8Vector TransactionArgument::U8Vector::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument::U8Vector>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument::U8Vector>::serialize(const diem_types::TransactionArgument::U8Vector &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument::U8Vector serde::Deserializable<diem_types::TransactionArgument::U8Vector>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionArgument::U8Vector obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionArgument::Bool &lhs, const TransactionArgument::Bool &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionArgument::Bool::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionArgument::Bool>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionArgument::Bool TransactionArgument::Bool::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionArgument::Bool>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionArgument::Bool>::serialize(const diem_types::TransactionArgument::Bool &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionArgument::Bool serde::Deserializable<diem_types::TransactionArgument::Bool>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionArgument::Bool obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionAuthenticator &lhs, const TransactionAuthenticator &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionAuthenticator::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionAuthenticator>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionAuthenticator TransactionAuthenticator::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionAuthenticator>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionAuthenticator>::serialize(const diem_types::TransactionAuthenticator &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::TransactionAuthenticator serde::Deserializable<diem_types::TransactionAuthenticator>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::TransactionAuthenticator obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionAuthenticator::Ed25519 &lhs, const TransactionAuthenticator::Ed25519 &rhs) {
        if (!(lhs.public_key == rhs.public_key)) { return false; }
        if (!(lhs.signature == rhs.signature)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionAuthenticator::Ed25519::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionAuthenticator::Ed25519>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionAuthenticator::Ed25519 TransactionAuthenticator::Ed25519::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionAuthenticator::Ed25519>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionAuthenticator::Ed25519>::serialize(const diem_types::TransactionAuthenticator::Ed25519 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.public_key)>::serialize(obj.public_key, serializer);
    serde::Serializable<decltype(obj.signature)>::serialize(obj.signature, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionAuthenticator::Ed25519 serde::Deserializable<diem_types::TransactionAuthenticator::Ed25519>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionAuthenticator::Ed25519 obj;
    obj.public_key = serde::Deserializable<decltype(obj.public_key)>::deserialize(deserializer);
    obj.signature = serde::Deserializable<decltype(obj.signature)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionAuthenticator::MultiEd25519 &lhs, const TransactionAuthenticator::MultiEd25519 &rhs) {
        if (!(lhs.public_key == rhs.public_key)) { return false; }
        if (!(lhs.signature == rhs.signature)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionAuthenticator::MultiEd25519::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionAuthenticator::MultiEd25519>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionAuthenticator::MultiEd25519 TransactionAuthenticator::MultiEd25519::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionAuthenticator::MultiEd25519>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionAuthenticator::MultiEd25519>::serialize(const diem_types::TransactionAuthenticator::MultiEd25519 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.public_key)>::serialize(obj.public_key, serializer);
    serde::Serializable<decltype(obj.signature)>::serialize(obj.signature, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionAuthenticator::MultiEd25519 serde::Deserializable<diem_types::TransactionAuthenticator::MultiEd25519>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionAuthenticator::MultiEd25519 obj;
    obj.public_key = serde::Deserializable<decltype(obj.public_key)>::deserialize(deserializer);
    obj.signature = serde::Deserializable<decltype(obj.signature)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionAuthenticator::MultiAgent &lhs, const TransactionAuthenticator::MultiAgent &rhs) {
        if (!(lhs.sender == rhs.sender)) { return false; }
        if (!(lhs.secondary_signer_addresses == rhs.secondary_signer_addresses)) { return false; }
        if (!(lhs.secondary_signers == rhs.secondary_signers)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionAuthenticator::MultiAgent::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionAuthenticator::MultiAgent>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionAuthenticator::MultiAgent TransactionAuthenticator::MultiAgent::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionAuthenticator::MultiAgent>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionAuthenticator::MultiAgent>::serialize(const diem_types::TransactionAuthenticator::MultiAgent &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.sender)>::serialize(obj.sender, serializer);
    serde::Serializable<decltype(obj.secondary_signer_addresses)>::serialize(obj.secondary_signer_addresses, serializer);
    serde::Serializable<decltype(obj.secondary_signers)>::serialize(obj.secondary_signers, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionAuthenticator::MultiAgent serde::Deserializable<diem_types::TransactionAuthenticator::MultiAgent>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionAuthenticator::MultiAgent obj;
    obj.sender = serde::Deserializable<decltype(obj.sender)>::deserialize(deserializer);
    obj.secondary_signer_addresses = serde::Deserializable<decltype(obj.secondary_signer_addresses)>::deserialize(deserializer);
    obj.secondary_signers = serde::Deserializable<decltype(obj.secondary_signers)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionPayload &lhs, const TransactionPayload &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionPayload::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionPayload>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionPayload TransactionPayload::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionPayload>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionPayload>::serialize(const diem_types::TransactionPayload &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::TransactionPayload serde::Deserializable<diem_types::TransactionPayload>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::TransactionPayload obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionPayload::WriteSet &lhs, const TransactionPayload::WriteSet &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionPayload::WriteSet::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionPayload::WriteSet>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionPayload::WriteSet TransactionPayload::WriteSet::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionPayload::WriteSet>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionPayload::WriteSet>::serialize(const diem_types::TransactionPayload::WriteSet &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionPayload::WriteSet serde::Deserializable<diem_types::TransactionPayload::WriteSet>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionPayload::WriteSet obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionPayload::Script &lhs, const TransactionPayload::Script &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionPayload::Script::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionPayload::Script>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionPayload::Script TransactionPayload::Script::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionPayload::Script>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionPayload::Script>::serialize(const diem_types::TransactionPayload::Script &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionPayload::Script serde::Deserializable<diem_types::TransactionPayload::Script>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionPayload::Script obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionPayload::Module &lhs, const TransactionPayload::Module &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionPayload::Module::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionPayload::Module>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionPayload::Module TransactionPayload::Module::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionPayload::Module>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionPayload::Module>::serialize(const diem_types::TransactionPayload::Module &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionPayload::Module serde::Deserializable<diem_types::TransactionPayload::Module>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionPayload::Module obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TransactionPayload::ScriptFunction &lhs, const TransactionPayload::ScriptFunction &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TransactionPayload::ScriptFunction::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TransactionPayload::ScriptFunction>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TransactionPayload::ScriptFunction TransactionPayload::ScriptFunction::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TransactionPayload::ScriptFunction>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TransactionPayload::ScriptFunction>::serialize(const diem_types::TransactionPayload::ScriptFunction &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TransactionPayload::ScriptFunction serde::Deserializable<diem_types::TransactionPayload::ScriptFunction>::deserialize(Deserializer &deserializer) {
    diem_types::TransactionPayload::ScriptFunction obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TravelRuleMetadata &lhs, const TravelRuleMetadata &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TravelRuleMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TravelRuleMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TravelRuleMetadata TravelRuleMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TravelRuleMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TravelRuleMetadata>::serialize(const diem_types::TravelRuleMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::TravelRuleMetadata serde::Deserializable<diem_types::TravelRuleMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::TravelRuleMetadata obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const TravelRuleMetadata::TravelRuleMetadataVersion0 &lhs, const TravelRuleMetadata::TravelRuleMetadataVersion0 &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TravelRuleMetadata::TravelRuleMetadataVersion0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TravelRuleMetadata::TravelRuleMetadataVersion0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TravelRuleMetadata::TravelRuleMetadataVersion0 TravelRuleMetadata::TravelRuleMetadataVersion0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TravelRuleMetadata::TravelRuleMetadataVersion0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TravelRuleMetadata::TravelRuleMetadataVersion0>::serialize(const diem_types::TravelRuleMetadata::TravelRuleMetadataVersion0 &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TravelRuleMetadata::TravelRuleMetadataVersion0 serde::Deserializable<diem_types::TravelRuleMetadata::TravelRuleMetadataVersion0>::deserialize(Deserializer &deserializer) {
    diem_types::TravelRuleMetadata::TravelRuleMetadataVersion0 obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TravelRuleMetadataV0 &lhs, const TravelRuleMetadataV0 &rhs) {
        if (!(lhs.off_chain_reference_id == rhs.off_chain_reference_id)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TravelRuleMetadataV0::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TravelRuleMetadataV0>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TravelRuleMetadataV0 TravelRuleMetadataV0::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TravelRuleMetadataV0>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TravelRuleMetadataV0>::serialize(const diem_types::TravelRuleMetadataV0 &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.off_chain_reference_id)>::serialize(obj.off_chain_reference_id, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::TravelRuleMetadataV0 serde::Deserializable<diem_types::TravelRuleMetadataV0>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::TravelRuleMetadataV0 obj;
    obj.off_chain_reference_id = serde::Deserializable<decltype(obj.off_chain_reference_id)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag &lhs, const TypeTag &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TypeTag::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag TypeTag::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag>::serialize(const diem_types::TypeTag &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::TypeTag serde::Deserializable<diem_types::TypeTag>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::TypeTag obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::Bool &lhs, const TypeTag::Bool &rhs) {
        return true;
    }

    inline std::vector<uint8_t> TypeTag::Bool::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::Bool>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::Bool TypeTag::Bool::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::Bool>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::Bool>::serialize(const diem_types::TypeTag::Bool &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::TypeTag::Bool serde::Deserializable<diem_types::TypeTag::Bool>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::Bool obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::U8 &lhs, const TypeTag::U8 &rhs) {
        return true;
    }

    inline std::vector<uint8_t> TypeTag::U8::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::U8>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::U8 TypeTag::U8::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::U8>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::U8>::serialize(const diem_types::TypeTag::U8 &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::TypeTag::U8 serde::Deserializable<diem_types::TypeTag::U8>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::U8 obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::U64 &lhs, const TypeTag::U64 &rhs) {
        return true;
    }

    inline std::vector<uint8_t> TypeTag::U64::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::U64>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::U64 TypeTag::U64::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::U64>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::U64>::serialize(const diem_types::TypeTag::U64 &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::TypeTag::U64 serde::Deserializable<diem_types::TypeTag::U64>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::U64 obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::U128 &lhs, const TypeTag::U128 &rhs) {
        return true;
    }

    inline std::vector<uint8_t> TypeTag::U128::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::U128>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::U128 TypeTag::U128::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::U128>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::U128>::serialize(const diem_types::TypeTag::U128 &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::TypeTag::U128 serde::Deserializable<diem_types::TypeTag::U128>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::U128 obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::Address &lhs, const TypeTag::Address &rhs) {
        return true;
    }

    inline std::vector<uint8_t> TypeTag::Address::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::Address>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::Address TypeTag::Address::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::Address>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::Address>::serialize(const diem_types::TypeTag::Address &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::TypeTag::Address serde::Deserializable<diem_types::TypeTag::Address>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::Address obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::Signer &lhs, const TypeTag::Signer &rhs) {
        return true;
    }

    inline std::vector<uint8_t> TypeTag::Signer::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::Signer>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::Signer TypeTag::Signer::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::Signer>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::Signer>::serialize(const diem_types::TypeTag::Signer &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::TypeTag::Signer serde::Deserializable<diem_types::TypeTag::Signer>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::Signer obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::Vector &lhs, const TypeTag::Vector &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TypeTag::Vector::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::Vector>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::Vector TypeTag::Vector::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::Vector>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::Vector>::serialize(const diem_types::TypeTag::Vector &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TypeTag::Vector serde::Deserializable<diem_types::TypeTag::Vector>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::Vector obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const TypeTag::Struct &lhs, const TypeTag::Struct &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> TypeTag::Struct::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<TypeTag::Struct>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline TypeTag::Struct TypeTag::Struct::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<TypeTag::Struct>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::TypeTag::Struct>::serialize(const diem_types::TypeTag::Struct &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::TypeTag::Struct serde::Deserializable<diem_types::TypeTag::Struct>::deserialize(Deserializer &deserializer) {
    diem_types::TypeTag::Struct obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const UnstructuredBytesMetadata &lhs, const UnstructuredBytesMetadata &rhs) {
        if (!(lhs.metadata == rhs.metadata)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> UnstructuredBytesMetadata::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<UnstructuredBytesMetadata>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline UnstructuredBytesMetadata UnstructuredBytesMetadata::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<UnstructuredBytesMetadata>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::UnstructuredBytesMetadata>::serialize(const diem_types::UnstructuredBytesMetadata &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.metadata)>::serialize(obj.metadata, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::UnstructuredBytesMetadata serde::Deserializable<diem_types::UnstructuredBytesMetadata>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::UnstructuredBytesMetadata obj;
    obj.metadata = serde::Deserializable<decltype(obj.metadata)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteOp &lhs, const WriteOp &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteOp::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteOp>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteOp WriteOp::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteOp>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteOp>::serialize(const diem_types::WriteOp &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::WriteOp serde::Deserializable<diem_types::WriteOp>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::WriteOp obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteOp::Deletion &lhs, const WriteOp::Deletion &rhs) {
        return true;
    }

    inline std::vector<uint8_t> WriteOp::Deletion::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteOp::Deletion>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteOp::Deletion WriteOp::Deletion::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteOp::Deletion>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteOp::Deletion>::serialize(const diem_types::WriteOp::Deletion &obj, Serializer &serializer) {
}

template <>
template <typename Deserializer>
diem_types::WriteOp::Deletion serde::Deserializable<diem_types::WriteOp::Deletion>::deserialize(Deserializer &deserializer) {
    diem_types::WriteOp::Deletion obj;
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteOp::Value &lhs, const WriteOp::Value &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteOp::Value::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteOp::Value>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteOp::Value WriteOp::Value::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteOp::Value>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteOp::Value>::serialize(const diem_types::WriteOp::Value &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::WriteOp::Value serde::Deserializable<diem_types::WriteOp::Value>::deserialize(Deserializer &deserializer) {
    diem_types::WriteOp::Value obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteSet &lhs, const WriteSet &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteSet::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteSet>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteSet WriteSet::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteSet>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteSet>::serialize(const diem_types::WriteSet &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::WriteSet serde::Deserializable<diem_types::WriteSet>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::WriteSet obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteSetMut &lhs, const WriteSetMut &rhs) {
        if (!(lhs.write_set == rhs.write_set)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteSetMut::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteSetMut>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteSetMut WriteSetMut::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteSetMut>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteSetMut>::serialize(const diem_types::WriteSetMut &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.write_set)>::serialize(obj.write_set, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::WriteSetMut serde::Deserializable<diem_types::WriteSetMut>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::WriteSetMut obj;
    obj.write_set = serde::Deserializable<decltype(obj.write_set)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteSetPayload &lhs, const WriteSetPayload &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteSetPayload::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteSetPayload>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteSetPayload WriteSetPayload::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteSetPayload>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteSetPayload>::serialize(const diem_types::WriteSetPayload &obj, Serializer &serializer) {
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
    serializer.decrease_container_depth();
}

template <>
template <typename Deserializer>
diem_types::WriteSetPayload serde::Deserializable<diem_types::WriteSetPayload>::deserialize(Deserializer &deserializer) {
    deserializer.increase_container_depth();
    diem_types::WriteSetPayload obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    deserializer.decrease_container_depth();
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteSetPayload::Direct &lhs, const WriteSetPayload::Direct &rhs) {
        if (!(lhs.value == rhs.value)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteSetPayload::Direct::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteSetPayload::Direct>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteSetPayload::Direct WriteSetPayload::Direct::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteSetPayload::Direct>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteSetPayload::Direct>::serialize(const diem_types::WriteSetPayload::Direct &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.value)>::serialize(obj.value, serializer);
}

template <>
template <typename Deserializer>
diem_types::WriteSetPayload::Direct serde::Deserializable<diem_types::WriteSetPayload::Direct>::deserialize(Deserializer &deserializer) {
    diem_types::WriteSetPayload::Direct obj;
    obj.value = serde::Deserializable<decltype(obj.value)>::deserialize(deserializer);
    return obj;
}

namespace diem_types {

    inline bool operator==(const WriteSetPayload::Script &lhs, const WriteSetPayload::Script &rhs) {
        if (!(lhs.execute_as == rhs.execute_as)) { return false; }
        if (!(lhs.script == rhs.script)) { return false; }
        return true;
    }

    inline std::vector<uint8_t> WriteSetPayload::Script::bcsSerialize() const {
        auto serializer = serde::BcsSerializer();
        serde::Serializable<WriteSetPayload::Script>::serialize(*this, serializer);
        return std::move(serializer).bytes();
    }

    inline WriteSetPayload::Script WriteSetPayload::Script::bcsDeserialize(std::vector<uint8_t> input) {
        auto deserializer = serde::BcsDeserializer(input);
        auto value = serde::Deserializable<WriteSetPayload::Script>::deserialize(deserializer);
        if (deserializer.get_buffer_offset() < input.size()) {
            throw serde::deserialization_error("Some input bytes were not read");
        }
        return value;
    }

} // end of namespace diem_types

template <>
template <typename Serializer>
void serde::Serializable<diem_types::WriteSetPayload::Script>::serialize(const diem_types::WriteSetPayload::Script &obj, Serializer &serializer) {
    serde::Serializable<decltype(obj.execute_as)>::serialize(obj.execute_as, serializer);
    serde::Serializable<decltype(obj.script)>::serialize(obj.script, serializer);
}

template <>
template <typename Deserializer>
diem_types::WriteSetPayload::Script serde::Deserializable<diem_types::WriteSetPayload::Script>::deserialize(Deserializer &deserializer) {
    diem_types::WriteSetPayload::Script obj;
    obj.execute_as = serde::Deserializable<decltype(obj.execute_as)>::deserialize(deserializer);
    obj.script = serde::Deserializable<decltype(obj.script)>::deserialize(deserializer);
    return obj;
}
