#pragma once
#include "afxwin.h"


// CSplashDlg dialog

class CSplashDlg : public CDialog
{
	DECLARE_DYNAMIC(CSplashDlg)
    DECLARE_MESSAGE_MAP()
public:
	CSplashDlg(CWnd* pParent = NULL);
	virtual ~CSplashDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPLASH };
#endif
private:
	virtual void DoDataExchange(CDataExchange* pDX); 
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
    CStatic m_sttImage;
    UINT_PTR m_nTimerId;
    BYTE m_byAlpha;
};
