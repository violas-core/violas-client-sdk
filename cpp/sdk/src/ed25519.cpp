/**
 * @file ed25519.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-11-17
 *
 * @copyright Copyright (c) 2021
 * Note: PureEdDSA算法不支持使用其他签名算法的流机制，例如，使用EVP_DigestUpdate（）。必须使用一次性EVP_DigestSign（）和EVP_DigestVerify（）函数传递要签名或验证的消息。
 *      调用EVP_DigestSignInit（）或EVP_DigestVerifyInit（）时，摘要类型参数必须设置为NULL。
 */
#include <algorithm>
#include <memory>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <exception>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "../include/ed25519.hpp"

using namespace std;

namespace crypto::ed25519
{
    static void check_ret(int ret, const char *error)
    {
        std::ostringstream oss;

        oss << error << "failed, error detail : " << ERR_error_string(ERR_get_error(), NULL) << std::endl;

        if (ret != 1)
            std::__throw_runtime_error(oss.str().c_str());
    }

    RawKey hex_to_raw_key(const string &hex_key)
    {
        RawKey raw_key;
        size_t len = raw_key.size();

        if (hex_key.length() != 64)
            __throw_invalid_argument("the length of hex key string must be equal to 64");

        auto iter = begin(raw_key);
        for (size_t i = 0; i < hex_key.length(); i += 2)
        {
            stringstream ss;
            uint16_t byte;

            ss << hex_key[i] << hex_key[i + 1];
            ss >> hex >> byte;

            // write raw_key from end to begin
            *iter++ = (uint8_t)byte;
        }

        return raw_key;
    }
    /********************************************************************
     *  Public Key
     ********************************************************************/

