#pragma once

/*
֤���ṩ�ߣ���Ҫ������������Ҫ�ĸ�֤��Ͷ��������վÿ��ǩ��֤��
*/
#include <windows.h>
#include <openssl/x509.h>
class CertificateProvider
{
public:
    CertificateProvider();
    ~CertificateProvider();

public:
    static X509*        csr2crt(X509_REQ *x509_req, EVP_PKEY *pKey);
    static X509*        generate_certificate(EVP_PKEY * pkey, char * url,int len,BOOL bRoot);
    static EVP_PKEY *   generate_keypair(int numofbits);  //������Կ��
    static int          addCert2WindowsAuth(unsigned char *buf_x509_der, int len_x509_der, const char *pos);
    static int          addCert2WindowsAuth(X509* x509, const char *pos);  
    static int          exportx509(X509* x509,unsigned char *buf,int len);
    static int          x509_certify(X509*x,X509*xca,EVP_PKEY*pkey_ca); /*CAǩ��*/
    static int          saveX509tofile(X509* x509,char *path);
private:
    static int          rand_serial(BIGNUM *b, ASN1_INTEGER *ai);
    static int          do_X509_sign(X509 *x, EVP_PKEY *pkey, const EVP_MD *md,STACK_OF(OPENSSL_STRING) *sigopts);
    static int          pkey_ctrl_string(EVP_PKEY_CTX *ctx, const char *value);
    static int          do_sign_init(EVP_MD_CTX *ctx, EVP_PKEY *pkey, const EVP_MD *md, STACK_OF(OPENSSL_STRING) *sigopts);
};