
// ProcessMemoryEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "ProcessMemoryEditorDlg.h"
#include "afxdialogex.h"
#include "SpawnProcessesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT CProcessMemoryEditorDlg::s_nCommunicateMessage = 0;

CProcessMemoryEditorDlg::CProcessMemoryEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MUHACKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pSearchThread = nullptr;
    m_nTimerID = 0;
}

CProcessMemoryEditorDlg::~CProcessMemoryEditorDlg()
{
    if (nullptr != m_pSearchThread) {
        delete m_pSearchThread;
        m_pSearchThread = nullptr;
    }
}

void CProcessMemoryEditorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDT_PROCESS_NAME, m_edtProcessName);
    DDX_Control(pDX, IDC_BTN_BROWSE_PROCESS, m_btnBrowseProcess);
    DDX_Control(pDX, IDC_EDT_PROCESS_ID, m_edtProcessID);
    DDX_Control(pDX, IDC_BTN_SPAWN_PROCESSES, m_btnSpawnProcess);
    DDX_Control(pDX, IDC_BTN_SEARCH, m_btnSearch);
    DDX_Control(pDX, IDC_EDT_SEACH_VALUE, m_edtSearchValue);
    DDX_Control(pDX, IDC_LVW_RESULTS, m_lvwResults);
    DDX_Control(pDX, IDC_CB_DATA_TYPE, m_cboDataTypes);
    DDX_Control(pDX, IDC_PRGB_SEARCH, m_prgbSearch);
    DDX_Control(pDX, IDC_RAD_PROCESS_NAME, m_radProcName);
    DDX_Control(pDX, IDC_RAD_PROCESS_ID, m_radProcID);
    DDX_Control(pDX, IDC_EDT_DATA, m_edtMemoryData);
    DDX_Control(pDX, IDC_BTN_SAVE, m_btnSaveData);
    DDX_Control(pDX, IDC_EDT_ADDRESS, m_edtMemoryAddr);
    DDX_Control(pDX, IDC_BTN_BROWSE_LAUNCH, m_btnLaunchProcess);
}

BEGIN_MESSAGE_MAP(CProcessMemoryEditorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_SPAWN_PROCESSES, &CProcessMemoryEditorDlg::OnBnClickedBtnSpawnProcesses)
    ON_BN_CLICKED(IDC_BTN_BROWSE_PROCESS, &CProcessMemoryEditorDlg::OnBnClickedBtnBrowseProcess)
    ON_BN_CLICKED(IDC_RAD_PROCESS_NAME, &CProcessMemoryEditorDlg::OnBnClickedRadProcessName)
    ON_BN_CLICKED(IDC_RAD_PROCESS_ID, &CProcessMemoryEditorDlg::OnBnClickedRadProcessId)
    ON_BN_CLICKED(IDC_BTN_SEARCH, &CProcessMemoryEditorDlg::OnBnClickedBtnSearch)
    ON_EN_CHANGE(IDC_EDT_PROCESS_ID, &CProcessMemoryEditorDlg::OnEnChangeEdtProcessId)
    ON_EN_CHANGE(IDC_EDT_PROCESS_NAME, &CProcessMemoryEditorDlg::OnEnChangeEdtProcessName)
    ON_REGISTERED_MESSAGE(CProcessMemoryEditorDlg::s_nCommunicateMessage, &CProcessMemoryEditorDlg::OnThreadMessage)
    ON_EN_CHANGE(IDC_EDT_SEACH_VALUE, &CProcessMemoryEditorDlg::OnEnChangeEdtSeachValue)
    ON_CBN_SELCHANGE(IDC_CB_DATA_TYPE, &CProcessMemoryEditorDlg::OnCbnSelchangeCbDataType)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LVW_RESULTS, &CProcessMemoryEditorDlg::OnLvnItemchangedLvwResults)
    ON_EN_CHANGE(IDC_EDT_DATA, &CProcessMemoryEditorDlg::OnEnChangeEdtData)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CProcessMemoryEditorDlg::OnBnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_BROWSE_LAUNCH, &CProcessMemoryEditorDlg::OnBnClickedBtnBrowseLaunch)
    ON_BN_CLICKED(IDC_CHB_TOPMOST, &CProcessMemoryEditorDlg::OnBnClickedChbTopmost)
