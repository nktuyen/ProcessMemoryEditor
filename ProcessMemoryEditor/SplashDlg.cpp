// SplashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "SplashDlg.h"
#include "afxdialogex.h"


// CSplashDlg dialog

IMPLEMENT_DYNAMIC(CSplashDlg, CDialog)

CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SPLASH, pParent)
{

}

CSplashDlg::~CSplashDlg()
{
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STT_IMAGE_CONTAINER, m_sttImage);
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CSplashDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
	CRect rc;
	m_sttImage.GetWindowRect(rc);
	MoveWindow(rc);
#ifdef _DEBUG
	m_byAlpha = 255;
#else
    m_byAlpha = 0;
#endif
	m_nTimerId = SetTimer(1988, 100, nullptr);
    return TRUE;
}

BOOL CSplashDlg::PreTranslateMessage(MSG* pMsg)
{
    if (WM_KEYDOWN == pMsg->message) {
        if ((VK_ESCAPE == pMsg->wParam) || (VK_RETURN == pMsg->wParam)) {
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CSplashDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1988) {
        if (m_byAlpha < 255) {
            m_byAlpha += 5;

            SetLayeredWindowAttributes(0, m_byAlpha, LWA_ALPHA);
        }
        else {
			Sleep(1000);
            KillTimer(m_nTimerId);
            SendMessage(WM_CLOSE);
        }
    }
}