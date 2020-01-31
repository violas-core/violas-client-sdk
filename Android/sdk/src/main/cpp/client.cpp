//
// Created by hunter on 20-1-14.
//
#include <string>
#include "client.h"
#include "violas_sdk.hpp"
//#include "../../../../../cppSdk/src/violas_sdk.hpp"

using namespace std;

void createNativeClient()
{
    string host = "";
    uint16_t port = 40001;
    string faucet_key = "";
    string mnemonic = "";

    auto client = Violas::Client::create(host,
                                         port,
                                         "consensus_peers.config.toml",
                                         faucet_key,
                                         false,
                                         "", //libra testnet use this url to get test libra coin
                                         mnemonic);
}