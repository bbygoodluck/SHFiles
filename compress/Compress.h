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
	bool Compress(const wxString& strFullPath, const wxString& strCompressedFile, COMPRESS_TYPE comType);
	bool Compress(std::vector<wxString>& vecDatas, const wxString& strCompressedFile, COMPRESS_TYPE comType);
	bool UnCompress(const wxString& strCompressFile);
	std::vector<wxString>& GetCompressList() { return m_vCompress; }
	
protected:
	void Init();
	
	
private:
	static std::unique_ptr<CCompress> m_pInstance;
	CCompressImpl* m_pCompressImpl = nullptr;
	
	std::vector<wxString> m_vCompress;
};
#endif