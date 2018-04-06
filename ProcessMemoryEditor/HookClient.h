#pragma once
#include "..\inc\HookEngine.h"

class CHookClient : public IHookEngine
{
protected:
    CWnd* m_pOwnerWnd;
    UINT m_nCommunicateMessage;
    int m_nId;
    HHOOK m_hHook;
public:
    inline int Id() { return m_nId; }
    inline HHOOK Handle() { return m_hHook; }

    bool Hook(HOOKPROC hookFunc, HINSTANCE hMod, DWORD dwThreadId);
    bool UnHook();

    virtual bool PreHook(int nCode, WPARAM wParam, LPARAM lParam, LRESULT *pResult) { return false; }
    virtual void PostHook(int nCode, WPARAM wParam, LPARAM lParam, LRESULT res) { ; }
protected:
    CHookClient(int nId, CWnd* pOwnerWnd = nullptr, UINT nCommunicateMsg = 0);
    virtual ~CHookClient();
};

