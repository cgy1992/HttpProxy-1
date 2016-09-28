
#include <Windows.h>
//#include <winhttp.h>
#include <Wininet.h>
#include "CommonFuncs.h"
#include "httpheaders.h"



struct _status_code_ {
    int status_code;
    char *descript;
}response_status[] = {
    { 100,"Continue" },
    { 101,"Switching Protocols" },
    { 200,"OK" },
    { 201,"Created" },
    { 202,"Accepted" },
    { 203,"Non-Authoritative Information" },
    { 204,"No Content" },
    { 205,"Reset Content" },
    { 206,"Partial Content" },
    { 300,"Multiple Choices" },
    { 301,"Moved Permanently" },
    { 302,"Found" },
    { 303,"See Other" },
    { 304,"Not Modified" },
    { 305,"Use Proxy" },
    { 307,"Temporary Redirect" },
    { 400,"Bad Request" },
    { 401,"Unauthorized" },
    { 402,"Payment Required" },
    { 403,"Forbidden" },
    { 404,"Not Found" },
    { 405,"Method Not Allowed" },
    { 406,"Not Acceptable" },
    { 407,"Proxy Authentication Required" },
    { 408,"Request Time-out" },
    { 409,"Conflict" },
    { 410,"Gone" },
    { 411,"Length Required" },
    { 412,"Precondition Failed" },
    { 413,"Request Entity Too Large" },
    { 414,"Request-URI Too Large" },
    { 415,"Unsupported Media Type" },
    { 416,"Requested range not satisfiable" },
    { 417,"Expectation Failed" },
    { 500,"Internal Server Error" },
    { 501,"Not Implemented" },
    { 502,"Bad Gateway" },
    { 503,"Service Unavailable" },
    { 504,"Gateway Time-out" },
    { 505,"HTTP Version not supported" },
    { 0,NULL }
};

char * HttpHeaders::get_status_code_descript(int statuscode) {
    struct _status_code_ *pstatus_code = response_status;
    while (pstatus_code->status_code != 0 && pstatus_code->descript != NULL) {
        if (pstatus_code->status_code == statuscode) {
            return pstatus_code->descript;
        }
        pstatus_code++;
    }

    return NULL;
}



WORD HttpHeaders::HTTP_DEFAULT_PORT = 80;
WORD HttpHeaders::HTTPS_DEFALUT_PORT = 443;
HttpHeaders::HttpHeaders() {
    InitializeListHead(&m_ListHeader);  //��ʼ��LIST_ENTRY

    memset(m_version, 0, 10);
    memset(m_method, 0, 10);
    //m_length = 0;
    m_count = 0;
    m_uri = NULL;
    m_host = NULL;
}

HttpHeaders::~HttpHeaders() {
    release();
}

//ͨ��key��ֵ��ȡ����
/*
key ��http�������ֶ����ƣ�����Э�飬key�����ִ�Сд
*/
char * HttpHeaders::operator[](char *key) {
    PLIST_ENTRY plist;
    BOOL bFind = FALSE;
    struct _list_ *pelem = NULL;
    char *val = NULL;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (_stricmp(pelem->key, key) == 0) {//���
            val = pelem->val;
            break;
        }
    }
    return val;
}

char *HttpHeaders::operator[](int index) {
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    char *val = NULL;
    int i = 0;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (i == index) {
            val = pelem->val;
            break;
        }
        i++;
    }

    return val;
}


/*
����key��ֵ������valֵ
*/
char* HttpHeaders::search(char*key)
{
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    char *val = NULL;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (_stricmp(pelem->key, key) == 0) {//���
            val = pelem->val;
            break;
        }
    }

    return val;
}

/*
����:
index ��������
key   ����key��ֵ
val   ����val��ֵ
����ֵ:
����ҵ�ֵ������1 ���򷵻�0
*/
int HttpHeaders::search(int index, char **key, char** val)
{
    int ret = 0;
    if (key == NULL || val == NULL)
        return ret;
    *key = NULL;
    *val = NULL;

    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    int i = 0;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (i == index) {

            *key = pelem->key;
            *val = pelem->val;
            ret = 1;
            break;
        }
        i++;
    }
    
    return ret;
}

