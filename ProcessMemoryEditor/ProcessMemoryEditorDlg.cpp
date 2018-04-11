
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

UINT CProcessMemoryEditorDlg::s_nSearchThreadNotifyMsg = 0;
UINT CProcessMemoryEditorDlg::s_nHookEngineNotifyMsg = 0;

CProcessMemoryEditorDlg::CProcessMemoryEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MUHACKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_hFindBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR);
    m_hFindingBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR);
    m_hFindingCursor = AfxGetApp()->LoadCursor(IDC_CURSOR1);
    m_hInjectedDll = nullptr;
    m_pSearchThread = nullptr;
    m_nTimerID = 0;
    m_dwProcessId = 0;
    m_pMouseHookEngine = nullptr;
    m_pHookManager = nullptr;
    m_eSeachKind = ESearch::eSearchNone;

    CString strMsgId;
    if (0 == s_nHookEngineNotifyMsg) {
        strMsgId.Format(_T("%s::HookEngineNotifyMessage::%p::%ld"), AfxGetAppName(), AfxGetInstanceHandle(), GetCurrentThreadId());
        s_nHookEngineNotifyMsg = RegisterWindowMessage(strMsgId);
    }
    if (0 == s_nSearchThreadNotifyMsg) {
        strMsgId.Format(_T("%s::SearchThreadNotifyMessage::%p::%ld"), AfxGetAppName(), AfxGetInstanceHandle(), GetCurrentThreadId());
        s_nSearchThreadNotifyMsg = RegisterWindowMessage(strMsgId);
    }

    m_hHookEngineDLL = LoadLibrary(_T("HookEngine.Dll"));
    if (nullptr != m_hHookEngineDLL) {
        GETHOOKMANAGER getHookManager = (GETHOOKMANAGER)GetProcAddress(m_hHookEngineDLL, GetHookManagerFunctionName);
        if(nullptr != getHookManager) {
            m_pHookManager = getHookManager();
            if (nullptr != m_pHookManager) {
                m_pMouseHookEngine =  CMouseHookEngine::createInstance(this, s_nHookEngineNotifyMsg);
                if (nullptr != m_pMouseHookEngine) {
                    m_pHookManager->registerEngine(m_pMouseHookEngine->Id(), m_pMouseHookEngine);
                }
            }
        }
    }

}

CProcessMemoryEditorDlg::~CProcessMemoryEditorDlg()
{
    if (nullptr != m_fontBold.GetSafeHandle()) {
        m_fontBold.DeleteObject();
    }

    if (nullptr != m_hHookEngineDLL) {
        if (FreeLibrary(m_hHookEngineDLL)) {
            m_hHookEngineDLL = nullptr;
        }
    }

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
    DDX_Control(pDX, IDC_BTN_SEARCH, m_btnSearchWhole);
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
    DDX_Control(pDX, IDC_BTN_SEARCH_IN_ADDRESSES, m_btnSearchInAddresses);
    DDX_Control(pDX, IDC_BTN_READ, m_btnReadProcessMemory);
    DDX_Control(pDX, IDC_EDT_DATA_LENGTH, m_edtDataLength);
    DDX_Control(pDX, IDC_STT_FIND_PROCESS, m_sttProcessFromWnd);
    DDX_Control(pDX, IDC_STT_STATUS_TEXT, m_sttStatusText);
    DDX_Control(pDX, IDC_RAD_HEXA, m_radHexa);
    DDX_Control(pDX, IDC_RAD_DECIMAL, m_radDecimal);
    DDX_Control(pDX, IDC_BTN_STOP_SEARCH, m_btnStopSearch);
    DDX_Control(pDX, IDC_GRP_PROCESS, m_grpProcess);
    DDX_Control(pDX, IDC_GRP_INJECTION, m_grpInjection);
    DDX_Control(pDX, IDC_GRP_SEARCH, m_grpSearch);
    DDX_Control(pDX, IDC_BTN_BROWSE_DLL, m_btnBrowseDll);
    DDX_Control(pDX, IDC_EDT_INJECT_DLL, m_edtDllName);
    DDX_Control(pDX, IDC_BTN_INJECT, m_btnInject);
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
    ON_EN_CHANGE(IDC_EDT_SEACH_VALUE, &CProcessMemoryEditorDlg::OnEnChangeEdtSeachValue)
    ON_CBN_SELCHANGE(IDC_CB_DATA_TYPE, &CProcessMemoryEditorDlg::OnCbnSelchangeCbDataType)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LVW_RESULTS, &CProcessMemoryEditorDlg::OnLvnItemchangedLvwResults)
    ON_EN_CHANGE(IDC_EDT_DATA, &CProcessMemoryEditorDlg::OnEnChangeEdtData)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CProcessMemoryEditorDlg::OnBnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_BROWSE_LAUNCH, &CProcessMemoryEditorDlg::OnBnClickedBtnBrowseLaunch)
    ON_BN_CLICKED(IDC_CHB_TOPMOST, &CProcessMemoryEditorDlg::OnBnClickedChbTopmost)
    ON_BN_CLICKED(IDC_BTN_SEARCH_IN_ADDRESSES, &CProcessMemoryEditorDlg::OnBnClickedBtnSearchInAddresses)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LVW_RESULTS, &CProcessMemoryEditorDlg::OnLvnGetdispinfoLvwResults)
    ON_EN_CHANGE(IDC_EDT_ADDRESS, &CProcessMemoryEditorDlg::OnEnChangeEdtAddress)
    ON_BN_CLICKED(IDC_BTN_READ, &CProcessMemoryEditorDlg::OnBnClickedBtnRead)
    //
    ON_REGISTERED_MESSAGE(CProcessMemoryEditorDlg::s_nSearchThreadNotifyMsg, &CProcessMemoryEditorDlg::OnThreadMessage)
    ON_REGISTERED_MESSAGE(CProcessMemoryEditorDlg::s_nHookEngineNotifyMsg, &CProcessMemoryEditorDlg::OnMouseHookEngineNotify)
    ON_BN_CLICKED(IDC_RAD_HEXA, &CProcessMemoryEditorDlg::OnBnClickedRadHexa)
    ON_BN_CLICKED(IDC_RAD_DECIMAL, &CProcessMemoryEditorDlg::OnBnClickedRadDecimal)
    ON_BN_CLICKED(IDC_BTN_STOP_SEARCH, &CProcessMemoryEditorDlg::OnBnClickedBtnStopSearch)
    ON_BN_CLICKED(IDC_BTN_BROWSE_DLL, &CProcessMemoryEditorDlg::OnBnClickedBtnBrowseDll)
    ON_BN_CLICKED(IDC_BTN_INJECT, &CProcessMemoryEditorDlg::OnBnClickedBtnInject)
    ON_EN_CHANGE(IDC_EDT_INJECT_DLL, &CProcessMemoryEditorDlg::OnEnChangeEdtInjectDll)
