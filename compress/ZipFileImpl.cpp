#include "../ginc.h"
#include "ZipFileImpl.h"
#include "./zlib/zip.h"
#include "./zlib/unzip.h"

#ifdef __WXMSW__
#include "./zlib/iowin32.h"
#else
#endif

#include "../dialog/DlgCompress.h"
#include "../dialog/DlgDeCompress.h"

CZipFileImpl::CZipFileImpl()
{
}

CZipFileImpl::~CZipFileImpl()
{
	CloseZip();
}

bool CZipFileImpl::DoCompress()
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		return false;
	}
	
	if(m_uzFile != nullptr)
		CloseZip();
	
	if(m_vecCompressList.size() == 0 || m_strCompressedFile.IsEmpty())
		return false;

	if(!OpenZip(m_strCompressedFile, true))
		return false;
	
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		return false;
	}
}

bool CZipFileImpl::DoDeCompress()
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		return false;
	}
	
	if(m_uzFile != nullptr)
		CloseUnZip();
	
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		return false;
	}
}
	
bool CZipFileImpl::CloseZip()
{
	int nRet = m_uzFile ? zipClose(m_uzFile, nullptr) : ZIP_OK;
	m_uzFile = nullptr;
	
	wxZeroMemory(m_info);
	
	return (nRet == ZIP_OK);
}

bool CZipFileImpl::CloseUnZip()
{
	int nRet = UNZ_OK;
	if(m_uzFile)
	{
		unzCloseCurrentFile(m_uzFile);
		nRet = unzClose(m_uzFile);
		m_uzFile = nullptr;
	}
	
	return (nRet == UNZ_OK);
}

void CZipFileImpl::GetFileInfo(Z_FileInfo& info)
{
	
}

int CZipFileImpl::GetUnzipFileCount()
{
	if (!m_uzFile)
		return 0;

	unz_global_info info;

	if (unzGetGlobalInfo(m_uzFile, &info) == UNZ_OK)
		return (int)info.number_entry;

	return 0;
}

wxThread::ExitCode CZipFileImpl::Entry()
{	
	int iCompleted = 0;
	wxDialog* pDialog = nullptr;
	if(m_pProgressDialog)
	{
		for(auto item : m_vecCompressList)
		{
			if(m_bCancel)
				break;
				
			wxString strAddName(wxT(""));
			if(wxDirExists(item))
			{
				strAddName = theCommonUtil->GetPathName(item);
				AddFolderToZip(item, strAddName);
				iCompleted++;
				m_pProgressDialog->SetCompressTotal(iCompleted);
				continue;
			}
			
			strAddName = theCommonUtil->GetFileName(item);
			AddFileToZip(item, strAddName);
			iCompleted++;
			m_pProgressDialog->SetCompressTotal(iCompleted);
		}
		
		CloseZip();
		pDialog = m_pProgressDialog;
	}
	
	if(m_pDeCompressDialog)
	{
		DoExtractZip();
		pDialog = m_pDeCompressDialog;
	}
	
	wxQueueEvent(pDialog, new wxThreadEvent());	
	return (wxThread::ExitCode)0;	
}

