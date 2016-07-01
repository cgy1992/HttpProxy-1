#ifndef _HTTPSESSION_H_
#define _HTTPSESSION_H_

#include <windows.h>

class HttpSession {
public:
    HttpSession();
    ~HttpSession();

    enum {
        HS_RESULT_OK,                   //��������
        HS_RESULT_DATAEMPTY,            //���ݽ��Ϊ��
        HS_RESULT_SERVER_NOEXIST,       //������������
    };

public:
    void revert();
public:
    char*   m_pSendbuf;
    DWORD   m_SizeofSendbuf;
    DWORD   m_resultstate;    //���ݽ��״̬
    BOOL    m_bKeepAlive;     //�Ƿ񱣳ֻ״̬

};

#endif
