// SpawnProcessesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "SpawnProcessesDlg.h"
#include "afxdialogex.h"
#include "ProcessInfo.h"
#include <Psapi.h>
#include <algorithm>

// CSpawnProcessesDlg dialog

IMPLEMENT_DYNAMIC(CSpawnProcessesDlg, CDialog)


CSpawnProcessesDlg::CSpawnProcessesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SPAWN_PROCESS, pParent)
{
    m_nSortColumn = 0;
    for (int i = 0; i < m_nColumns; i++) {
        m_bSortDirection[i] = FALSE;
    }
}

CSpawnProcessesDlg::~CSpawnProcessesDlg()
{
    DeleteAllProcesses();
}

void CSpawnProcessesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LVW_PROCESSES, m_lvwProcesses);
    DDX_Control(pDX, IDC_BTN_SEARCH_PROC_NAME, m_btnSearch);
    DDX_Control(pDX, IDC_EDT_SEARCH_PROC_NAME, m_edtProcName);
}


BEGIN_MESSAGE_MAP(CSpawnProcessesDlg, CDialog)
    ON_WM_DESTROY()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LVW_PROCESSES, &CSpawnProcessesDlg::OnLvnItemchangedLvwProcesses)
    ON_NOTIFY(NM_DBLCLK, IDC_LVW_PROCESSES, &CSpawnProcessesDlg::OnNMDblclkLvwProcesses)
    ON_BN_CLICKED(IDC_BTN_REFRESH, &CSpawnProcessesDlg::OnBnClickedBtnRefresh)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LVW_PROCESSES, &CSpawnProcessesDlg::OnLvnGetdispinfoLvwProcesses)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LVW_PROCESSES, &CSpawnProcessesDlg::OnLvnColumnclickLvwProcesses)
    ON_BN_CLICKED(IDC_BTN_SEARCH_PROC_NAME, &CSpawnProcessesDlg::OnBnClickedBtnSearchProcName)
    ON_EN_CHANGE(IDC_EDT_SEARCH_PROC_NAME, &CSpawnProcessesDlg::OnEnChangeEdtSearchProcName)
END_MESSAGE_MAP()

BOOL CSpawnProcessesDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_lvwProcesses.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_GRIDLINES);
    m_lvwProcesses.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 70);
    m_lvwProcesses.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 450);

    m_imgList.Create(15, 15, ILC_COLOR32 | ILC_MASK, int(0), int(100));
    m_lvwProcesses.SetImageList(&m_imgList, LVSIL_SMALL);

    m_dwProcessId = 0;

    m_edtProcName.SetLimitText(256);

    OnBnClickedBtnRefresh();

    return TRUE;
}

void CSpawnProcessesDlg::OnDestroy()
{
    CDialog::OnDestroy();
}

void CSpawnProcessesDlg::DeleteAllProcesses()
{
    std::vector<CProcessInfo*>::iterator Iter = m_lstProcesses.begin();
    CProcessInfo* pProc = nullptr;
    DWORD dwProcId = 0;
    for(;Iter!=m_lstProcesses.end();++Iter)
    {
        pProc = (*Iter);
        if (nullptr != pProc) {
            delete pProc;
            pProc = nullptr;
        }
    }

    m_lstProcesses.clear();
}

BOOL CSpawnProcessesDlg::SpawnProcesses()
{
    m_mutex.Lock();
    DeleteAllProcesses();
    while (m_imgList.GetImageCount() > 0) { m_imgList.Remove(0); }

    DWORD dwCount = 1000;
    DWORD dwSize = dwCount*sizeof(DWORD);
    DWORD * pdwProcessIds = new DWORD[dwCount];
    ZeroMemory(pdwProcessIds, dwSize);

    if (!EnumProcesses(pdwProcessIds, dwSize, &dwSize)) {
        delete[] pdwProcessIds;
        pdwProcessIds = nullptr;
        m_mutex.Unlock();
        return FALSE;
    }

    dwCount = dwSize / sizeof(DWORD);
    HANDLE hProcess = nullptr;
    const UINT cchProcessName = 256;
    TCHAR szProcessName[cchProcessName] = { 0 };
    int nChars = 0;
    SHFILEINFO si = { 0 };
    DWORD_PTR dwRet = 0;
    CProcessInfo* pProc = nullptr;

    for (DWORD n = 0; n < dwCount; n++) {
        if(pdwProcessIds[n] == GetCurrentProcessId())
            continue;

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pdwProcessIds[n]);
        if (nullptr != hProcess) {
            dwSize = cchProcessName;
            if (QueryFullProcessImageName(hProcess, 0, szProcessName, &dwSize)) {
                szProcessName[dwSize] = 0;

                pProc = new CProcessInfo(pdwProcessIds[n], szProcessName);
                m_lstProcesses.push_back(pProc);

                ZeroMemory(&si, sizeof(SHFILEINFO));
                dwRet = SHGetFileInfo(szProcessName, 0, &si, sizeof(SHFILEINFO), SHGFI_ICON);
                if (0 != dwRet) {
                    pProc->setIcon(si.hIcon, m_imgList.Add(si.hIcon));
                }
            }

            CloseHandle(hProcess);
        }
    }

    delete[] pdwProcessIds;
    pdwProcessIds = nullptr;
    m_mutex.Unlock();
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