/*
���أ���ɹ��캯������ȫ����
*/
HttpHeaders & HttpHeaders::operator=(const HttpHeaders hh)
{
    return *this;
}
void HttpHeaders::insert(const char *key, const char *val) {
    struct _list_ *pKeyVal = NULL;
    size_t len_key = 0;
    size_t len_val = 0;

    //init
    len_key = strlen(key);
    len_val = strlen(val);

    pKeyVal = (struct _list_*)malloc(sizeof(struct _list_));
    memset(pKeyVal, 0, sizeof(struct _list_));

    pKeyVal->key = (char*)malloc(len_key + 1);
    memset(pKeyVal->key, 0, len_key + 1);
    pKeyVal->key_len = len_key;
    pKeyVal->val = (char*)malloc(len_val + 1);
    memset(pKeyVal->val, 0, len_val + 1);
    pKeyVal->val_len = len_val;

    strcpy_s(pKeyVal->key, len_key + 1, key);
    strcpy_s(pKeyVal->val, len_val + 1, val);

    //���뵽�б���
    InsertTailList(&m_ListHeader, (PLIST_ENTRY)&pKeyVal->list_entry);

    //����buf����
   // m_length += len_key;
   // m_length += len_val;
    //������һ
    m_count++;
}

void HttpHeaders::insert(const char *key, size_t len_key, const char *val, size_t len_val) {
    struct _list_ *pKeyVal = NULL;

    pKeyVal = (struct _list_*)malloc(sizeof(struct _list_));
    memset(pKeyVal, 0, sizeof(struct _list_));

    pKeyVal->key = (char*)malloc(len_key + 1);
    memset(pKeyVal->key, 0, len_key + 1);
    pKeyVal->key_len = len_key;
    pKeyVal->val = (char*)malloc(len_val + 1);
    memset(pKeyVal->val, 0, len_val + 1);
    pKeyVal->val_len = len_val;
    memcpy_s(pKeyVal->key, len_key + 1, key,len_key);
    memcpy_s(pKeyVal->val, len_val + 1, val,len_val);

    //���뵽�б���
    InsertTailList(&m_ListHeader, (PLIST_ENTRY)&pKeyVal->list_entry);

    //������һ
    m_count++;
}

/*
��httpͷ�е����ݰ���httpЭ��ĸ�ʽ���ػ�����
len ���ػ���������
����ֵ: ����ָ��
*/
char * HttpHeaders::getbuffer(size_t *len) {
    char *result = NULL;
    size_t pos = 0;
    //����ĳ��ȵĴ�С�����Եĸ����Ϳ��е�httpͷ�����Է����ڴ�
    //2*m_count =": \r\n"*m_count, 2="\r\n" 1=space
    size_t len_result = length() + 3 * m_count + 2 + 1;
    result = (char*)malloc(len_result);
    memset(result, 0, len_result);
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    char *val = NULL;
    int i = 0;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        //
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        memcpy_s(result + pos, len_result - pos, pelem->key, pelem->key_len);
        pos = pos + pelem->key_len;
        memcpy_s(result + pos, len_result - pos, ":", 1);
        pos = pos + 1;
        memcpy_s(result + pos, len_result - pos, pelem->val, pelem->val_len);
        pos = pos + pelem->val_len;

        memcpy_s(result + pos, len_result - pos, "\r\n", 2);
        pos = pos + 2;
    }

    //�������Ŀ���
    memcpy_s(result + pos, len_result - pos, "\r\n", 2);
    pos += 2;

    
    *len = pos;
    return result;
}

PLIST_ENTRY HttpHeaders::enumheaders(int index)
{
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    char *val = NULL;
    int i = 0;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (i == index) {
            break;
        }
        i++;
    }

    return plist;

}

