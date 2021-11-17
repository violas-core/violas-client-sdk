#pragma once
/**
 * @file ed25519.hpp
 * @author Hunter Sun (HunterSun2018@gmail.com)
 * @brief  Ed25519 signing and verifying algorithom wrapper with openssl
 * @version 0.1
 * @date 2021-11-17
 *
 * @copyright Copyright (c) 2021
 *  PureEdDSA算法不支持使用其他签名算法的流机制，例如，使用EVP_DigestUpdate（）。必须使用一次性EVP_DigestSign（）和EVP_DigestVerify（）函数传递要签名或验证的消息。
 *  调用EVP_DigestSignInit（）或EVP_DigestVerifyInit（）时，摘要类型参数必须设置为NULL。
 */
#include <openssl/evp.h>
#include <array>

namespace ed25519
{
    const size_t KEY_LENGTH = 32;
    const size_t SIGNATURE_LENGTH = 64;
    using RawKey = std::array<uint8_t, KEY_LENGTH>;
    using Signature = std::array<uint8_t, SIGNATURE_LENGTH>;

    class PublicKey
    {
    private:
        /* data */
        EVP_PKEY *m_pkey = nullptr;

    public:
        PublicKey(RawKey raw_key);

        static PublicKey from_hex_string(const std::string &hex_str);

        ~PublicKey();

        RawKey get_raw_key();

        std::string dump_hex();

        bool verify(const Signature &sig, uint8_t *data, size_t len);
    };

    class PrivateKey
    {
    private:
        /* data */
        EVP_PKEY *m_pkey = NULL;

        PrivateKey(EVP_PKEY *pkey);

    public:
        ~PrivateKey();

        //PrivateKey(const PrivateKey &priv_key) = delete;
        PrivateKey operator=(const PrivateKey &priv_key) = delete;

        static PrivateKey generate();

        static PrivateKey from_raw_key(const RawKey &key);

        static PrivateKey from_hex_string(const std::string &hex_str);

        RawKey get_raw_key();

        std::string dump_hex();

        PublicKey get_public_key();
        //
        // sign for message
        //
        Signature sign(uint8_t *data, size_t len);
    };

    void run_test_case();
}
