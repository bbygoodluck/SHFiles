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
		
	bool UnCompress(const wxString& strCompressFile);
	void ClearCompressInfo();
	
	CCompressImpl* GetCompressImpl();
	std::vector<wxString>& GetCompressList() { return m_vCompress; }
	
protected:
	void Init();
	
private:
	static std::unique_ptr<CCompress> m_pInstance;
	CCompressImpl* m_pCompressImpl = nullptr;
	wxString m_strCompressedFile = wxT("");
	COMPRESS_TYPE m_pCompressType;
	
	std::vector<wxString> m_vCompress;
};
#endif