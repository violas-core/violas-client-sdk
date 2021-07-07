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

struct TeaInfo
{
    bool limited;
    uint64_t total;
    uint64_t amount;
    violas::Address admin;
    std::map<std::vector<uint8_t>, violas::Address> owners;
    // mint_events : EventHandle<MintEvent>,
    // burn_events : EventHandle<BurnEvent>,

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && limited && total && amount && admin && owners;
    }
};

std::ostream &operator<<(std::ostream &os, const TeaInfo &tea)
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

#endif
