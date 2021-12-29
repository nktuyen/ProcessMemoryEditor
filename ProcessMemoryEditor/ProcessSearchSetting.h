#pragma once
class CProcessSearchSetting
{
public:
	CProcessSearchSetting();
	virtual ~CProcessSearchSetting();
	inline const CString& SearchValue() { return m_strValue; }
	inline EDataType DataType() { return m_eDataType; }
	inline void SetSearchValue(const CString& value) { m_strValue = value; }
	inline void SetDataType(EDataType eType) { m_eDataType = eType; }
private:
	CString m_strValue;
	EDataType m_eDataType;
};

