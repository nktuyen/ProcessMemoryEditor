#pragma once

typedef enum { eSearchNone = 0, eSearchWhole, wSearchSpecifiedAddress }ESearch;

class CMemorySearchThread : public CWinThread
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
    volatile BOOL m_bStop;
    SYSTEM_INFO m_SysInfo;
    HANDLE m_hProcess;
    BYTE* m_pOriginBuffer;
    SIZE_T m_nBufSize;
    CArray<LPVOID, LPVOID> m_arrAddress;
public:
	explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCWSTR lpszSearchValueW, CArray<LPVOID,LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, LPCSTR lpszSearchValueA, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, BYTE bySearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, short shSearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, WORD wSearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, DWORD dwSearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, QWORD qwSearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, float fSearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);
    explicit CMemorySearchThread(CWnd* pOwnerWnd, UINT nCommMsg, DWORD dwProcessId, double dblSearchValue, CArray<LPVOID, LPVOID>* pAddressList = nullptr);

	virtual ~CMemorySearchThread();
    void Stop();
    BOOL IsStopped();
    inline int ExitCode() { return m_nExitCode; }
private:
    LRESULT SendMessageToOwner(EThreadNotifyCode code, LPARAM data = NULL);
    LRESULT PostMessageToOwner(EThreadNotifyCode code, LPARAM data = NULL);
    inline void SetExitCode(EThreadExitCode eCode) { m_nExitCode = static_cast<int>(eCode); }
    void Construct();
    BOOL Init();
	BOOL InitInstance();
	int ExitInstance();
    int Run();
    int SearchFull();
    int SearchInSpecifiedAddresses();
    BOOL IsByteArrayEqual(BYTE* p1, SIZE_T sz1, BYTE* p2, SIZE_T sz2);
};


