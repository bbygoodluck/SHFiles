#include "../ginc.h"
#include "../dialog/DlgCompress.h"
#include "CompressImpl.h"

CCompressImpl::CCompressImpl()
{
	
}

CCompressImpl::~CCompressImpl()
{
	
}

void CCompressImpl::SetCompressDialog(DlgCompress* pDialog)
{
	m_pProgressDialog = pDialog;
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

void CCompressImpl::CompressCancel()
{
	m_bCancel = true;
}

DlgCompress* CCompressImpl::GetCompressDialog()
{
	return m_pProgressDialog;
}

void CCompressImpl::SetCompressInfo(const std::vector<wxString>& strCompList, const wxString& strCompressedFile)
{
	m_vecCompressList = strCompList;
	m_strCompressedFile = strCompressedFile;
}