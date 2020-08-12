#ifndef __ZIP_FILE_IMPL_H_INCLUDED
#define __ZIP_FILE_IMPL_H_INCLUDED

#include "CompressImpl.h"
constexpr unsigned int MAX_COMMENT = 255;
class CZipFileImpl : public CCompressImpl
{
	struct Z_FileInfo
	{
		int nFileCount;
		int nFolderCount;
		unsigned long ulUncompressedSize;
	};
	
	struct UZ_FileInfo
	{
		char szFileName[MAX_PATH + 1];
		char szComment[MAX_COMMENT + 1];
		
		unsigned long ulVersion;  
		unsigned long ulVersionNeeded;
		unsigned long ulFlags;	 
		unsigned long ulCompressionMethod; 
		unsigned long ulDosDate;	
		unsigned long ulCRC;   
		unsigned long ulCompressedSize; 
		unsigned long ulUncompressedSize;
		unsigned long ulInternalAttrib; 
		unsigned long ulExternalAttrib; 
		bool bFolder;
	};

public:
	CZipFileImpl();
	virtual ~CZipFileImpl();
	
protected:
	wxThread::ExitCode Entry() override;
	bool AddFileToZip(const wxString& strPathName, const wxString& strAddName = wxT(""));
	bool AddFolderToZip(const wxString& strPathName, const wxString& strAddName = wxT(""));
	bool OpenZip(const wxString& strFilePath, bool bAppend = false);
	bool OpenUnZip();
	bool DoExtractZip();
	bool DoExtractFileFromZip(const wxString& strDir);
	bool GotoFirstFile();
	bool GotoNextFile();
public:
	bool DoCompress() override;
	bool DoDeCompress() override;
	
	bool CloseZip();
	bool CloseUnZip();
	
	void GetFileInfo(Z_FileInfo& info);
	bool GetExtractFileInfo(UZ_FileInfo& info);
	
	int GetUnzipFileCount();
	
protected:
	void* m_uzFile = nullptr;
	Z_FileInfo m_info;
};

#endif