END_MESSAGE_MAP()


BOOL CProcessMemoryEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    CheckDlgButton(IDC_RAD_PROCESS_ID, BST_CHECKED);
    OnBnClickedRadProcessId();

    m_edtProcessName.SetLimitText(MAXINT);
    m_edtProcessID.SetLimitText(10);

    m_lvwResults.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_lvwResults.InsertColumn(0, _T("#"), LVCFMT_LEFT, 40);
    m_lvwResults.InsertColumn(1, _T("Address"), LVCFMT_LEFT, 200);

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
    m_edtDataLength.SetLimitText(3);

    m_sttProcessFromWnd.ModifyStyle(0, SS_BITMAP);
    m_sttProcessFromWnd.SetBitmap(m_hFindBmp);

    CheckDlgButton(IDC_RAD_DECIMAL, BST_CHECKED);

    CFont* pFont = GetFont();
    if(nullptr != pFont) {
        LOGFONT lf = { 0 };
        if (pFont->GetLogFont(&lf) > 0) {
            lf.lfWeight = FW_BOLD;
            if (m_fontBold.CreateFontIndirect(&lf)) {
                m_grpSearch.SetFont(&m_fontBold);
                m_grpInjection.SetFont(&m_fontBold);
                m_grpProcess.SetFont(&m_fontBold);
            }
        }
    }

    if (m_bRandomizeTitle) {
        SetWindowText(GenerateRandomizeText(255));
    }

#ifdef _DEBUG
    m_edtProcessName.SetWindowText(_T("D:\\Games\\MU\\MUViet\\MUViet\\mu.exe"));
    m_edtDllName.SetWindowText(_T("D:\\Projects\\ProcessMemoryEditor\\Debug\\ICMAnti.dll"));
#endif

	return TRUE;
}

void CProcessMemoryEditorDlg::OnCancel()
{
    if (nullptr != m_pSearchThread) {
        if (m_pSearchThread->IsStopped()) {
            delete m_pSearchThread;
            m_pSearchThread = nullptr;
        }
        else {
            return;
        }
    }

    CDialogEx::OnCancel();
}

void CProcessMemoryEditorDlg::OnDestroy()
{
    if (nullptr != m_pHookManager) {
        if (nullptr != m_pMouseHookEngine) {
            m_pHookManager->removeEngine(m_pMouseHookEngine->Id());
            m_pMouseHookEngine->UnHook();
            m_pMouseHookEngine->deleteInstance();
            m_pMouseHookEngine = nullptr;
        }
    }

    if (nullptr != m_hHookEngineDLL) {
        if (FreeLibrary(m_hHookEngineDLL)) {
            m_hHookEngineDLL = nullptr;
            m_pHookManager = nullptr;
        }
    }

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
    static TCHAR BASED_CODE szFilters[] = _T("All File Types(*.*)|*.*|Executable files(*.exe)|*.exe||");
    
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
    m_btnLaunchProcess.EnableWindow(m_edtProcessName.GetWindowTextLength()>0);
    m_edtProcessID.EnableWindow(FALSE);
    m_btnSpawnProcess.EnableWindow(FALSE);
    m_edtProcessID.SetWindowText(nullptr);
    m_sttProcessFromWnd.EnableWindow(FALSE);
}


void CProcessMemoryEditorDlg::OnBnClickedRadProcessId()
{
    m_edtProcessName.EnableWindow(FALSE);
    m_btnBrowseProcess.EnableWindow(FALSE);
    m_btnLaunchProcess.EnableWindow(FALSE);

    m_edtProcessID.EnableWindow(TRUE);
    m_btnSpawnProcess.EnableWindow(TRUE);
    m_sttProcessFromWnd.EnableWindow(TRUE);
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
    if (nullptr == m_pSearchThread) { //Start search
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

        CString strProcID;
        CStringA strProcIDA;
        m_edtProcessID.GetWindowText(strProcID);
        if (strProcID.IsEmpty()) {
            AfxMessageBox(_T("Invalid process ID."));
            m_edtProcessID.SetFocus();
            return;
        }
        strProcIDA = strProcID;
        m_dwProcessId = static_cast<DWORD>(atol(strProcIDA));
        if (0 == m_dwProcessId) {
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
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, (LPCSTR)strValA);
        }
        break;
        case EDataType::eUnicodeString:
        {
            CStringW strValW;
            CString strVal;
            m_edtSearchValue.GetWindowText(strVal);
            strValW = strVal;
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, (LPCWSTR)strValW);
        }
        break;
        case  EDataType::eByte:
        {
            BYTE byval = static_cast<BYTE>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, byval);
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, byval);
        }
        break;
        case EDataType::eShort:
        {
            SHORT shVal = static_cast<SHORT>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, shVal);
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, shVal);
        }
        break;
        case EDataType::eWord:
        {
            WORD wVal = static_cast<WORD>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, wVal);
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, wVal);
        }
        break;
        case EDataType::eDoubleWord:
        {
            DWORD dwVal = static_cast<DWORD>(GetDlgItemInt(IDC_EDT_SEACH_VALUE));
            SetDlgItemInt(IDC_EDT_SEACH_VALUE, dwVal);
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, dwVal);
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
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, qwVal);
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
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, fVal);
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
            m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, dblVal);
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

        m_eSeachKind = ESearch::eSearchWhole;
        m_btnSearchWhole.EnableWindow(FALSE);
    }
}


