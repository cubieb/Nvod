#ifndef _Des3_h_
#define _Des3_h_

/**********************class Des3**********************/
class Des3
{
public:
    Des3();
    ~Des3();

    /************************************************************************
    ** �������ã�
    ** 3des-ecb���ܷ�ʽ��
    ** 24λ��Կ��
    ** ��������8�ֽڲ���, ������ֽڲ�0x00
    ** ������8�ֽڶ���ģ��������봦��
    ** Example: 
       char *plaintext = "this is plain text",
       char ciphertext[64];
       Encrypt(plaintextSize, strlen(plaintextSize), ciphertext, sizeof(ciphertext));
    ************************************************************************/
    static size_t Encrypt(const char *plaintext, size_t plaintextSize, char *ciphertext, size_t ciphertextSize);
    static size_t Decrypt(const char *ciphertext, size_t ciphertextSize, char *plaintext, size_t plaintextSize);
};

#endif /* Des3 */