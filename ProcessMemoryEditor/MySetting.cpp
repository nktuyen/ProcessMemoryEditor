#include "stdafx.h"
#include "MySetting.h"

CMySetting* CMySetting::m_pInstance = NULL;

CMySetting* CMySetting::GetInstance(const CString& appName)
{
	if (CMySetting::m_pInstance == NULL) {
		CMySetting::m_pInstance = new CMySetting(appName);
	}
	return CMySetting::m_pInstance;
}

CMySetting::CMySetting(const CString& appName)
{
	m_bTopMost = FALSE;
	m_strAppname = appName;
}


CMySetting::~CMySetting()
{
}


BOOL CMySetting::AddProcessSettings(const CString& processName, const CString& searchValue, EDataType eDataType)
{
	CProcessSearchSetting* mProcSettings = NULL;
	if (!m_mapProcessSettings.Lookup(processName, (void*&)mProcSettings)) {
		mProcSettings = new CProcessSearchSetting();
		m_mapProcessSettings.SetAt(processName, mProcSettings);
	}
	mProcSettings->SetSearchValue(searchValue);
	mProcSettings->SetDataType(eDataType);

	return TRUE;
}

CProcessSearchSetting* CMySetting::GetProcessSettings(const CString& processName)
{
	CProcessSearchSetting* mProcSettings = NULL;
	if (m_mapProcessSettings.Lookup(processName, (void*&) mProcSettings)) {
		return mProcSettings;
	}
	return NULL;
}