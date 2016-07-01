#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "header.h"
#include "AcceptConnection.h"



//��������acceptConnection
typedef struct _list_acceptconnection_
{
    LIST_ENTRY list_entry;
    AcceptConnection* pAcceptConnection;
}LIST_ACCEPTCONNECTION, *PLIST_ACCEPTCONNECTION;

#define CONTROL_EVENT "Global\\{385EF596-7239-421F-8A68-981052921C7E}"  //���Ƴ�����ͣ������ִ������
class BaseServer {
public:
    BaseServer(HTTPSERVICE_PARAMS *pHttpService_Params);
    ~BaseServer();  //��������
    enum {
        BHSR_SUCCESS = 0x0000000,
        BHSR_ERROR_PORT,
        BHSR_ERROR_IP,
        BHSR_ERROR_INITLISTENSOCKET,
        BHSR_ERROR_START,                   //��������ʧ��
        BHSR_ERROR_INITSSL,                 //��ʼ��SSLʧ��
        BHSR_ERROR_GENERATE_CONTROLEVENT,   //����״̬�����¼�����
        BHSR_ERROR_GEN_SOCKET,              //�����׽��ֳ���
        BHSR_ERROR_GEN_CONNECT,             //���ӳ���
        BHSR_ERROR_SOCKET_OPT,              //opt���ô���
        BHSR_ERROR_CREATE_SEVICE,           //��������ʧ��
        BHSR_ERROR_CLOSEWORKTHREAD,         //�ر�IO�����߳�ʧ��
    };

    enum {
        STATUS_GET      =0x00000000,
        STATUS_RUN      =0x00000001,
        STATUS_PAUSE    =0x00000002,
        STATUS_CONTINUE =0x00000004,
        STATUS_STOP     =0x00000008,
    };
public:
    //operation
    ULONG               status(ULONG bPause, ULONG *dwRet);  //����״̬����
private:
    ULONG               http_server();
    static DWORD WINAPI work_proc(LPVOID lparam);
    void                server_loop(LPVOID lparam, size_t len);     //
    void                counter(BOOL bDec);                         //����
    static DWORD WINAPI ServerThread(LPVOID lparam);                //���������߳�
    ULONG               handle_status();
    static DWORD WINAPI StopServerThread(LPVOID lparam);
    void                stopserver();
    void                Release_AcceptConnectionList();
    void                DeleteElem_AcceptConnectionList(AcceptConnection *addr);
    ULONG               init_httpserver();                          //��ʼ������
    void                uninit_httpserver();                //����ʼ��
    ULONG               server_forever(int bNewThread);     //��������
    ULONG               stop();                             //�رշ���

private:
    sockaddr_in         m_httpserver_address;   //��Ҫ�����ķ�������ַ
    HTTPSERVICE_PARAMS  m_HttpService_Params;    //����Ĳ���
    UINT_PTR            m_listensocket;
    int                 m_ThreadCount;
    HANDLE*             m_pThreadArray;
    HANDLE              m_hServerThread;    //ָ������ķ����߳̾��
    HANDLE              m_hCompletionPort;
    HANDLE              m_hMainThread;      //������߳�
    DWORD               m_counter;  //������
    ULONG               m_status;
    LIST_ENTRY          m_AcceptConnectList; //�����ڴ��list��״̬
#ifdef _DEBUG
    CRITICAL_SECTION    m_lock_counter;
#endif
private:
    //���º�����Ŀ����ȷ��BaseHttpServer��Ϊ�����������Ķ�����
    int                 init_httpserver_ex();
    void                uninit_httpserver_ex();
};