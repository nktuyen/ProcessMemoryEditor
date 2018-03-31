#pragma once


class CSearchThread : public CWinThread
{
private:
    DWORD m_dwProcessId;
    UINT m_nCommunicateMsg;
    WCHAR *m_pszSearchValueW;
    CHAR *m_pszSearchValueA;
    BYTE m_bySearchvalue;
    SHORT m_shSearchValue;
    WORD m_wSearchvalue;
    DWORD m_dwSearchvalue;
    QWORD m_qwSearchvalue;
    FLOAT m_fSearchvalue;
    DOUBLE m_dblSearchvalue;
    EDataType m_eSearchType;
    int m_nExitCode;
    BOOL m_bCancel;
    CMutex m_locker;
public:
	explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCWSTR lpszSearchValueW);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCSTR lpszSearchValueA);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, BYTE bySearchValue);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, short shSearchValue);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, WORD wSearchValue);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, DWORD dwSearchValue);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, QWORD qwSearchValue);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, float fSearchValue);
    explicit CSearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, double dblSearchValue);

	virtual ~CSearchThread();
    void Stop();
    BOOL IsStopped();
private:
    LRESULT SendMessageToOwner(EThreadNotifyCode code, LPARAM data = NULL);
    LRESULT PostMessageToOwner(EThreadNotifyCode code, LPARAM data = NULL);
    inline void ExitCode(EThreadExitCode eCode) { m_nExitCode = static_cast<int>(eCode); }
    void Construct();
	BOOL InitInstance();
	int ExitInstance();
    int Run();
};


