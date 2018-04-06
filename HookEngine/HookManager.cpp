#include "HookManager.h"

CHookManager::CHookManager()
{
    for (int i = WH_MIN; i <= WH_MAX; i++) {
        ResetEngine(i);
    }
}

void CHookManager::ResetEngine(int Id)
{
    int nIndex = (Id + 1);
    m_pEngines[nIndex] = nullptr;
}

CHookManager::~CHookManager()
{
}

bool CHookManager::registerEngine(int nId, IHookEngine * pEngine)
{
    if ((nId < WH_MIN) || (nId > WH_MAX)) {
        return false;
    }

    int nIndex = (nId + 1);
    
    if (nullptr != m_pEngines[nIndex]) {
        return false;   //Already exist
    }

    m_pEngines[nIndex] = pEngine;
    return true;
}

IHookEngine* CHookManager::findEngine(int nId)
{
    if ((nId < WH_MIN) || (nId > WH_MAX)) {
        return nullptr;
    }

    int nIndex = (nId + 1);
    return m_pEngines[nIndex];
}

bool CHookManager::removeEngine(int nId)
{
    if ((nId < WH_MIN) || (nId > WH_MAX)) {
        return false;
    }

    int nIndex = (nId + 1);
    if (nullptr != m_pEngines[nIndex]) {
        m_pEngines[nIndex]->UnHook();
        m_pEngines[nIndex] = nullptr;
    }
    return true;
}

LRESULT CHookManager::OnHook(int nId, int nCode, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0L;
    IHookEngine* pEngine = findEngine(nId);
    if (nullptr != pEngine) {
        if (pEngine->PreHook(nCode, wParam, lParam, &res)) {
            return res;
        }

        res = CallNextHookEx(pEngine->Handle(), nCode, wParam, lParam);

        pEngine->PostHook(nCode, wParam, lParam, res);
    }

    return res;    
}
