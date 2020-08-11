#ifndef __ZIP_FILE_IMPL_H_INCLUDED
#define __ZIP_FILE_IMPL_H_INCLUDED

#include "CompressImpl.h"
class CZipFileImpl : public CCompressImpl
{
	struct Z_FileInfo
	{
		int nFileCount;
		int nFolderCount;
		unsigned long ulUncompressedSize;
	};

public:
	CZipFileImpl();
	virtual ~CZipFileImpl();
	
protected:
	wxThread::ExitCode Entry() override;
	bool AddFileToZip(const wxString& strPathName, const wxString& strAddName = wxT(""));
	bool AddFolderToZip(const wxString& strPathName, const wxString& strAddName = wxT(""));
	bool OpenZip(const wxString& strFilePath, bool bAppend = false);
	
public:
	bool DoCompress() override;
	bool DoUnCompress(const wxString& strCompressFile) override;
	
	bool CloseZip();
	void GetFileInfo(Z_FileInfo& info);
	
protected:
	void* m_uzFile = nullptr;
	Z_FileInfo m_info;
};

#endif