    PublicKey::PublicKey(RawKey raw_key)
    {
        m_pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, NULL, raw_key.data(), raw_key.size());
    }

    PublicKey::~PublicKey()
    {
        EVP_PKEY_free(m_pkey);
    }

    PublicKey PublicKey::from_hex_string(const std::string &hex_str)
    {
        return PublicKey(hex_to_raw_key(hex_str));
    }

    RawKey PublicKey::get_raw_key() const
    {
        RawKey raw_key;
        size_t len = raw_key.size();

        EVP_PKEY_get_raw_public_key(m_pkey, raw_key.data(), &len);

        return raw_key;
    }

    std::string PublicKey::dump_hex() const
    {
        auto raw_key = get_raw_key();
        ostringstream oss;

        for_each(begin(raw_key), end(raw_key), [&](uint8_t v)
                 { oss << hex << setw(2) << setfill('0') << (uint16_t)v; });

        return oss.str();
    }

    bool PublicKey::verify(const Signature &sig, uint8_t *data, size_t len)
    {
        using ctx_ptr_t = unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

        ctx_ptr_t md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
        int ret = 0;

        ret = EVP_DigestVerifyInit(md_ctx.get(), nullptr, NULL, NULL, m_pkey); // EVP_sha3_256()
        check_ret(ret, "EVP_DigestVerifyInit");

        ret = EVP_DigestVerify(md_ctx.get(), sig.data(), sig.size(), data, len);
        // check_ret(ret, "EVP_DigestVerify");

        return ret == 1;
    }

    /********************************************************************
     *  Private Key
     ********************************************************************/

    PrivateKey::PrivateKey(EVP_PKEY *pkey)
    {
        m_pkey = pkey;
    }

    PrivateKey::PrivateKey(PrivateKey &&r)
    {
        m_pkey = r.m_pkey;
        r.m_pkey = nullptr;
    }

    PrivateKey::~PrivateKey()
    {
        EVP_PKEY_free(m_pkey);
    }

    PrivateKey::PrivateKey(const PrivateKey &r)
    {
        m_pkey = r.m_pkey;

        EVP_PKEY_up_ref(m_pkey);
    }

    PrivateKey PrivateKey::operator=(const PrivateKey &r)
    {
        m_pkey = r.m_pkey;

        EVP_PKEY_up_ref(m_pkey);

        return *this;
    }

    PrivateKey PrivateKey::generate()
    {
        EVP_PKEY_CTX *m_pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
        EVP_PKEY *pkey = nullptr;
        int ret = 0;

        ret = EVP_PKEY_keygen_init(m_pctx);
        check_ret(ret, "EVP_PKEY_keygen_init");

        ret = EVP_PKEY_keygen(m_pctx, &pkey);

        EVP_PKEY_CTX_free(m_pctx);
        check_ret(ret, "EVP_PKEY_keygen");

        return PrivateKey(pkey);
    }

    PrivateKey PrivateKey::from_raw_key(const RawKey &raw_key)
    {
        EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, NULL, raw_key.data(), raw_key.size());

        uint8_t priv[32];
        size_t private_size = sizeof(priv);
        int ret = EVP_PKEY_get_raw_private_key(pkey, priv, &private_size);
        check_ret(ret, "EVP_PKEY_get_raw_private_key");

        return PrivateKey(pkey);
    }

    PrivateKey PrivateKey::from_hex_string(const std::string &hex_key)
    {
        return from_raw_key(hex_to_raw_key(hex_key));
    }

    RawKey PrivateKey::get_raw_key() const
    {
        RawKey raw_key;
        size_t len = raw_key.size();

        int ret = EVP_PKEY_get_raw_private_key(m_pkey, raw_key.data(), &len);

        return raw_key;
    }

    std::string PrivateKey::dump_hex() const
    {
        auto raw_key = get_raw_key();
        ostringstream oss;

        for_each(begin(raw_key), end(raw_key), [&](uint8_t v)
                 { oss << hex << setw(2) << setfill('0') << (uint16_t)v; });

        return oss.str();
    }

    PublicKey PrivateKey::get_public_key() const
    {
        RawKey raw_key = {0};
        size_t len = raw_key.size();

        int ret = EVP_PKEY_get_raw_public_key(m_pkey, raw_key.data(), &len);
        check_ret(ret, "EVP_PKEY_get_raw_public_key");

        return PublicKey(raw_key);
    }

    Signature PrivateKey::sign(uint8_t *data, size_t data_len)
    {
        using ctx_ptr_t = unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;
        ctx_ptr_t md_ctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);

        Signature signature;
        size_t sig_len = signature.size();
        int ret = 0;

        ret = EVP_DigestSignInit(md_ctx.get(), NULL, NULL, NULL, m_pkey);
        check_ret(ret, "EVP_DigestSignInit");

        ret = EVP_DigestSign(md_ctx.get(), signature.data(), &sig_len, data, data_len);
        check_ret(ret, "EVP_DigestSign");

        return signature;
    }

    void run_test_case()
    {
        try
        {
            bool ret = false;
            string msg = "This is a test of the tsunami alert system.";
            string hex_key = "4cc9cd70d755484327b5164fa8f3f080b12aea9cbcc7bf0d4e7d92f58d4ae990";
            string hex_key1 = "d8b5edb968050bc9589b64e1d2445a5455745630449eef2f0005fe362b4379d4";

            auto priv_key = PrivateKey::from_hex_string(hex_key);
            string hex_pub_key = priv_key.get_public_key().dump_hex();
            assert(hex_pub_key == "5efff47b51644c4a247fc73b3e20e3ee5d44ff31de2ad7192aa6e00c724b6bc1");

            auto priv_key1 = PrivateKey::from_hex_string(hex_key1);
            string hex_pub_key1 = priv_key1.get_public_key().dump_hex();

            Signature sig = priv_key.sign((uint8_t *)msg.data(), msg.length());

            PublicKey pub_key = PublicKey::from_hex_string(hex_pub_key);
            ret = pub_key.verify(sig, (uint8_t *)msg.data(), msg.length());
            assert(ret);

            auto priv_key2 = PrivateKey::generate();

            sig = priv_key2.sign((uint8_t *)msg.data(), msg.length());

            ret = priv_key2.get_public_key().verify(sig, (uint8_t *)msg.data(), msg.length());
            assert(ret);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            assert(false);
        }
    }
}

namespace crypto::x25519
{

}