#include "../ginc.h"
#include "../dialog/DlgCompress.h"
#include "../dialog/DlgDeCompress.h"
#include "CompressImpl.h"

CCompressImpl::CCompressImpl()
{
	m_pCompressDialog = nullptr;
	m_pDeCompressDialog = nullptr;
	
	m_bThreadCreated = true;
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
		m_bThreadCreated = false;
}

CCompressImpl::~CCompressImpl()
{
	m_pCompressDialog = nullptr;
	m_pDeCompressDialog = nullptr;
}

void CCompressImpl::SetOwnerDialog(wxDialog* pOwnerDlg)
{
	m_pCompressDialog = nullptr;
	m_pDeCompressDialog = nullptr;

	if(theCompress->IsExtract())
	{	
		m_pDeCompressDialog = (DlgDeCompress *)pOwnerDlg;
		return;
	}
	
	m_pCompressDialog = (DlgCompress *)pOwnerDlg;
}

#ifdef __WXMSW__
bool CCompressImpl::GetLastModified(const wxString& strPathName, SYSTEMTIME& sysTime, bool bLocalTime)
{
	wxZeroMemory(sysTime);
	DWORD dwAttr = ::GetFileAttributes(strPathName);
	
	// files only
	if (dwAttr == 0xFFFFFFFF)
		return false;

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(strPathName, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	FindClose(hFind);

	FILETIME ft = findFileData.ftLastWriteTime;

	if (bLocalTime)
		FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &ft);

	FileTimeToSystemTime(&ft, &sysTime);
	return true;
	
}

bool CCompressImpl::SetFileModifyTime(const wxString& strFilePathName, DWORD dwDosDate)
{
	HANDLE hFile = CreateFile(strFilePathName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (!hFile)
		return FALSE;
	
	FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

	bool bRes = (GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite) == TRUE);

	if (bRes)
		bRes = (TRUE == DosDateTimeToFileTime((WORD)(dwDosDate >> 16), (WORD)dwDosDate, &ftLocal));

	if (bRes)
		bRes = (TRUE == LocalFileTimeToFileTime(&ftLocal, &ftm));

	if (bRes)
		bRes = (TRUE == SetFileTime(hFile, &ftm, &ftLastAcc, &ftm));

	CloseHandle(hFile);
	return bRes;
}
#else
time_t CCompressImpl::GetLastModified(const wxString& strPathName)
{
	struct _stat st;

	if (_stat(szPath, &st) != 0)
		return 0;

	// files only
	if ((st.st_mode & _S_IFDIR) == _S_IFDIR)
		return 0;

	return st.st_mtime;
}
#endif

DlgCompress* CCompressImpl::GetCompressDialog()
{
	return m_pCompressDialog;
}

DlgDeCompress* CCompressImpl::GetDeCompressDialog()
{
	return m_pDeCompressDialog;
}