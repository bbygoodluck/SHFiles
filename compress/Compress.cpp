#include "../ginc.h"
#include "Compress.h"

class CCompressImpl
{
public:
	CCompressImpl() {};
	~CCompressImpl() {};
	
public:
	virtual bool DoCompress(const std::vector<wxString>& strCompList, const wxString& strCompressedFile) = 0;
	virtual bool DoUnCompress(const wxString& strCompressFile) = 0;
};


class CZipFileImpl : public CCompressImpl
{
public:
	CZipFileImpl() {};
	~CZipFileImpl() {};
	
public:
	bool DoCompress(const std::vector<wxString>& strCompList, const wxString& strCompressedFile) override
	{
		return true;
	}
	
	bool DoUnCompress(const wxString& strCompressFile) override
	{
		return true;
	}
};

std::unique_ptr<CCompress> CCompress::m_pInstance = nullptr;
CCompress* CCompress::Get()
{
	if (m_pInstance.get() == nullptr)
	{
		m_pInstance.reset(new CCompress());
		m_pInstance->Init();
	}
	
	return m_pInstance.get();
}

void CCompress::Init()
{
	m_vCompress.emplace_back(wxT("zip"));
	m_vCompress.emplace_back(wxT("gz"));
}

bool CCompress::Compress(const wxString& strFullPath, const wxString& strCompressedFile, COMPRESS_TYPE comType)
{
	std::vector<wxString> vecDatas;
	vecDatas.emplace_back(strFullPath);
		
	return Compress(vecDatas, strCompressedFile, comType);
}

bool CCompress::Compress(std::vector<wxString>& vecDatas, const wxString& strCompressedFile, COMPRESS_TYPE comType)
{
	switch(comType)
	{
		case COMPTYPE_ZIP:
			m_pCompressImpl = new CZipFileImpl();
			m_pCompressImpl->DoCompress(vecDatas, strCompressedFile);
			break;
	}
	
	return true;
}

bool CCompress::UnCompress(const wxString& strCompressFile)
{
	return true;
}