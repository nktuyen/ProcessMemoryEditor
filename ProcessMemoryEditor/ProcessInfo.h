#pragma once
#include "afx.h"
class CProcessInfo : public CObject
{
private:
    HICON m_hIcon;
    int m_nIconIndex;
    CString m_strName;
    DWORD m_dwId;
    BOOL m_bSelected;
    HWND m_hwndMain;
public:
    CProcessInfo(DWORD dwId, LPCTSTR lpszName, HICON hIcon=nullptr, int nIconIndex = -1);
    virtual ~CProcessInfo();
    inline HICON getIcon() { return m_hIcon; }
    inline int getIconIndex() { return m_nIconIndex; }
    inline const CString& getName() { return m_strName; }
    inline DWORD getID() { return m_dwId; }
    inline void setIcon(HICON hIcon, int index) { m_hIcon = hIcon; m_nIconIndex = index; }
    inline void setSelected(BOOL bSelected) { m_bSelected = bSelected; }
    inline BOOL isSelected() { return m_bSelected; }
};