void CProcessMemoryEditorDlg::OnEnChangeEdtProcessId()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
    m_btnSearchWhole.EnableWindow((m_edtProcessID.GetWindowTextLength() > 0) && (m_edtSearchValue.GetWindowTextLength() > 0));
    m_btnReadProcessMemory.EnableWindow((m_edtMemoryAddr.GetWindowTextLength() > 0) && (m_edtProcessID.GetWindowTextLength() > 0));
}


void CProcessMemoryEditorDlg::OnEnChangeEdtProcessName()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
    m_btnLaunchProcess.EnableWindow(m_edtProcessName.GetWindowTextLength() > 0);
    m_btnSearchWhole.EnableWindow(m_edtProcessID.GetWindowTextLength() > 0);
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
        m_arrMatchAddress.RemoveAll();
        OnListResultItemChanged();

        EnableWindows(m_radProcName.GetSafeHwnd(), FALSE);
        EnableWindows(m_radProcID.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtProcessName.GetSafeHwnd(), FALSE);
        EnableWindows(m_btnBrowseProcess.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtProcessID.GetSafeHwnd(), FALSE);
        EnableWindows(m_btnSpawnProcess.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtSearchValue.GetSafeHwnd(), FALSE);
        EnableWindows(m_cboDataTypes.GetSafeHwnd(), FALSE);
        EnableWindows(m_btnSearchInAddresses.GetSafeHwnd(), FALSE);
        EnableWindows(m_sttProcessFromWnd.GetSafeHwnd(), FALSE);
        EnableWindows(m_radHexa.GetSafeHwnd(), FALSE);
        EnableWindows(m_radDecimal.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtMemoryAddr.GetSafeHwnd(), FALSE);
        EnableWindows(m_edtMemoryData.GetSafeHwnd(), FALSE);
        EnableWindows(m_lvwResults.GetSafeHwnd(), FALSE);

        m_btnStopSearch.EnableWindow(TRUE);

        m_prgbSearch.ModifyStyle(0, PBS_MARQUEE);
        m_prgbSearch.SetPos(0);
        m_prgbSearch.ShowWindow(SW_SHOW);
        m_btnStopSearch.ShowWindow(SW_SHOW);

        CString strStatus;
        strStatus = _T("Total: 0 matched addresses");
        m_sttStatusText.SetWindowText(strStatus);
        m_sttStatusText.ShowWindow(SW_HIDE);

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
        RestoreWindowsEnable(m_btnSearchInAddresses.GetSafeHwnd());
        RestoreWindowsEnable(m_sttProcessFromWnd.GetSafeHwnd());
        RestoreWindowsEnable(m_radHexa.GetSafeHwnd());
        RestoreWindowsEnable(m_radDecimal.GetSafeHwnd());
        RestoreWindowsEnable(m_lvwResults.GetSafeHwnd());
        RestoreWindowsEnable(m_edtMemoryData.GetSafeHwnd());
        RestoreWindowsEnable(m_edtMemoryAddr.GetSafeHwnd());

        m_btnSearchInAddresses.EnableWindow(m_lvwResults.GetItemCount() > 0);

        m_btnStopSearch.EnableWindow(FALSE);

        m_prgbSearch.SetPos(0);
        m_prgbSearch.ShowWindow(SW_HIDE);
        m_btnStopSearch.ShowWindow(SW_HIDE);

        m_sttStatusText.ShowWindow(SW_SHOW);

        EThreadExitCode nError = EThreadExitCode::eSuccess;

        if (nullptr != m_pSearchThread) {
            nError = static_cast<EThreadExitCode>( m_pSearchThread->ExitCode() );
        }

        m_lvwResults.SetItemCount(static_cast<int>( m_arrMatchAddress.GetCount()) );
        m_lvwResults.RedrawItems(0, static_cast<int>( m_arrMatchAddress.GetCount() - 1) );

        switch (nError)
        {
        case EThreadExitCode::eSuccess:
        {
            MessageBox(_T("Search completed."), AfxGetAppName(), MB_ICONINFORMATION);
        }
        break;
        case  EThreadExitCode::eUserAborted:
        {
            //Do nothing
        }
        break;
        default:
            MessageBox(_T("Search was finished with error."), AfxGetAppName(), MB_ICONWARNING);
            break;
        }

        switch (m_eSeachKind)
        {
        case ESearch::eSearchWhole:
        {
            m_btnSearchWhole.EnableWindow(TRUE);
        }
        break;
        case ESearch::wSearchSpecifiedAddress:
        {
            m_btnSearchInAddresses.EnableWindow(TRUE);
        }
        break;
        default:
            break;
        }

        if (nullptr != m_pSearchThread) {
            delete m_pSearchThread;
            m_pSearchThread = nullptr;
        }
        m_eSeachKind = ESearch::eSearchNone;
        m_lvwResults.SetItemCount(static_cast<int>(m_arrMatchAddress.GetCount()));
        m_lvwResults.RedrawItems(0, static_cast<int>(m_arrMatchAddress.GetCount()) - 1);
    }
    break;
    case  EThreadNotifyCode::eData:
    {
        CString strItemCount;
        DWORD_PTR ptrAddress = (DWORD_PTR)lParam;
        m_arrMatchAddress.Add(LPVOID(ptrAddress));
        INT_PTR nCount = m_arrMatchAddress.GetCount();

        strItemCount.Format(_T("Total: %ld matched addresses"), nCount);
        m_sttStatusText.SetWindowText(strItemCount);

        //m_lvwResults.SetItemCount(nCount);
    }
    break;
    case EThreadNotifyCode::eProgressRange:
    {
        int nMin = (int)LOWORD(lParam);
        int nMax = (int)HIWORD(lParam);
        if (m_prgbSearch.GetSafeHwnd()) {
            m_prgbSearch.SetRange32(nMin, nMax);
            m_prgbSearch.SetPos(nMin);
            m_prgbSearch.ModifyStyle(PBS_MARQUEE, 0);
        }
        if (0 != m_nTimerID) {
            KillTimer(m_nTimerID);
            m_nTimerID = 0;
        }
    }
    break;
    case  EThreadNotifyCode::eProgressValue:
    {
        int nPos = (int)lParam;
        if (m_prgbSearch.GetSafeHwnd()) {
            m_prgbSearch.SetPos(nPos);
        }
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

    m_btnSearchWhole.EnableWindow((m_edtProcessID.GetWindowTextLength() > 0) && (m_edtSearchValue.GetWindowTextLength() > 0));

    if ((m_cboDataTypes.GetCurSel() == (int)EDataType::eANSIString) || (m_cboDataTypes.GetCurSel() == (int)EDataType::eUnicodeString)) {
        SetDlgItemInt(IDC_EDT_DATA_LENGTH, m_edtSearchValue.GetWindowTextLength());
    }
}


