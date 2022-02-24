#pragma once
class CProcessSearchSetting
{
public:
	CProcessSearchSetting();
	virtual ~CProcessSearchSetting();
	inline const CString& SearchValue() { return m_strValue; }
	inline EDataType DataType() { return m_eDataType; }
	inline const CString& LoopWriteElapse() { return m_strLoopWriteElapse; }
	inline const CString& LastAddress() { return m_strLastAddress; }
	inline const CString& LastData() { return m_strLastData; }

	inline void SetSearchValue(const CString& value) { m_strValue = value; }
	inline void SetDataType(EDataType eType) { m_eDataType = eType; }
	inline void SetLoopWriteElapse(const CString& elapse) { m_strLoopWriteElapse = elapse; }
	inline void SetLastAddress(const CString& addr) { m_strLastAddress = addr; }
	inline void SetLastData(const CString& data) { m_strLastData = data; }
private:
	CString m_strValue;
	CString m_strLoopWriteElapse;
	EDataType m_eDataType;
	CString m_strLastAddress;
	CString m_strLastData;
};

