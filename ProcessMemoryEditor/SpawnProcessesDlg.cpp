// SpawnProcessesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "SpawnProcessesDlg.h"
#include "afxdialogex.h"
#include <Psapi.h>

// CSpawnProcessesDlg dialog

IMPLEMENT_DYNAMIC(CSpawnProcessesDlg, CDialog)

CSpawnProcessesDlg::CSpawnProcessesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SPAWN_PROCESS, pParent)
{
    
}

CSpawnProcessesDlg::~CSpawnProcessesDlg()
{
}

void CSpawnProcessesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LVW_PROCESSES, m_lvwProcesses);
}


BEGIN_MESSAGE_MAP(CSpawnProcessesDlg, CDialog)
    ON_WM_DESTROY()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LVW_PROCESSES, &CSpawnProcessesDlg::OnLvnItemchangedLvwProcesses)
    ON_NOTIFY(NM_DBLCLK, IDC_LVW_PROCESSES, &CSpawnProcessesDlg::OnNMDblclkLvwProcesses)
END_MESSAGE_MAP()

BOOL CSpawnProcessesDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_lvwProcesses.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_GRIDLINES);
    m_lvwProcesses.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 60);
    m_lvwProcesses.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 350);

    m_dwProcessId = 0;

    SpawnProcesses();

    return TRUE;
}

void CSpawnProcessesDlg::OnDestroy()
{
    CDialog::OnDestroy();
}

BOOL CSpawnProcessesDlg::SpawnProcesses()
{
    DWORD dwCount = 1000;
    DWORD dwSize = dwCount*sizeof(DWORD);
    DWORD * pdwProcessIds = new DWORD[dwCount];
    ZeroMemory(pdwProcessIds, dwSize);

    if (!EnumProcesses(pdwProcessIds, dwSize, &dwSize)) {
        delete[] pdwProcessIds;
        pdwProcessIds = nullptr;
        return FALSE;
    }

    dwCount = dwSize / sizeof(DWORD);
    HANDLE hProcess = nullptr;
    const UINT cchProcessName = 256;
    TCHAR szProcessName[cchProcessName] = { 0 };
    LVITEM lvi = { 0 };
    int nItem = 0;
    CString strProcName;
    int nChars = 0;

    for (DWORD n = 0; n < dwCount; n++) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (pdwProcessIds[n]));
        if (nullptr != hProcess) {
            dwSize = cchProcessName;
            if (QueryFullProcessImageName(hProcess, 0, szProcessName, &dwSize)) {
                szProcessName[dwSize] = 0;
                strProcName = szProcessName;

                nChars = _stprintf_s(szProcessName, cchProcessName, _T("%ld"), pdwProcessIds[n]);
                szProcessName[nChars] = 0;

                ZeroMemory(&lvi, sizeof(LVITEM));
                lvi.iItem = m_lvwProcesses.GetItemCount();
                lvi.iSubItem = 0;
                lvi.pszText = szProcessName;
                lvi.mask = LVIF_TEXT;

                nItem = m_lvwProcesses.InsertItem(&lvi);

                if (-1 != nItem) {
                    lvi.iSubItem++;
                    nChars = _stprintf_s(szProcessName, cchProcessName, _T("%s"), (LPCTSTR)strProcName);
                    szProcessName[nChars] = 0;
                    lvi.pszText = szProcessName;

                    nItem = m_lvwProcesses.SetItem(&lvi);
                }
            }

            CloseHandle(hProcess);
        }
    }

    delete[] pdwProcessIds;
    pdwProcessIds = nullptr;

    return TRUE;
}

void CSpawnProcessesDlg::OnLvnItemchangedLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}


void CSpawnProcessesDlg::OnNMDblclkLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;

    if (nullptr == pNMItemActivate) {
        return;
    }

    const UINT cchItem = 256;
    TCHAR szItem[cchItem] = { 0 };
    
    int nChars = m_lvwProcesses.GetItemText(pNMItemActivate->iItem, 0, szItem, cchItem);
    szItem[nChars] = 0;
    m_dwProcessId = static_cast<DWORD>( atoi(CStringA(szItem)) );
    EndDialog(IDOK);
}