void CProcessMemoryEditorDlg::OnCbnSelchangeCbDataType()
{
    m_lvwResults.DeleteAllItems();
    OnListResultItemChanged();
    SetDlgItemInt(IDC_EDT_DATA_LENGTH, 0);
    m_edtDataLength.SetReadOnly(TRUE);
    int nSel = m_cboDataTypes.GetCurSel();
    if (CB_ERR != nSel) {
        switch ( static_cast<EDataType>(nSel))
        {
        case EDataType::eANSIString:
        case EDataType::eUnicodeString:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, m_edtSearchValue.GetWindowTextLength());
            m_edtDataLength.SetReadOnly(FALSE);
            break;
        case EDataType::eByte:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, 1);
            break;
        case EDataType::eWord:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, sizeof(WORD));
            break;
        case EDataType::eShort:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, sizeof(short));
            break;
        case EDataType::eDoubleWord:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, sizeof(DWORD));
            break;
        case EDataType::eQuartWord:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, sizeof(QWORD));
            break;
        case EDataType::eFloat:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, sizeof(float));
            break;
        case EDataType::eDouble:
            SetDlgItemInt(IDC_EDT_DATA_LENGTH, sizeof(double));
            break;
        default:
            break;
        }
    }
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
        m_arrMatchAddress.RemoveAll();
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

    if (IsDlgButtonChecked(IDC_RAD_HEXA)) {
        strAddress.Format(_T("0x%p"), m_arrMatchAddress.GetAt(nIndex));
    }
    else  if (IsDlgButtonChecked(IDC_RAD_DECIMAL)) {
        strAddress.Format(_T("%ld"), reinterpret_cast<DWORD_PTR>(m_arrMatchAddress.GetAt(nIndex)));
    }

    m_edtMemoryAddr.SetWindowText(strAddress);
    m_edtSearchValue.GetWindowText(strData);
    m_edtMemoryData.SetWindowText(strData);
}

void CProcessMemoryEditorDlg::OnEnChangeEdtData()
{
    m_btnSaveData.EnableWindow(TRUE);
    m_btnSearchInAddresses.EnableWindow(m_edtMemoryData.GetWindowTextLength() > 0);
}


