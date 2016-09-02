#ifndef _BASEHTTPREQUESTHANDLER_H_
#define _BASEHTTPREQUESTHANDLER_H_

#include "header.h"
#include "httpheaders.h"
#include "httpcontent.h"
#include "httprequest.h"
#include "httpsession.h"    
#include "BaseSocketStream.h"
#include "BaseDataHandler.h"

class BaseHTTPRequestHandler:public BaseDataHandler {
public:
    BaseHTTPRequestHandler(HTTPSERVICE_PARAMS *pHttpService_Params, HttpSession * pHttpSession);
    ~BaseHTTPRequestHandler();
public:
    virtual void do_OPTIONS();
    virtual void do_GET();
    virtual void do_HEAD();
    virtual void do_POST();
    virtual void do_PUT();
    virtual void do_DELETE();
    virtual void do_TRACE();
    virtual void do_CONNECT();
private:
    //
    virtual void connect_intercept();   //�жϺ���
    virtual void connect_relay();       //ֱ��ת��
public:
    virtual void handler_request(void *buf, DWORD len, BaseDataHandler_RET * ret);
private:
    size_t find_httpheader(const char* buf, size_t bufsize);
private:
    void invokeMethod(const char *methdo);
    void invokeRequestCallback(HttpHeaders *http_headers);
    void invokeResponseCallback(char *buf,size_t len);
    void reset();   //���ã��ڲ���ȫ�Ľ��յ��������ݣ�����Ҫ����

private:
    //�ض����ܺ���
    void headerfilterforAgent(HttpHeaders*pHttpHeaders);  //���ݴ������������Ҫ����ָ����ͷ

private:
    HttpHeaders             http_items;
    HttpContent             httpcontent;
    HttpRequest             httprequest;
    HTTPSERVICE_PARAMS*     m_pHttpService_Params;
    BaseSocketStream *      m_pBaseSockeStream;
    WORD                    m_port;        //����˿ں�
    char                    m_uri[1024];   //����������ַ
public:
    char *                  m_precv_buf;      //�������ݵĻ�������Ҫ������
    DWORD                   m_len_recvbuf;    //�����õ��Ļ���������
    HttpSession *           m_pHttpSession;
};

#endif
