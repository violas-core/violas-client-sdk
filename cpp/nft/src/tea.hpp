#ifndef TEA_HPP
#define TEA_HPP
//#include ""

using TokenId = std::array<uint8_t, 32>;
struct Tea;

TokenId compute_token_id(const Tea &t);

struct Tea
{
    std::vector<uint8_t> identity;
    uint8_t kind;
    std::vector<uint8_t> manufacture;
    uint64_t date;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && identity && kind && manufacture && date;
    }
};

std::ostream &operator<<(std::ostream &os, const Tea &tea)
{
    std::string identity(tea.identity.begin(), tea.identity.end());
    std::string manufacture(begin(tea.manufacture), end(tea.manufacture));

    std::cout << "Tea { "
              << "Identity : " << identity << ", "
              << "Kind : " << short(tea.kind) << ", "
              << "manufacture : " << manufacture << ", "
              << "Date : " << tea.date
              << " }, "
              << "token id : " << compute_token_id(tea);

    return os;
}

struct NftInfo
{
    bool limited;
    uint64_t total;
    uint64_t amount;
    violas::Address admin;
    std::map<std::vector<uint8_t>, std::vector<violas::Address>> owners;
    violas::EventHandle mint_event;
    violas::EventHandle burn_event;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && limited && total && amount && admin && owners && mint_event && burn_event;
    }
};

std::ostream &operator<<(std::ostream &os, const NftInfo &tea)
{
    // std::string identity(tea.identity.begin(), tea.identity.end());
    // std::string manufacture(begin(tea.manufacture), end(tea.manufacture));

    // std::cout << "Tea { "
    //           << "Identity : " << identity << ", "
    //           << "Kind : " << short(tea.kind) << ", "
    //           << "manufacture : " << manufacture << ", "
    //           << "Date : " << tea.date
    //           << " }, "
    //           << "token id : " << compute_token_id(tea);

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