END_MESSAGE_MAP()


BOOL CProcessMemoryEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    CString strCommMsg;
    strCommMsg.Format(_T("%s-%p-%ld"), AfxGetAppName(), AfxGetInstanceHandle(), GetCurrentThreadId());
    CProcessMemoryEditorDlg::s_nCommunicateMessage = RegisterWindowMessage(strCommMsg);

    CheckDlgButton(IDC_RAD_PROCESS_NAME, BST_CHECKED);
    m_edtProcessName.SetLimitText(MAXINT);
    m_edtProcessID.SetLimitText(10);

    m_lvwResults.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_lvwResults.InsertColumn(0, _T("#"), LVCFMT_LEFT, 40);
    m_lvwResults.InsertColumn(1, _T("Address"), LVCFMT_LEFT, 400);

    m_cboDataTypes.AddString(_T(" < Data type > "));
    m_cboDataTypes.AddString(_T("ANSI String"));
    m_cboDataTypes.AddString(_T("Unicode String"));
    m_cboDataTypes.AddString(_T("Byte"));
    m_cboDataTypes.AddString(_T("Short"));
    m_cboDataTypes.AddString(_T("Word"));
    m_cboDataTypes.AddString(_T("Double Word"));
    m_cboDataTypes.AddString(_T("Quart Word"));
    m_cboDataTypes.AddString(_T("Float"));
    m_cboDataTypes.AddString(_T("Double"));

    m_cboDataTypes.SetCurSel(0);

	return TRUE;
}


void CProcessMemoryEditorDlg::OnDestroy()
{
    if (0 != m_nTimerID) {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }
    CDialogEx::OnDestroy();
}

void CProcessMemoryEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessMemoryEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessMemoryEditorDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == m_nTimerID) {
        int nPos, nMax, nMin;
        m_prgbSearch.GetRange(nMin, nMax);
        nPos = m_prgbSearch.GetPos();
        if (nPos >= nMax) {
            nPos = nMin;
        }
        else {
            nPos += 10;
        }
        m_prgbSearch.SetPos(nPos);
    }
}

void CProcessMemoryEditorDlg::OnBnClickedBtnSpawnProcesses()
{
    CSpawnProcessesDlg dlg;

    if (dlg.DoModal() != IDOK) {
        return;
    }

    SetDlgItemInt(IDC_EDT_PROCESS_ID, dlg.ProcessId());
}


void CProcessMemoryEditorDlg::OnBnClickedBtnBrowseProcess()
{
    static TCHAR BASED_CODE szFilters[] = _T("All files(*.*)|*.*|Executable files)*.exe)|*.exe||");
    
    const UINT nMax = 256;
    TCHAR szFileName[nMax] = { 0 };
    int nChars = m_edtProcessName.GetWindowText(szFileName, nMax-1);
    szFileName[nChars] = 0;

    CFileDialog dlgOpenFile(TRUE, nullptr, szFileName, OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, szFilters);
    if (dlgOpenFile.DoModal() != IDOK) {
        return;
    }

    m_edtProcessName.SetWindowText(dlgOpenFile.GetPathName());
}


void CProcessMemoryEditorDlg::OnBnClickedRadProcessName()
{
    m_edtProcessName.EnableWindow(TRUE);
    m_btnBrowseProcess.EnableWindow(TRUE);

    m_edtProcessID.EnableWindow(FALSE);
    m_btnSpawnProcess.EnableWindow(FALSE);
    m_edtProcessID.SetWindowText(nullptr);
}


void CProcessMemoryEditorDlg::OnBnClickedRadProcessId()
{
    m_edtProcessName.EnableWindow(FALSE);
    m_btnBrowseProcess.EnableWindow(FALSE);

    m_edtProcessID.EnableWindow(TRUE);
    m_btnSpawnProcess.EnableWindow(TRUE);
}


