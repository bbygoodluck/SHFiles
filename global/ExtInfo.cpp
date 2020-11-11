#include "global.h"
#include "ExtInfo.h"

#include <iostream>
#include <fstream>
#include <streambuf>

std::unique_ptr<CExtInfo> CExtInfo::m_pInstance(nullptr);

CExtInfo::~CExtInfo()
{
	SaveExtInfo();
	_jsonDoc.RemoveAllMembers();
}

CExtInfo* CExtInfo::Get()
{
	if (m_pInstance.get() == NULL)
	{
		m_pInstance.reset(new CExtInfo());
		m_pInstance->Load();

	}

	return m_pInstance.get();
}

bool CExtInfo::Load()
{
	wxString strJson(theCommonUtil->GetWorkDir() + SLASH + wxT("settings") + SLASH + wxT("extinfo.json"));

	if (!wxFileName::FileExists(strJson))
		return false;

	bool bReturn = DoLoad(strJson);
	return bReturn;
}

wxString CExtInfo::GetExtInfo(const wxString& strExt, const wxString& strFullPathName)
{
	assert(_jsonDoc.IsObject());

	wxString _strFullPath(strFullPathName);

	wxString _strExt(strExt);
	wxString strExtInfo(wxT(""));

	if (_strExt.IsEmpty())
		_strExt = theCommonUtil->GetPathName(strFullPathName);

	_strExt = _strExt.MakeUpper();

	Value::ConstMemberIterator Iter = _jsonDoc.FindMember(_strExt.c_str());
	if (Iter == _jsonDoc.MemberEnd())
	{
#ifdef __WXMSW__
		int flags = SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES;
		SHFILEINFO shfinfo;
		wxZeroMemory(shfinfo);

		HRESULT hr = SHGetFileInfo(_strFullPath, FILE_ATTRIBUTE_NORMAL, &shfinfo, sizeof(shfinfo), flags);
		if(SUCCEEDED(hr))
		{
			strExtInfo = shfinfo.szTypeName;

			if (strExtInfo.IsEmpty())
			{
				if (!_strExt.IsEmpty())
					strExtInfo = _strExt + theMsgManager->GetMessage(wxT("MSG_FILE_DESCRIPTION"));
				else
					strExtInfo = theMsgManager->GetMessage(wxT("MSG_FILE_GENERIC"));
			}
		}
#else
#endif
		Value _Key(_strExt.c_str(), _jsonDoc.GetAllocator());
		Value _Value(strExtInfo.c_str(), _jsonDoc.GetAllocator());

		_jsonDoc.AddMember(_Key, _Value, _jsonDoc.GetAllocator());

	}
	else
		strExtInfo = Iter->value.GetString();

	return strExtInfo;
}

void CExtInfo::SaveExtInfo()
{
	wxString strOutJson(m_strJsonPath);
	std::ofstream ofs;

	ofs.open(strOutJson.char_str());
/*
#ifdef _UNICODE
	ofs.open(strOutJson.wchar_str());
//	ofs.open(strOutJson.ToStdWstring());
#else
	ofs.open(strOutJson.char_str());
//	ofs.open(strOutJson.ToStdstring());
#endif
*/
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	_jsonDoc.Accept(writer);
}
