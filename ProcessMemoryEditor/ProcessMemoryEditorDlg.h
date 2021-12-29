
// ProcessMemoryEditorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MemorySearchThread.h"
#include "MouseHookEngine.h"
#include "MySetting.h"

// CProcessMemoryEditorDlg dialog
class CProcessMemoryEditorDlg : public CDialogEx
{
    DECLARE_MESSAGE_MAP()
public:
	CProcessMemoryEditorDlg(CMySetting* settings = NULL, CWnd* pParent = NULL);	// standard constructor
    virtual ~CProcessMemoryEditorDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUHACKER_DIALOG };
#endif
private:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual void OnCancel();
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
    afx_msg void OnEnChangeEdtData();
    afx_msg void OnBnClickedBtnSave();
    afx_msg void OnBnClickedBtnBrowseLaunch();
    afx_msg void OnBnClickedChbTopmost();
    afx_msg void OnBnClickedBtnSearchInAddresses();
    afx_msg void OnLvnGetdispinfoLvwResults(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeEdtAddress();
    afx_msg void OnBnClickedBtnRead();
    afx_msg LRESULT OnMouseHookEngineNotify(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedRadHexa();
    afx_msg void OnBnClickedRadDecimal();
    afx_msg void OnBnClickedBtnStopSearch();
    afx_msg void OnBnClickedBtnBrowseDll();
    afx_msg void OnBnClickedBtnInject();
    afx_msg void OnEnChangeEdtInjectDll();
	afx_msg void OnBnClickedBtnResetAddresses();
	afx_msg void OnBnClickedBtnLoopSave();
	afx_msg void OnEnChangeEdtLoopWriteTimeelapse();
private:
    BOOL IsNumeric(LPCTSTR lpszString);
    void EnableWindows(HWND hWnd, BOOL bEnable);
    void RestoreWindowsEnable(HWND hWnd);
    void OnListResultItemChanged();
    BOOL PreTranslateMessage(MSG* pMsg);
    BOOL IsMyWindowFamily(CWnd* pWnd);
    CString GenerateRandomizeText(int nMaxLength);
	BOOL WriteProcessDataToSpecifiedAddress(HANDLE hProcess);
private:
	CMySetting * m_pMySettings;
    static UINT s_nHookEngineNotifyMsg;
    ESearchSource m_eSeachKind;
    IHookManager* m_pHookManager;
    HINSTANCE m_hHookEngineDLL;
    CMouseHookEngine* m_pMouseHookEngine;
	HICON m_hIcon;
    HBITMAP m_hFindBmp;
    HBITMAP m_hFindingBmp;
    HCURSOR m_hPrevCursor;
    HCURSOR m_hFindingCursor;
    CEdit m_edtProcessName;
    CButton m_btnBrowseProcess;
    CEdit m_edtProcessID;
    static UINT s_nSearchThreadNotifyMsg;   
    CButton m_btnSpawnProcess;
    CButton m_btnSearchFromMemory;
    CEdit m_edtSearchValue;
    CListCtrl m_lvwResults;
    CComboBox m_cboDataTypes;
    CMemorySearchThread* m_pSearchThread;
    CMap<HWND,HWND,BOOL, BOOL> m_mapWndEnable;
    CProgressCtrl m_prgbSearch;
    CButton m_radProcName;
    CButton m_radProcID;
    UINT_PTR m_nTimerID;
    CEdit m_edtMemoryData;
    DWORD m_dwProcessId;
    CButton m_btnWriteProcessMemory;
    CEdit m_edtMemoryAddr;
    CButton m_btnLaunchProcess;
    CButton m_btnSearchFromAddressList;
    CArray<LPVOID, LPVOID> m_arrMatchAddress;
	CArray<LPVOID, LPVOID> m_arrOriginalAddress;
	BOOL m_bInternalEditChanged;
    CButton m_btnReadProcessMemory;
    CEdit m_edtDataLength;
    CStatic m_sttProcessFromWnd;
	INT_PTR m_nLoopWriteTimerID;
    CStatic m_sttStatusText;
    CButton m_radHexa;
    CButton m_radDecimal;
    CButton m_btnStopSearch;
    CFont m_fontBold;
    CStatic m_grpProcess;
    CStatic m_grpInjection;
    CStatic m_grpSearch;
    CButton m_btnBrowseDll;
    CEdit m_edtDllName;
    CButton m_btnInject;
    HMODULE m_hInjectedDll;
    BOOL m_bRandomizeTitle;
	CButton m_btnResetAddr;
	CButton m_chkTopMost;
	CMySetting* m_pSettings;
	CEdit m_edtLoopWriteElapse;
	CButton m_btnLoopWriteProcessMemory;
};
