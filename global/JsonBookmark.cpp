#include <iostream>
#include <fstream>
#include <streambuf>

#include "global.h"
#include "../EvtHandler.h"
#include "../MainFrame.h"
#include "JsonBookmark.h"

std::unique_ptr<CJsonBookmark> CJsonBookmark::m_pInstance(nullptr);

CJsonBookmark::CJsonBookmark()
	: m_iMenuID(wxFAVIROTE_ID_START)
	, m_iAccel(1)
{
	m_bmpBookmark = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	m_bmpGroup = wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_OTHER, wxSize(16, 16));

	m_vecMenuAccel.push_back(wxT("Ctrl+Shift+%d"));
	m_vecMenuAccel.push_back(wxT("Alt+Shift+%d"));
}

CJsonBookmark::~CJsonBookmark()
{
	wxVector<wxString>().swap(m_vecMenuAccel);
}

CJsonBookmark* CJsonBookmark::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CJsonBookmark());

	return m_pInstance.get();
}

bool CJsonBookmark::Load()
{
	wxString strJson(theCommonUtil->GetWorkDir() + SLASH + wxT("settings") + SLASH + wxT("bookmark.json"));
	if (!wxFileName::FileExists(strJson))
		return false;

	bool bReturn = DoLoad(strJson);
	return bReturn;
}

unsigned int CJsonBookmark::GetBookmarkSize()
{
	if (!HasMember(wxT("bookmarks")))
		return 0;

	const Value& bookmarlst = _jsonDoc["bookmarks"];
	assert(bookmarlst.IsArray());

	return bookmarlst.Size();
}

void CJsonBookmark::CreateBookmarkMenu(wxMenu* pMenu)
{
	m_iMenuID = wxFAVIROTE_ID_START;
	m_iAccel = 1;
	m_iMenuAccelIndex = 0;
	m_iAddFavoriteCount = 1;
	if (!HasMember(wxT("bookmarks")))
		return;

	m_mapFavorite.clear();
	DoCreateBookmark(pMenu, _jsonDoc["bookmarks"]);
}

void CJsonBookmark::DoCreateBookmark(wxMenu* pMenu, const Value& items)
{
	assert(items.IsArray());

	unsigned int iBookMSize = items.Size();
	for (unsigned int i = 0; i < iBookMSize; i++)
	{
		wxString strName = items[i]["name"].GetString();
		wxString strType = items[i]["type"].GetString();
		wxString strPath = items[i]["path"].GetString();

		if (strType.Cmp(wxT("item")) == 0)
		{
			wxMenuItem* pMenuItem = pMenu->Append(m_iMenuID, strName, strPath);

			if (m_iAddFavoriteCount < 19)
			{	//즐겨찾기 메뉴 추가시 메뉴 단축키는 Ctrl+Shift+1 ~ 9 / Alt+Shift+1 ~ 9 까지만 설정
				wxAcceleratorEntry* pAccel = new wxAcceleratorEntry();
				if ((m_iAddFavoriteCount % 10) == 0)
					m_iMenuAccelIndex++;

				pAccel->FromString(wxString::Format(m_vecMenuAccel.at(m_iMenuAccelIndex), m_iAccel));
				pMenuItem->SetAccel(pAccel);
				delete pAccel;
			}

			pMenuItem->SetBitmap(m_bmpBookmark);

			std::unordered_map<int, wxString>::value_type value(m_iMenuID, strPath);
			m_mapFavorite.insert(value);

			CEvtHandler* pEvtHandler = (CEvtHandler *)_gMainFrame->GetEventHandler();
			pEvtHandler->Bind(wxEVT_COMMAND_MENU_SELECTED, &CEvtHandler::OnBookmarkSelected, pEvtHandler, m_iMenuID);

			m_iAccel++;
			m_iMenuID++;
			m_iAddFavoriteCount++;
		}
		else
		{
			const Value& arrChilds = items[i]["child"];
			assert(arrChilds.IsArray());

			if (arrChilds.Size() >= 0)
			{
				wxMenu *pSubMenu = new wxMenu();
				wxMenuItem *pSubMenuItem = pMenu->AppendSubMenu(pSubMenu, strName);
				pSubMenuItem->SetBitmap(m_bmpGroup);

				DoCreateBookmark(pSubMenu, arrChilds);
			}
		}
	}
}

wxString CJsonBookmark::GetBookmarkPath(int iFavoriteID)
{
	std::unordered_map<int, wxString>::const_iterator iTer;

	iTer = m_mapFavorite.find(iFavoriteID);
	if (iTer == m_mapFavorite.end())
		return wxT("");

	return iTer->second;
}

void CJsonBookmark::RemoveAll()
{
	_jsonDoc.RemoveAllMembers();
}

void CJsonBookmark::Save()
{
	wxString strOutJson(m_strJsonPath);

	std::ofstream ofs;
	ofs.open(strOutJson.char_str());

	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	_jsonDoc.Accept(writer);
}
