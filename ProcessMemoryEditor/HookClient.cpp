#include "stdafx.h"
#include "HookClient.h"

CHookClient::CHookClient(int nId,CWnd * pOwnerWnd, UINT nCommunicateMsg)
{
    m_hHook = nullptr;
    m_nId = nId;
    m_pOwnerWnd = pOwnerWnd;
    m_nCommunicateMessage = nCommunicateMsg;
}

CHookClient::~CHookClient()
{
}


bool CHookClient::Hook(HOOKPROC hookFunc, HINSTANCE hMod, DWORD dwThreadId)
{
    if (nullptr != m_hHook) {
        return true;
    }

    m_hHook = ::SetWindowsHookEx(Id(), hookFunc, hMod, dwThreadId);

    return (nullptr != m_hHook);
}

bool CHookClient::UnHook()
{
    if (nullptr == m_hHook) {
        return true;
    }

    if (::UnhookWindowsHookEx(m_hHook)) {
        m_hHook = nullptr;
    }

    return (nullptr == m_hHook);
}
