#include "stdafx.h"
#include "MouseHookEngine.h"

CMouseHookEngine* CMouseHookEngine::s_pInstance = nullptr;


CMouseHookEngine::CMouseHookEngine(CWnd* pWnd /* = nullptr */, UINT nMsgId /* = 0 */)
    :CHookClient(WH_MOUSE_LL, pWnd, nMsgId)
{
    ZeroMemory(&m_mouse, sizeof(MOUSEHOOKSTRUCT));
}


CMouseHookEngine::~CMouseHookEngine()
{
}

void CMouseHookEngine::PostHook(int nCode, WPARAM wParam, LPARAM lParam, LRESULT res)
{
    if (HC_ACTION == nCode) {
        ZeroMemory(&m_mouse, sizeof(MOUSEHOOKSTRUCT));
        CopyMemory(&m_mouse, (MOUSEHOOKSTRUCT*)lParam, sizeof(MOUSEHOOKSTRUCT));
        if ((nullptr != m_pOwnerWnd) && (0 != m_nCommunicateMessage)) {
            m_pOwnerWnd->PostMessage(m_nCommunicateMessage, wParam, (LPARAM)&m_mouse);
        }
    }
}