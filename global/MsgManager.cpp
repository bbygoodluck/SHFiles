#include "global.h"
#include "MsgManager.h"

std::unique_ptr<CMsgManager> CMsgManager::m_pMsgInstance(nullptr);

bool CMsgManager::Load()
{
	wxString strMsgPath = theCommonUtil->GetWorkDir() + SLASH + wxT("lang") + SLASH;

	if (_gSystemLang.CmpNoCase(wxT("ko")) == 0)
		strMsgPath += wxT("ko_lang.json");
	else
		strMsgPath += wxT("en_lang.json");
	
	if(!wxFileExists(strMsgPath))
		return false;
		
	bool bReturn = DoLoad(strMsgPath);
	return bReturn;
}

wxString CMsgManager::GetMessage(const wxString& strMessageDefine)
{
	wxString _strKey(strMessageDefine);

	assert(_jsonDoc.IsObject());
	assert(_jsonDoc.HasMember(_strKey.char_str()));

	Value::ConstMemberIterator Iter = _jsonDoc.FindMember(_strKey.c_str());
	if (Iter == _jsonDoc.MemberEnd())
		return wxT("");

	wxString strRetMsg(Iter->value.GetString());
	return strRetMsg;
}

/* static */ CMsgManager* CMsgManager::Get()
{
	if (m_pMsgInstance.get() == nullptr)
		m_pMsgInstance.reset(new CMsgManager());
	
	return m_pMsgInstance.get();
}
