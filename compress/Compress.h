#ifndef __COMPRESS_H_INCLUDED
#define __COMPRESS_H_INCLUDED

class CCompressImpl;
class CCompress
{	
private:
	CCompress() {};
	
public:
	~CCompress() {};
	static CCompress* Get();

public:
	bool SetCompressInfo(const wxString& strFullPath, const wxString& strCompressedFile);
	bool SetCompressInfo(const std::vector<wxString>& vecDatas, const wxString& strCompressedFile);
	bool SetUnCompressedInfo(const wxString& strCompressedFile, const wxString& strDeCompressDir);
	
	void DoCancel() { m_bCancel = true; }
	bool IsCancel() { return m_bCancel; }
	bool IsExtract() { return m_bDeCompress; }
	void ClearCompressInfo();
	
	CCompressImpl* GetCompressImpl();
	std::vector<wxString>& GetCompressList() { return m_vCompress; }
	
	bool IsCompressedFile(const wxString& strExt);
	
	std::vector<wxString> GetCompressingDatas() { return m_vecCompressingDatas; }
	wxString GetCompressedFile() { return m_strCompressedFile; }
	wxString GetDeCompressDir() { return m_strDeCompressDir; }
	
	void DoStart(wxDialog* pOwnerDlg);
	
protected:
	void Init();
	bool CreateCompressImpl();
	
private:
	static std::unique_ptr<CCompress> m_pInstance;
	CCompressImpl* m_pCompressImpl = nullptr;
	
	std::vector<wxString> m_vecCompressingDatas;
	wxString m_strCompressedFile = wxT("");
	wxString m_strDeCompressDir = wxT("");
	
	COMPRESS_TYPE m_pCompressType;
	
	std::vector<wxString> m_vCompress;
	bool m_bDeCompress = false;
	bool m_bCancel = false;
};
#endif