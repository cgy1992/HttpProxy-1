#ifndef _HEADER_H_
#define _HEADER_H_


/*#ifdef HTTPCORE_EXPORTS
#   define HTTPCORE_API __declspec(dllexport)
#   define EXPIMP_TEMPLATE
#else
#   define HTTPCORE_API __declspec(dllimport)
#   define EXPIMP_TEMPLATE extern
#endif*/

#ifdef _DLL_EXPORT
#   define HTTPCORE_API __declspec(dllexport)
#elif _DLL_IMPORT
#   define HTTPCORE_API __declspec(dllimport)
#elif _STATIC
#   define HTTPCORE_API
#endif


//custom http tag
#define CHT_HTTPREQUESTURI  "HttpRequestUri"


//CALLBACK_DATA_TYPE
#define CDT_REQUEST 0x00000001
#define CDT_RESPONSE 0x00000002


typedef struct _CALLBACK_DATA_ {
    ULONG   cdt; //�ص�����(��ȱ)
    char *  buf;
    DWORD   len;
}CALLBACK_DATA,*PCALLBACK_DATA;

#define SCT_NORMAL      0x00000000  //��ִ���κβ���
#define SCT_PAUSE       0x00000001  //��ͣ����
#define SCT_CONTINUE    0x00000002  //����

typedef struct _SERVICE_CONTROL_ {
    ULONG ct;   //��������
    char *buf;  //��������
    DWORD len;  //���ݳ���
}SERVICE_CONTROL,*PSERVICE_CONTROL;

//����״̬����
#define SERVER_STATUS_RUN       0x00000001
#define SERVER_STATUS_PAUSE     0x00000002
#define SERVER_STATUS_CONTINUE  0x00000004
#define SERVER_STATUS_STOP      0x00000008


//�ص������Ĺ��̲�Ӧ���ں�ʱ����ʱ̫�������������������ʧ�ܣ����ش������Ϣ
// *ע��* �ص���������ݲ���Ҫ�û��ͷ�
typedef void( __stdcall *_Request_Callback_)(PCALLBACK_DATA);                 //���յ�����������ͷ�����
typedef void( __stdcall *_Response_Callback_)(PCALLBACK_DATA);                //���յ����������Ӧ��ִ�е���

//״̬�ص�
typedef void(__stdcall *_ServerStatus_Callback_)(/*OUT*/ULONG status);        //���������״̬�󷵻�

//������ƻص�(��ʼ) ���ظ����������������
//���صķ����������������ڴ棬�ɽ����ص������ͷ�
typedef PSERVICE_CONTROL (__stdcall *_ServiceControl_Callback_Begin_)();  
//������ƻص�(����) 
//����pservice_control���Ƿ�����ƻص�(��ʼ)�з��ص�PSERVICE_CONTROL�ṹ��ַ
typedef void(__stdcall * _ServiceControl_Callback_End_)(PSERVICE_CONTROL pservice_control);



typedef struct _HTTPSERVICE_PARAMS_ {
    //����������
    char ip[16];
    WORD port;
    BOOL bSSH;
    DWORD numofworkthread;  //�����������߳�����Ĭ��Ϊ��������������
    _Request_Callback_  request_callback;
    _Response_Callback_ response_callback;
    _ServerStatus_Callback_ serverstatus_callback; //��ʱû��ʹ��
}HTTPSERVICE_PARAMS, *PHTTPSERVICE_PARAMS;


#endif
