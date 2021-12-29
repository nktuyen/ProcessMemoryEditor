#pragma once

#include "stdafx.h"
#include "ProcessSearchSetting.h"
class CMySetting
{
public:
	static CMySetting * GetInstance(const CString& appName);
	inline BOOL IsTopMost() { return m_bTopMost; }
	inline const CString& AppName() { return m_strAppname; }
	CProcessSearchSetting* GetProcessSettings(const CString& processName);
	BOOL AddProcessSettings(const CString& processName, const CString& searchValue, EDataType eDataType);
	virtual ~CMySetting();
	inline void SetTopMost(int nTopMost) { m_bTopMost = nTopMost; }
private:
	CMySetting(const CString& appName);
private:
	static CMySetting* m_pInstance;
	BOOL m_bTopMost;
	CString m_strAppname;
	CMapStringToPtr m_mapProcessSettings;
};

