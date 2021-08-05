#ifndef TEA_HPP
#define TEA_HPP
#include <nft.hpp>

struct Tea;

violas::TokenId compute_token_id(const Tea &t);

struct Tea
{
    uint8_t kind;
    std::vector<uint8_t> manufacture;
    std::vector<uint8_t> PA; // Production Area
    uint64_t PD;             // Production Date
    std::vector<uint8_t> SN; // Sequence Number
    std::vector<uint8_t> url;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && kind && manufacture && PA && PD && SN && url;
    }

    static const violas::TypeTag &type_tag()
    {
        static const violas::TypeTag tea_tag(violas::VIOLAS_STDLIB_ADDRESS, "MountWuyi", "Tea");

        return tea_tag;
    }
};

std::ostream &operator<<(std::ostream &os, const Tea &tea)
{
    std::cout << "\t" "Tea { "
              << "Kind : " << short(tea.kind) << ", "
              << "Manufacture : " << std::string(begin(tea.manufacture), end(tea.manufacture)) << ", "
              << "Production area : " << std::string(begin(tea.PA), end(tea.PA)) << ", "
              << "Production date : " << std::put_time(std::localtime((time_t*)&tea.PD), "%F") << ", "
              << "SN : " << std::string(tea.SN.begin(), tea.SN.end()) << ", "
              << "URL : " << std::string(std::begin(tea.url), std::end(tea.url))
              << " } \n"
              << "\t" "token id : " << violas::compute_token_id(tea);

    return os;
}

struct NftTea
{
    std::vector<Tea> teas;
    violas::EventHandle sent_event;
    violas::EventHandle received_event;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && teas && sent_event && received_event;
    }
};

inline std::ostream &operator<<(std::ostream &os, const NftTea &tea_nft)
{
    for (const auto &t : tea_nft.teas)
    {
        std::cout << t << std::endl;
    }

    return os;
}

struct ReceivedEvent
{
    std::vector<uint8_t> token_id;
    violas::Address payer;
    std::vector<uint8_t> metadata;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && token_id && payer && metadata;
    }
};

#endif
