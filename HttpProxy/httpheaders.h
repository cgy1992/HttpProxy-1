#ifndef _HTTPHEADERS_H_
#define _HTTPHEADERS_H_

#include "header.h"
#include "list_entry.h"

typedef struct _list_ {
    LIST_ENTRY list_entry;
    char *key;
    char *val;
    int  key_len;   //key�ĳ���
    int  val_len;   //val�ĳ���
}HttpHeader_Attribute;

class  HttpHeaders {
public:
    HttpHeaders();
    ~HttpHeaders();
    enum {
        HTTP_REQUEST,
        HTTP_RESPONSE,
    };
public:
    
public:
    char *operator[](char *key);
    char *operator[](int index);
    char *search(char*key); //����key,����val
    int  search(int index, char **key, char** val);
    PLIST_ENTRY enumheaders(int index);
    HttpHeaders& operator=(const HttpHeaders hh);
    
    void insert(const char *key, const char *val);
    void insert(const char *key, size_t len_key, const char *val, size_t len_val);
    void del(const char *key);
    void del(int index);    //��������ɾ����Ϣ
    void release(); //�ͷ�
    int  parse_httpheaders(const char *headers, size_t len,int bHttpRequest);
    int  get_count() const { return m_count; }
    int  separat_httpattributes(const char *attr);
    char* getbuffer(size_t *len);   //���س���
    static char * get_status_code_descript(int statuscode);
    int  parse_request_line(char *rl, int len);
    int  parse_response_line(char *rl, int len);
    size_t  get_request_uri(char *buf, size_t len);   //��ȡ���������uri,����https����ʽ��һ������host����
    int  set_request_uri(char *buf, size_t len);      //����������uri,��Ҫ���������е�,��������host���޸ģ�����޸�host����
    size_t length();    //��ȡͷ�ĳ��ȣ�������content���ݲ���
private:
    int m_count;                //�����key_val�����鳤��
    LIST_ENTRY m_ListHeader;
public:
    static WORD HTTP_DEFAULT_PORT;
    static WORD HTTPS_DEFALUT_PORT;
public:
    DWORD m_response_status;    //http����״̬
    char m_version[10];         //http�汾
    char m_method[10];          //http����
    char *m_uri;                //�����uri
    char *m_host;               //����host��ַ
    //size_t m_length;            //length of httpheaders
    WORD m_port;
};
#endif