BOOL CProcessMemoryEditorDlg::IsNumeric(LPCTSTR lpszString)
{
    if (nullptr == lpszString) {
        return FALSE;
    }

    CString strTemp = lpszString;
    TCHAR c[2] = { 0 };
    for (int n = 0; n < strTemp.GetLength(); n++) {
        c[0] = strTemp.GetAt(n);
        if (0 == isdigit(c[0])) {
            return FALSE;
        }
    }

    return TRUE;
}

void CProcessMemoryEditorDlg::OnBnClickedBtnSearch()
{
    if (nullptr==m_pSearchThread) { //Start search
        if (m_edtSearchValue.GetWindowTextLength() <= 0) {
            AfxMessageBox(_T("Search value is not inputed."));
            m_edtSearchValue.SetFocus();
            return;
        }

        int nSel = m_cboDataTypes.GetCurSel();
        CString strVal;
        EDataType  eType = static_cast<EDataType>(nSel);

        m_edtSearchValue.GetWindowText(strVal);
        switch (eType)
        {
        case EDataType::eByte:
        case EDataType::eShort:
        case EDataType::eWord:
        case EDataType::eDoubleWord:
        case EDataType::eQuartWord:
        case EDataType::eFloat:
        case EDataType::eDouble:
            if (!IsNumeric(strVal)) {
                AfxMessageBox(_T("Search value is invalid numeric value."));
                m_edtSearchValue.SetFocus();
                return;
            }
            break;
        case EDataType::eUnknow:
            nSel = CB_ERR;
            break;
        default:
            break;
        }

        if (CB_ERR == nSel) {
            AfxMessageBox(_T("Please select a data type."));
            m_cboDataTypes.SetFocus();
            m_cboDataTypes.ShowDropDown(TRUE);
            return;
        }


        if (nullptr != m_pSearchThread) {
            delete m_pSearchThread;
        }

        DWORD dwProcID = 0;
        CString strProcID;
        CStringA strProcIDA;
        m_edtProcessID.GetWindowText(strProcID);
        if (strProcID.IsEmpty()) {
            AfxMessageBox(_T("Invalid process ID."));
            m_edtProcessID.SetFocus();
            return;
        }
        strProcIDA = strProcID;
        dwProcID = static_cast<DWORD>(atol(strProcIDA));
        if (0 == dwProcID) {
            AfxMessageBox(_T("Invalid process ID."));
            m_edtProcessID.SetFocus();
            return;
        }

        switch (eType)
        {
        case  EDataType::eANSIString:
        {
            CStringA strValA;
            CString strVal;
            m_edtSearchValue.GetWindowText(strVal);
            strValA = strVal;
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, (LPCSTR)strValA);
        }
        break;
        case EDataType::eUnicodeString:
        {
            CStringW strValW;
            CString strVal;
            m_edtSearchValue.GetWindowText(strVal);
            strValW = strVal;
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, (LPCWSTR)strValW);
        }
        break;
        case  EDataType::eByte:
        {
            BYTE byval = static_cast<BYTE>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, byval);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, byval);
        }
        break;
        case EDataType::eShort:
        {
            SHORT shVal = static_cast<SHORT>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, shVal);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, shVal);
        }
        break;
        case EDataType::eWord:
        {
            WORD wVal = static_cast<WORD>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, wVal);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, wVal);
        }
        break;
        case EDataType::eDoubleWord:
        {
            DWORD dwVal = static_cast<DWORD>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, dwVal);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, dwVal);
        }
        break;
        case EDataType::eQuartWord:
        {
            CStringA strValA;
            strValA = strVal;
            QWORD qwVal = static_cast<QWORD>( atoll(strValA) );
            CString strTemp;
            strTemp.Format(_T("%I64d"), qwVal);
            m_edtSearchValue.SetWindowText(strTemp);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, qwVal);
        }
        break;
        case  EDataType::eFloat:
        {
            CStringA strValA;
            strValA = strVal;
            FLOAT fVal = static_cast<FLOAT>(atof(strValA));
            CString strTemp;
            strTemp.Format(_T("%f"), fVal);
            m_edtSearchValue.SetWindowText(strTemp);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, fVal);
        }
        break;
        case EDataType::eDouble:
        {
            CStringA strValA;
            strValA = strVal;
            DOUBLE dblVal = static_cast<DOUBLE>(atof(strValA));
            CString strTemp;
            strTemp.Format(_T("%lf"), dblVal);
            m_edtSearchValue.SetWindowText(strTemp);
            m_pSearchThread = new CSearchThread(this, s_nCommunicateMessage, dwProcID, dblVal);
        }
        break;
        
        default:
            break;
        }

        if (nullptr != m_pSearchThread) {
            if (!m_pSearchThread->CreateThread()) {
                MessageBox(_T("Cannot execute searching."), _T("Internal Error"), MB_ICONERROR);
                delete m_pSearchThread;
                m_pSearchThread = nullptr;
                return;
            }
        }
    }
    else { //Stop search
        if (nullptr != m_pSearchThread) {
            m_pSearchThread->SuspendThread();
            int nConfirm = MessageBox(_T("Are you sure to abort current operation?"), _T("Confirmation"), MB_YESNO | MB_OK);
            m_pSearchThread->ResumeThread();
            if (IDYES == nConfirm) {
                m_pSearchThread->Stop();
            }
            else {
                return;
            }
        }
    }
}


