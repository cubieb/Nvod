#ifndef _Des3_h_
#define _Des3_h_

/**********************class Des3**********************/
class Des3
{
public:
    Des3();
    ~Des3();

    /************************************************************************
    ** 本例采用：
    ** 3des-ecb加密方式；
    ** 24位密钥；
    ** 加密内容8字节补齐, 不足的字节补0x00
    ** 本身已8字节对齐的，不做补齐处理。
    ** Example: 
       char *plaintext = "this is plain text",
       char ciphertext[64];
       Encrypt(plaintextSize, strlen(plaintextSize), ciphertext, sizeof(ciphertext));
    ************************************************************************/
    static size_t Encrypt(const char *plaintext, size_t plaintextSize, char *ciphertext, size_t ciphertextSize);
    static size_t Decrypt(const char *ciphertext, size_t ciphertextSize, char *plaintext, size_t plaintextSize);
};

#endif /* Des3 */