bool CZipFileImpl::AddFileToZip(const wxString& strPathName, const wxString& strAddName)
{
	if(m_bCancel)
		return true;
		
	if (!m_uzFile)
		return false;
	
	int nRet;
	// save file attributes
	zip_fileinfo zfi;

	zfi.internal_fa = 0;
#ifdef __WXMSW__
	zfi.external_fa = ::GetFileAttributes(strPathName);
	
	// save file time
	SYSTEMTIME st;
	GetLastModified(strPathName, st, true);

	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;
	
	// load input file
	HANDLE hInputFile = ::CreateFile(strPathName, 
									GENERIC_READ,
									0,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_READONLY,
									NULL);

	if (hInputFile == INVALID_HANDLE_VALUE)
		return false;
#endif
	nRet = zipOpenNewFileInZip(m_uzFile, 
								strAddName,
								&zfi, 
								NULL, 
								0,
								NULL,
								0, 
								NULL,
								Z_DEFLATED,
								Z_DEFAULT_COMPRESSION);

	if (nRet == ZIP_OK)
	{
		wxFileName fn(strPathName);
		wxULongLong ull = fn.GetSize();
		unsigned long ulTotal = ull.ToULong();
		if((ulTotal >> 10) > 0)
			ulTotal = ulTotal >> 10;
			
		m_pProgressDialog->SetCurrentFile(strPathName);
		m_pProgressDialog->SetCurrentFileTotalSize(ulTotal);
		
		m_info.nFileCount++;

		// read the file and output to zip
		char pBuffer[BUFFERSIZE] = {0x00, };
#ifdef __WXMSW__
		DWORD dwBytesRead = 0;
		DWORD dwFileSize = 0;

		while (nRet == ZIP_OK && ::ReadFile(hInputFile, pBuffer, BUFFERSIZE, &dwBytesRead, NULL))
		{
			if(m_bCancel)
				break;
				
			dwFileSize += dwBytesRead;
			m_pProgressDialog->SetCurrentFileSize(dwFileSize >> 10);
			
			if (dwBytesRead)
				nRet = zipWriteInFileInZip(m_uzFile, pBuffer, dwBytesRead);
			else
				break;
		}
#endif
		m_info.ulUncompressedSize += dwFileSize;
	}

	zipCloseFileInZip(m_uzFile);
	
#ifdef __WXMSW__
	::CloseHandle(hInputFile);
#endif
	return (nRet == ZIP_OK);
}

bool CZipFileImpl::AddFolderToZip(const wxString& strPathName, const wxString& strAddName)
{
	if(m_bCancel)
		return true;
		
	if(!m_uzFile)
		return false;
		
	m_info.nFolderCount++;
	
	zip_fileinfo zfi;
	zfi.internal_fa = 0;
#ifdef __WXMSW__
	zfi.external_fa = ::GetFileAttributes(strPathName);
	
	SYSTEMTIME st;
	GetLastModified(strPathName, st, true);
	
	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;
	
	wxString strFolderFileName(strAddName);
	strFolderFileName += SLASH;
	int nRet = zipOpenNewFileInZip(m_uzFile, 
		strFolderFileName,
		&zfi, 
		NULL, 
		0,
		NULL,
		0, 
		NULL,
		Z_DEFLATED,
		Z_DEFAULT_COMPRESSION);
	
	zipCloseFileInZip(m_uzFile);

	wxString strSearch(strPathName);
	strSearch += SLASH;
	strSearch += wxT("*");
	
	WIN32_FIND_DATA finfo;
	HANDLE hSearch = FindFirstFile(strSearch, &finfo);
	if (hSearch != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if(m_bCancel)
				break;
				
			wxString strFullPath(strPathName); 
			wxString strNewAddName(strFolderFileName);
			if (finfo.cFileName[0] != '.') 
			{
				strFullPath += SLASH;
				strFullPath += finfo.cFileName;
				
				strNewAddName += finfo.cFileName;
				
				if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					AddFolderToZip(strFullPath, strNewAddName);
				else 
					AddFileToZip(strFullPath, strNewAddName);
			}
		} while (FindNextFile(hSearch, &finfo));
		
		FindClose(hSearch);
	}
#else
#endif

	return true;
}

bool CZipFileImpl::OpenZip(const wxString& strFilePath, bool bAppend)
{
	CloseZip();
	
	if(strFilePath.IsEmpty() || !strFilePath.Len())
		return false;
		
	if(bAppend && !wxFileExists(strFilePath))
		bAppend = false;
		
	m_uzFile = zipOpen(strFilePath, bAppend ? 1 : 0);
	return (m_uzFile != nullptr);
}

bool CZipFileImpl::OpenUnZip()
{
	CloseUnZip();
	if(m_strCompressedFile.IsEmpty())
		return false;
		
	m_uzFile = unzOpen(m_strCompressedFile);
	return (m_uzFile != nullptr);
}

bool CZipFileImpl::DoExtractZip()
{
	if (!m_uzFile)
		return false;
		
	if (GetUnzipFileCount() == 0)
		return false;
	
	if(!GotoFirstFile())
		return false;
	
	do
	{
		if(!DoExtractFileFromZip(m_strUnCompressDir))
			return false;
	} while(GotoNextFile());
}