void CProcessMemoryEditorDlg::OnEnChangeEdtProcessId()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
    m_btnSearch.EnableWindow(m_edtProcessID.GetWindowTextLength() > 0);
}


void CProcessMemoryEditorDlg::OnEnChangeEdtProcessName()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
    m_btnLaunchProcess.EnableWindow(m_edtProcessName.GetWindowTextLength() > 0);
    m_btnSearch.EnableWindow(m_edtProcessID.GetWindowTextLength() > 0);
}


LRESULT CProcessMemoryEditorDlg::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0L;
    EThreadNotifyCode eCode = static_cast<EThreadNotifyCode>(wParam);

    switch (eCode)
    {
    case EThreadNotifyCode::eStart:
    {
        m_lvwResults.DeleteAllItems();
        OnListResultItemChanged();

        EnableWindows(m_radProcName.GetSafeHwnd(), FALSE);
        EnableWindows(m_radProcID.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtProcessName.GetSafeHwnd(), FALSE);
        EnableWindows(m_btnBrowseProcess.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtProcessID.GetSafeHwnd(), FALSE);
        EnableWindows(m_btnSpawnProcess.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtSearchValue.GetSafeHwnd(), FALSE);
        EnableWindows(m_cboDataTypes.GetSafeHwnd(), FALSE);

        m_btnSearch.SetWindowText(_T("St&op"));

        m_prgbSearch.SetPos(0);
        m_prgbSearch.ShowWindow(SW_SHOW);

        m_nTimerID = SetTimer(12345, 10, nullptr);
    }
    break;
    case EThreadNotifyCode::eFinish:
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
        RestoreWindowsEnable(m_radProcName.GetSafeHwnd());
        RestoreWindowsEnable(m_radProcID.GetSafeHwnd());
        RestoreWindowsEnable(m_edtProcessName.GetSafeHwnd());
        RestoreWindowsEnable(m_btnBrowseProcess.GetSafeHwnd());
        RestoreWindowsEnable(m_edtProcessID.GetSafeHwnd());
        RestoreWindowsEnable(m_btnSpawnProcess.GetSafeHwnd());
        RestoreWindowsEnable(m_edtSearchValue.GetSafeHwnd());
        RestoreWindowsEnable(m_cboDataTypes.GetSafeHwnd());

        m_btnSearch.SetWindowText(_T("&Search"));
        m_prgbSearch.SetPos(0);
        m_prgbSearch.ShowWindow(SW_HIDE);

        if (nullptr != m_pSearchThread) {
            delete m_pSearchThread;
            m_pSearchThread = nullptr;
        }
    }
    break;
    case  EThreadNotifyCode::eData:
    {
        DWORD_PTR ptrAddress = (DWORD_PTR)lParam;
        CString strAddress;

        strAddress.Format(_T("%d"), m_lvwResults.GetItemCount() + 1);
        int nItem = m_lvwResults.InsertItem(m_lvwResults.GetItemCount(), strAddress);
        strAddress.Format(_T("0x%p"), ptrAddress);
        m_lvwResults.SetItemText(nItem, 1, strAddress);
    }
    break;
    default:
        break;
    }

    return res;
}