void CProcessMemoryEditorDlg::OnBnClickedBtnSave()
{
    CString strAddress;
    m_edtMemoryAddr.GetWindowText(strAddress);
    if (strAddress.IsEmpty()) {
        AfxMessageBox(_T("Please input memory address!"));
        m_edtMemoryAddr.SetFocus();
        return;
    }

    CString strProcID;
    CStringA strProcIDA;
    m_edtProcessID.GetWindowText(strProcID);
    if (strProcID.IsEmpty()) {
        AfxMessageBox(_T("Invalid process ID."));
        m_edtProcessID.SetFocus();
        return;
    }
    strProcIDA = strProcID;
    m_dwProcessId = static_cast<DWORD>(atol(strProcIDA));
    if (0 == m_dwProcessId) {
        AfxMessageBox(_T("Invalid process ID."));
        m_edtProcessID.SetFocus();
        return;
    }

    int nDataSize = GetDlgItemInt(IDC_EDT_DATA_LENGTH);
    switch (static_cast<EDataType>(m_cboDataTypes.GetCurSel()))
    {
    case EDataType::eUnicodeString:
    case EDataType::eANSIString:
        nDataSize *= sizeof(TCHAR);
        break;
    default:
        break;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwProcessId);
    if (nullptr == hProcess) {
        AfxMessageBox(_T("Cannot open process."));
        return;
    }

    BYTE* pBuffer = new BYTE[nDataSize + 1];
    ZeroMemory(pBuffer, nDataSize + 1);
    SIZE_T nByteWriten = 0;
    LPVOID addr = nullptr;

    if (IsDlgButtonChecked(IDC_RAD_HEXA)) {

    }
    else if (IsDlgButtonChecked(IDC_RAD_DECIMAL)) {
        CStringA strAddressA;
        strAddressA = strAddress;
        LONGLONG lAddress = atoll(strAddressA);
        addr = reinterpret_cast<LPVOID>(lAddress);
    }

    switch (static_cast<EDataType>(m_cboDataTypes.GetCurSel()))
    {
    case EDataType::eByte:
    {
        BYTE by = (BYTE)GetDlgItemInt(IDC_EDT_DATA);
        CopyMemory(pBuffer, &by, nDataSize);
    }
    break;
    case EDataType::eShort:
    {
        SHORT sh = (SHORT)GetDlgItemInt(IDC_EDT_DATA);
        CopyMemory(pBuffer, &sh, nDataSize);
    }
    break;
    case EDataType::eWord:
    {
        WORD w = (WORD)GetDlgItemInt(IDC_EDT_DATA);
        CopyMemory(pBuffer, &w, nDataSize);
    }
    break;
    case EDataType::eDoubleWord:
    {
        DWORD dw = 0;
        CString strDw;
        m_edtMemoryData.GetWindowText(strDw);
        dw = (DWORD)_tstoll(strDw);
        CopyMemory(pBuffer, &dw, nDataSize);
    }
    break;
    case EDataType::eQuartWord:
    {
        DWORD qw = 0;
        CString strQw;
        m_edtMemoryData.GetWindowText(strQw);
        qw = (DWORD)_tstoll(strQw);
        CopyMemory(pBuffer, &qw, nDataSize);
    }
    break;
    case EDataType::eFloat:
    {
        float f = 0;
        CString strQw;
        m_edtMemoryData.GetWindowText(strQw);
        f = (float)_tstoll(strQw);
        CopyMemory(pBuffer, &f, nDataSize);
    }
    break;
    case EDataType::eDouble:
    {
        double dbl = 0;
        CString strDbl;
        m_edtMemoryData.GetWindowText(strDbl);
        dbl = (float)_tstoll(strDbl);
        CopyMemory(pBuffer, &dbl, nDataSize);
    }
    break;
    case EDataType::eANSIString:
    {
        CStringA strA;
        CopyMemory(strA.GetBuffer(nDataSize / sizeof(CHAR)), pBuffer, nDataSize);
        strA.ReleaseBuffer();
        m_edtMemoryData.SetWindowTextW(CString(strA));
    }
    break;
    case EDataType::eUnicodeString:
    {
        CStringW strW;
        CopyMemory(strW.GetBuffer(nDataSize / sizeof(WCHAR)), pBuffer, nDataSize);
        strW.ReleaseBuffer();
        m_edtMemoryData.SetWindowTextW(CString(strW));
    }
    break;
    default:
        break;
    }

    POSITION pos = m_lvwResults.GetFirstSelectedItemPosition();
    int nItem = -1;
    MEMORY_BASIC_INFORMATION memInfo = { 0 };
    while (pos)
    {
        nItem = m_lvwResults.GetNextSelectedItem(pos);
        addr = m_arrMatchAddress.GetAt(nItem);
        ZeroMemory(&memInfo, sizeof(MEMORY_BASIC_INFORMATION));
        if (sizeof(MEMORY_BASIC_INFORMATION) == VirtualQueryEx(hProcess, addr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION))) {
            if (PAGE_READWRITE == memInfo.Protect) {
                if (WriteProcessMemory(hProcess, addr, pBuffer, nDataSize, &nByteWriten)) {
                    m_btnSaveData.EnableWindow(FALSE);
                }
            }
        }
    }

    delete[] pBuffer;
    pBuffer = nullptr;
    CloseHandle(hProcess);
    //
    m_btnSaveData.EnableWindow(FALSE);
}


