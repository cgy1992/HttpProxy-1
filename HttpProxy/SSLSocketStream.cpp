#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <openssl\ssl.h>
#include <openssl\err.h>
#ifdef __cplusplus
}
#endif

#include "BaseSSLConfig.h"
#include "CommonFuncs.h"
#include "SSLSocketStream.h"
#include "CertificateProvider.h"
#include "RootCert_PriKey/RootCertConfig.h"

extern BaseSSLConfig* g_BaseSSLConfig;

BOOL SSLSocketStream::bInitCritical_section=FALSE;
CRITICAL_SECTION SSLSocketStream::m_cert_lock={0};

SSLSocketStream::SSLSocketStream(char**pprecv_buf, DWORD *plen_recv_buf, char**ppsend_buf, DWORD *plen_send_buf):
    BaseSocketStream(pprecv_buf,plen_recv_buf,ppsend_buf,plen_send_buf)
{
    m_ctx=NULL;
    m_x509=NULL;
    m_keypair=NULL;
    m_send_bio=NULL;
    m_recv_bio=NULL;
    m_ssl=NULL;
}
SSLSocketStream::~SSLSocketStream() {
    uninit();   //�ͷ�
}

char* SSLSocketStream::_classname(char *buf, DWORD len)
{
    if (buf == NULL || len == 0) return NULL;

    strcpy_s(buf, len, "SSLSocketStream");
    return buf;
}

/*
*��ʼ�����������е�SSL��ʼ�������������
*/
int SSLSocketStream::init(void *buf,int len)
{
    SSL_METHOD *method;
    int ret=0;
    method = (SSL_METHOD*)SSLv23_method();
   // method = (SSL_METHOD*)TLSv1_2_method();
    //method = (SSL_METHOD*)TLSv1_method();
    m_ctx = SSL_CTX_new(method);
    if(m_ctx!=NULL){
        SSL_CTX_set_verify(m_ctx, SSL_VERIFY_NONE, NULL);
        SSL_CTX_set_mode(m_ctx, SSL_MODE_AUTO_RETRY);

        init_keycert(buf,len);

        if (SSL_CTX_use_PrivateKey(m_ctx, m_keypair) <= 0)
        {
            ret=0;
            goto tag;
        }
        if (SSL_CTX_use_certificate(m_ctx, m_x509) <= 0)
        {
            ret=0;
            goto tag;
        }

        if (!SSL_CTX_check_private_key(m_ctx))
        {
            ret=0;
            goto tag;
        }

    }
    else
        return 0;


    m_ssl = SSL_new(m_ctx);

    if (m_ssl != NULL)
    {
            m_send_bio = BIO_new(BIO_s_mem());
            m_recv_bio = BIO_new(BIO_s_mem());
            SSL_set_bio(m_ssl, m_recv_bio, m_send_bio);
            SSL_set_accept_state(m_ssl);
            ret=1;
    }
    else{
        ret=0;
    }

tag:
    if(ret==0)
    {
        if(m_ctx!=NULL)
        {
            SSL_CTX_free(m_ctx);
            m_ctx=NULL;
        }

        if(m_ssl!=NULL)
        {
            SSL_free(m_ssl);
            m_ssl=NULL;
        }

        if(m_send_bio!=NULL)
        {
            BIO_free(m_send_bio);
            m_send_bio=NULL;
        }

        if(m_recv_bio!=NULL)
        {
            BIO_free(m_recv_bio);
            m_recv_bio=NULL;
        }
    }
    return ret;
}

void SSLSocketStream::uninit()
{
    if(m_ssl!=NULL)
    {
        SSL_shutdown(m_ssl);
        SSL_free(m_ssl);
        m_ssl=NULL;
    }

    if(m_ctx!=NULL)
    {
        SSL_CTX_free(m_ctx);
        m_ctx=NULL;
    }
    if(m_x509!=NULL)
    {
        X509_free(m_x509);
        m_x509=NULL;
    }

    if(m_keypair!=NULL)
    {
        EVP_PKEY_free(m_keypair);
        m_keypair=NULL;
    }

    
}
char * SSLSocketStream::get_OpenSSL_Error() 
{
    memset(m_szErrorMsg, 0, 1024);
    unsigned long ulErr = ERR_get_error();
    char *pTmp = NULL;
    // ��ʽ��error:errId:��:����:ԭ��
    pTmp = ERR_error_string(ulErr, m_szErrorMsg);
    return pTmp;
}


//����������Ҫ����ط�
int SSLSocketStream::write(void *buf,DWORD len) 
{
    int result = 0;
    int bytes = SSL_write(m_ssl, buf, len);
    if (bytes) 
    {
        //
        if (BIO_pending(m_send_bio)) {
            bytes = BIO_ctrl_pending(m_send_bio);
            *m_ppsend_buf = (char*)malloc(bytes);
            memset(*m_ppsend_buf, 0, bytes);
            result=BIO_read(m_send_bio, *m_ppsend_buf, bytes);
            if (result > 0) {
                *m_plen_send_buf = result;
            }
        }
    }
    return BaseSocketStream::BSS_RET_RESULT;
}

