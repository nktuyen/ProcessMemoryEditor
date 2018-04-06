#include "stdafx.h"
#include "ProcessInfo.h"


CProcessInfo::CProcessInfo(DWORD dwId, LPCTSTR lpszName, HICON hIcon/* =nullptr */, int nIconIndex /* = -1 */)
{
    m_dwId = dwId;
    m_strName = lpszName;
    m_hIcon = hIcon;
    m_nIconIndex = nIconIndex;
    m_hwndMain = nullptr;
}


CProcessInfo::~CProcessInfo()
{
    if (nullptr != m_hIcon) {
        DestroyIcon(m_hIcon);
        m_hIcon = nullptr;
    }
}
