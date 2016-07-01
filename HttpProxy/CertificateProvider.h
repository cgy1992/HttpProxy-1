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
    static int          write_to_disk(EVP_PKEY * pkey, X509 * x509);
    static X509*        csr2crt(X509_REQ *x509_req, EVP_PKEY *pKey);
    static X509*        CreateCertificate(EVP_PKEY * pkey, BOOL bRoot);
    static EVP_PKEY *   Generate_KeyPair(int numofbits);  //������Կ��
    static int          addCert2WindowsAuth(unsigned char *buf_x509_der, int len_x509_der, const char *pos);
    static int          addCert2WindowsAuth(X509* x509, const char *pos);
    static int          rand_serial(BIGNUM *b, ASN1_INTEGER *ai);
    static int          exportx509(X509* x509,unsigned char *buf,int len);
};