//! Note further that the Key Derivation Function (KDF) chosen in the derivation of Child
//! Private Keys adheres to [HKDF RFC 5869](https://tools.ietf.org/html/rfc5869).

/**
 * @file wallet.cpp
 * @author Hunter (HunterSun2018@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-22
 *
 * @copyright Copyright (c) 2021
 *
 * the 11 bits of word index stores in two bytes
 * 10 9 8 7 6 5 4 3 2 1 0
 * [    byte 0    ] [ byte1 ]
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <bitset>
#include <memory>
// Open SSL
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#include <utils.hpp>
#include "mnemonic.hpp"
#include "wallet.hpp"

using namespace std;
using namespace crypto;

namespace violas
{
    const string MNEMONIC_SALT_PREFIX = "DIEM WALLET: mnemonic salt prefix$";
    const string MAIN_KEY_SALT = "DIEM WALLET: main key salt$";
    const string INFO_PREFIX = "DIEM WALLET: derived key$";

    const size_t iter_count = 10'000;

    static void check_ret(int ret, const char *error)
    {
        std::ostringstream oss;

        oss << error << "failed, error detail : " << ERR_error_string(ERR_get_error(), NULL) << std::endl;

        if (ret != 1)
            std::__throw_runtime_error(oss.str().c_str());
    }

    template <size_t N>
    array<uint8_t, N> hash(uint8_t *data, size_t len, const EVP_MD *md)
    {
        array<uint8_t, N> output;
        uint32_t out_len = output.size();
        EVP_MD_CTX *context = EVP_MD_CTX_new();

        EVP_DigestInit_ex(context, md, nullptr);
        EVP_DigestUpdate(context, data, len);
        EVP_DigestFinal_ex(context, output.data(), &out_len);

        EVP_MD_CTX_destroy(context);

        return output;
    }

    std::array<uint8_t, 32> sha_256(uint8_t *data, size_t len)
    {
        return hash<32>(data, len, EVP_sha256());
    }

    std::array<uint8_t, 64> sha_512(uint8_t *data, size_t len)
    {
        return hash<64>(data, len, EVP_sha512());
    }

    array<uint8_t, 32> sha3_256(uint8_t *data, size_t len)
    {
        array<uint8_t, 32> output;
        uint32_t out_len = output.size();
        EVP_MD_CTX *context = EVP_MD_CTX_new();

        EVP_DigestInit_ex(context, EVP_sha3_256(), nullptr);
        EVP_DigestUpdate(context, data, len);
        EVP_DigestFinal_ex(context, output.data(), &out_len);

        EVP_MD_CTX_destroy(context);

        return output;
    }

    Wallet::Wallet()
    {
    }

    Wallet::Wallet(Key &&entropy)
    {
        copy(begin(entropy), end(entropy), begin(m_entropy_hash));

        uint8_t hash[32];

        SHA256(entropy.data(), entropy.size(), hash);
        entropy[32] = hash[0];
    }

    Wallet::~Wallet()
    {
    }

    crypto::ed25519::PrivateKey
    Wallet::get_account_priv_key(size_t index)
    {
        return m_private_keys.at(index);
    }

    Wallet Wallet::generate_from_random()
    {
        Key entropy;

        return Wallet(move(entropy));
    }

    void Wallet::generate_seed(string_view salt)
    {
        auto mnemoic = export_mnemonic();
        string salt_ex = MNEMONIC_SALT_PREFIX + salt.data();

        int ret = PKCS5_PBKDF2_HMAC((const char *)mnemoic.data(), mnemoic.length(),
                                    (const uint8_t *)salt_ex.data(), salt_ex.length(),
                                    2048,
                                    EVP_sha3_256(),
                                    m_seed.size(), m_seed.data());
    }
    // An implementation of HKDF, the [HMAC-based Extract-and-Expand Key Derivation Function][1].
    void Wallet::extract_main_key()
    {
        int ret = 0;
        using hmac_ctx_ptr = unique_ptr<HMAC_CTX, decltype(&HMAC_CTX_free)>;
        hmac_ctx_ptr ctx(HMAC_CTX_new(), &HMAC_CTX_free);

        ret = HMAC_Init_ex(ctx.get(), MAIN_KEY_SALT.data(), MAIN_KEY_SALT.length(), EVP_sha3_256(), nullptr);
        check_ret(ret, "HMAC_Init_ex");

        ret = HMAC_Update(ctx.get(), m_seed.data(), m_seed.size());
        check_ret(ret, "HMAC_Update");

        uint32_t len = main_key.size();
        ret = HMAC_Final(ctx.get(), main_key.data(), &len);
        check_ret(ret, "HMAC_Final");
    }

    Wallet::Key Wallet::extend_child_private_key(uint64_t index)
    {
        int ret = 0;
        std::array<uint8_t, 32> out;
        size_t outlen = out.size();
        using evp_pkey_ctx_ptr = unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
        evp_pkey_ctx_ptr ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL), &EVP_PKEY_CTX_free);

        ret = EVP_PKEY_derive_init(ctx.get());
        check_ret(ret, "EVP_PKEY_derive_init");

        ret = EVP_PKEY_CTX_set_hkdf_md(ctx.get(), EVP_sha3_256());
        check_ret(ret, "EVP_PKEY_CTX_set_hkdf_md");

        ret = EVP_PKEY_CTX_set1_hkdf_salt(ctx.get(), MAIN_KEY_SALT.data(), MAIN_KEY_SALT.length());
        check_ret(ret, "EVP_PKEY_CTX_set1_hkdf_salt");

        ret = EVP_PKEY_CTX_set1_hkdf_key(ctx.get(), m_seed.data(), m_seed.size());
        check_ret(ret, "EVP_PKEY_CTX_set1_hkdf_key");

        vector<uint8_t> info(INFO_PREFIX.size() + 8, 0);
        copy(begin(INFO_PREFIX), end(INFO_PREFIX), begin(info));
        copy((uint8_t *)&index, (uint8_t *)&index + sizeof(index), begin(info) + INFO_PREFIX.size());

        ret = EVP_PKEY_CTX_add1_hkdf_info(ctx.get(), info.data(), info.size());
        check_ret(ret, "EVP_PKEY_CTX_add1_hkdf_info");

        ret = EVP_PKEY_derive(ctx.get(), out.data(), &outlen);
        check_ret(ret, "EVP_PKEY_derive");

        return out;
    }

    Wallet Wallet::generate_from_mnemonic(string_view mnemonic)
    {
        array<uint8_t, 33> entropy = {0};

        istringstream oss(mnemonic.data());
        size_t count = distance(istream_iterator<string>(oss), {});

        vector<size_t> count_define = {24, 21, 18, 15, 12};
        if (end(count_define) == find(begin(count_define), end(count_define), count))
            __throw_runtime_error("Mnemonic must have a word count of the following lengths: 24, 21, 18, 15, 12");

        // search(begin(count_define), end(count_define), )
        bitset<264> bits;
        int bit_index = 0;

        oss.clear();
        oss.str(mnemonic.data());

        for_each(istream_iterator<string>(oss),
                 {},
                 [&bits, &bit_index](auto word)
                 {
                     auto found = lower_bound(begin(WORDS), end(WORDS), word);
                     if (found == end(WORDS))
                         __throw_runtime_error((string("Mnemonic contains a unkonwn word : ") + word).c_str());

                     size_t index = distance(begin(WORDS), found);
                     uint8_t len = (uint8_t)index;
                     // convert index to 11 bits to bitset
                     for (int i = 10; i >= 0; i--)
                     {
                         if (index & (1 << i))
                             bits.set(bit_index);

                         ++bit_index;
                     }
                 });

        for (size_t i = 0; i < entropy.size() * 8; i++)
        {
            size_t index = i / 8;
            size_t bit = 7 - i % 8;
            uint8_t byte = ((bits.test(i) ? 1 : 0) << bit);

            entropy[index] |= byte;
        }

        // verify hash sum
        uint8_t hash[32];

        SHA256(entropy.data(), 32, hash);
        if (entropy[32] != hash[0])
            __throw_runtime_error("Failed to verify mnemonic.");

        Wallet wallet;

        wallet.m_entropy_hash = move(entropy);

        wallet.generate_seed();

        wallet.extract_main_key();

        return wallet;
    }

    string Wallet::export_mnemonic()
    {
        uint8_t hash[32];

        uint8_t *r = SHA256(m_entropy_hash.data(), 32, hash);
        m_entropy_hash[32] = hash[0];

        ostringstream oss;
        size_t word_index = 0;

        for (int i = 0; i < m_entropy_hash.size() * 8; i++)
        {
            size_t byte_index = i / 8;
            size_t bit_index = 7 - i % 8;

            uint8_t byte = m_entropy_hash[byte_index];
            if (byte & (1 << bit_index))
                word_index |= (1 << (10 - i % 11));

            if (i % 11 == 10)
            {
                if (i != 10)
                    oss << " ";

                oss << WORDS[word_index];

                word_index = 0;
            }
        }

        oss.seekp(-1, ios::end);
        oss.flush();

        return oss.str();
    }

    enum Scheme
    {
        Ed25519 = 0,
        MultiEd25519 = 1,
        // ... add more schemes here
    };

    Wallet::Key Wallet::pub_key_to_auth_key(const ed25519::PublicKey &pub_key)
    {
        auto raw_pub_key = pub_key.get_raw_key();

        vector<uint8_t> auth_key_preimage(begin(raw_pub_key), end(raw_pub_key));
        auth_key_preimage.push_back(Scheme::Ed25519);

        auto hash = sha3_256(auth_key_preimage.data(), auth_key_preimage.size());

        return hash;
    }

    diem_types::AccountAddress Wallet::pub_key_account_address(const ed25519::PublicKey &pub_key)
    {
        diem_types::AccountAddress address;

        auto auth_key = pub_key_to_auth_key(pub_key);

        // copy 16 bytes suffix
        copy(begin(auth_key) + 16, end(auth_key), begin(address.value));

        return address;
    }

    std::tuple<size_t, diem_types::AccountAddress> Wallet::create_next_account()
    {
        size_t keys_size = m_private_keys.size();

        Key raw_key = extend_child_private_key(keys_size);
        auto priv_key = ed25519::PrivateKey::from_raw_key(ed25519::RawKey(raw_key));

        diem_types::AccountAddress address;
        auto raw_pub_key = priv_key.get_public_key().get_raw_key();

        m_private_keys.push_back(priv_key);

        vector<uint8_t> auth_key_preimage(begin(raw_pub_key), end(raw_pub_key));
        auth_key_preimage.push_back(Scheme::Ed25519);

        auto hash = sha3_256(auth_key_preimage.data(), auth_key_preimage.size());
        // copy sha3 256 hash
        copy(begin(hash) + 16, end(hash), begin(address.value));

        return make_tuple<>(keys_size, address);
    }

    array<uint8_t, 16>
    Wallet::get_account_address(size_t index)
    {
        auto pubkey = ed25519::PrivateKey::generate().get_public_key().get_raw_key();
        array<uint8_t, 16> address;

        copy(begin(pubkey), begin(pubkey) + 16, begin(address));

        return address;
    }

    std::vector<Wallet::Account> Wallet::get_all_accounts()
    {
        std::vector<Account> accounts;
        size_t index = 0;

        for (const auto &priv_key : m_private_keys)
        {
            Account account{
                index++,
                pub_key_account_address(priv_key.get_public_key()),
                pub_key_to_auth_key(priv_key.get_public_key())};

            accounts.push_back(move(account));
        }

        return accounts;
    }

    void Wallet::run_test_case()
    {
        cout << "run test for Wallet::run_test_case " << endl;

        //"7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f";
        // string mnemonic = export_mnemonic(key);
        Wallet wallet = Wallet::generate_from_mnemonic("legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth title");

        auto [index, account_address] = wallet.create_next_account();
        assert(bytes_to_hex(account_address.value) == "11699be59c340efc1cb0e740b539b620");

        cout << bytes_to_hex(account_address.value) << endl;

        tie(index, account_address) = wallet.create_next_account();
        assert(bytes_to_hex(account_address.value) == "3383b94a7bfeefe571579cff58b20a7e");

        cout << bytes_to_hex(account_address.value) << endl;

        Key key = wallet.extend_child_private_key(0);

        assert(bytes_to_hex(key) == "d8b5edb968050bc9589b64e1d2445a5455745630449eef2f0005fe362b4379d4");

        Key key1 = wallet.extend_child_private_key(1);

        generate_from_mnemonic("hamster diagram private dutch cause delay private meat slide toddler razor book happy fancy gospel tennis maple dilemma loan word shrug inflict delay length");

        string hex = "000000000000000000000000000b1e55ed0f0000000000000001b901a11ceb0b010000000601000403040b040f0205111b072c4a087610000000010102000100000302010101010402060c030005060c050a020a020106060c03050a020a02010109000b4469656d4163636f756e740c536c6964696e674e6f6e6365157265636f72645f6e6f6e63655f6f725f61626f72741a6372656174655f706172656e745f766173705f6163636f756e7400000000000000000000000000000001010103010a0a000a0111000b000a020b030b040a0538000201070000000000000000000000000000000103564c5303564c53000501000000000000000003dc37cd5bdbac5a3f4083a1549469c456041062778143194d4a113133cc5cafd22bc604084e46542056415350050140420f0000000000000000000000000003564c530fd8896100000000040020123eeb8d9434839c7bfbf327659f34a07eac4a728f6ab6a5f5ff081553db26ee40e1fea9ed6d1485613e0286acff88ce3c85f0ddc799760ca036dd917af238b4f6693580bf3478629cb5f763858b8a5cdaf08cfb1febaa452936e39cda57f44006";
        auto bytes = hex_to_bytes(hex);

        auto hash_sha3 = sha3_256(bytes.data(), bytes.size());

        auto hash_sha256 = sha_256(bytes.data(), bytes.size());

        auto hash_sha512 = sha_512(bytes.data(), bytes.size());
    }
}
