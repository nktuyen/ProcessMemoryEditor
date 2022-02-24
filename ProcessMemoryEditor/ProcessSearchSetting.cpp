#include "stdafx.h"
#include "ProcessSearchSetting.h"


CProcessSearchSetting::CProcessSearchSetting():
	m_strValue(),
	m_eDataType(EDataType::eUnknow),
	m_strLoopWriteElapse(),
	m_strLastAddress(),
	m_strLastData()
{
}


CProcessSearchSetting::~CProcessSearchSetting()
{
}
