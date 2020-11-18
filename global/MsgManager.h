#ifndef __MESSAGE_MANAGER_H__
#define __MESSAGE_MANAGER_H__

#include "JsonBase.h"
class CMsgManager : public CJsonBase
{
private:
	CMsgManager() {};

public:
	~CMsgManager() {
		_jsonDoc.RemoveAllMembers();
	};

public:
	virtual bool Load();
	wxString GetMessage(const wxString& strMessageDefine);

public:
	static CMsgManager *Get();

private:
	static std::unique_ptr<CMsgManager> m_pMsgInstance;
};
#endif