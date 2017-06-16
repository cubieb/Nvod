#include "Des3.h"

#include <openssl/des.h>   

/* Type */
#include "BaseType.h"

/* Foundation */
#include "SystemInclude.h"

#ifdef _WIN32
#	pragma comment(lib, "libcrypto-37.lib")
#endif

using namespace std;

/**********************class Des3**********************/
Des3::Des3()
{}

Des3::~Des3()
{}

size_t Des3::Encrypt(const char *plaintext, size_t plaintextSize, char *ciphertext, size_t ciphertextSize)
{
    char *key = "sjz#Nu!i.GrptE>S-a*M~'D3";
    const size_t keySize = 24;
    assert(strlen(key) == keySize);
    
    /* 分析补齐明文所需空间及补齐填充数据 */
    size_t padSize = 8 - (plaintextSize % 8);
    size_t dstSize = plaintextSize + padSize;
    assert(dstSize <= ciphertextSize);

    /* 构造补齐后的加密内容 */
    char *src = new char[dstSize]; 
    memcpy(src, plaintext, plaintextSize);
    memset(src + plaintextSize, 0x00, padSize);

    /* 密钥置换 */
    DES_key_schedule ks[3];
    for (size_t i = 0; i < 3; ++i)
    {
         DES_set_key_unchecked(reinterpret_cast<const_DES_cblock*>(key + 8 * i), &ks[i]);
    }

    char *dst = ciphertext;
    for (const char *ptr = src; ptr < src + dstSize; ptr = ptr + 8, dst = dst + 8)
    {
        DES_ecb3_encrypt((const_DES_cblock*)ptr, (DES_cblock*)dst, &ks[0], &ks[1], &ks[2], DES_ENCRYPT);
    }

    return dstSize;
}

size_t Des3::Decrypt(const char *ciphertext, size_t ciphertextSize, char *plaintext, size_t plaintextSize)
{
    char *key = "sjz#Nu!i.GrptE>S-a*M~'D3";
    const size_t keySize = 24;
    assert(strlen(key) == keySize);

    /* 密钥置换 */
    DES_key_schedule ks[3];
    for (size_t i = 0; i < 3; ++i)
    {
        DES_set_key_unchecked(reinterpret_cast<const_DES_cblock*>(key + 8 * i), &ks[i]);
    }

    assert((ciphertextSize % 8) == 0);
    assert(ciphertextSize <= plaintextSize);
    char *dst = plaintext;
    for (const char *ptr = ciphertext; ptr < ciphertext + ciphertextSize; ptr = ptr + 8, dst = dst + 8)
    {
        DES_ecb3_encrypt((const_DES_cblock*)ptr, (DES_cblock*)dst, &ks[0], &ks[1], &ks[2], DES_DECRYPT);
    }

    return ciphertextSize;
}