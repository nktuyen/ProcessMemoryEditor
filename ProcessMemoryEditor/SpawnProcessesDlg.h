#pragma once
#include "afxcmn.h"

class CSpawnProcessesDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpawnProcessesDlg)
    DECLARE_MESSAGE_MAP()
public:
	CSpawnProcessesDlg(CWnd* pParent = NULL);
	virtual ~CSpawnProcessesDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPAWN_PROCESS };
#endif
private:
    BOOL OnInitDialog();
private:
    afx_msg void OnDestroy();
    afx_msg void OnLvnItemchangedLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult);
private:
    void DoDataExchange(CDataExchange* pDX);
    BOOL SpawnProcesses();
public:
    inline DWORD ProcessId() { return m_dwProcessId; }
private:
    CListCtrl m_lvwProcesses;
    DWORD m_dwProcessId;
};
