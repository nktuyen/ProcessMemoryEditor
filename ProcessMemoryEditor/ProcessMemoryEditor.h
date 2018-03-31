
// ProcessMemoryEditor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

class CProcessMemoryEditorApp : public CWinApp
{
public:
	CProcessMemoryEditorApp();
	BOOL InitInstance();
};

extern CProcessMemoryEditorApp theApp;