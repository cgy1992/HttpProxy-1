#include "httpsession.h"


HttpSession::HttpSession()
{
    m_pSendbuf = NULL;
    m_SizeofSendbuf = 0;
    m_resultstate = HS_RESULT_OK;
    m_bKeepAlive = TRUE;
}


HttpSession::~HttpSession()
{
    if (m_pSendbuf != NULL) {
        free(m_pSendbuf);
        m_pSendbuf = NULL;
    }
    m_SizeofSendbuf = 0;
    m_resultstate = HS_RESULT_OK;
}



//�ͷ�HttpSession����ԭ������״̬
void HttpSession::revert()
{
    if (m_pSendbuf != NULL) {
        free(m_pSendbuf);
        m_pSendbuf = NULL;
    }

    m_SizeofSendbuf = 0;
    m_resultstate = HS_RESULT_OK;
}