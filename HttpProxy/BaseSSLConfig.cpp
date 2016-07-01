
#include "CertificateProvider.h"
#include "BaseSSLConfig.h"

//��ʼ��SSL
typedef CRITICAL_SECTION	ssl_lock;
struct CRYPTO_dynlock_value {
    ssl_lock lock;
};
int number_of_locks = 0;
ssl_lock *ssl_locks = NULL;


void ssl_lock_callback(int mode, int n, const char *file, int line)
{
    if (mode & CRYPTO_LOCK)
        EnterCriticalSection(&ssl_locks[n]);
    else
        LeaveCriticalSection(&ssl_locks[n]);
}

CRYPTO_dynlock_value* ssl_lock_dyn_create_callback(const char *file, int line)
{
    CRYPTO_dynlock_value *l = (CRYPTO_dynlock_value*)malloc(sizeof(CRYPTO_dynlock_value));
    InitializeCriticalSection(&l->lock);
    return l;
}

void ssl_lock_dyn_callback(int mode, CRYPTO_dynlock_value* l, const char *file, int line)
{
    if (mode & CRYPTO_LOCK)
        EnterCriticalSection(&l->lock);
    else
        LeaveCriticalSection(&l->lock);
}

void ssl_lock_dyn_destroy_callback(CRYPTO_dynlock_value* l, const char *file, int line)
{
    DeleteCriticalSection(&l->lock);
    free(l);
}


BaseSSLConfig* BaseSSLConfig::instance = NULL;  /*��ʼ��ΪNULL*/

BaseSSLConfig::BaseSSLConfig()
{
    m_rootcert = NULL;
    m_rootkeypair = NULL;
    m_status = STATUS_UNINIT;
}

BaseSSLConfig::~BaseSSLConfig()
{

}


BaseSSLConfig* BaseSSLConfig::CreateInstance()
{
    if (instance == NULL) {
        instance = new BaseSSLConfig();
    }

    return instance;
}

BOOL BaseSSLConfig::CreateRootCert()
{
    m_rootkeypair = CertificateProvider::Generate_KeyPair(NUMOfKEYBITS);
    if (m_rootkeypair == NULL)
        return FALSE;

    m_rootcert = CertificateProvider::CreateCertificate(m_rootkeypair, TRUE);
    if (m_rootcert == NULL)/*����ط����Ǵ������⣬Ӧ����Ϊ�յ�������ͷ�m_rootkeypair*/
        return FALSE;

    return TRUE;
}

BOOL BaseSSLConfig::TrustRootCert()
{
    if (m_status == BaseSSLConfig::STATUS_INITFINAL) {
        
        return CertificateProvider::addCert2WindowsAuth(m_rootcert, "ROOT");
    }

    return FALSE;
}

/*
����������
*/
SSL_CTX * BaseSSLConfig::context()
{
    return m_ctx;
}

BOOL BaseSSLConfig::init_ssl()
{
    BOOL bRet = FALSE;
    do {

        number_of_locks = CRYPTO_num_locks();
        if (number_of_locks > 0)
        {
            ssl_locks = (ssl_lock*)malloc(number_of_locks * sizeof(ssl_lock));
            for (int n = 0; n < number_of_locks; ++n)
                InitializeCriticalSection(&ssl_locks[n]);
        }

        CRYPTO_set_locking_callback(&ssl_lock_callback);
        CRYPTO_set_dynlock_create_callback(&ssl_lock_dyn_create_callback);
        CRYPTO_set_dynlock_lock_callback(&ssl_lock_dyn_callback);
        CRYPTO_set_dynlock_destroy_callback(&ssl_lock_dyn_destroy_callback);
        //init
        SSL_load_error_strings();
        SSL_library_init();


        /*ע��*/
        //����ط������ƶ���SSLSocketStream��ȥ���Ϳ������֣�
        //��ʱ���ƶ��ˣ���Ϊ��û�������ص�֤��ǩ���Ĺ���
        SSL_METHOD *method;
        method = (SSL_METHOD*)SSLv23_method();
        m_ctx = SSL_CTX_new(method);
        if (m_ctx != NULL)
        {
            SSL_CTX_set_verify(m_ctx, SSL_VERIFY_NONE, NULL);
            SSL_CTX_set_mode(m_ctx, SSL_MODE_AUTO_RETRY);

            if (CreateRootCert()) {

                if (SSL_CTX_use_PrivateKey(m_ctx, m_rootkeypair) <= 0)
                {
                    break;
                }
                if (SSL_CTX_use_certificate(m_ctx, m_rootcert) <= 0)
                {
                    break;
                }

                if (!SSL_CTX_check_private_key(m_ctx))
                {
                    break;
                }
            }
        }
        else break;/*����ط�Ӧ���ͷŲ���ȷ�����*/

        bRet = TRUE;
    } while (0);

    if (bRet) {
        m_status = STATUS_INITFINAL;
    }
    return bRet;
}

void BaseSSLConfig::uninit_ssl()
{
    SSL_CTX_free(m_ctx);
    m_ctx = NULL;

    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);

    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_remove_state(0);
    ERR_free_strings();

    if (NULL != ssl_locks)
    {
        for (int n = 0; n < number_of_locks; ++n)
            DeleteCriticalSection(&ssl_locks[n]);

        free(ssl_locks);
        ssl_locks = NULL;
        number_of_locks = 0;
    }

    m_status = STATUS_UNINIT;
}

BOOL BaseSSLConfig::ExportRootCert(unsigned char *buf,int *len)
{
    BOOL bRet=FALSE;
    int ret=0;

    if(len==NULL || buf==NULL)return FALSE;

    ret=CertificateProvider::exportx509(m_rootcert,buf,*len);
    if(ret>0)
        *len=ret;

    return (BOOL)ret;
}