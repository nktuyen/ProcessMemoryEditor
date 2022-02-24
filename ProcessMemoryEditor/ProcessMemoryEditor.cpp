
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
	SetRegistryKey(APP_AUTHOR);
	CMySetting* pSettings = CMySetting::GetInstance(APP_NAME);

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

void CProcessMemoryEditorApp::LoadSettings()
{
	CMySetting* pSettings = CMySetting::GetInstance(APP_NAME);
	CRegKey regKey;
	CString strSubKey;
	strSubKey.Format(_T("SOFTWARE\\%s"), APP_AUTHOR);
	if (regKey.Open(HKEY_CURRENT_USER, strSubKey) == ERROR_SUCCESS) {
		*(pSettings) << regKey;
	}
}

void CProcessMemoryEditorApp::SaveSettings()
{
	CMySetting* pSettings = CMySetting::GetInstance(APP_NAME);
	if (pSettings) {
		
		CString strSubKey;
		strSubKey.Format(_T("SOFTWARE\\%s"), APP_AUTHOR);

		CRegKey regKey;
		DWORD dwDispos = 0;
		LSTATUS res = regKey.Create(HKEY_CURRENT_USER, strSubKey, nullptr, 0, KEY_ALL_ACCESS, nullptr, &dwDispos);
		if ((res == ERROR_SUCCESS) || (dwDispos == 1 || dwDispos == 2))
		{
			*(pSettings) >> regKey;
		}
	}
}