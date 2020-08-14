#ifndef __ZIP_FILE_IMPL_H_INCLUDED
#define __ZIP_FILE_IMPL_H_INCLUDED

//#include "../CompressImpl.h"
class CCompressImpl;
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
	bool OpenZip(bool bAppend = false);

public:
	bool DoCompress() override;
	bool DoDeCompress() override;
	bool CloseZip();
		
	void GetFileInfo(Z_FileInfo& info);
	
private:
	void DoZipStart();
	
protected:
	Z_FileInfo m_info;
};

#endif