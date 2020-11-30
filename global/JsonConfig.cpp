#include "global.h"
#include "JsonConfig.h"

#include <iostream>
#include <fstream>
#include <streambuf>

std::unique_ptr<CJsonConfig> CJsonConfig::m_pInstance(nullptr);

CJsonConfig::CJsonConfig()
	: m_nWidth(1480)
	, m_nHeight(800)
	, m_nXPos(0)
	, m_nYPos(0)
	, m_nSplitWindowStyle(WINDOW_SPLIT_NONE)
	, m_nDispColumn(1)
	, m_nDispFloppy(0)
	, m_nDispSystemFile(0)
	, m_nDispHiddenFile(0)
	, m_nDispHeader(0)
	, m_nMinThreadRead(50)
	, m_nFileInfoView(1)
	, m_nTimeFormat(1)
	, m_nFontSize(10)
	, m_nPreIconReadCount(400)
	, m_nCurrentMonitor(0)
	, m_nMaximized(0)
	, m_nLastSessionSave(0)
	, m_iPreRead(0)
	, m_iUseSHBrowseForFolder(-1)
	, m_strFontName(wxT("consolas"))
	, m_strFileEditProgram(wxT(""))
	, m_strLeftPath(wxT("C:"))
	, m_strRightPath(wxT("C:"))
	, m_tabStyle(0)
	, m_rcDefault(100, 100, 1280, 900)
	, m_defaultColor(128, 128, 128)
	, m_dirColor(128, 255, 0)
	, m_drvColor(128, 255, 0)
	, m_readOnlyColor(128, 255, 0)
	, m_selectItemColor(255, 255, 255)
	, m_backgroundColor(0, 0, 0)
	, m_attributeColor(128, 128, 128)
	, m_descColor(128, 0, 255)
{
	m_tabStyle = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
	m_tabStyle |= wxAUI_NB_CLOSE_ON_ALL_TABS;
	m_tabStyle |= wxAUI_NB_WINDOWLIST_BUTTON;
	m_tabStyle |= wxAUI_NB_SCROLL_BUTTONS;
//	m_tabStyle |= wxAUI_NB_TAB_FIXED_WIDTH;
};

CJsonConfig::~CJsonConfig()
{
	_jsonDoc.RemoveAllMembers();
}

CJsonConfig* CJsonConfig::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CJsonConfig());

	return m_pInstance.get();
}

bool CJsonConfig::Load()
{
	wxString strJson(theCommonUtil->GetWorkDir() + SLASH + wxT("settings") + SLASH + wxT("settings.json"));
	if (!wxFileName::FileExists(strJson))
		return false;

	bool bReturn = DoLoad(strJson);
	if (bReturn)
	{
		SetGlobalConfig();
		SetDefaultColor();
	}

	return bReturn;
}

bool CJsonConfig::IsExistMember(const wxString& strGroup, const wxString& strMember)
{
	assert(_jsonDoc.IsObject());

	wxString _strKey(strGroup);
	wxString _strMember(strMember);

	if (_strKey.IsEmpty()) return HasMember(_strMember);// _jsonDoc.HasMember(_strMember.char_str());
	else                   return HasMember(_strMember, _strKey);
	/*
	else
	{
		Value _Key (_strKey.c_str(), _jsonDoc.GetAllocator());
		return _jsonDoc[_Key].HasMember(_strMember.char_str());
	}
	*/
	return true;
}

void CJsonConfig::SetGlobalConfig()
{
	assert(_jsonDoc.IsObject());
	assert(HasMember(wxT("globalsettings")));// _jsonDoc.HasMember("globalsettings"));

	m_nDispMsgBoxDirWatcher                  = _jsonDoc["globalsettings"]["DispMsgBoxDirWatcher"].GetInt();
	m_nDispFloppy                            = _jsonDoc["globalsettings"]["DispFloppy"].GetInt();
	m_nDispSystemFile                        = _jsonDoc["globalsettings"]["DispSystemFile"].GetInt();
	m_nDispHiddenFile                        = _jsonDoc["globalsettings"]["DispHiddenFile"].GetInt();
	m_nDispAllFile                           = _jsonDoc["globalsettings"]["DispAllFile"].GetInt();
	m_nDispHeader                            = _jsonDoc["globalsettings"]["DispHeader"].GetInt();
	m_nDispDriveListView                     = _jsonDoc["globalsettings"]["DispDriveListView"].GetInt();
	//************************************************************************************************
	//2018.08.20 Add
	m_iDirectoryWatcherAsync                 = _jsonDoc["globalsettings"]["DirectoryWatcherAsync"].GetInt();
	//************************************************************************************************
	m_iUseSHBrowseForFolder                  = _jsonDoc["globalsettings"]["UseWindowSHBrowseFolder"].GetInt();

	m_strLeftPath        = _jsonDoc["globalsettings"]["LeftPath"].GetString();
	m_strRightPath       = _jsonDoc["globalsettings"]["RightPath"].GetString();

	const Value& FileEditPGList = _jsonDoc["globalsettings"]["FileEditProgram"];
	assert(FileEditPGList.IsArray());

	for (SizeType i = 0; i < FileEditPGList.Size(); i++)
	{
		EXTERNAL_EDIT_PROGRAM extPG;
		extPG._strPGName = FileEditPGList[i]["name"].GetString();
		extPG._strPGPath = FileEditPGList[i]["path"].GetString();
		extPG._strPGArgs = FileEditPGList[i]["args"].GetString();

		std::unordered_map<int, EXTERNAL_EDIT_PROGRAM>::value_type value(i, extPG);
		_gExternalPGList.insert(value);
	}
}

