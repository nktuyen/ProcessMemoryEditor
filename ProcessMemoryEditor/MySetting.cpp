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

void CMySetting::operator<<(CRegKey& regKey)
{
	CRegKey regSubKey;
	LSTATUS res = regSubKey.Open(regKey.m_hKey, m_strAppname, KEY_READ);
	if (res != ERROR_SUCCESS)
		return;

	CRegKey regSettingsKey;
	res = regSettingsKey.Open(regSubKey.m_hKey, SETTINGS_KEY, KEY_READ);
	if (res != ERROR_SUCCESS)
		return;

	DWORD dwVal = 0;
	CString strVal;
	res = regSettingsKey.QueryDWORDValue(TOPMOST_ENTRY, dwVal);
	if (res == ERROR_SUCCESS)
	{
		this->SetTopMost(dwVal);
	}

	CString strAppSettingsKey;
	DWORD dwAppSettingsKeyLen = 255;
	DWORD dwIndex = 0;
	res = regSettingsKey.EnumKey(dwIndex, strAppSettingsKey.GetBuffer(255), &dwAppSettingsKeyLen, nullptr);
	while (res == ERROR_SUCCESS)
	{
		strAppSettingsKey.ReleaseBuffer();
		CRegKey regAppSettingsKey;
		LSTATUS resApp = regAppSettingsKey.Open(regSettingsKey.m_hKey, strAppSettingsKey, KEY_READ);
		if (resApp == ERROR_SUCCESS)
		{
			strAppSettingsKey.Replace(_T("//"), _T("\\"));
			CProcessSearchSetting* pAppSettings = AddProcessSettings(strAppSettingsKey, _T(""), EDataType::eUnknow);
			if (pAppSettings)
			{
				CString strValue;
				DWORD dwVal = 0;
				ULONG ulValLen = 255;
				resApp = regAppSettingsKey.QueryStringValue(SEARCHVALUE_ENTRY, strVal.GetBuffer(ulValLen), &ulValLen);
				if (resApp == ERROR_SUCCESS)
				{
					strVal.ReleaseBuffer();
					pAppSettings->SetSearchValue(strVal);
				}

				resApp = regAppSettingsKey.QueryDWORDValue(DATATYPE_ENTRY, dwVal);
				if (resApp == ERROR_SUCCESS)
				{
					pAppSettings->SetDataType((EDataType)dwVal);
				}

				ulValLen = 255;
				resApp = regAppSettingsKey.QueryStringValue(LOOP_WRITE_ELAPSE_ENTRY, strVal.GetBuffer(ulValLen), &ulValLen);
				if (resApp == ERROR_SUCCESS)
				{
					strVal.ReleaseBuffer();
					pAppSettings->SetLoopWriteElapse(strVal);
				}

				ulValLen = 255;
				resApp = regAppSettingsKey.QueryStringValue(LOOP_WRITE_ELAPSE_ENTRY, strVal.GetBuffer(ulValLen), &ulValLen);
				if (resApp == ERROR_SUCCESS)
				{
					strVal.ReleaseBuffer();
					pAppSettings->SetLoopWriteElapse(strVal);
				}

				ulValLen = 255;
				resApp = regAppSettingsKey.QueryStringValue(LASTADDR_ENTRY, strVal.GetBuffer(ulValLen), &ulValLen);
				if (resApp == ERROR_SUCCESS)
				{
					strVal.ReleaseBuffer();
					pAppSettings->SetLastAddress(strVal);
				}

				ulValLen = 255;
				resApp = regAppSettingsKey.QueryStringValue(LASTDATA_ENTRY, strVal.GetBuffer(ulValLen), &ulValLen);
				if (resApp == ERROR_SUCCESS)
				{
					strVal.ReleaseBuffer();
					pAppSettings->SetLastData(strVal);
				}
			}
		}
		//Query next key
		dwAppSettingsKeyLen = 255;
		res = regSettingsKey.EnumKey(++dwIndex, strAppSettingsKey.GetBuffer(255), &dwAppSettingsKeyLen, nullptr);
	}
}

void CMySetting::operator>>(CRegKey& regKey)
{
	DWORD dwDispos = 0;
	CRegKey regSubKey;
	LSTATUS res = regSubKey.Create(regKey.m_hKey, m_strAppname, nullptr, 0, KEY_ALL_ACCESS, nullptr, &dwDispos);
	if (!((res == ERROR_SUCCESS) || (dwDispos == 1 || dwDispos == 2)))
		return;

	dwDispos = 0;
	CRegKey regSettingsKey;
	res = regSettingsKey.Create(regSubKey.m_hKey, SETTINGS_KEY, nullptr, 0, KEY_ALL_ACCESS, nullptr, &dwDispos);
	if (!((res == ERROR_SUCCESS) || (dwDispos == 1 || dwDispos == 2)))
		return;

	regSettingsKey.SetDWORDValue(TOPMOST_ENTRY, this->IsTopMost() ? 1 : 0);

	POSITION pos = m_mapProcessSettings.GetStartPosition();
	CString strProcessName;
	CProcessSearchSetting* pSettings = nullptr;
	while (pos)
	{
		m_mapProcessSettings.GetNextAssoc(pos, strProcessName, (void*&)pSettings);
		if (pSettings != nullptr) {
			CRegKey regAppKey;

			dwDispos = 0; strProcessName.Replace(_T("\\"), _T("//"));
			res = regAppKey.Create(regSettingsKey.m_hKey, strProcessName, nullptr, 0, KEY_ALL_ACCESS, nullptr, &dwDispos);
			if ((res == ERROR_SUCCESS) || (dwDispos == 1 || dwDispos == 2))
			{
				regAppKey.SetStringValue(SEARCHVALUE_ENTRY, pSettings->SearchValue());
				regAppKey.SetDWORDValue(DATATYPE_ENTRY, pSettings->DataType());
				regAppKey.SetStringValue(LASTADDR_ENTRY, pSettings->LastAddress());
				regAppKey.SetStringValue(LASTDATA_ENTRY, pSettings->LastData());
				regAppKey.SetStringValue(LOOP_WRITE_ELAPSE_ENTRY, pSettings->LoopWriteElapse());
			}
		}
	}
}

CMySetting::~CMySetting()
{
	POSITION pos = m_mapProcessSettings.GetStartPosition();
	CString strProcess;
	CProcessSearchSetting* pSetting = nullptr;
	while (pos)
	{
		m_mapProcessSettings.GetNextAssoc(pos, strProcess, (void*&)pSetting);
		if (nullptr != pSetting)
			delete pSetting;
		pSetting = nullptr;
	}
}


CProcessSearchSetting* CMySetting::AddProcessSettings(const CString& processName, const CString& searchValue, EDataType eDataType)
{
	CProcessSearchSetting* mProcSettings = NULL;
	if (!m_mapProcessSettings.Lookup(processName, (void*&)mProcSettings)) {
		mProcSettings = new CProcessSearchSetting();
		m_mapProcessSettings.SetAt(processName, mProcSettings);
	}
	mProcSettings->SetSearchValue(searchValue);
	mProcSettings->SetDataType(eDataType);

	return mProcSettings;
}

CProcessSearchSetting* CMySetting::GetProcessSettings(const CString& processName)
{
	CProcessSearchSetting* mProcSettings = NULL;
	if (m_mapProcessSettings.Lookup(processName, (void*&) mProcSettings)) {
		return mProcSettings;
	}
	return NULL;
}