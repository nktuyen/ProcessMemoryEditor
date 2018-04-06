#pragma once
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"

class CProcessInfo;
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
    afx_msg void OnBnClickedBtnRefresh();
    afx_msg void OnLvnGetdispinfoLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnColumnclickLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnSearchProcName();
    afx_msg void OnEnChangeEdtSearchProcName();
private:
    void DoDataExchange(CDataExchange* pDX);
    BOOL SpawnProcesses();
    void DeleteAllProcesses();
    BOOL SortItems();
public:
    inline DWORD ProcessId() { return m_dwProcessId; }
private:
    CListCtrl m_lvwProcesses;
    DWORD m_dwProcessId;
    CImageList m_imgList;
    std::vector<CProcessInfo*> m_lstProcesses;
    std::vector<CProcessInfo*> m_lstFilteredProcesses;
    CButton m_btnSearch;    
    CEdit m_edtProcName;
    int m_nSortColumn;
    static const UINT m_nColumns = 10;
    CMutex m_mutex;
    BOOL m_bSortDirection[m_nColumns];
};
