#ifndef __DECOMPRESS_DUP_CHECK_INCLUDED
#define __DECOMPRESS_DUP_CHECK_INCLUDED

class CDeCompressDupCheck : public wxEvtHandler
{
private:
	CDeCompressDupCheck() {};

public:
	public:
	~CDeCompressDupCheck() {};
	static CDeCompressDupCheck* Get();
	
public:
	void SetDuplicateFile(const wxString& strFullPathName, const wxString& strFileName, unsigned long ulDosDate, unsigned long ulUncompressedSize);
	
private:
	static std::unique_ptr<CDeCompressDupCheck> m_pInstance;
	
	wxString m_strFullPathName = wxT("");
	wxString m_strFileName = wxT("");
	unsigned long m_ulDosDate = 0;
	unsigned long m_ulUncompressedSize = 0;
	
	
private:
	void OnDuplicateCheck(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
#endif