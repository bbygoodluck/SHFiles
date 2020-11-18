#include "../ginc.h"
#include "LocalFileSystem.h"

static wxInt64 EPOCH_OFFSET_IN_MSEC = wxLL(11644473600000);

CLocalFileSystem::CLocalFileSystem()
	: m_bDirs_only(false)
#ifdef __WXMSW__
	, m_hFind(INVALID_HANDLE_VALUE)
	, m_bfound(false)
	, m_strFind_path(wxT(""))
#else
	, m_raw_path()
	, m_file_part()
	, m_buffer_length()
	, m_dir()
#endif
{
}

CLocalFileSystem::~CLocalFileSystem()
{
	EndFindFiles();
}

bool CLocalFileSystem::BeginFindFiles(wxString path, bool dirs_only)
{
	EndFindFiles();
	m_bDirs_only = dirs_only;

#ifdef __WXMSW__
	if (path.Last() != '/' && path.Last() != '\\')
	{
		m_strFind_path = path + SLASH;
		path += SLASH + wxT("*");
	}
	else
	{
		m_strFind_path = path;
		path += '*';
	}

	m_hFind = FindFirstFileEx(path, FindExInfoStandard, &m_find_data, dirs_only ? FindExSearchLimitToDirectories : FindExSearchNameMatch, NULL, 0);
	if (m_hFind == INVALID_HANDLE_VALUE)
	{
		m_bfound = false;
		return false;
	}

	m_bfound = true;
	return true;
#else
	if (path != _T("/") && path.Last() == '/')
		path.RemoveLast();

	const wxCharBuffer s = path.fn_str();

	m_dir = opendir(s);
	if (!m_dir)
		return false;

	const wxCharBuffer p = path.fn_str();
	const int len = strlen(p);
	m_raw_path = new char[len + 2048 + 2];
	m_buffer_length = len + 2048 + 2;
	strcpy(m_raw_path, p);
	if (len > 1)
	{
		m_raw_path[len] = '/';
		m_file_part = m_raw_path + len + 1;
	}
	else
		m_file_part = m_raw_path + len;

	return true;
#endif
}

