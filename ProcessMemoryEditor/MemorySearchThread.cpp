// SearchThread.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "MemorySearchThread.h"

#define MAX_SEARCH_STRING_LEN   256

void CMemorySearchThread::Construct()
{
    m_bAutoDelete = FALSE;
    m_nCommunicateMsg = 0;
    m_pszSearchValueA = nullptr;
    m_pszSearchValueW = nullptr;
    m_bySearchvalue = 0;
    m_shSearchValue = 0;
    m_wSearchvalue = 0;
    m_dwSearchvalue = 0;
    m_qwSearchvalue = 0;
    m_fSearchvalue = 0.0f;
    m_dblSearchvalue = 0.0;
    m_pOriginBuffer = nullptr;
    m_nBufSize = 0;
    m_eSearchType = EDataType::eUnknow;
    m_bCancel = TRUE;
    m_dwProcessId = 0;
    m_hProcess = nullptr;
    m_arrAddress.RemoveAll();
    ZeroMemory(&m_SysInfo, sizeof(SYSTEM_INFO));

    GetSystemInfo(&m_SysInfo);
    SetExitCode(EThreadExitCode::eSuccess);
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCWSTR lpszWSearchValueW, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eUnicodeString;
    m_dwProcessId = dwProcessId;
    if (nullptr != m_pszSearchValueW) {
        delete[] m_pszSearchValueW;
        m_pszSearchValueW = nullptr;
    }
    m_pszSearchValueW = new WCHAR[MAX_SEARCH_STRING_LEN];
    ZeroMemory(m_pszSearchValueW, MAX_SEARCH_STRING_LEN * sizeof(WCHAR));
    wcscpy_s(m_pszSearchValueW, MAX_SEARCH_STRING_LEN, lpszWSearchValueW);
    m_nBufSize = sizeof((m_pszSearchValueW));
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, m_pszSearchValueW, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCSTR lpszSearchValueA, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eANSIString;
    m_dwProcessId = dwProcessId;
    if (nullptr != m_pszSearchValueA) {
        delete[] m_pszSearchValueA;
        m_pszSearchValueA = nullptr;
    }
    m_pszSearchValueA = new CHAR[MAX_SEARCH_STRING_LEN];
    ZeroMemory(m_pszSearchValueA, MAX_SEARCH_STRING_LEN * sizeof(CHAR));
    strcpy_s(m_pszSearchValueA, MAX_SEARCH_STRING_LEN, lpszSearchValueA);
    m_nBufSize = sizeof((m_pszSearchValueA));
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, m_pszSearchValueA, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, BYTE bySearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eByte;
    m_dwProcessId = dwProcessId;
    m_bySearchvalue = bySearchValue;
    m_nBufSize = sizeof(BYTE);
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, &m_bySearchvalue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, short shSearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eShort;
    m_dwProcessId = dwProcessId;
    m_shSearchValue = shSearchValue;
    m_nBufSize = sizeof(short);
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, &m_shSearchValue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, WORD wSearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eWord;
    m_dwProcessId = dwProcessId;
    m_wSearchvalue = wSearchValue;
    m_nBufSize = sizeof(WORD);
    m_pOriginBuffer= new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize+1);
    CopyMemory(m_pOriginBuffer, &m_wSearchvalue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, DWORD dwSearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eDoubleWord;
    m_dwProcessId = dwProcessId;
    m_dwSearchvalue = dwSearchValue;
    m_nBufSize = sizeof(DWORD);
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, &m_dwSearchvalue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, QWORD qwSearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eQuartWord;
    m_dwProcessId = dwProcessId;
    m_qwSearchvalue = qwSearchValue;
    m_nBufSize = sizeof(QWORD);
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, &m_qwSearchvalue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, float fSearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eFloat;
    m_dwProcessId = dwProcessId;
    m_fSearchvalue = fSearchValue;
    m_nBufSize = sizeof(float);
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, &m_fSearchvalue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, double dblSearchValue, CArray<LPVOID, LPVOID>* pAddressList)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eDouble;
    m_dwProcessId = dwProcessId;
    m_dblSearchvalue = dblSearchValue;
    m_nBufSize = sizeof(double);
    m_pOriginBuffer = new BYTE[m_nBufSize + 1];
    ZeroMemory(m_pOriginBuffer, m_nBufSize + 1);
    CopyMemory(m_pOriginBuffer, &m_dblSearchvalue, m_nBufSize);
    if (nullptr != pAddressList) {
        m_arrAddress.Copy((*pAddressList));
    }
}