void CProcessMemoryEditorDlg::OnBnClickedBtnBrowseLaunch()
{
    //
    CString strProcName;
    CString strCurDir;
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    si.cb = sizeof(STARTUPINFO);
    si.wShowWindow = SW_SHOWNORMAL;
    
    m_edtProcessName.GetWindowText(strProcName);
    GetCurrentDirectory(MAX_PATH - 1, strCurDir.GetBuffer(_MAX_PATH));
    strCurDir.ReleaseBuffer();
    int nBackSlash = strProcName.ReverseFind('\\');
    if (nBackSlash) {
        strCurDir = strProcName.Left(nBackSlash);
    }

    if (!CreateProcess(nullptr, strProcName.GetBuffer(), nullptr, nullptr, FALSE, 0, nullptr, (LPCTSTR)strCurDir, &si, &pi)) {
        DWORD dwErr = GetLastError();
        CString strErr;
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, dwErr, 0, strErr.GetBuffer(255), 255, nullptr);
        strErr.ReleaseBuffer();
        MessageBox(strErr, _T("Error"), MB_ICONERROR);
        return;
    }

    if (nullptr == pi.hProcess) {
        return;
    }

    WaitForInputIdle(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    strProcName.Format(_T("%ld"), pi.dwProcessId);
    m_edtProcessID.SetWindowText(strProcName);
    m_btnLaunchProcess.EnableWindow(FALSE);
    m_btnSearchWhole.EnableWindow(m_edtProcessID.GetWindowTextLength() > 0);
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


void CProcessMemoryEditorDlg::OnBnClickedBtnSearchInAddresses()
{
    // TODO: Add your control notification handler code here
    CString strVal;
    m_edtMemoryData.GetWindowText(strVal);
    if (0 >= strVal.GetLength()) {
        AfxMessageBox(_T("Search data cannot be empty."));
        m_edtMemoryData.SetFocus();
        return;
    }

    int nSel = m_cboDataTypes.GetCurSel();
    if (CB_ERR == nSel) {
        AfxMessageBox(_T("Please select a data type."));
        m_cboDataTypes.SetFocus();
        m_cboDataTypes.ShowDropDown(TRUE);
        return;
    }

    EDataType  eType = static_cast<EDataType>(nSel);
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

    if (nullptr != m_pSearchThread) {
        delete m_pSearchThread;
        m_pSearchThread = nullptr;
    }

    switch (eType)
    {
    case  EDataType::eANSIString:
    {
        CStringA strValA;
        CString strVal;
        m_edtMemoryData.GetWindowText(strVal);
        strValA = strVal;
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, (LPCSTR)strValA, &m_arrMatchAddress);
    }
    break;
    case EDataType::eUnicodeString:
    {
        CStringW strValW;
        CString strVal;
        m_edtMemoryData.GetWindowText(strVal);
        strValW = strVal;
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, (LPCWSTR)strValW, &m_arrMatchAddress);
    }
    break;
    case  EDataType::eByte:
    {
        BYTE byval = static_cast<BYTE>(GetDlgItemInt(IDC_EDT_DATA));
        SetDlgItemInt(IDC_EDT_DATA, byval);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, byval, &m_arrMatchAddress);
    }
    break;
    case EDataType::eShort:
    {
        SHORT shVal = static_cast<SHORT>(GetDlgItemInt(IDC_EDT_DATA));
        SetDlgItemInt(IDC_EDT_DATA, shVal);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, shVal, &m_arrMatchAddress);
    }
    break;
    case EDataType::eWord:
    {
        WORD wVal = static_cast<WORD>(GetDlgItemInt(IDC_EDT_DATA));
        SetDlgItemInt(IDC_EDT_DATA, wVal);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, wVal, &m_arrMatchAddress);
    }
    break;
    case EDataType::eDoubleWord:
    {
        DWORD dwVal = static_cast<DWORD>(GetDlgItemInt(IDC_EDT_DATA));
        SetDlgItemInt(IDC_EDT_DATA, dwVal);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, dwVal, &m_arrMatchAddress);
    }
    break;
    case EDataType::eQuartWord:
    {
        CStringA strValA;
        strValA = strVal;
        QWORD qwVal = static_cast<QWORD>(atoll(strValA));
        CString strTemp;
        strTemp.Format(_T("%I64d"), qwVal);
        m_edtMemoryData.SetWindowText(strTemp);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, qwVal, &m_arrMatchAddress);
    }
    break;
    case  EDataType::eFloat:
    {
        CStringA strValA;
        strValA = strVal;
        FLOAT fVal = static_cast<FLOAT>(atof(strValA));
        CString strTemp;
        strTemp.Format(_T("%f"), fVal);
        m_edtMemoryData.SetWindowText(strTemp);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, fVal, &m_arrMatchAddress);
    }
    break;
    case EDataType::eDouble:
    {
        CStringA strValA;
        strValA = strVal;
        DOUBLE dblVal = static_cast<DOUBLE>(atof(strValA));
        CString strTemp;
        strTemp.Format(_T("%lf"), dblVal);
        m_edtMemoryData.SetWindowText(strTemp);
        m_pSearchThread = new CMemorySearchThread(this, s_nSearchThreadNotifyMsg, m_dwProcessId, dblVal, &m_arrMatchAddress);
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

    m_eSeachKind = ESearch::wSearchSpecifiedAddress;
    m_btnSearchInAddresses.EnableWindow(FALSE);
}


void CProcessMemoryEditorDlg::OnLvnGetdispinfoLvwResults(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    if ((NULL == pDispInfo) || (0 >= m_arrMatchAddress.GetCount())) {
        return;
    }

    if ((nullptr != m_pSearchThread) && (!m_pSearchThread->IsStopped())) {
        return;
    }

    if (pDispInfo->item.mask & LVIF_TEXT) {
        if (0 == pDispInfo->item.iSubItem) {
            CString strNo;
            strNo.Format(_T("%d"), pDispInfo->item.iItem + 1);
            _tcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, (LPCTSTR)strNo);
        }
        else if (1 == pDispInfo->item.iSubItem) {
            LPVOID lpAddr = m_arrMatchAddress[pDispInfo->item.iItem];
            CString strAddr;
            strAddr.Format(_T("0x%p"), lpAddr);
            _tcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, (LPCTSTR)strAddr);
        }
    }
}


void CProcessMemoryEditorDlg::OnEnChangeEdtAddress()
{
    m_btnReadProcessMemory.EnableWindow((m_edtMemoryAddr.GetWindowTextLength() > 0) && (m_edtProcessID.GetWindowTextLength() > 0));
}


