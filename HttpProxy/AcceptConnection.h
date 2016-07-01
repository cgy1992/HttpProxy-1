#ifndef _ACCEPTCONNECTION_H_
#define _ACCEPTCONNECTION_H_


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include "httpsession.h"
#include "header.h"
#include "BaseDataHandler.h"

class AcceptConnection;
typedef struct _session_overlapped_ {
    WSAOVERLAPPED overlapped;
    AcceptConnection *pac;
}session_overlapped;

class AcceptConnection {
public:
    session_overlapped  m_overlapped[2];
    DWORD               m_flags[2];
    DWORD               m_bytes_transferred[2];
    HANDLE              m_hCompletionPort;
    DWORD               m_commstate;       //ͨѶ״̬
    CRITICAL_SECTION    m_lock;            //ͨѶͬ����


    SOCKET      m_accept;               //���ӵ�socket
    sockaddr_in m_remoteaddr;           //���ܵ�Զ�̵�ַ
    WSABUF      m_wsabuf[2];            //�ͻ��˵�ַ��Ϣ
    CHAR        m_recvbuf[4096];        //�������ݻ�����
    DWORD       m_bytessend;            //�Ѿ����͵����ݳ���
    HttpSession m_httpsession;          //HttpSession��ᴩ�����Ĵ�����̣�����Ҫ�Խ����״̬��������session��
    HTTPSERVICE_PARAMS  m_httpservice_params;   //����http����ı���
    enum {
        RECV,
        SEND,
    };

    enum {
        RECVING,
        SENDING,
        CLOSING, 
    };
public:
    AcceptConnection(HANDLE hCompletionPort, SOCKET acp, sockaddr_in remoteaddr, HTTPSERVICE_PARAMS *pHttpService_Params);
    ~AcceptConnection();
    void init();
public:
    /*
    ������Ϣ
    ����ֵ:���󷵻�-1���ɹ����� >0��ֵ(������ɵ��ֽ���)
    */
    int HandleIoCompletion(DWORD numberbytes,session_overlapped  *poverlapped);
private:
    void session_recv();    //��������
    void session_send();    //��������
    void session_close();   //�ر�

    void _entry_();
    void _leave_();

public:
    BaseDataHandler *m_handler;

public:
    //���ݴ������ĳ�ʼ������
    void Init_DataHandlerObj(); //�����أ��û��л����ݴ�����
};

typedef AcceptConnection* PACCEPTCONNECTION;

#endif