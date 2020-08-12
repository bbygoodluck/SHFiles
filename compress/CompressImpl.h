#ifndef __COMPRESS_IMPL_H_INCLUDED
#define __COMPRESS_IMPL_H_INCLUDED

class DlgCompress;
class DlgDeCompress;
class CCompressImpl : public wxThreadHelper
{
public:
	CCompressImpl();
	~CCompressImpl();
	
public:
	void SetCompressDialog(DlgCompress* pDialog);
	void SetDeCompressDialog(DlgDeCompress* pDialog);
	
	void SetCompressInfo(const std::vector<wxString>& strCompList, const wxString& strCompressedFile);
	void SetDeCompressInfo(const wxString& strCompressedFile, const wxString& strUnCompressDir);
	
	virtual bool DoCompress() = 0;
	virtual bool DoDeCompress() = 0;
	virtual wxThread::ExitCode Entry() = 0;
	
	void CompressCancel();
	bool GetCompressCancel() { return m_bCancel;}
	
	wxString GetCompressedFile() { return m_strCompressedFile; }
	
protected:
#ifdef __WXMSW__
	bool GetLastModified(const wxString& strPathName, SYSTEMTIME& sysTime, bool bLocalTime);
	bool SetFileModifyTime(const wxString& strFilePathName, DWORD dwDosDate);
#else
	time_t GetLastModified(const wxString& strPathName);
#endif
	
	DlgCompress* GetCompressDialog();
	DlgDeCompress* GetDeCompressDialog();
	
protected:
	DlgCompress* m_pProgressDialog;
	DlgDeCompress* m_pDeCompressDialog;
	
	std::vector<wxString> m_vecCompressList;
	wxString m_strCompressedFile = wxT("");
	wxString m_strUnCompressDir = wxT("");
	static constexpr unsigned int BUFFERSIZE = 2048;
	
	bool m_bCancel = false;
	bool m_bEndCompress = false;
};

#endif