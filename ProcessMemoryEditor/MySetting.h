#pragma once

#include "stdafx.h"
#include "ProcessSearchSetting.h"

#define APP_AUTHOR								_T("NKTUYEN")
#define APP_NAME								_T("ProcessMemoryEditor")
#define SETTINGS_KEY							_T("Settings")
#define APP_SETTINGS_KEY						_T("Apps")
#define	TOPMOST_ENTRY							_T("TopMost")
#define SEARCHVALUE_ENTRY						_T("SearchValue")
#define DATATYPE_ENTRY							_T("DataType")
#define	LOOP_WRITE_ELAPSE_ENTRY					_T("LoopWriteElapse")
#define	LASTADDR_ENTRY							_T("LastAddress")
#define	LASTDATA_ENTRY							_T("LastData")

class CMySetting
{
public:
	static CMySetting * GetInstance(const CString& appName);
	inline BOOL IsTopMost() { return m_bTopMost; }
	inline const CString& AppName() { return m_strAppname; }
	CProcessSearchSetting* GetProcessSettings(const CString& processName);

	CProcessSearchSetting* AddProcessSettings(const CString& processName, const CString& searchValue, EDataType eDataType);
	virtual ~CMySetting();
	inline void SetTopMost(int nTopMost) { m_bTopMost = nTopMost; }

	virtual void operator >>(CRegKey& regKey);
	virtual void operator << (CRegKey& regKey);
private:
	CMySetting(const CString& appName);
private:
	static CMySetting* m_pInstance;
	BOOL m_bTopMost;
	CString m_strAppname;
	CMapStringToPtr m_mapProcessSettings;
};

