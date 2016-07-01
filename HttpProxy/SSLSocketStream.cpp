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

extern BaseSSLConfig* g_BaseSSLConfig;

SSLSocketStream::SSLSocketStream(char**pprecv_buf, DWORD *plen_recv_buf, char**ppsend_buf, DWORD *plen_send_buf):
    BaseSocketStream(pprecv_buf,plen_recv_buf,ppsend_buf,plen_send_buf)
{
    init();
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
��ʼ�����������е�SSL��ʼ�������������
*/
void SSLSocketStream::init()
{

    m_ssl = SSL_new(g_BaseSSLConfig->context());//����ط�������Ƚ����أ�������Ϊg_BaseSSLConfig���ܴ���Ϊ�յ����
    m_send_bio = BIO_new(BIO_s_mem());
    m_recv_bio = BIO_new(BIO_s_mem());
    SSL_set_bio(m_ssl, m_recv_bio, m_send_bio);
    SSL_set_accept_state(m_ssl);
}

void SSLSocketStream::uninit()
{

    SSL_shutdown(m_ssl);
    SSL_free(m_ssl);
    BIO_free(m_send_bio);
    BIO_free(m_recv_bio);
}
char * SSLSocketStream::get_OpenSSL_Error() {
    memset(m_szErrorMsg, 0, 1024);
    unsigned long ulErr = ERR_get_error();
    char *pTmp = NULL;
    // ��ʽ��error:errId:��:����:ԭ��
    pTmp = ERR_error_string(ulErr, m_szErrorMsg);
    return pTmp;
}


//����������Ҫ����ط�
int SSLSocketStream::write(void *buf,DWORD len) {
    int result = 0;
    int bytes = SSL_write(m_ssl, buf, len);
    if (bytes) {
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
int SSLSocketStream::read(void *buf,DWORD len) {

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