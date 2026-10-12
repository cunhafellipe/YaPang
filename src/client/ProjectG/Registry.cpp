// Original: d:\build\custom\temp\code\client\projectg\registry.cpp
#include "Registry.h"

#define CLASS_NAME_LENGTH 255

// registry.cpp:28
CRegistry::CRegistry(void) {
	m_hRootKey = HKEY_CURRENT_USER;
	m_bLazyWrite = TRUE;
	m_nLastError = 0;
}

// registry.cpp:35
CRegistry::~CRegistry(void) {
	ClearKey();
}

// registry.cpp:41
int CRegistry::ClearKey(void) {
	m_strCurrentPath.Empty();
	m_hRootKey = HKEY_CURRENT_USER;
	m_bLazyWrite = TRUE;
	return TRUE;
}

// registry.cpp:55
int CRegistry::SetRootKey(HKEY hRootKey) {
	if (hRootKey != HKEY_CLASSES_ROOT && hRootKey != HKEY_CURRENT_USER && hRootKey != HKEY_LOCAL_MACHINE && hRootKey != HKEY_USERS)
		return FALSE;

	m_hRootKey = hRootKey;
	return TRUE;
}

// registry.cpp:69
int CRegistry::CreateKey(WTL::CString strKey) {
	HKEY hKey;
	DWORD dwDisposition = 0;

	if (::RegCreateKeyEx(m_hRootKey, LPCTSTR(strKey), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
		&dwDisposition) != ERROR_SUCCESS) return FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	m_strCurrentPath = strKey;
	return TRUE;
}

// registry.cpp:93
int CRegistry::DeleteKey(WTL::CString strKey) {
	if (!KeyExists(strKey, NULL))
		return TRUE;
	if (::RegDeleteKey(m_hRootKey, strKey) != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}

// registry.cpp:112
int CRegistry::DeleteValue(WTL::CString strName) {
	HKEY hKey;
	LONG lResult;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
		return FALSE;

	lResult = ::RegDeleteValue(hKey, LPCTSTR(strName));
	::RegCloseKey(hKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

// registry.cpp:135
int CRegistry::GetDataSize(WTL::CString strValueName) {
	HKEY hKey;
	LONG lResult;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return -1;

	DWORD dwSize = 1;
	lResult = ::RegQueryValueEx(hKey, LPCTSTR(strValueName),
		NULL, NULL, NULL, &dwSize);
	::RegCloseKey(hKey);

	if (lResult != ERROR_SUCCESS) return -1;
	return (int)dwSize;
}

// registry.cpp:159
unsigned long CRegistry::GetDataType(WTL::CString strValueName) {
	HKEY hKey;
	m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_QUERY_VALUE, &hKey);

	if (m_nLastError != ERROR_SUCCESS)
		return 0;

	DWORD dwType = 1;
	m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strValueName), NULL, &dwType, NULL, NULL);
	::RegCloseKey(hKey);

	if (m_nLastError == ERROR_SUCCESS)
		return dwType;
	return 0;
}

// registry.cpp:181
int CRegistry::GetSubKeyCount(void) {
	HKEY hKey;
	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return -1;

	LONG lResult;
	DWORD dwSubKeyCount, dwValueCount, dwClassNameLength, dwMaxSubKeyName, dwMaxValueName, dwMaxValueLength;
	FILETIME ftLastWritten;

	TCHAR szClassBuffer[CLASS_NAME_LENGTH];

	dwClassNameLength = CLASS_NAME_LENGTH;
	lResult = ::RegQueryInfoKey(hKey, szClassBuffer, &dwClassNameLength, NULL, &dwSubKeyCount, &dwMaxSubKeyName, NULL, &dwValueCount, &dwMaxValueName, &dwMaxValueLength, NULL, &ftLastWritten);

	::RegCloseKey(hKey);
	if (lResult != ERROR_SUCCESS)
		return -1;

	return (int)dwSubKeyCount;
}