void CProcessMemoryEditorDlg::OnBnClickedBtnRead()
{
    CString strAddress;
    m_edtMemoryAddr.GetWindowText(strAddress);
    if (strAddress.IsEmpty()) {
        AfxMessageBox(_T("Please input memory address!"));
        m_edtMemoryAddr.SetFocus();
        return;
    }

    CString strProcID;
    CStringA strProcIDA;
    m_edtProcessID.GetWindowText(strProcID);
    if (strProcID.IsEmpty()) {
        AfxMessageBox(_T("Invalid process ID."));
        m_edtProcessID.SetFocus();
        return;
    }
    strProcIDA = strProcID;
    m_dwProcessId = static_cast<DWORD>(atol(strProcIDA));
    if (0 == m_dwProcessId) {
        AfxMessageBox(_T("Invalid process ID."));
        m_edtProcessID.SetFocus();
        return;
    }

    int nDataSize = GetDlgItemInt(IDC_EDT_DATA_LENGTH);
    switch (static_cast<EDataType>(m_cboDataTypes.GetCurSel()))
    {
    case EDataType::eUnicodeString:
    case EDataType::eANSIString:
        nDataSize *= sizeof(TCHAR);
        break;
    default:
        break;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwProcessId);
    if (nullptr == hProcess) {
        AfxMessageBox(_T("Cannot open process."));
        return;
    }

    BYTE* pBuffer = new BYTE[nDataSize + 1];
    ZeroMemory(pBuffer, nDataSize + 1);
    SIZE_T nByteRead = 0;
    LPCVOID addr = nullptr;

    if (IsDlgButtonChecked(IDC_RAD_HEXA)) {

    }
    else if(IsDlgButtonChecked(IDC_RAD_DECIMAL) ){
        CStringA strAddressA;
        strAddressA = strAddress;
        LONGLONG lAddress = atoll(strAddressA);
        addr = reinterpret_cast<LPVOID>(lAddress);
    }

    SetDlgItemInt(IDC_EDT_DATA, 0);

    if (ReadProcessMemory(hProcess, addr, pBuffer, nDataSize, &nByteRead)) {
        switch (static_cast<EDataType>(m_cboDataTypes.GetCurSel()))
        {
        case EDataType::eByte:
            SetDlgItemInt(IDC_EDT_DATA, (UINT)pBuffer[0]);
            break;
        case EDataType::eShort:
        {
            SHORT sh = 0;
            CopyMemory(&sh, pBuffer, sizeof(SHORT));
            SetDlgItemInt(IDC_EDT_DATA, sh);
        }
        break;
        case EDataType::eWord:
        {
            WORD w = 0;
            CopyMemory(&w, pBuffer, sizeof(WORD));
            SetDlgItemInt(IDC_EDT_DATA, w);
        }
        break;
        case EDataType::eDoubleWord:
        {
            DWORD dw = 0;
            CopyMemory(&dw, pBuffer, sizeof(DWORD));
            CString strDw;
            strDw.Format(_T("%ld"), dw);
            m_edtMemoryData.SetWindowText(strDw);
        }
        break;
        case EDataType::eQuartWord:
        {
            QWORD qw = 0;
            CopyMemory(&qw, pBuffer, sizeof(QWORD));
            CString strQw;
            strQw.Format(_T("%I64d"), qw);
            m_edtMemoryData.SetWindowText(strQw);
        }
        break;
        case EDataType::eFloat:
        {
            float f = 0;
            CopyMemory(&f, pBuffer, sizeof(float));
            CString strF;
            strF.Format(_T("%f"), f);
            m_edtMemoryData.SetWindowText(strF);
        }
        break;
        case EDataType::eDouble:
        {
            double d = 0;
            CopyMemory(&d, pBuffer, sizeof(double));
            CString strD;
            strD.Format(_T("%lf"), d);
            m_edtMemoryData.SetWindowText(strD);
        }
        break;
        case EDataType::eANSIString:
        {
            CStringA strA;
            CopyMemory(strA.GetBuffer(nDataSize / sizeof(CHAR)), pBuffer, nDataSize);
            strA.ReleaseBuffer();
            m_edtMemoryData.SetWindowTextW(CString(strA));
        }
        break;
        case EDataType::eUnicodeString:
        {
            CStringW strW;
            CopyMemory(strW.GetBuffer(nDataSize / sizeof(WCHAR)), pBuffer, nDataSize);
            strW.ReleaseBuffer();
            m_edtMemoryData.SetWindowTextW(CString(strW));
        }
        break;
        default:
            break;
        }
    }
    delete[] pBuffer;
    pBuffer = nullptr;
    CloseHandle(hProcess);
}


