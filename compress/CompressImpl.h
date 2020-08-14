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
	void SetOwnerDialog(wxDialog* pOwnerDlg);
	
	virtual bool DoCompress() = 0;
	virtual bool DoDeCompress() = 0;
	virtual wxThread::ExitCode Entry() = 0;
	
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
	DlgCompress* m_pCompressDialog;
	DlgDeCompress* m_pDeCompressDialog;
	
	std::vector<wxString> m_vecCompressList;
	
	wxString m_strCompressedFile = wxT("");
	wxString m_strUnCompressDir = wxT("");
	
	bool m_bThreadCreated = true;
	void* m_uzFile = nullptr;
};

#endif