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
	bool SetCompressInfo(const wxString& strFullPath, const wxString& strCompressedFile, const wxString& strCompressType);
	bool SetCompressInfo(const std::vector<wxString>& vecDatas, const wxString& strCompressedFile, const wxString& strCompressType);
	bool SetUnCompressedInfo(const wxString& strCompressedFile, const wxString& strDeCompressDir);
	
	void ClearCompressInfo();
	
	CCompressImpl* GetCompressImpl();
	std::vector<wxString>& GetCompressList() { return m_vCompress; }
	
	bool IsCompressedFile(const wxString& strExt);
	
protected:
	void Init();
	bool CreateCompressImpl();
	
private:
	static std::unique_ptr<CCompress> m_pInstance;
	CCompressImpl* m_pCompressImpl = nullptr;
	wxString m_strCompressedFile = wxT("");
	COMPRESS_TYPE m_pCompressType;
	
	std::vector<wxString> m_vCompress;
};
#endif