// registry.cpp:210
int CRegistry::GetValueCount(void) {
	HKEY hKey;
	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return -1;

	LONG lResult;
	DWORD dwSubKeyCount, dwValueCount, dwClassNameLength, dwMaxSubKeyName, dwMaxValueName, dwMaxValueLength;
	FILETIME ftLastWritten;

	TCHAR szClassBuffer[CLASS_NAME_LENGTH];

	dwClassNameLength = CLASS_NAME_LENGTH;
	lResult = ::RegQueryInfoKey(hKey, szClassBuffer, &dwClassNameLength, NULL, &dwSubKeyCount, &dwMaxSubKeyName, NULL, &dwValueCount, &dwMaxValueName, &dwMaxValueLength, NULL, &ftLastWritten);

	::RegCloseKey(hKey);
	if (lResult != ERROR_SUCCESS)
		return -1;

	return (int)dwValueCount;
}

// registry.cpp:239
int CRegistry::KeyExists(WTL::CString strKey, HKEY hRootKey) {
	HKEY hKey;

	if (hRootKey == NULL)
		hRootKey = m_hRootKey;

	LONG lResult = ::RegOpenKeyEx(hRootKey, LPCTSTR(strKey), 0, KEY_ALL_ACCESS, &hKey);
	::RegCloseKey(hKey);
	if (lResult == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

// registry.cpp:256
int CRegistry::SetKey(WTL::CString strKey, int bCanCreate) {
	HKEY hKey;

	if (strKey.GetLength() == 0) {
		m_strCurrentPath.Empty();
		return TRUE;
	}

	DWORD dwDisposition;
	if (bCanCreate) {
		if (::RegCreateKeyEx(m_hRootKey, LPCTSTR(strKey), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
			return FALSE;
		m_strCurrentPath = strKey;
		if (!m_bLazyWrite)
			::RegFlushKey(hKey);
		::RegCloseKey(hKey);
		return TRUE;
	}

	m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(strKey), 0, KEY_ALL_ACCESS, &hKey);
	if (m_nLastError != ERROR_SUCCESS)
		return FALSE;
	m_strCurrentPath = strKey;
	if (!m_bLazyWrite)
		::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return TRUE;
}

// registry.cpp:306
int CRegistry::ValueExists(WTL::CString strName) {
	HKEY hKey;
	LONG lResult;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return FALSE;

	lResult = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, NULL, NULL, NULL);
	::RegCloseKey(hKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

// registry.cpp:332
void CRegistry::RenameValue(WTL::CString strOldName, WTL::CString strNewName) {
}

// registry.cpp:375
double CRegistry::ReadFloat(WTL::CString strName, double fDefault) {
	DWORD dwType = REG_BINARY;
	double d;
	DWORD dwSize = sizeof(d);
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return fDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS)
		d = fDefault;
	::RegCloseKey(hKey);
	return d;
}

// registry.cpp:400
WTL::CString CRegistry::ReadString(WTL::CString strName, WTL::CString strDefault) {
	DWORD dwType = REG_SZ;
	DWORD dwSize = 255;
	int bSuccess = TRUE;
	TCHAR sz[256];
	HKEY hKey;

	dwType = GetDataType(strName);

	if (dwType != REG_SZ && dwType != REG_EXPAND_SZ)
		return strDefault;

	m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey);
	if (m_nLastError != ERROR_SUCCESS)
		return strDefault;

	m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)sz, &dwSize);
	if (m_nLastError != ERROR_SUCCESS)
		bSuccess = FALSE;
	::RegCloseKey(hKey);

	if (!bSuccess)
		return strDefault;
	return WTL::CString(sz);
}

// registry.cpp:432
unsigned long CRegistry::ReadDword(WTL::CString strName, unsigned long dwDefault) {
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwValue;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return dwDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS)
		dwValue = dwDefault;
	::RegCloseKey(hKey);
	return dwValue;
}

// registry.cpp:451
int CRegistry::ReadInt(WTL::CString strName, int nDefault) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(int);
	int n;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return nDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)&n, &dwSize) != ERROR_SUCCESS)
		n = nDefault;
	::RegCloseKey(hKey);
	return n;
}

