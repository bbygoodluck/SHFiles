#ifndef __UNZIP_FILE_IMPL_H_INCLUDED
#define __UNZIP_FILE_IMPL_H_INCLUDED

//#include "../CompressImpl.h"

class CCompressImpl;
class CUnZipFileImpl : public CCompressImpl
{
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
	CUnZipFileImpl();
	virtual ~CUnZipFileImpl();

public:	
	bool DoCompress() override;
	bool DoDeCompress() override;
	
protected:	
	wxThread::ExitCode Entry() override;
	
	bool CloseUnZip();
	bool GetExtractFileInfo(UZ_FileInfo& info);
	int GetUnzipFileCount();

	bool OpenUnZip();
	bool DoExtractZip();
	bool DoExtractFileFromZip(const wxString& strDir);
	bool GotoFirstFile();
	bool GotoNextFile();
	
private:
	wxString GetRename(const wxString& strFullPathName, const wxString& strOldName);

};
#endif