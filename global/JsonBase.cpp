#include "global.h"
#include "JsonBase.h"

bool CJsonBase::DoLoad(const wxString& strPath)
{
	m_strJsonPath = strPath;
	ifstream ifs;
	
	ifs.open(strPath.char_str());//.ToStdstring());
/*	
#ifdef _UNICODE
	ifs.open(strPath.wchar_str());
//	ifs.open(strPath.ToStdWstring());
#else
	ifs.open(strPath.char_str());
//	ifs.open(strPath.ToStdstring());
#endif
*/
	IStreamWrapper isw(ifs);

	bool bReturn = !_jsonDoc.ParseStream(isw).HasParseError();
	return bReturn;
}