//�������ݵĵط�
int SSLSocketStream::read(void *buf,DWORD len) 
{

    //�������
    *m_ppsend_buf = NULL;
    *m_plen_send_buf = 0;

    *m_pprecv_buf = NULL;
    *m_plen_recv_buf = 0;

    int bio_read_bufsize = 0;
    int ssl_read_bufsize = 0;

    char ssl_read_tmpbuf[1024] = { 0 };
    int ssl_readedsize = 0;//��¼�Ѿ���ȡ�����ݳ���

    int ret = 0;
    char error[1024] = { 0 };
    

    int bytes = BIO_write(m_recv_bio, buf, len);
    if (bytes != len)
    {//��ȷ��ȡ������
        return BaseSocketStream::BSS_RET_ERROR; //���ش���
    }
    
    do {
        ssl_read_bufsize = SSL_read(m_ssl, ssl_read_tmpbuf, 1024);
        if (ssl_read_bufsize < 0)
            break;
        else {
            *m_pprecv_buf = CommonFuncs::_realloc(m_pprecv_buf, *m_plen_recv_buf, *m_plen_recv_buf + ssl_read_bufsize);
            memset(*m_pprecv_buf + *m_plen_recv_buf, 0, ssl_read_bufsize);
            memcpy_s(*m_pprecv_buf + *m_plen_recv_buf, *m_plen_recv_buf + ssl_read_bufsize+1, ssl_read_tmpbuf, ssl_read_bufsize);
            *m_plen_recv_buf += ssl_read_bufsize;
            if(ssl_read_bufsize< 1024)
                return BaseSocketStream::BSS_RET_RESULT;    //ֱ�ӷ��ؽ�����ɺ�����do_GET����
        }
    } while (ssl_read_bufsize);
    
    if (BIO_pending(m_send_bio)) {
        bio_read_bufsize = BIO_ctrl_pending(m_send_bio);
        *m_ppsend_buf = (char*)malloc(bio_read_bufsize);
        memset(*m_ppsend_buf, 0, bio_read_bufsize);
        bytes = BIO_read(m_send_bio, *m_ppsend_buf, bio_read_bufsize);
        if (bytes == bio_read_bufsize) {
            *m_plen_send_buf = bytes;
            return BaseSocketStream::BSS_RET_SEND;
        }
    }
    return BaseSocketStream::BSS_RET_RECV;
}


//
void SSLSocketStream::init_keycert(void*buf,int len)
{
    char *purl = NULL;
    int ret = 0;
    X509* CA = NULL;
    PKCS12*pkcs12 = NULL;

    if(len <= 0) return;
   
    
 
    SSLSocketStream::_entry_();

    purl = (char*)malloc(len+1);

    if(purl!=NULL){
        memset(purl, 0, len+1);
        memcpy_s(purl, len+1, buf, len);
    }
    else return;

    ret = CertificateProvider::is_certexist(ISSUSER,"MY",purl);
    if(ret)
    {//
        pkcs12 = CertificateProvider::get_pkcs12fromWindowsAuth(PASSWORD,ISSUSER,"MY",purl);

        if(pkcs12 != NULL)
        {
            ret = CertificateProvider::pkcs12_getx509(pkcs12, PASSWORD, 6, &m_x509, &m_keypair, &CA);
            if(!ret)
            {
                CertificateProvider::del_certs(ISSUSER,"MY", purl); 
                m_keypair = CertificateProvider::generate_keypair(2048);
                m_x509 = CertificateProvider::generate_certificate(m_keypair, purl, purl, purl);
                g_BaseSSLConfig->CA(m_x509);
                pkcs12 = CertificateProvider::x509topkcs12(m_x509, m_keypair, PASSWORD, purl, g_BaseSSLConfig->rootcert());
                CertificateProvider::addCert2WindowsAuth_MY(pkcs12, PASSWORD);
                
            }
            else{

            }

        }
        
    }
    else
    {
        m_keypair = CertificateProvider::generate_keypair(2048);
        m_x509 = CertificateProvider::generate_certificate(m_keypair,purl, purl, purl, CertificateProvider::DEF_DAYS);
        ret = g_BaseSSLConfig->CA(m_x509);
        pkcs12 = CertificateProvider::x509topkcs12(m_x509,m_keypair,PASSWORD,NULL,NULL);
        CertificateProvider::addCert2WindowsAuth_MY(pkcs12, PASSWORD);
    }

    if(purl != NULL) free(purl);
    if(pkcs12 != NULL) PKCS12_free(pkcs12);

    SSLSocketStream::_leave_();
}

void SSLSocketStream::_init_syn()
{
    if(!bInitCritical_section)
    {
        InitializeCriticalSection(&SSLSocketStream::m_cert_lock);
        bInitCritical_section=TRUE;
    }

}

void SSLSocketStream::_entry_()
{
    ::EnterCriticalSection(&SSLSocketStream::m_cert_lock);
}

void SSLSocketStream::_leave_()
{
    ::LeaveCriticalSection(&SSLSocketStream::m_cert_lock);
}