void CJsonConfig::SetDefaultColor()
{
	wxString strdefaultColor(_jsonDoc["defaultcolor"]["DefaultColor"].GetString());
	wxString strdirColor(_jsonDoc["defaultcolor"]["DirColor"].GetString());
	wxString strdrvColor(_jsonDoc["defaultcolor"]["DrvColor"].GetString());
	wxString strreadOnlyColor(_jsonDoc["defaultcolor"]["ReadOnlyColor"].GetString());
	wxString strselectItemColor(_jsonDoc["defaultcolor"]["SelectItemColor"].GetString());
	wxString strbackgroundColor(_jsonDoc["defaultcolor"]["BackgroundColor"].GetString());
	wxString strattributeColor(_jsonDoc["defaultcolor"]["AttributeColor"].GetString());
	wxString strdescColor(_jsonDoc["defaultcolor"]["DescColor"].GetString());
	wxString strcolumnColor(_jsonDoc["defaultcolor"]["ColumnColor"].GetString());
	wxString strinfoviewColor(_jsonDoc["defaultcolor"]["InfoViewBackColor"].GetString());
	wxString strinfoviewtxtColor(_jsonDoc["defaultcolor"]["InfoViewTextColor"].GetString());
	wxString str_matchColor(_jsonDoc["defaultcolor"]["matchColor"].GetString());

	unsigned long nDefaultColor     = ConvertStringToColor(strdefaultColor);
	unsigned long nDirColor         = ConvertStringToColor(strdirColor);
	unsigned long nDrvColor         = ConvertStringToColor(strdrvColor);
	unsigned long nReadOnlyColor    = ConvertStringToColor(strreadOnlyColor);
	unsigned long nSelectItemColor  = ConvertStringToColor(strselectItemColor);
	unsigned long nBackgroundColor  = ConvertStringToColor(strbackgroundColor);
	unsigned long nAttributeColor   = ConvertStringToColor(strattributeColor);
	unsigned long nDescColor        = ConvertStringToColor(strdescColor);
	unsigned long nColumnColor      = ConvertStringToColor(strcolumnColor);
	unsigned long ninfoviewColor    = ConvertStringToColor(strinfoviewColor);
	unsigned long ninfoviewtxtColor = ConvertStringToColor(strinfoviewtxtColor);
	unsigned long ulmatchColor      = ConvertStringToColor(str_matchColor);

	m_defaultColor     = theCommonUtil->GetColor(nDefaultColor);
	m_dirColor         = theCommonUtil->GetColor(nDirColor);
	m_drvColor         = theCommonUtil->GetColor(nDrvColor);
	m_readOnlyColor    = theCommonUtil->GetColor(nReadOnlyColor);
	m_selectItemColor  = theCommonUtil->GetColor(nSelectItemColor);
	m_backgroundColor  = theCommonUtil->GetColor(nBackgroundColor);
	m_attributeColor   = theCommonUtil->GetColor(nAttributeColor);
	m_descColor        = theCommonUtil->GetColor(nDescColor);
	m_columnColor      = theCommonUtil->GetColor(nColumnColor);
	m_infoviewColor    = theCommonUtil->GetColor(ninfoviewColor);
	m_infoviewtxtColor = theCommonUtil->GetColor(ninfoviewtxtColor);
	m_matchColor       = theCommonUtil->GetColor(ulmatchColor);
}

void CJsonConfig::SetChangeGlobalVal(const wxString& strMember, int iVal)
{
	wxString _strMember(strMember);

	Value _Key(_strMember.c_str(), _jsonDoc.GetAllocator());
	_jsonDoc["globalsettings"][_Key] = iVal;
}