int HttpHeaders::parse_httpheaders(const char *headers, size_t len,int bHttpRequest)
{
    int ret = 0;
    char * cheaders = NULL;
    char *pItem = NULL;
    char *next_token = NULL;
    char *next_token2 = NULL;
    char *d = "\r\n";
    char *d2 = " "; //�ո�ָ�HttpЭ������
    char *d3 = ":"; //����httpЭ������
    char *pInfo = NULL;
    char *endstr = NULL;


    if (len == 0 || headers == NULL)
        return 0L;

    cheaders = (char*)malloc(len + 1);
    memset(cheaders, 0, len + 1);
    memcpy_s(cheaders, len + 1, headers, len);

    pItem = strtok_s(cheaders, d, &next_token);
    switch (bHttpRequest) 
    {
    case HTTP_REQUEST:
    {
        pInfo = strtok_s(pItem, d2, &next_token2);
        if (pInfo == NULL)goto dail;
        memcpy_s(m_method, 10, pInfo,CommonFuncs::_min(strlen(pInfo),10));   //����Э��汾
        pInfo = strtok_s(NULL, d2, &next_token2);
        if (pInfo == NULL)goto dail;
        m_uri = (char*)malloc(strlen(pInfo)+1);
        memset(m_uri, 0, strlen(pInfo) + 1);
        strcpy_s(m_uri, strlen(pInfo)+1, pInfo);
        pInfo = strtok_s(NULL, d2, &next_token2);
        if (pInfo == NULL)goto dail;
        memcpy_s(m_version, 10, pInfo, CommonFuncs::_min(strlen(pInfo), 10));
    }
    break;
    case HTTP_RESPONSE:
    {
        //��ȡ״̬
        pInfo = strtok_s(pItem, d2, &next_token2);
        if (pInfo == NULL)goto dail;
        memcpy_s(m_version, 10, pInfo, CommonFuncs::_min(strlen(pInfo), 10));
        pInfo = strtok_s(NULL, d2, &next_token2);
        if (pInfo == NULL)goto dail;
        m_response_status = strtol(pInfo, &endstr, 10);
    }
    break;
    }

    //
    pItem = strtok_s(NULL, d, &next_token);
    while (pItem)
    {
        //��ȡ����Ϣ
        char *pTemp = (char*)(pItem + separat_httpattributes(pItem));
        if (pTemp == "") {
            pItem = strtok_s(NULL, d, &next_token);
            continue;
        }
        *(pTemp - 1) = '\0';

        
        insert(pItem, pTemp);

        pItem = strtok_s(NULL, d, &next_token);
        if (cheaders+len <= next_token)
            break;
    }

    /*��ʱ�������1.1Э������ݣ�����ͨ��host���Եõ��˿ں�*/
    char*pHost = (*this)["HOST"];
    m_host = NULL;
    size_t len_host = 0;
    if (pHost != NULL) {

        len_host = strlen(pHost) + 1;
        m_host = (char*)malloc(len_host);
        memset(m_host, 0, len_host);

        char*pPort = strstr(pHost, ":");
        if (pPort != NULL) {

            char *endptr = NULL;
            m_port = (WORD)strtol(pPort + 1, &endptr, 10);
            memcpy_s(m_host, len_host, pHost, pPort - pHost);
            
        }
        else {
            m_port = HttpHeaders::HTTP_DEFAULT_PORT;
            strcpy_s(m_host, len_host, pHost);
        }

        CommonFuncs::trim(m_host, strlen(m_host));
    }


dail:
    if (cheaders != NULL)
        free(cheaders);

    return 1L;
}

/*
����request the first line
*/
//request the first line format:Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
int HttpHeaders::parse_request_line(char *rl, int len)
{
    int ret = -1;
    int pos[3] = { 0 }; //��¼��ʼλ��
    int i=0,j = 0;
    if (rl == NULL) return -1;  //�������ֱ�ӷ���

    char *pTemp = NULL;
    pTemp = rl;


    if (isalpha(*pTemp)) return -1; //����

    //Method
    for (i = 0; i < len-1; i++) {
        if (pTemp[i] !=0x20 && isspace(pTemp[i+1])) {
            pos[j] = i;
            j++;
        }
    }
    
    return ret;
}

/*
����response the first line
*/
int HttpHeaders::parse_response_line(char *rl, int len)
{
    int ret = -1;


    return ret;
}

