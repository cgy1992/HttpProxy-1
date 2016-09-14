
// HttpProxyServerDlg.h : ͷ�ļ�
//

#pragma once
#include <header.h>
#include <httpcore.h>

// CHttpProxyServerDlg �Ի���
class CHttpProxyServerDlg : public CDialog
{
// ����
public:
	CHttpProxyServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

    enum {
        STATE_START=1,
        STATE_STOP,
    };
// �Ի�������
	enum { IDD = IDD_HTTPPROXYSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
    void ControlOpt();
    void OnOK();
    void OnCancel();
// ʵ��
protected:
	HICON m_hIcon;
    HTTPSERVICE_PARAMS m_httpservice_params;
    PHS_HANDLE  m_hProxyHttpService;
    SCG_HANDLE  m_hSSLConfigHandle;
    DWORD m_state;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnClose();
    afx_msg void OnConfig();
    afx_msg void OnBnClickedButton1();
	DECLARE_MESSAGE_MAP()

public:
    void insert_reqlist(const char* szurl, int len);
private:
    CListBox m_ctrl_requestlistbox;
    
};
