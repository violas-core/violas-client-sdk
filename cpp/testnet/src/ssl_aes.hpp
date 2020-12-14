#ifndef AES_256_CBC_HPP
#define AES_256_CBC_HPP
#include <string_view>
#include <string>
#include <array>
#include <random>
#include <openssl/aes.h>
#include <openssl/evp.h>


const size_t KEY_LEN = AES_BLOCK_SIZE * 3; // 32 bits password, 16 bits AES IV
const size_t ITERATE_COUNT = 1'000'000;
const uint8_t FLAG1 = 0xFF;
const uint8_t FLAG2 = 0xEE;

template <typename InputIterator, typename OutputIterator>
void aes_256_cbc_encrypt(std::string_view password, InputIterator first, InputIterator last, OutputIterator result)
{
    using namespace std;

    //static_assert(std::iterator_traits<InputIterator>::value_type == uint8_t, "");

    int ret = 0;
    uint8_t salt[4] = {'s', 'a', 'l', 't'};
    uint8_t key_out[KEY_LEN] = {0};
    auto iv = key_out + AES_BLOCK_SIZE * 2;

    // Update salt with the first 4 bytes of content
    *(size_t *)salt = random_device()();

    ret = PKCS5_PBKDF2_HMAC(password.data(), password.length(), salt, sizeof(salt), ITERATE_COUNT, EVP_sha256(), KEY_LEN, key_out);
    if (!ret)
        __throw_runtime_error("aes_256_cbc_encrypt -> PKCS5_PBKDF2_HMAC error");

    AES_KEY aes_key;
    //uint8_t iv[AES_BLOCK_SIZE] = {0};

    ret = AES_set_encrypt_key(key_out, 256, &aes_key);
    if (ret)
        __throw_runtime_error("aes_256_cbc_encrypt -> AES_set_encrypt_key error.");

    // Write encryption flag
    *result++ = FLAG1;
    *result++ = FLAG2;

    // Write salt to output interator
    copy(begin(salt), end(salt), result);

    while (first != last)
    {
        uint8_t input[AES_BLOCK_SIZE] = {0};
        uint8_t out[AES_BLOCK_SIZE] = {0};

        for (auto i = 0; i < AES_BLOCK_SIZE && first != last; i++)
            input[i] = *first++;

        AES_cbc_encrypt(input, out, AES_BLOCK_SIZE, &aes_key, iv, AES_ENCRYPT);

        copy(begin(out), end(out), result);
    }
}

template <typename InputIterator, typename OutputIterator>
void aes_256_cbc_decrypt(std::string_view password, InputIterator first, InputIterator last, OutputIterator result)
{
    using namespace std;

    int ret = 0;
    uint8_t salt[4] = {'s', 'a', 'l', 't'};
    uint8_t key_out[KEY_LEN] = {0};
    auto iv = key_out + AES_BLOCK_SIZE * 2;

    // Verify 2 bytes of content header 
    uint8_t flag_byte1 = *first++;
    uint8_t flag_byte2 = *first++;
    if (flag_byte1 != FLAG1 && flag_byte2 != FLAG2)
        __throw_runtime_error("the input is not the encrypted content.");

    // Update salt with the first 4 bytes of content
    for (auto i = 0; i < 4; i++)
        salt[i] = *first++;

    ret = PKCS5_PBKDF2_HMAC(password.data(), password.length(), salt, sizeof(salt), ITERATE_COUNT, EVP_sha256(), KEY_LEN, key_out);
    if (!ret)
        __throw_runtime_error("PKCS5_PBKDF2_HMAC error.");

    AES_KEY aes_key;

    ret = AES_set_decrypt_key(key_out, 256, &aes_key);
    if (ret)
        __throw_runtime_error("AES_set_encrypt_key error.");

    while (first != last)
    {
        uint8_t input[AES_BLOCK_SIZE] = {0};
        uint8_t out[AES_BLOCK_SIZE] = {0};

        for (auto i = 0; i < AES_BLOCK_SIZE && first != last; i++)
            input[i] = *first++;

        AES_cbc_encrypt(input, out, AES_BLOCK_SIZE, &aes_key, iv, AES_DECRYPT);

        copy(begin(out), end(out), result);
    }
}

#endif