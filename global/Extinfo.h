#ifndef __EXT_INFO_H__
#define __EXT_INFO_H__

#include "JsonBase.h"
class CExtInfo : public CJsonBase
{
private:
	CExtInfo() {

	};

public:
	~CExtInfo();
	static CExtInfo* Get();

	bool Load();
	wxString GetExtInfo(const wxString& strExt, const wxString& strFullPathName);
	void SaveExtInfo();

private:
	static std::unique_ptr<CExtInfo> m_pInstance;
//	Document _jsonDoc;

//	wxString m_strJsonPath;
};
#endif