bool CLocalFileSystem::GetNextFile(wxString& strName, bool* isDir, unsigned long* lattr, wxLongLong* llSize, wxDateTime* dt)
{
#ifdef __WXMSW__
	if (!m_bfound)
		return false;
	do
	{
		if (!m_find_data.cFileName[0])
		{
			m_bfound = FindNextFile(m_hFind, &m_find_data) != 0;
			return true;
		}

		if (m_bDirs_only && !(m_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		if ((m_find_data.cFileName[0] == '.') && !m_find_data.cFileName[1])
			continue;

		strName = m_find_data.cFileName;
		if(lattr && lattr != nullptr)
			*lattr = m_find_data.dwFileAttributes;

		if(isDir && isDir != nullptr)
			*isDir = (m_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (llSize && llSize != nullptr)
		{
			if (*isDir)
				*llSize = -1;
			else
				*llSize = wxLongLong(m_find_data.nFileSizeHigh, m_find_data.nFileSizeLow);
		}

		if (dt && dt != nullptr)
		{
			wxLongLong t(m_find_data.ftLastWriteTime.dwHighDateTime, m_find_data.ftLastWriteTime.dwLowDateTime);
			t /= 10000; // Convert hundreds of nanoseconds to milliseconds.
			t -= EPOCH_OFFSET_IN_MSEC;

			*dt = wxDateTime(t);
		}

		m_bfound = FindNextFile(m_hFind, &m_find_data) != 0;
		return true;
	} while ((m_bfound = FindNextFile(m_hFind, &m_find_data) != 0));

	return false;
#else
	return false;
#endif
}

void CLocalFileSystem::EndFindFiles()
{
#ifdef __WXMSW__
	m_bfound = false;
	if (m_hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(m_hFind);
		m_hFind = INVALID_HANDLE_VALUE;
	}
#else
	if (m_dir)
	{
		closedir(m_dir);
		m_dir = 0;
	}

	delete[] m_raw_path;
	m_raw_path = 0;
	m_file_part = 0;
#endif
}

bool CLocalFileSystem::IsCanReadDir(const wxString& strPath)
{
	if (!wxFileName::IsDirReadable(strPath))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DIREDTORY_READ_FAIL")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return false;
	}

	return true;
}
/*
	- 파일 복사/이동/삭제 시 IFileOperation COM 인터페이스를 사용하지 않는 이유
		>> 가상(VMWare 등)PC환경의 경우 IFileOperation이 정상적으로 작동하지 않는 오류가 있음(gcc의 컴파일환경 영향이 있는 듯함)
*/
bool CLocalFileSystem::RecursiveCopyOrMove(std::list<wxString>& dirsToVisit, const wxString& strDest, wxWindow* parent, bool bMove)
{
#ifdef __WXMSW__
	//윈도우 시스템에서는 SHFileOperation을 이용함
	size_t len = 1;
	for (auto const& item : dirsToVisit)
		len += item.size() + 1;

	wxChar* pBuffer = new wxChar[len];
	wxChar* p = pBuffer;

	for (auto& visitItem : dirsToVisit)
	{
		if (visitItem.Last() == wxFileName::GetPathSeparator())
			visitItem.RemoveLast();

		if (GetFileType(visitItem) == unknown)
			continue;

		_tcscpy(p, visitItem);
		p += visitItem.size() + 1;
	}

	int iDestLen = 1;
	iDestLen += strDest.size() + 1;

	wxChar* pBuffTo = new wxChar[iDestLen];
	wxChar* pTo = pBuffTo;
	_tcscpy(pTo, strDest);

	if (p != pBuffer)
	{
		*p = 0;

		SHFILEOPSTRUCT op;
		memset(&op, 0, sizeof(op));

		op.wFunc = bMove ? FO_MOVE : FO_COPY;
		op.pFrom = pBuffer;
		op.fFlags = FOF_NOCONFIRMMKDIR;
		op.pTo = pTo;

		int iRet = SHFileOperation(&op);
		if (iRet != 0)
		{
			// SHFileOperation may return non-Win32 error codes, so the error
			// message can be incorrect
			wxLogApiError(wxT("SHFileOperation"), iRet);
			return false;
		}

		delete[] pBuffer;
		delete[] pTo;
	}
	/*
	HRESULT hr;
	IFileOperation* pfo;
	//복사(잘라내기) 대상자 PIDL
	wxVector<PIDLIST_ABSOLUTE> vctIDLists;
	//COM 초기화
	hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		return false;

	hr = ::CoCreateInstance(__uuidof(FileOperation), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
	if (FAILED(hr))
		return false;

	//복사(잘라내기) Target IShellItem 설정
	IShellItem* psiDest = NULL;
	hr = SHCreateItemFromParsingName(CONVSTR(strDest), NULL, IID_PPV_ARGS(&psiDest));
	if(FAILED(hr))
		return false;

	//복사(잘라내기)대상자의 ShellItem를 vector에 저정
	for (auto const& item : dirsToVisit)
	{
		IShellItem* shlItem = NULL;
		//ShellItem 생성
		hr = SHCreateItemFromParsingName(CONVSTR(item), NULL, IID_PPV_ARGS(&shlItem));
		if (FAILED(hr))
			break;

		PIDLIST_ABSOLUTE _pidlistAbsolute = NULL;
		hr = ::SHGetIDListFromObject(shlItem, &_pidlistAbsolute);
		if (SUCCEEDED(hr))
			vctIDLists.push_back(_pidlistAbsolute);

		shlItem->Release();
	}

	int iItemCounts = vctIDLists.size();
	if (iItemCounts > 0)
	{
		PIDLIST_ABSOLUTE* paIDs = new PIDLIST_ABSOLUTE[iItemCounts];
		for (int idx = 0; idx < iItemCounts; idx++)
			paIDs[idx] = vctIDLists[idx];

		IShellItemArray* psia = NULL;
		hr = SHCreateShellItemArrayFromIDLists(iItemCounts, (LPCITEMIDLIST *)paIDs, &psia);

		if (SUCCEEDED(hr))
		{
			hr = bMove ? pfo->MoveItems(psia, psiDest) : pfo->CopyItems(psia, psiDest);
			if (SUCCEEDED(hr))
			{
				hr = pfo->PerformOperations();
				psiDest->Release();
			}
		}

		for (int idx = 0; idx < iItemCounts; idx++)
			::CoTaskMemFree(paIDs[idx]);

		delete[] paIDs;
	}

	CoUninitialize();
	 */
	return true;
#else

#endif
}

#ifdef __WXMSW__
// 같은 폴더내 파일/디렉토리 복사는 IFileOperation COM 인터페이를 사용해서 처리
bool CLocalFileSystem::RecursiveCopyOrMoveSameTarget(std::list<wxString>& dirsToVisit, const wxString& strDest, wxWindow* parent, bool bMove)
{
	wxString strName(wxT(""));
	wxString strFullPath(wxT(""));
	bool bReturn = true;

	IFileOperation* pfo;
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		return false;

	hr = ::CoCreateInstance(__uuidof(FileOperation), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
	if (FAILED(hr))
		return false;

	IShellItem* psiTo = nullptr;
	hr = SHCreateItemFromParsingName(CONVSTR(strDest), NULL, IID_PPV_ARGS(&psiTo));

	if(FAILED(hr))
		return false;

	for (auto& strItem : dirsToVisit)
	{
		IShellItem* psiFrom = nullptr;
		hr = SHCreateItemFromParsingName(CONVSTR(strItem), NULL, IID_PPV_ARGS(&psiFrom));

		if(SUCCEEDED(hr))
		{
			strName = theCommonUtil->GetFileName(strItem);
			if(theCommonUtil->Compare(strDest.Right(1), SLASH) == 0)
				strFullPath = strDest + strName;
			else
				strFullPath = strDest + SLASH + strName;

			if (strFullPath.CmpNoCase(strItem) == 0)
				strName  = theCommonUtil->ChangeName(strFullPath);

			hr = pfo->CopyItem(psiFrom, psiTo, strName, nullptr);
			if(SUCCEEDED(hr))
				hr = pfo->PerformOperations();

			psiFrom->Release();
		}
		else
		{
			bReturn = false;
			break;
		}
	}

	psiTo->Release();
	CoUninitialize();
	return bReturn;
}
#endif

bool CLocalFileSystem::GetAttributeInfo(const wxString& strName, bool& isDir, unsigned long& lattr, wxLongLong* llSize, wxDateTime* dt)
{
#ifdef __WXMSW__
	WIN32_FILE_ATTRIBUTE_DATA attributes = { 0, };
	BOOL result = GetFileAttributesEx(strName, GetFileExInfoStandard, &attributes);
	DWORD dwErr = GetLastError();

	if (!result)
		return false;

	if (!result && (dwErr != 0x0000)) //작업을 완료했습니다.
		return false;

	isDir = (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	lattr = attributes.dwFileAttributes;
	*llSize = 0;

	if (!isDir)
		*llSize = wxLongLong(attributes.nFileSizeHigh, attributes.nFileSizeLow);

	wxLongLong t(attributes.ftLastWriteTime.dwHighDateTime, attributes.ftLastWriteTime.dwLowDateTime);
	t /= 10000; // Convert hundreds of nanoseconds to milliseconds.
	t -= EPOCH_OFFSET_IN_MSEC;

	*dt = wxDateTime(t);

#else
#endif

	return true;
}

bool CLocalFileSystem::IsCheckedFileOpen(const wxString& strFullPathName)
{
	bool bReturn = false;

#ifdef __WXMSW__
	HANDLE hFile = CreateFile(CONVSTR(strFullPathName)
						, 0x10000 //DELETE
						, FILE_SHARE_DELETE
						, NULL
						, OPEN_EXISTING
						, FILE_FLAG_OPEN_REPARSE_POINT// | FILE_FLAG_DELETE_ON_CLOSE
						, NULL);
	//파일이 열려있지 않은 경우는 Handle 값을 읽어옴
	if((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
		bReturn = false;

	DWORD dwRet = GetLastError();
	if (dwRet == ERROR_SHARING_VIOLATION)
		bReturn = true;

	CloseHandle(hFile);

#else
#endif

	return bReturn;
}

bool CLocalFileSystem::RecursiveDelete(const wxString& path, wxWindow* parent, bool bGoTrash, int& iRet)
{
	std::list<wxString> paths;
	paths.push_back(path);
	return RecursiveDelete(paths, parent, bGoTrash, iRet);
}

bool CLocalFileSystem::RecursiveDelete(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet)
{
	bool bRet = true;
#ifdef __WXMSW__

	size_t len = 1; // String list terminated by empty string

	for (auto const& dirItem : dirsToVisit)
		len += dirItem.size() + 1;

	// Allocate memory
	wxChar* pBuffer = new wxChar[len];
	wxChar* p = pBuffer;

	for (auto dir : dirsToVisit)
	{
		if (dir.Last() == wxFileName::GetPathSeparator())
			dir.RemoveLast();

		if (GetFileType(dir) == unknown)
			continue;

		_tcscpy(p, dir);
		p += dir.size() + 1;
	}

	if (p != pBuffer)
	{
		*p = 0;

		FILEOP_FLAGS flag = FOF_NOCONFIRMATION;
	//	if (theJsonConfig->GetCopyMoveUseWindowShell() != 1)
	//		flag = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

		SHFILEOPSTRUCT fileop;
		wxZeroMemory(fileop);
		fileop.wFunc = FO_DELETE;
		fileop.pFrom = pBuffer;
		fileop.pTo = NULL;
		fileop.fAnyOperationsAborted = false;
		fileop.hNameMappings = NULL;
		fileop.lpszProgressTitle = NULL;
		fileop.fFlags = flag;
		if (bGoTrash)
			fileop.fFlags |= FOF_ALLOWUNDO;

		int ret = SHFileOperation(&fileop);
		if (ret != 0)
		{
			// SHFileOperation may return non-Win32 error codes, so the error
			// message can be incorrect
			wxLogApiError(wxT("SHFileOperation"), ret);
			return false;
		}
	}

	delete[] pBuffer;

	/*
	HRESULT hr;
	IFileOperation* pfo;

	wxVector<PIDLIST_ABSOLUTE> vctIDLists;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		return false;

	hr = CoCreateInstance(__uuidof(FileOperation), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
	if (FAILED(hr))
		return false;

	for (auto const& item : dirsToVisit)
	{
		IShellItem* shlItem = NULL;
		hr = SHCreateItemFromParsingName(item.wchar_str(), NULL, IID_PPV_ARGS(&shlItem));
		if (SUCCEEDED(hr))
		{
			PIDLIST_ABSOLUTE _pidlistAbsolute = NULL;
			hr = SHGetIDListFromObject(shlItem, &_pidlistAbsolute);
			if (SUCCEEDED(hr))
				vctIDLists.push_back(_pidlistAbsolute);
		}

		if (shlItem)
			shlItem->Release();
	}

	int iItemCounts = vctIDLists.size();
	if (iItemCounts > 0)
	{
		PIDLIST_ABSOLUTE* paIDs = new PIDLIST_ABSOLUTE[iItemCounts];
		for (int idx = 0; idx < iItemCounts; idx++)
			paIDs[idx] = vctIDLists[idx];

		IShellItemArray* psia = NULL;
		hr = SHCreateShellItemArrayFromIDLists(iItemCounts, (LPCITEMIDLIST *)paIDs, &psia);

		if (SUCCEEDED(hr))
		{
			DWORD dwFlags = FOF_NOCONFIRMATION;
		//	if (theJsonConfig->GetCopyMoveUseWindowShell() != 1)
		//		dwFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

			if (bGoTrash)
				dwFlags |= FOF_ALLOWUNDO;

			pfo->SetOperationFlags(dwFlags);
			hr = pfo->DeleteItems(psia);

			if (SUCCEEDED(hr))
			{
				hr = pfo->PerformOperations();
				if (FAILED(hr))
				{
					wxMessageBox(wxT("Delete failed : ") + dir, wxT("Delete - ") + PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
					return false;
				}
			}
		}

		for (int idx = 0; idx < iItemCounts; idx++)
			CoTaskMemFree(paIDs[idx]);

		delete[] paIDs;
		CoUninitialize();
	}
	*/
	return true;
#else
#endif
}

enum CLocalFileSystem::local_filetype CLocalFileSystem::GetFileType(const wxString& path)
{
#ifdef __WXMSW__
	DWORD result = GetFileAttributes(path);
	if (result == INVALID_FILE_ATTRIBUTES)
		return unknown;

	if (result & FILE_ATTRIBUTE_REPARSE_POINT)
		return link;

	if (result & FILE_ATTRIBUTE_DIRECTORY)
		return dir;

	return file;
#else
	if (path.Last() == '/' && path != _T("/"))
	{
		wxString tmp = path;
		tmp.RemoveLast();
		return GetFileType(tmp);
	}

	wxStructStat buf;
	int result = wxLstat(path, &buf);
	if (result)
		return unknown;

#ifdef S_ISLNK
	if (S_ISLNK(buf.st_mode))
		return link;
#endif

	if (S_ISDIR(buf.st_mode))
		return dir;

	return file;
#endif
}
