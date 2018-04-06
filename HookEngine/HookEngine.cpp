// HookEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HookEngine.h"
#include "HookManager.h"

CHookManager g_HookManager;

HOOKENGINE_API IHookManager* CALLBACK getHookManager(void) {
    return &g_HookManager;
}

// WH_MOUSE handler
HOOKENGINE_API LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return g_HookManager.OnHook(WH_MOUSE, nCode, wParam, lParam);
}

// WH_MOUSE_LL handler
HOOKENGINE_API LRESULT CALLBACK LowlevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return g_HookManager.OnHook(WH_MOUSE_LL, nCode, wParam, lParam);
}

// WH_KEYBOARD handler
HOOKENGINE_API LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return g_HookManager.OnHook(WH_KEYBOARD, nCode, wParam, lParam);
}

// WH_KEYBOARD_LL handler
HOOKENGINE_API LRESULT CALLBACK LowlevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return g_HookManager.OnHook(WH_KEYBOARD_LL, nCode, wParam, lParam);
}

// WH_CBT handler
HOOKENGINE_API LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return g_HookManager.OnHook(WH_CBT, nCode, wParam, lParam);
}
