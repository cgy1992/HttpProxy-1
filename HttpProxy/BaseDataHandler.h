#pragma once

#include <Windows.h>


//BaseHTTPRequestHandler�������֮���ͨѶ����
typedef struct _ret_ {
    DWORD dwOpt;    //��������:�����������ݣ�ֹͣ�������ݣ��жϣ���������
}BaseDataHandler_RET;


class BaseDataHandler {
public:
    BaseDataHandler();
    virtual ~BaseDataHandler();
    enum {
        RET_FINISH = 1,
        RET_SEND = 2,
        RET_RECV = 3,
        RET_ERROR = 4,
        RET_UNKNOWN = 0xFFFFFFFF,
    };
public:
    virtual void handler_request(void *buf, DWORD len, BaseDataHandler_RET * ret)=0;
};