#pragma once
#include "HookEngine.h"

class HOOKENGINE_API CHookManager : public IHookManager
{
private:
    IHookEngine* m_pEngines[WH_MAX-WH_MIN+1];
public:
    CHookManager& operator=(const CHookManager&) = delete;
    CHookManager();
    virtual ~CHookManager();
    bool registerEngine(int nId, IHookEngine* pEngine);
    IHookEngine* findEngine(int nId);
    bool removeEngine(int nId);
    LRESULT OnHook(int nId, int nCode, WPARAM wParam, LPARAM lParam);
protected:
private:
    void ResetEngine(int Id);
};

extern "C" CHookManager g_HookManager;