void CSpawnProcessesDlg::OnBnClickedBtnRefresh()
{
    BOOL bEnabled = m_btnSearch.IsWindowEnabled();
    m_btnSearch.EnableWindow(FALSE);
    m_edtProcName.SetWindowText(nullptr);
    SpawnProcesses();
    OnBnClickedBtnSearchProcName();
    m_btnSearch.EnableWindow(bEnabled);
}


void CSpawnProcessesDlg::OnLvnGetdispinfoLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;
    m_mutex.Lock();
    int nItem = pDispInfo->item.iItem;
    CProcessInfo* pProc = m_lstFilteredProcesses.at(nItem);
    if (nullptr != pProc) {
        switch (pDispInfo->item.iSubItem)
        {
        case 0: //ID
        {
            CString strID;
            
            strID.Format(_T("%d"), pProc->getID());
            if (0 < pDispInfo->item.cchTextMax) {
                _tcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, strID);
                if ((pDispInfo->item).mask & LVIF_IMAGE) {
                    pDispInfo->item.iImage = pProc->getIconIndex();
                }
                if (pProc->isSelected()) {
                    pDispInfo->item.state |= LVIS_SELECTED;
                }
                else {
                    //pDispInfo->item.state = 0;
                }
            }
        }
        break;
        case 1: //Name
        {
            if (0 < pDispInfo->item.cchTextMax) {
                _tcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, pProc->getName());
            }
        }
        break;
        default:
            break;
        }
    }

    m_mutex.Unlock();
}

struct{
    bool operator()(CProcessInfo* p1, CProcessInfo* p2) const {
        return _tcscmp(p1->getName(), p2->getName()) > 0;
    }
}ProcCmpAsc;
struct {
    bool operator()(CProcessInfo* p1, CProcessInfo* p2) const {
        return _tcscmp(p1->getName(), p2->getName()) < 0;
    }
}ProcCmpDesc;

BOOL CSpawnProcessesDlg::SortItems()
{
    if (0 == m_nSortColumn) {
        m_mutex.Lock();
        std::sort(m_lstFilteredProcesses.begin(), m_lstFilteredProcesses.end(), [this](CProcessInfo* p1, CProcessInfo* p2) {
            if (m_bSortDirection[m_nSortColumn]) {
                return p1->getID() > p2->getID();
            }
            else {
                return p1->getID() < p2->getID();
            }
        });
        m_mutex.Unlock();
        return TRUE;
    }

    else if (1 == m_nSortColumn) {
        m_mutex.Lock();
        if (m_bSortDirection[m_nSortColumn]) {
            std::sort(m_lstFilteredProcesses.begin(), m_lstFilteredProcesses.end(), ProcCmpAsc);
        }
        else {
            std::sort(m_lstFilteredProcesses.begin(), m_lstFilteredProcesses.end(), ProcCmpDesc);
        }

        m_mutex.Unlock();
        return TRUE;
    }

    return FALSE;
}

void CSpawnProcessesDlg::OnLvnColumnclickLvwProcesses(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    if ( (nullptr == pNMLV) || (pNMLV->iItem != -1) )
        return;

    m_nSortColumn = pNMLV->iSubItem;
    m_bSortDirection[m_nSortColumn] = (!m_bSortDirection[m_nSortColumn]);
    OnBnClickedBtnSearchProcName();
}


void CSpawnProcessesDlg::OnBnClickedBtnSearchProcName()
{
    m_lstFilteredProcesses.clear();
    if (0 >= m_lstProcesses.size()) {
        m_lvwProcesses.DeleteAllItems();
        return;
    }

    CString strSearch;
    m_edtProcName.GetWindowText(strSearch);
    
    std::vector<CProcessInfo*>::const_iterator cIter = m_lstProcesses.begin();
    CString strProcName;
    CProcessInfo* pProc = nullptr;
    CString strSub;

    m_mutex.Lock();
    for (; cIter != m_lstProcesses.end(); ++cIter) {
        pProc = (*cIter);
        if (nullptr != pProc) {
            if(0>=strSearch.GetLength()){
                m_lstFilteredProcesses.push_back(pProc);
            }
            else {
                strProcName = pProc->getName();
                for (int i = 0; i < strProcName.GetLength() - strSearch.GetLength(); i++) {
                    strSub = strProcName.Mid(i, strSearch.GetLength());
                    if (0 == strSub.CompareNoCase(strSearch)) {
                        m_lstFilteredProcesses.push_back(pProc);
                        break;
                    }
                }
            }
        }
    }
    m_mutex.Unlock();

    SortItems();
    strSub.Format(_T("Total: %d processes"), m_lstFilteredProcesses.size());
    SetDlgItemText(IDC_STT_STATUS, strSub);

    m_lvwProcesses.SetItemCount(static_cast<int>( m_lstFilteredProcesses.size()) );
    m_lvwProcesses.RedrawItems(0, static_cast<int>( m_lstFilteredProcesses.size() - 1) );

}


void CSpawnProcessesDlg::OnEnChangeEdtSearchProcName()
{
    OnBnClickedBtnSearchProcName();
    m_btnSearch.EnableWindow(m_edtProcName.GetWindowTextLength() > 0);
}
