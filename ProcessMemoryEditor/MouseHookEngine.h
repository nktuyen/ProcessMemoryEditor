#pragma once
#include "HookClient.h"
class CMouseHookEngine : public CHookClient
{
private:
    MOUSEHOOKSTRUCT m_mouse;
    static CMouseHookEngine* s_pInstance;
public:
    static CMouseHookEngine* createInstance(CWnd* pWnd, UINT nMsg) {
        if (nullptr == s_pInstance) {
            s_pInstance = new CMouseHookEngine(pWnd, nMsg);
        }
        return s_pInstance;
    }

    static void deleteInstance() {
        if (nullptr != s_pInstance) {
            delete s_pInstance;
            s_pInstance = nullptr;
        }
    }
private:
    CMouseHookEngine(CWnd* pWnd = nullptr, UINT nMsgId = 0);
    virtual ~CMouseHookEngine();
private:
    void PostHook(int nCode, WPARAM wParam, LPARAM lParam, LRESULT res);
};

