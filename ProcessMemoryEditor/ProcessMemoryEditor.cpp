
// ProcessMemoryEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ProcessMemoryEditor.h"
#include "ProcessMemoryEditorDlg.h"

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

	CProcessMemoryEditorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		
	}

	return FALSE;
}

