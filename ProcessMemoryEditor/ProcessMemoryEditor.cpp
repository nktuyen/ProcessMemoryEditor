
// ProcessMemoryEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "SplashDlg.h"
#include "ProcessMemoryEditorDlg.h"
#include "MySetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CProcessMemoryEditorApp::CProcessMemoryEditorApp()
{
	
}

CProcessMemoryEditorApp theApp;


BOOL CProcessMemoryEditorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	SetRegistryKey(_T("NKTUYEN"));
	CMySetting* pSettings = CMySetting::GetInstance(_T("NKTUYEN"));

    CSplashDlg splashDlg;
    splashDlg.DoModal();

	CProcessMemoryEditorDlg dlg(pSettings);
	m_pMainWnd = &dlg;
	LoadSettings();
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		
	}
	SaveSettings();
	delete pSettings;
	return FALSE;
}

#define SETTINGS_KEY							_T("Settings")
#define				TOPMOST_ENTRY				_T("TopMost")
#define				LOOP_WRITE_ELAPSE_ENTRY		_T("LoopWriteElapse")

void CProcessMemoryEditorApp::LoadSettings()
{
	CMySetting* pSettings = CMySetting::GetInstance(_T("NKTUYEN"));
	if (pSettings) {
		pSettings->SetTopMost(GetProfileInt(SETTINGS_KEY, TOPMOST_ENTRY, 0));
		pSettings->SetLoopWriteElapse(GetProfileString(SETTINGS_KEY, LOOP_WRITE_ELAPSE_ENTRY, _T("1")));
	}
}

void CProcessMemoryEditorApp::SaveSettings()
{
	CMySetting* pSettings = CMySetting::GetInstance(_T("NKTUYEN"));
	if (pSettings) {
		WriteProfileInt(SETTINGS_KEY, TOPMOST_ENTRY, pSettings->IsTopMost());
		WriteProfileString(SETTINGS_KEY, LOOP_WRITE_ELAPSE_ENTRY, pSettings->LoopWriteElapse());
	}
}