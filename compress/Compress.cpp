#include "../ginc.h"
#include "CompressImpl.h"
#include "Compress.h"
#include "ZipFileImpl.h"

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

bool CCompress::IsCompressedFile(const wxString& strExt)
{
	bool bCompressedFile = false;
	wxString strExtItem(strExt);
	for(auto item : m_vCompress)
	{
		if(theCommonUtil->Compare(item.MakeLower(), strExtItem.MakeLower()) == 0)
		{
			bCompressedFile = true;
			break;
		}
	}
	
	return bCompressedFile;
}

bool CCompress::SetCompressInfo(const wxString& strFullPath, const wxString& strCompressedFile, const wxString& strCompressType)
{
	std::vector<wxString> vecDatas;
	vecDatas.emplace_back(strFullPath);
		
	return SetCompressInfo(vecDatas, strCompressedFile, strCompressType);
}

bool CCompress::SetCompressInfo(const std::vector<wxString>& vecDatas, const wxString& strCompressedFile, const wxString& strCompressType)
{
	m_strCompressedFile = strCompressedFile;
	if(!CreateCompressImpl())
		return false;
		
	m_pCompressImpl->SetCompressInfo(vecDatas, strCompressedFile);
	return true;
}

CCompressImpl* CCompress::GetCompressImpl()
{
	return m_pCompressImpl;
}

bool CCompress::SetUnCompressedInfo(const wxString& strCompressedFile, const wxString& strDeCompressDir)
{
	m_strCompressedFile = strCompressedFile;
	if(!CreateCompressImpl())
		return false;
		
	m_pCompressImpl->SetDeCompressInfo(strCompressedFile, strDeCompressDir);
}

bool CCompress::CreateCompressImpl()
{
	wxString strExt = theCommonUtil->GetExt(m_strCompressedFile);
	m_pCompressType = COMPTYPE_NONE;
	if(theCommonUtil->Compare(strExt, wxT("zip")) == 0)
		m_pCompressType = COMPTYPE_ZIP;
		
	switch(m_pCompressType)
	{
		case COMPTYPE_ZIP:
		{
			if(m_pCompressImpl == nullptr)
				m_pCompressImpl = new CZipFileImpl();
		}
			break;
			
		default:
			break;
	}
	
	if(!m_pCompressImpl)
		return false;
		
	return true;
}

void CCompress::ClearCompressInfo()
{
	if(m_pCompressImpl->GetCompressCancel())
		wxRemoveFile(m_strCompressedFile);
	
	if(m_pCompressImpl->GetThread() && m_pCompressImpl->GetThread()->IsRunning())
		m_pCompressImpl->GetThread()->Wait();

	if(m_pCompressImpl)
		delete m_pCompressImpl;
		
	m_pCompressImpl = nullptr;
}