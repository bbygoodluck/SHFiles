#ifndef __JSON_BOOKMARK_H__
#define __JSON_BOOKMARK_H__

#include "JsonBase.h"

class CJsonBookmark : public CJsonBase
{
private:
	CJsonBookmark();

public:
	~CJsonBookmark();
	static CJsonBookmark* Get();

	virtual bool Load();
	unsigned int GetBookmarkSize();

	void CreateBookmarkMenu(wxMenu* pMenu);
	wxString GetBookmarkPath(int iFavoriteID);
	
	void RemoveAll();
	void Save();

private:
	void DoCreateBookmark(wxMenu* pMenu, const Value& items);

private:
	static std::unique_ptr<CJsonBookmark> m_pInstance;

	int m_iMenuID;
	int m_iAccel;
	int m_iMenuAccelIndex;
	int m_iAddFavoriteCount;

	wxBitmap m_bmpBookmark;
	wxBitmap m_bmpGroup;

	wxVector<wxString> m_vecMenuAccel;
	std::unordered_map<int, wxString> m_mapFavorite;
};
#endif
