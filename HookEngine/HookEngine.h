// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HOOKENGINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HOOKENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef __HOOK_ENGINE_DLL__
#define __HOOK_ENGINE_DLL__

#include <Windows.h>

#ifdef HOOKENGINE_EXPORTS
#define HOOKENGINE_API __declspec(dllexport)
#else
#define HOOKENGINE_API __declspec(dllimport)
#endif

#define GetHookManagerFunctionName      "getHookManager"
#define MouseProcName                   "MouseProc"
#define LowlevelMouseProcName           "LowlevelMouseProc"
#define KeyboardProcName                "KeyboardProc"
#define LowlevelKeyboardProcName        "LowlevelKeyboardProc"
#define CBTProcName                     "CBTProc"


__interface IHookEngine
{
public:
    virtual int Id();
    virtual HHOOK Handle();
    virtual bool Hook(HOOKPROC hookFunc, HINSTANCE hMod, DWORD dwThreadId);
    virtual bool UnHook();
    virtual bool PreHook(int nCode, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
    virtual void PostHook(int nCode, WPARAM wParam, LPARAM lParam, LRESULT res);
};

__interface IHookManager
{
public:
    virtual bool registerEngine(int nId, IHookEngine* pEngine);
    virtual IHookEngine* findEngine(int nId);
    virtual bool removeEngine(int nId);
};


typedef IHookManager* (WINAPI* GETHOOKMANAGER)(void);

HOOKENGINE_API LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
HOOKENGINE_API LRESULT CALLBACK LowlevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

#endif // !__HOOK_ENGINE_DLL__  