bool CZipFileImpl::GotoFirstFile()
{
	if (!m_uzFile)
		return false;
	
	return (unzGoToFirstFile(m_uzFile) == UNZ_OK);
/*	
//	if (!szExt || !lstrlen(szExt))
//		return (unzGoToFirstFile(m_uzFile) == UNZ_OK);

	// else
	if (unzGoToFirstFile(m_uzFile) == UNZ_OK)
	{
		UZ_FileInfo info;

		if (!GetFileInfo(info))
			return FALSE;

		// test extension
		char szFExt[_MAX_EXT] = {0x00, };
		_splitpath(info.szFileName, NULL, NULL, NULL, szFExt);

		if (szFExt)
		{
			if (lstrcmpi(szExt, szFExt + 1) == 0)
				return TRUE;
		}

		return GotoNextFile(szExt);
	}

	return true;
*/
}

bool CZipFileImpl::GotoNextFile()
{
	if (!m_uzFile)
		return false;

	return (unzGoToNextFile(m_uzFile) == UNZ_OK);
}

bool CZipFileImpl::DoExtractFileFromZip(const wxString& strDir)
{
	if (!m_uzFile)
		return false;
		
	wxString strTargetDir(strDir);
	bool bAddSlash = theCommonUtil->Compare(strTargetDir.Right(1), SLASH) == 0 ? false : true;
		
	UZ_FileInfo info;
	GetExtractFileInfo(info);
	
	wxString strName(info.szFileName);
	if(info.bFolder)
	{
		wxString strNewPath = strTargetDir;
		strNewPath += bAddSlash ? SLASH + strName : strName;
		
		if(!wxDirExists(strNewPath))
			return wxMkDir(strNewPath);
			
		return true;
	}
	
	wxString strFilePathName(bAddSlash ? strTargetDir + SLASH + strName : strTargetDir + strName);
	
	HANDLE hOutputFile = ::CreateFile(strFilePathName, 
										GENERIC_WRITE,
										0,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

	if (INVALID_HANDLE_VALUE == hOutputFile)
		return false;

	if (unzOpenCurrentFile(m_uzFile) != UNZ_OK)
		return false;
	
	int nRet = UNZ_OK;
	char pBuffer[BUFFERSIZE] = {0x00, };

	do
	{
		nRet = unzReadCurrentFile(m_uzFile, pBuffer, BUFFERSIZE);

		if (nRet > 0)
		{
#ifdef __WXMSW__
			// output
			DWORD dwBytesWritten = 0;
			if (!::WriteFile(hOutputFile, pBuffer, nRet, &dwBytesWritten, NULL) || dwBytesWritten != (DWORD)nRet)
			{
				nRet = UNZ_ERRNO;
				break;
			}
#endif
		}
	}
	while (nRet > 0);

#ifdef __WXMSW__
	CloseHandle(hOutputFile);
#else
#endif
	
	unzCloseCurrentFile(m_uzFile);

	if (nRet == UNZ_OK)
		SetFileModifyTime(strFilePathName, info.ulDosDate);

	return (nRet == UNZ_OK);
}

bool CZipFileImpl::GetExtractFileInfo(UZ_FileInfo& info)
{
	if (!m_uzFile)
		return false;

	unz_file_info uzfi;

	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&uzfi, sizeof(uzfi));

	if (UNZ_OK != unzGetCurrentFileInfo(m_uzFile, &uzfi, info.szFileName, MAX_PATH, NULL, 0, info.szComment, MAX_COMMENT))
		return false;

	info.ulVersion           = uzfi.version;	
	info.ulVersionNeeded     = uzfi.version_needed;
	info.ulFlags             = uzfi.flag;	
	info.ulCompressionMethod = uzfi.compression_method; 
	info.ulDosDate           = uzfi.dosDate;  
	info.ulCRC               = uzfi.crc;	 
	info.ulCompressedSize    = uzfi.compressed_size; 
	info.ulUncompressedSize  = uzfi.uncompressed_size;
	info.ulInternalAttrib    = uzfi.internal_fa; 
	info.ulExternalAttrib    = uzfi.external_fa; 

	// replace filename forward slashes with backslashes
	int nLen = wxString(info.szFileName).Len();

	while (nLen--)
	{
		if (info.szFileName[nLen] == '/')
			info.szFileName[nLen] = '\\';
	}

#ifdef __WXMSW__
	// is it a folder?
	info.bFolder = ((info.ulExternalAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
#else
#endif

	return true;
}