BOOL CProcessMemoryEditorDlg::PreTranslateMessage(MSG* lpMsg)
{
    if (nullptr == lpMsg) {
        return CDialogEx::PreTranslateMessage(lpMsg);
    }

    if (WM_LBUTTONDOWN == lpMsg->message) {
        if (lpMsg->hwnd == m_sttProcessFromWnd.GetSafeHwnd()) {
            //Start hook procedure
            m_sttProcessFromWnd.SetBitmap(m_hFindingBmp);
            m_hPrevCursor = ::SetCursor(m_hFindingCursor);
            
            if (nullptr != m_hHookEngineDLL) {
                HOOKPROC MouseProc = (HOOKPROC)GetProcAddress(m_hHookEngineDLL, LowlevelMouseProcName);
                if ((nullptr != MouseProcName) && (nullptr != m_pMouseHookEngine)) {
                    m_pMouseHookEngine->Hook(MouseProc, m_hHookEngineDLL, 0);
                    SetCapture();
                }
            }
        }
    }

    else if (WM_KEYDOWN == lpMsg->message) {
        if (VK_RETURN == lpMsg->wParam) {
            return TRUE;
        }

        else if (VK_ESCAPE == lpMsg->wParam) {
            return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(lpMsg);
}

BOOL CProcessMemoryEditorDlg::IsMyWindowFamily(CWnd * pWnd)
{
    if (nullptr == pWnd) {
        return FALSE;
    }

    CWnd* pParent = pWnd;
    CWnd* pDesktop = GetDesktopWindow();
    while (nullptr!=pParent)
    {
        if (pParent == pDesktop) {
            break;
        }
        if (pParent == this) {
            return TRUE;
        }
        pParent = pParent->GetParent();
    }
    return FALSE;
}

LRESULT CProcessMemoryEditorDlg::OnMouseHookEngineNotify(WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT ms = { 0 };
    CopyMemory(&ms, (MSLLHOOKSTRUCT*)lParam, sizeof(MSLLHOOKSTRUCT));
    if (WM_LBUTTONUP == wParam) {
        //Release hook
        if (nullptr != m_pMouseHookEngine) {
            ReleaseCapture();
            m_pMouseHookEngine->UnHook();
            m_sttProcessFromWnd.SetBitmap(m_hFindBmp);
            ::SetCursor(m_hPrevCursor);
        }
    }

    else if (WM_MOUSEMOVE == wParam) {
        CPoint pt = ms.pt;
        CWnd* pWndFromPt = WindowFromPoint(pt);
        if (nullptr != pWndFromPt) {
            if (!IsMyWindowFamily(pWndFromPt)) {
                DWORD dwProcessId = 0;
                DWORD dwThreadId = GetWindowThreadProcessId(pWndFromPt->GetSafeHwnd(), &dwProcessId);
                SetDlgItemInt(IDC_EDT_PROCESS_ID, dwProcessId);                
            }
        }
    }

    return 0L;
}


void CProcessMemoryEditorDlg::OnBnClickedRadHexa()
{
    OnListResultItemChanged();
}


void CProcessMemoryEditorDlg::OnBnClickedRadDecimal()
{
    OnListResultItemChanged();
}


void CProcessMemoryEditorDlg::OnBnClickedBtnStopSearch()
{
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


void CProcessMemoryEditorDlg::OnBnClickedBtnBrowseDll()
{
    static TCHAR BASED_CODE szFilters[] = _T("All File Types(*.*)|*.*|Dynamic Link Library files(*.dll)|*.dll||");

    const UINT nMax = 256;
    TCHAR szFileName[nMax] = { 0 };
    int nChars = m_edtDllName.GetWindowText(szFileName, nMax - 1);
    szFileName[nChars] = 0;

    CFileDialog dlgOpenFile(TRUE, nullptr, szFileName, OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, szFilters);
    if (dlgOpenFile.DoModal() != IDOK) {
        return;
    }

    m_edtDllName.SetWindowText(dlgOpenFile.GetPathName());
}


void CProcessMemoryEditorDlg::OnBnClickedBtnInject()
{
    CString strProcId;
    m_edtProcessID.GetWindowText(strProcId);
    CStringA strProcIdA;
    strProcIdA = strProcId;
    DWORD dwProcID = static_cast<DWORD>(atol(strProcIdA));
    if (0 == dwProcID) {
        AfxMessageBox(_T("Process ID cannot be empty."));
        m_edtProcessID.SetFocus();
        return;
    }

    CString strDllname;
    m_edtDllName.GetWindowText(strDllname);

    SIZE_T nSize = (strDllname.GetLength() * sizeof(TCHAR));
    if (0 >= nSize) {
        AfxMessageBox(_T("Dll name cannot be empty."));
        m_edtDllName.SetFocus();
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
    if (nullptr == hProcess) {
        AfxMessageBox(_T("Cannot attach the process."));
        return;
    }


    HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
    if (nullptr == hKernel32) {
        AfxMessageBox(_T("Cannot get Kernel32 module handle."));
        CloseHandle(hProcess);
        return;
    }

    LPVOID lpBuffer = VirtualAllocEx(hProcess, nullptr, nSize + sizeof(TCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (nullptr == lpBuffer) {
        CloseHandle(hProcess);
        AfxMessageBox(_T("Cannot allocate buffer on target process."));
        return;
    }

    SIZE_T nWritenBytes = 0;
    if (!WriteProcessMemory(hProcess, lpBuffer, strDllname.GetBuffer(), nSize, &nWritenBytes)) {
        AfxMessageBox(_T("Cannot write data to buffer on target process."));
        VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW"), lpBuffer, 0, nullptr);
    if (nullptr == hThread) {
        VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return;
    }
    WaitForSingleObject(hThread, INFINITE);
    DWORD dwExitCode = 0;
    GetExitCodeThread(hThread,  &dwExitCode);
    CopyMemory(&m_hInjectedDll, &dwExitCode, sizeof(DWORD));


    CloseHandle(hThread);
    hThread = nullptr;
    VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE);
    lpBuffer = nullptr;
    CloseHandle(hProcess);
    hProcess = nullptr;
}


void CProcessMemoryEditorDlg::OnEnChangeEdtInjectDll()
{
    m_btnInject.EnableWindow((m_edtDllName.GetWindowTextLength() > 0) && (m_edtProcessID.GetWindowTextLength() > 0));
}


CString CProcessMemoryEditorDlg::GenerateRandomizeText(int nMaxLength)
{
    char characters[] = { 'A', 'B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','Z','W','0','1','2','3','4','5','6','7','8','9',' '};
    srand(nMaxLength);
    CStringA str;
    int nIndex = 0;
    char temp = '\0';
    int range = rand();
    while (range <= 0 || range > nMaxLength) { range = rand(); }
    for(int n = 0; n < range; n++) {
        nIndex = rand();
        while (nIndex < 0 || nIndex>32) { nIndex = rand(); }
        str += (characters[nIndex]);
    }

    return CString( str );
}