CMemorySearchThread::~CMemorySearchThread()
{

    if (nullptr != m_pOriginBuffer) {
        delete[] m_pOriginBuffer;
        m_pOriginBuffer = nullptr;
    }

    if (nullptr != m_pszSearchValueA) {
        delete[] m_pszSearchValueA;
        m_pszSearchValueA = nullptr;
    }

    if (nullptr != m_pszSearchValueW) {
        delete[] m_pszSearchValueW;
        m_pszSearchValueW = nullptr;
    }
}

LRESULT CMemorySearchThread::SendMessageToOwner(EThreadNotifyCode code, LPARAM data /* = NULL */)
{
    LRESULT res = 0;

    if ((nullptr!=m_pMainWnd) && (nullptr!=m_pMainWnd->GetSafeHwnd()) && (0 != m_nCommunicateMsg)) {
        res = m_pMainWnd->SendMessage(m_nCommunicateMsg, static_cast<WPARAM>(code), data);
    }

    return res;
}

LRESULT CMemorySearchThread::PostMessageToOwner(EThreadNotifyCode code, LPARAM data /* = NULL */)
{
    LRESULT res = 0;

    if ((nullptr != m_pMainWnd) && (nullptr != m_pMainWnd->GetSafeHwnd()) && (0 != m_nCommunicateMsg)) {
        res = m_pMainWnd->PostMessageW(m_nCommunicateMsg, static_cast<WPARAM>(code), data);
    }

    return res;
}

BOOL CMemorySearchThread::InitInstance()
{
	return TRUE;
}

int CMemorySearchThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BOOL CMemorySearchThread::IsStopped()
{
    return m_bCancel;
}

void CMemorySearchThread::Stop()
{
    m_locker.Lock();
    m_bCancel = TRUE;
    SetExitCode(EThreadExitCode::eUserAborted);
    m_locker.Unlock();
}

BOOL CMemorySearchThread::Init()
{
    m_hProcess = nullptr;
    m_bCancel = TRUE;

    if (0 == m_dwProcessId) {
        SetExitCode(EThreadExitCode::eInvalidProcessId);
        return FALSE;
    }

    m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_dwProcessId);
    if (nullptr == m_hProcess) {
        SetExitCode(EThreadExitCode::eProcesssOpenFailed);
        return FALSE;
    }

    m_bCancel = FALSE;
    SetExitCode(EThreadExitCode::eSuccess);

    return TRUE;
}

int CMemorySearchThread::Run()
{
    if (m_arrAddress.GetSize() > 0) {
        return SearchInSpecifiedAddresses();
    }
    else {
        return SearchFull();
    }
}

BOOL CMemorySearchThread::IsByteArrayEqual(BYTE* p1, SIZE_T sz1, BYTE* p2, SIZE_T sz2)
{
    if (sz1 != sz2) {
        return FALSE;
    }

    if ((nullptr == p1) || (nullptr == p2)) {
        if (p1 != p2) {
            return FALSE;
        }

        return TRUE;
    }

    for (SIZE_T index = 0; index < sz1; index++) {
        if (p1[index] != p2[index]) {
            return FALSE;
        }
    }

    return TRUE;
}

