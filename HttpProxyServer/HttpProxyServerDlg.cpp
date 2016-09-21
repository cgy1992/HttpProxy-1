
// HttpProxyServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CommonFuncs.h"
#include "HttpProxyServer.h"
#include "HttpProxyServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CHttpProxyServerDlg �Ի���


CHttpProxyServerDlg * theHPSDialog = NULL;

CHttpProxyServerDlg::CHttpProxyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHttpProxyServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    memset(&m_httpservice_params, 0, sizeof(HTTPSERVICE_PARAMS));
    m_state = 0;
}

void CHttpProxyServerDlg::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX,IDC_LIST1,m_ctrl_requestlistbox);

	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHttpProxyServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BT_CONFIG,OnConfig)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &CHttpProxyServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CHttpProxyServerDlg ��Ϣ�������

BOOL CHttpProxyServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ControlOpt();

    theHPSDialog = this;

    m_ctrl_requestlistbox.ResetContent();

    this->SetDlgItemInt(IDC_ET_PORT,8889);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CHttpProxyServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHttpProxyServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHttpProxyServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHttpProxyServerDlg::ControlOpt()
{

    switch (m_state) {
    case 0:
        {
            this->GetDlgItem(IDC_BT_CONFIG)->EnableWindow(TRUE);
            this->GetDlgItem(IDOK)->EnableWindow(FALSE);
            this->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
        }
        break;
    case STATE_STOP:
        {
            this->GetDlgItem(IDC_BT_CONFIG)->EnableWindow(FALSE);
            this->GetDlgItem(IDOK)->EnableWindow(TRUE);
            this->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
        }
        break;
    case STATE_START:
        {
            this->GetDlgItem(IDC_BT_CONFIG)->EnableWindow(FALSE);
            this->GetDlgItem(IDOK)->EnableWindow(FALSE);
            this->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
        }
        break;
    }
}

BOOL g_bFind360COM = FALSE;

void __stdcall Request_Callback(PCALLBACK_DATA pcallback_data)
{
    theHPSDialog->insert_reqlist(pcallback_data->buf,pcallback_data->len);
    
    char * pFind = StrStrIA(pcallback_data->buf,"www.360.com");
    if(pFind != NULL)
    {
        g_bFind360COM = TRUE;
    }
}


void __stdcall Response_Callback(PCALLBACK_DATA pcallback_data)
{
    char * relocation = "HTTP/1.1 301 Moved Permanently\r\n"
                        "Location: http://www.qq.com/\r\n\r\n";

    if( g_bFind360COM ){



        if(pcallback_data->buf != NULL){

            free(pcallback_data->buf);
            pcallback_data->buf = NULL;
        }

        pcallback_data->buf = (char*) malloc(strlen(relocation));
        memset(pcallback_data->buf, 0, strlen(relocation));

        memcpy_s(pcallback_data->buf, strlen(relocation), relocation, strlen(relocation));
        pcallback_data->len = strlen(relocation);    
    
        g_bFind360COM = FALSE;
    }


}

//����
void CHttpProxyServerDlg::OnConfig()
{
    strcpy_s(m_httpservice_params.ip, 16, "127.0.0.1");
    m_httpservice_params.port = (WORD)this->GetDlgItemInt(IDC_ET_PORT);
    m_httpservice_params.request_callback = Request_Callback;
    m_httpservice_params.response_callback = Response_Callback;
    m_httpservice_params.bSSH = FALSE;

    m_hProxyHttpService = Create_ProxyHttpService(&m_httpservice_params);
    if (m_hProxyHttpService != NULL) {
        m_hSSLConfigHandle = Hijack_Https(m_hProxyHttpService);
        if (m_hSSLConfigHandle != NULL) {
            TrustRootCert(m_hSSLConfigHandle);
            m_state = STATE_STOP;
        }

    }

    ControlOpt();
}


//����
void CHttpProxyServerDlg::OnOK()
{
    BOOL bRet = TRUE;
    bRet = Start_ProxyHttpService(m_hProxyHttpService);
    if (bRet)
        m_state = STATE_START;

    ControlOpt();
}


//ֹͣ
void CHttpProxyServerDlg::OnCancel()
{
    BOOL bRet = TRUE;
    bRet = Stop_ProxyHttpService(m_hProxyHttpService);
    if (bRet)
        m_state = STATE_STOP;

    ControlOpt();
}

//�رնԻ���
void CHttpProxyServerDlg::OnClose()
{
    this->EndDialog(0);
}



//����
void CHttpProxyServerDlg::OnBnClickedButton1()
{
#ifdef _DEBUG
    Unittest();
#endif
}



void CHttpProxyServerDlg::insert_reqlist(const char* szurl, int len)
{
    wchar_t *pws = NULL;
    CommonFuncs::a2w(szurl,&pws);
    m_ctrl_requestlistbox.AddString((LPCTSTR)pws);
    
    if( pws != NULL){
        free(pws);
    }
}