void CProcessMemoryEditorDlg::EnableWindows(HWND hWnd, BOOL bEnable)
{
    m_mapWndEnable[hWnd] = ::IsWindowEnabled(hWnd);
    ::EnableWindow(hWnd, bEnable);
}

void CProcessMemoryEditorDlg::RestoreWindowsEnable(HWND hWnd)
{
    ::EnableWindow(hWnd, m_mapWndEnable[hWnd]);
}

void CProcessMemoryEditorDlg::OnEnChangeEdtSeachValue()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
}


void CProcessMemoryEditorDlg::OnCbnSelchangeCbDataType()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
}


void CProcessMemoryEditorDlg::OnLvnItemchangedLvwResults(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    OnListResultItemChanged();
}


void CProcessMemoryEditorDlg::OnListResultItemChanged()
{
    m_edtMemoryAddr.SetWindowText(nullptr);
    m_edtMemoryData.SetWindowText(nullptr);
    m_btnSaveData.EnableWindow(FALSE);

    if (0 >= m_lvwResults.GetItemCount()) {
        return;
    }

    if (0 >= m_lvwResults.GetSelectedCount()) {
        return;
    }

    POSITION pos = m_lvwResults.GetFirstSelectedItemPosition();
    int nIndex = m_lvwResults.GetNextSelectedItem(pos);
    if (0 > nIndex) {
        return;
    }

    CString strAddress;
    CString strData;
    strAddress = m_lvwResults.GetItemText(nIndex, 1);
    m_edtMemoryAddr.SetWindowText(strAddress);
    m_edtSearchValue.GetWindowText(strData);
    m_edtMemoryData.SetWindowText(strData);
}

void CProcessMemoryEditorDlg::OnEnChangeEdtData()
{
    m_btnSaveData.EnableWindow(TRUE);
}


void CProcessMemoryEditorDlg::OnBnClickedBtnSave()
{
    //
    m_btnSaveData.EnableWindow(FALSE);
}


void CProcessMemoryEditorDlg::OnBnClickedBtnBrowseLaunch()
{
    //
    CString strProcName;
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    si.cb = sizeof(STARTUPINFO);
    si.wShowWindow = SW_SHOWNORMAL;
    
    m_edtProcessName.GetWindowText(strProcName);
    if (!CreateProcess(nullptr, strProcName.GetBuffer(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        strProcName.ReleaseBuffer();
        AfxMessageBox(_T("Cannot launch process."));
        return;
    }
    strProcName.ReleaseBuffer();
    if (nullptr == pi.hProcess) {
        return;
    }

    strProcName.Format(_T("0x%p"), pi.hProcess);
    WaitForSingleObject(pi.hProcess, 100);
    m_edtProcessID.SetWindowText(strProcName);
    m_btnLaunchProcess.EnableWindow(FALSE);
    m_btnSearch.EnableWindow(m_edtProcessID.GetWindowTextLength() > 0);
}


void CProcessMemoryEditorDlg::OnBnClickedChbTopmost()
{
    BOOL bTopMost = (IsDlgButtonChecked(IDC_CHB_TOPMOST) & BST_CHECKED);
    if (bTopMost) {
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
    }
    else {
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
    }
}