// registry.cpp:468
int CRegistry::ReadBool(WTL::CString strName, int bDefault) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(int);
	int b;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return bDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)&b, &dwSize) != ERROR_SUCCESS)
		b = bDefault;
	::RegCloseKey(hKey);
	return b;
}

// registry.cpp:486
unsigned long CRegistry::ReadColor(WTL::CString strName, unsigned long rgbDefault) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(unsigned long);
	unsigned long rgb;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return rgbDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)&rgb, &dwSize) != ERROR_SUCCESS)
		rgb = rgbDefault;
	::RegCloseKey(hKey);
	return rgb;
}

// registry.cpp:503
int CRegistry::ReadFont(WTL::CString strName, WTL::CFont* pFont) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(LOGFONT);
	int bSuccess = TRUE;
	HKEY hKey;
	LOGFONT lf;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)&lf, &dwSize) != ERROR_SUCCESS) bSuccess = FALSE;
	::RegCloseKey(hKey);
	if (bSuccess) {
		pFont->Detach();
		pFont->CreateFontIndirect(&lf);
	}
	return bSuccess;
}

// registry.cpp:527
int CRegistry::ReadPoint(WTL::CString strName, WTL::CPoint* pPoint) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(WTL::CPoint);
	HKEY hKey;
	int bSuccess = TRUE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)pPoint, &dwSize) != ERROR_SUCCESS)
		bSuccess = FALSE;
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:544
int CRegistry::ReadSize(WTL::CString strName, WTL::CSize* pSize) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(WTL::CSize);
	HKEY hKey;
	int bSuccess = TRUE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)pSize, &dwSize) != ERROR_SUCCESS)
		bSuccess = FALSE;
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:561
int CRegistry::ReadRect(WTL::CString strName, WTL::CRect* pRect) {
	DWORD dwType = REG_BINARY;
	DWORD dwSize = sizeof(WTL::CRect);
	HKEY hKey;
	int bSuccess = TRUE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL, &dwType, (LPBYTE)pRect, &dwSize) != ERROR_SUCCESS)
		bSuccess = FALSE;
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:581
int CRegistry::WriteBool(WTL::CString strName, int bValue) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&bValue, sizeof(bValue))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:618
int CRegistry::WriteString(WTL::CString strName, WTL::CString strValue) {
	int bSuccess = TRUE;
	HKEY hKey;
	char sz[255];

	if (strValue.GetLength() > 254) return FALSE;

	strcpy(sz, LPCTSTR(strValue));

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_SZ, (LPBYTE)sz, strlen(sz) + 1) != ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:652
int CRegistry::WriteFloat(WTL::CString strName, double fValue) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&fValue, sizeof(fValue))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:670
int CRegistry::WriteInt(WTL::CString strName, int nValue) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&nValue, sizeof(nValue))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:688
int CRegistry::WriteDword(WTL::CString strName, unsigned long dwValue) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&dwValue, sizeof(dwValue))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:706
int CRegistry::WriteColor(WTL::CString strName, unsigned long rgbValue) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&rgbValue, sizeof(rgbValue))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:725
int CRegistry::WriteFont(WTL::CString strName, WTL::CFont* pFont) {
	int bSuccess = TRUE;
	HKEY hKey;

	LOGFONT lf;
	pFont->GetLogFont(&lf);

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&lf, sizeof(lf))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:747
int CRegistry::WritePoint(WTL::CString strName, WTL::CPoint* pPoint) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)pPoint, sizeof(WTL::CPoint))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:766
int CRegistry::WriteSize(WTL::CString strName, WTL::CSize* pSize) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)pSize, sizeof(WTL::CSize))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

// registry.cpp:784
int CRegistry::WriteRect(WTL::CString strName, WTL::CRect* pRect) {
	int bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;

	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)pRect, sizeof(WTL::CRect))
		!= ERROR_SUCCESS) bSuccess = FALSE;

	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}
