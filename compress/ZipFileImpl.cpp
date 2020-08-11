#include "../ginc.h"
#include "ZipFileImpl.h"
#include "./zlib/zip.h"

#ifdef __WXMSW__
#include "./zlib/iowin32.h"
#else
#endif

#include "../dialog/DlgCompress.h"


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

bool CZipFileImpl::DoUnCompress(const wxString& strCompressFile)
{
	
}
	
bool CZipFileImpl::CloseZip()
{
	int nRet = m_uzFile ? zipClose(m_uzFile, NULL) : ZIP_OK;
	m_uzFile = nullptr;
	
	wxZeroMemory(m_info);
	
	return (nRet == ZIP_OK);
}

void CZipFileImpl::GetFileInfo(Z_FileInfo& info)
{
	
}

wxThread::ExitCode CZipFileImpl::Entry()
{	
	int iCompleted = 0;
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
			GetCompressDialog()->SetCompressTotal(iCompleted);
			continue;
		}
		
		strAddName = theCommonUtil->GetFileName(item);
		AddFileToZip(item, strAddName);
		iCompleted++;
		GetCompressDialog()->SetCompressTotal(iCompleted);
	}
	
	CloseZip();
	
	wxQueueEvent(GetCompressDialog(), new wxThreadEvent());	
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
		
		GetCompressDialog()->SetCurrentFile(strPathName);
		GetCompressDialog()->SetCurrentFileTotalSize(ulTotal);
		
		m_info.nFileCount++;

		// read the file and output to zip
		char pBuffer[BUFFERSIZE];
#ifdef __WXMSW__
		DWORD dwBytesRead = 0, dwFileSize = 0;

		while (nRet == ZIP_OK && ::ReadFile(hInputFile, pBuffer, BUFFERSIZE, &dwBytesRead, NULL))
		{
			if(m_bCancel)
				break;
				
			dwFileSize += dwBytesRead;
			GetCompressDialog()->SetCurrentFileSize(dwFileSize);
			
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