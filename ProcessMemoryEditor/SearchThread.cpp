// SearchThread.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "SearchThread.h"

#define MAX_SEARCH_STRING_LEN   256

void CSearchThread::Construct()
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
    m_eSearchType = EDataType::eUnknow;
    m_bCancel = TRUE;
    m_dwProcessId = 0;
    ExitCode(EThreadExitCode::eThreadExitSuccess);
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCWSTR lpszWSearchValue)
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
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCSTR lpszSearchValueA)
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
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, BYTE bySearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eByte;
    m_dwProcessId = dwProcessId;
    m_bySearchvalue = bySearchValue;
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, short shSearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eShort;
    m_dwProcessId = dwProcessId;
    m_shSearchValue = shSearchValue;
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, WORD wSearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eWord;
    m_dwProcessId = dwProcessId;
    m_wSearchvalue = wSearchValue;
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, DWORD dwSearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eDoubleWord;
    m_dwProcessId = dwProcessId;
    m_dwSearchvalue = dwSearchValue;
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, QWORD qwSearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eQuartWord;
    m_dwProcessId = dwProcessId;
    m_qwSearchvalue = qwSearchValue;
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, float fSearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eFloat;
    m_dwProcessId = dwProcessId;
    m_fSearchvalue = fSearchValue;
}

CSearchThread::CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, double dblSearchValue)
{
    Construct();
    m_pMainWnd = pOwnerWnd;
    m_nCommunicateMsg = nCommMsg;
    m_eSearchType = EDataType::eDouble;
    m_dwProcessId = dwProcessId;
    m_dblSearchvalue = dblSearchValue;
}

CSearchThread::~CSearchThread()
{
    if (nullptr != m_pszSearchValueA) {
        delete[] m_pszSearchValueA;
        m_pszSearchValueA = nullptr;
    }

    if (nullptr != m_pszSearchValueW) {
        delete[] m_pszSearchValueW;
        m_pszSearchValueW = nullptr;
    }
}

LRESULT CSearchThread::SendMessageToOwner(EThreadNotifyCode code, LPARAM data /* = NULL */)
{
    LRESULT res = 0;

    if ((nullptr!=m_pMainWnd) && (nullptr!=m_pMainWnd->GetSafeHwnd()) && (0 != m_nCommunicateMsg)) {
        res = m_pMainWnd->SendMessage(m_nCommunicateMsg, static_cast<WPARAM>(code), data);
    }

    return res;
}

LRESULT CSearchThread::PostMessageToOwner(EThreadNotifyCode code, LPARAM data /* = NULL */)
{
    LRESULT res = 0;

    if ((nullptr != m_pMainWnd) && (nullptr != m_pMainWnd->GetSafeHwnd()) && (0 != m_nCommunicateMsg)) {
        res = m_pMainWnd->PostMessageW(m_nCommunicateMsg, static_cast<WPARAM>(code), data);
    }

    return res;
}

BOOL CSearchThread::InitInstance()
{
	return TRUE;
}

int CSearchThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BOOL CSearchThread::IsStopped()
{
    return m_bCancel;
}

void CSearchThread::Stop()
{
    m_locker.Lock();
    m_bCancel = TRUE;
    m_locker.Unlock();
}

int CSearchThread::Run()
{
    m_bCancel = FALSE;
    PostMessageToOwner(EThreadNotifyCode::eStart);
    DWORD_PTR ptrAddress = 0x1001;


    while (ptrAddress < 0xFFFFFFF)
    {
        m_locker.Lock();
        if (IsStopped()) {
            m_locker.Unlock();
            break;
        }
        m_locker.Unlock();

        PostMessageToOwner(EThreadNotifyCode::eData, (LPARAM)ptrAddress);

        Sleep(10);
        ptrAddress += 0x2;
    }

    PostMessageToOwner(EThreadNotifyCode::eFinish, (LPARAM)m_nExitCode);
    return 0;
}