int CMemorySearchThread::SearchFull() 
{
    PostMessageToOwner(EThreadNotifyCode::eStart);

    if (Init()) {
        LPVOID ptrCurrentAddress = m_SysInfo.lpMinimumApplicationAddress;
        LPVOID ptrMaxAddress = m_SysInfo.lpMaximumApplicationAddress;

        SIZE_T nStructSize = 0;
        MEMORY_BASIC_INFORMATION memInfo;
        BYTE* pRegionBuffer = nullptr;
        BYTE* pCompareBuffer = new BYTE[m_nBufSize];
        SIZE_T nReadBytes = 0;
        SIZE_T index = 0;
        DWORD dwNewProtection = 0;
        DWORD dwOldProtection = 0;
        BOOL bReadable = FALSE;

        do
        {
            m_locker.Lock();
            if (IsStopped()) {
                m_locker.Unlock();
                break;
            }
            m_locker.Unlock();

            ZeroMemory(&memInfo, sizeof(MEMORY_BASIC_INFORMATION));
            nStructSize = VirtualQueryEx(m_hProcess, ptrCurrentAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
            if ( (0 < nStructSize) && (MEM_COMMIT == memInfo.State) ) {
                bReadable = TRUE;
                if ((PAGE_READONLY != memInfo.Protect) && (PAGE_READWRITE != memInfo.Protect)) {
                    dwNewProtection = memInfo.Protect | PAGE_READWRITE;
                    bReadable = VirtualProtectEx(m_hProcess, memInfo.BaseAddress, memInfo.RegionSize, dwNewProtection, &dwOldProtection);
                }

                if ( (bReadable) && (0 < memInfo.RegionSize) ) {
                    pRegionBuffer = new  BYTE[memInfo.RegionSize];
                    ZeroMemory(pRegionBuffer, memInfo.RegionSize);
                    nReadBytes = 0;
                    try
                    {
                        if (ReadProcessMemory(m_hProcess, memInfo.BaseAddress, pRegionBuffer, memInfo.RegionSize, &nReadBytes)) {
                            if (0 < nReadBytes) {
                                for (index = 0; index < (memInfo.RegionSize - m_nBufSize); index++) {
                                    m_locker.Lock();
                                    if (IsStopped()) {
                                        m_locker.Unlock();
                                        break;
                                    }
                                    m_locker.Unlock();

                                    ZeroMemory(pCompareBuffer, m_nBufSize);
                                    CopyMemory(pCompareBuffer, pRegionBuffer + index, m_nBufSize);
                                    if (IsByteArrayEqual(pCompareBuffer, m_nBufSize, m_pOriginBuffer, m_nBufSize)) {
                                        PostMessageToOwner(EThreadNotifyCode::eData, (LPARAM)LPVOID(SIZE_T(memInfo.BaseAddress) + index));
                                    }
                                }
                            }
                        }
                    }
                    catch (...) {;}

                    if (nullptr != pRegionBuffer) {
                        delete[] pRegionBuffer;
                        pRegionBuffer = nullptr;
                    }
                }
            }

            ptrCurrentAddress = LPVOID(SIZE_T(memInfo.BaseAddress) + memInfo.RegionSize);
        } while (ptrCurrentAddress < ptrMaxAddress);   //End of "while (ptrCurrentAddress < ptrMaxAddress)"

        if (nullptr != pCompareBuffer) {
            delete[] pCompareBuffer;
            pCompareBuffer = nullptr;
        }

        if (nullptr != pRegionBuffer) {
            delete[] pRegionBuffer;
            pRegionBuffer = nullptr;
        }

        CloseHandle(m_hProcess);
        m_hProcess = nullptr;
    }

    PostMessageToOwner(EThreadNotifyCode::eFinish, (LPARAM)m_nExitCode);
    return m_nExitCode;
}

int CMemorySearchThread::SearchInSpecifiedAddresses()
{
    PostMessageToOwner(EThreadNotifyCode::eStart);

    if (Init()) {
        INT_PTR nSize = m_arrAddress.GetCount();
        PostMessageToOwner(EThreadNotifyCode::eProgressRange, MAKELPARAM(0, 100));

        INT_PTR nCount = m_arrAddress.GetCount();
        LPVOID lpCurrentAddress = nullptr;
        BYTE* pBuffer = new BYTE[m_nBufSize];
        SIZE_T nReadBytes = 0;

        for (INT_PTR n = 0; n < nCount; n++) {
            lpCurrentAddress = m_arrAddress.GetAt(n);
            if (nullptr != lpCurrentAddress) {
                ZeroMemory(pBuffer, m_nBufSize);
                nReadBytes = 0;
                if (ReadProcessMemory(m_hProcess, lpCurrentAddress, pBuffer, m_nBufSize, &nReadBytes)) {
                    if (0 < nReadBytes) {
                        if (IsByteArrayEqual(pBuffer, m_nBufSize, m_pOriginBuffer, m_nBufSize)) {
                            PostMessageToOwner(EThreadNotifyCode::eData, (LPARAM)lpCurrentAddress);
                        }
                    }
                }
            }

            PostMessageToOwner(EThreadNotifyCode::eProgressValue, (LPARAM)int(float(n)/float(nCount)*100.0f));
        }

        CloseHandle(m_hProcess);
        m_hProcess = nullptr;
    }

    PostMessageToOwner(EThreadNotifyCode::eFinish, (LPARAM)m_nExitCode);
    return m_nExitCode;
}