/*
�õ������url
*/
size_t HttpHeaders::get_request_uri(char *buf, size_t len)
{
    char *prequest_uri = NULL;
    size_t result = 0;
    char *phost = (*this)["Host"];
    if (phost != NULL) {
        if (_strnicmp(m_uri, "http",4) == 0) {
            //��������еõ��ķ����а���http/http(s)���ֶΣ�˵��uri�а���������ַ�����ʱ�򣬾ͷ���m_uri����Ϳ��������
            prequest_uri = m_uri;
            result = strlen(m_uri);
        }

        if (_strnicmp(m_uri, "/", 1) == 0) {
            size_t len_host = strlen(m_host);
            size_t len_uri = strlen(m_uri);
            

            prequest_uri = (char*)malloc(len_host + len_uri);
            memset(prequest_uri, 0, len_host + len_uri);
            memcpy_s(prequest_uri, len_host + len_uri, m_host, len_host);
            memcpy_s(prequest_uri + len_host, len_uri, m_uri, len_uri);

            result = len_host + len_uri;
            if (buf == NULL || len == 0) {
                free(prequest_uri);
                prequest_uri = NULL;
            }
        }
    }

    if(buf!=NULL && len>0){

        size_t dd = result > len ? len : result;
        memcpy_s(buf, len, prequest_uri, (result > len ? len : result));
    }
    return result;
}
/*��ȷ���ط�0*/
/*
�����httpheaders��Ҫ���е���
���Է�����޸����Ե�����
*/
int HttpHeaders::set_request_uri(char *buf, size_t len)
{
    int ret = 0;
    
    if(buf == NULL || len == 0 ) return 0;

    do 
    {
        //�ж�ԭ�е�m_uri�Ƿ���http��https����ʽ
        if (_strnicmp(m_uri, "http",4) == 0) {
                //�ͷ�ԭ�е�m_uri
            free( m_uri );
            m_uri = NULL;

            m_uri = (char*) malloc(len + 1);
            memset( m_uri, 0, len + 1);

            memcpy_s(m_uri, len, buf, len);

            ret = 1;
            break;
        }

        if( _strnicmp(buf, m_host, strlen(m_host)) == 0){
            
            char *pUri =(char*)(buf+ strlen(m_host));
            
            free(m_uri);
            m_uri = NULL;

            m_uri = (char*) malloc(len - strlen(m_host) + 1);
            memset( m_uri, 0, len - strlen(m_host) + 1);

            memcpy_s(m_uri, len - strlen(m_host), pUri, len - strlen(m_host));
            ret = 1;
            break;
        }

    } while (0);


    return ret;
}

int HttpHeaders::separat_httpattributes(const char *attr)
{
    const char *pPos = attr;
    while (*pPos != '\0') if (*pPos++ == ':')break;
    return (int)(pPos - attr);
}

void HttpHeaders::del(const char *key) {
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    BOOL bFind = FALSE;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (_stricmp(pelem->key, key) == 0) {//���
            
            //�����ڴ�
            if (pelem->key != NULL) free(pelem->key);
            if (pelem->val != NULL) free(pelem->val);
            

            //������һ
            m_count--;

            bFind = TRUE;
            break;
        }
    }
    
    if (bFind) {
        RemoveElement(plist);
        free(plist);
    }

}

void HttpHeaders::del(int index)
{
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    int i = 0;
    BOOL bFind = FALSE;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        //
        if (i==index) {//���
            
            //�����ڴ�
            if (pelem->key != NULL) free(pelem->key);
            if (pelem->val != NULL) free(pelem->val);


            //������һ
            m_count--;

            bFind = TRUE;
            break;
        }
        i++;
    }

    if (bFind) {
        RemoveElement(plist);
        free(plist);
    }

}

//�ͷ����е��ڴ�
void  HttpHeaders::release() {
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    int i = 0;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        if (pelem->key != NULL) free(pelem->key);
        if (pelem->val != NULL) free(pelem->val);
    }

    for (i = 0; i < m_count; i++)
    {
        PLIST_ENTRY pList=RemoveHeadList(&m_ListHeader);
        free(pList);
    }

    m_count = 0;

    memset(m_method, 0, 10);
    memset(m_version, 0, 10);

    //���m_uri
    if (m_uri != NULL) {
        free(m_uri);
        m_uri = NULL;
    }

    if (m_host != NULL) {
        free(m_host);
        m_host = NULL;
    }
}

size_t HttpHeaders::length()
{
    size_t result = 0;
    PLIST_ENTRY plist;
    struct _list_ *pelem = NULL;
    for (plist = m_ListHeader.Flink; plist != &m_ListHeader; plist = plist->Flink)
    {
        pelem = CONTAINING_RECORD(plist, struct _list_, list_entry);
        result += pelem->key_len;
        result += pelem->val_len;
    }

    return result;
}