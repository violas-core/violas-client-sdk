#ifndef TEA_HPP
#define TEA_HPP
#pragma once
#include <nft.hpp>

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
        static const violas::TypeTag tea_tag(module_address(), module_name(), resource_name());

        return tea_tag;
    }

    static violas::Address module_address() { return violas::VIOLAS_STDLIB_ADDRESS; }
    static std::string module_name() { return "MountWuyi"; }
    static std::string resource_name() { return "Tea"; }
};

std::ostream &operator<<(std::ostream &os, const Tea &tea)
{
    std::cout << "\t"
                 "Tea { "
              << "Kind : " << short(tea.kind) << ", "
              << "Manufacture : " << std::string(begin(tea.manufacture), end(tea.manufacture)) << ", "
              << "Production area : " << std::string(begin(tea.PA), end(tea.PA)) << ", "
              << "Production date : " << std::put_time(std::localtime((time_t *)&tea.PD), "%F") << ", "
              << "SN : " << std::string(tea.SN.begin(), tea.SN.end()) << ", "
              << "URL : " << std::string(std::begin(tea.url), std::end(tea.url))
              << " } \n"
              << "\t"
                 "token id : "
              << violas::nft::compute_token_id(tea);

    return os;
}

#endif
