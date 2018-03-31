
// ProcessMemoryEditorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SearchThread.h"

// CProcessMemoryEditorDlg dialog
class CProcessMemoryEditorDlg : public CDialogEx
{
    DECLARE_MESSAGE_MAP()
public:
	CProcessMemoryEditorDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~CProcessMemoryEditorDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUHACKER_DIALOG };
#endif
private:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
private:
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnBnClickedBtnSpawnProcesses();
    afx_msg void OnBnClickedBtnBrowseProcess();
    afx_msg void OnBnClickedRadProcessName();
    afx_msg void OnBnClickedRadProcessId();
    afx_msg LRESULT OnThreadMessage(WPARAM wParam, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedBtnSearch();
    afx_msg void OnEnChangeEdtProcessId();
    afx_msg void OnEnChangeEdtProcessName();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEnChangeEdtSeachValue();
    afx_msg void OnCbnSelchangeCbDataType();
    afx_msg void OnLvnItemchangedLvwResults(NMHDR *pNMHDR, LRESULT *pResult);
private:
    BOOL IsNumeric(LPCTSTR lpszString);
    void EnableWindows(HWND hWnd, BOOL bEnable);
    void RestoreWindowsEnable(HWND hWnd);
    void OnListResultItemChanged();
private:
	HICON m_hIcon;
    CEdit m_edtProcessName;
    CButton m_btnBrowseProcess;
    CEdit m_edtProcessID;
    static UINT s_nCommunicateMessage;   
    CButton m_btnSpawnProcess;
    CButton m_btnSearch;
    CEdit m_edtSearchValue;
    CListCtrl m_lvwResults;
    CComboBox m_cboDataTypes;
    CSearchThread* m_pSearchThread;
    CMap<HWND,HWND,BOOL, BOOL> m_mapWndEnable;
    CProgressCtrl m_prgbSearch;
    CButton m_radProcName;
    CButton m_radProcID;
    UINT_PTR m_nTimerID;
public:
    
    CEdit m_edtMemoryData;
    CButton m_btnSaveData;
    CEdit m_edtMemoryAddr;
    afx_msg void OnEnChangeEdtData();
    afx_msg void OnBnClickedBtnSave();
    CButton m_btnLaunchProcess;
    afx_msg void OnBnClickedBtnBrowseLaunch();
    afx_msg void OnBnClickedChbTopmost();
};