void CJsonConfig::SetChangeGlobalVal(const wxString& strMember, const wxString& strVal)
{
	wxString _strMember(strMember);
	wxString _strVal(strVal);

	Value _Key(_strMember.c_str(), _jsonDoc.GetAllocator());
	Value _Val(_strVal.c_str(), _jsonDoc.GetAllocator());

	_jsonDoc["globalsettings"][_Key] = _Val;
}

void CJsonConfig::SetChangeColorVal(const wxString& strKey, const wxString& strMember, const wxString& strVal)
{
	if (!HasMember(strKey))
		return;

	if (!HasMember(strMember, strKey))
		return;

	Value _Key(strKey.c_str(), _jsonDoc.GetAllocator());
	Value _Member(strMember.c_str(), _jsonDoc.GetAllocator());
	Value _Val(strVal.c_str(), _jsonDoc.GetAllocator());

	_jsonDoc[_Key][_Member] = _Val;
}

const wxColour CJsonConfig::GetExtColor(const wxString& strExt)
{
	wxString _strExt(strExt);

	Value::ConstMemberIterator Iter = _jsonDoc["extcolor"].FindMember(_strExt.c_str());
	if (Iter == _jsonDoc["extcolor"].MemberEnd())
		return m_defaultColor;

	wxString strColorValue(Iter->value.GetString());

	unsigned long lColorValue = ConvertStringToColor(strColorValue);
	wxColour col = theCommonUtil->GetColor(lColorValue);
	return col;
}

unsigned long CJsonConfig::ConvertStringToColor(wxString& strColor)
{
	unsigned long lvalue = 0;
	strColor.ToULong(&lvalue, 16);

	return lvalue;
}

void CJsonConfig::SetSplitStyle(const int iViewStyle)
{
	_jsonDoc["globalsettings"]["SplitWindowStyle"] = iViewStyle;
	m_nSplitWindowStyle = iViewStyle;
}

void CJsonConfig::SetViewFileKind(enum FILE_VIEW_OPTION fvo)
{
	if (fvo == FILE_VIEW_OP_DEFAULT)
	{
		m_nDispAllFile = 0;
		m_nDispHiddenFile = 0;
		m_nDispSystemFile = 0;

		_jsonDoc["globalsettings"]["DispHiddenFile"] = m_nDispHiddenFile;
		_jsonDoc["globalsettings"]["DispSystemFile"] = m_nDispSystemFile;
		_jsonDoc["globalsettings"]["DispAllFile"] = m_nDispAllFile;
	}
	else if (fvo == FILE_VIEW_OP_HIDDEN)
	{
		m_nDispAllFile = 0;
		m_nDispHiddenFile = m_nDispHiddenFile ? 0 : 1;

		_jsonDoc["globalsettings"]["DispHiddenFile"] = m_nDispHiddenFile;
		_jsonDoc["globalsettings"]["DispAllFile"] = m_nDispAllFile;
	}
	else if (fvo == FILE_VIEW_OP_ALL)
	{
		m_nDispAllFile = m_nDispAllFile ? 0 : 1;

		_jsonDoc["globalsettings"]["DispHiddenFile"] = 0;
		_jsonDoc["globalsettings"]["DispSystemFile"] = 0;
		_jsonDoc["globalsettings"]["DispAllFile"] = m_nDispAllFile;
	}
}

void CJsonConfig::SetColumnCount(const int iColumnCnt)
{
	m_nDispColumn = iColumnCnt;
	_jsonDoc["globalsettings"]["DispColumn"] = iColumnCnt;
}

void CJsonConfig::SaveConfig()
{
	wxString strOutJson(m_strJsonPath);
	std::ofstream ofs;

	ofs.open(strOutJson.char_str());

	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	_jsonDoc.Accept(writer);
}

void CJsonConfig::SetExternalEditProgram(wxVector<EXTERNAL_EDIT_PROGRAM>& vecExternal)
{
	_gExternalPGList.clear();
	Document::AllocatorType& allocator = _jsonDoc.GetAllocator();
	Value valExtPG(kArrayType);

	int iExtPGCount = vecExternal.size();
	for (int i = 0; i < iExtPGCount; i++)
	{
		EXTERNAL_EDIT_PROGRAM extPG = vecExternal.at(i);

		Value item;
		item.SetObject();

		Value _name(extPG._strPGName.c_str(), allocator);
		Value _path(extPG._strPGPath.c_str(), allocator);
		Value _args(extPG._strPGArgs.c_str(), allocator);

		item.AddMember("name", _name, allocator);
		item.AddMember("path", _path, allocator);
		item.AddMember("args", _args, allocator);

		valExtPG.PushBack(item, allocator);

		std::unordered_map<int, EXTERNAL_EDIT_PROGRAM>::value_type value(i, extPG);
		_gExternalPGList.insert(value);
	}

	_jsonDoc["globalsettings"]["FileEditProgram"] = valExtPG;
}
