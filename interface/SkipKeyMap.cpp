#include "../ginc.h"
#include "SkipKeyMap.h"

std::unique_ptr<CSkipKeyMap> CSkipKeyMap::m_pInstance = nullptr;

/*
CSkipKeyMap::CSkipKeyMap()
{
	InitSkipKeyMap();
}

CSkipKeyMap::~CSkipKeyMap()
{
	m_SkipKeyMap.clear();
}
*/

CSkipKeyMap* CSkipKeyMap::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CSkipKeyMap());

	return m_pInstance.get();
}

void CSkipKeyMap::InitSkipKeyMap()
{
	m_SkipKeyMap.clear();
	std::unordered_map<int, int> skipKeymap = {
		{ WXK_LEFT, WXK_LEFT },
		{ WXK_RIGHT, WXK_RIGHT },
		{ WXK_UP, WXK_UP },
		{ WXK_DOWN, WXK_DOWN },
		{ WXK_PAGEDOWN, WXK_PAGEDOWN },
		{ WXK_PAGEUP, WXK_PAGEUP },
		{ WXK_HOME, WXK_HOME },
		{ WXK_END, WXK_END },
		{ WXK_TAB, WXK_TAB },
		{ WXK_SHIFT, WXK_SHIFT },
		{ WXK_F1, WXK_F1 },
		{ WXK_F2, WXK_F2 },
		{ WXK_F3, WXK_F3 },
		{ WXK_F4, WXK_F4 },
		{ WXK_F5, WXK_F5 },
		{ WXK_F6, WXK_F6 },
		{ WXK_F7, WXK_F7 },
		{ WXK_F8, WXK_F8 },
		{ WXK_F9, WXK_F9 },
		{ WXK_F10, WXK_F10 },
		{ WXK_F11, WXK_F11 },
		{ WXK_F12, WXK_F12 },
		{ WXK_F13, WXK_F13 },
		{ WXK_F14, WXK_F14 },
		{ WXK_F15, WXK_F15 },
		{ WXK_F16, WXK_F16 },
		{ WXK_F17, WXK_F17 },
		{ WXK_F18, WXK_F18 },
		{ WXK_F19, WXK_F19 },
		{ WXK_F20, WXK_F20 },
		{ WXK_F21, WXK_F21 },
		{ WXK_F22, WXK_F22 },
		{ WXK_F23, WXK_F23 },
		{ WXK_F24, WXK_F24 },
		{ WXK_NUMLOCK, WXK_NUMLOCK },
		{ WXK_ESCAPE, WXK_ESCAPE },
		{ WXK_SPACE, WXK_SPACE },
		{ WXK_DELETE, WXK_DELETE },
		{ WXK_ALT, WXK_ALT },
		{ WXK_CONTROL, WXK_CONTROL },
		{ WXK_INSERT, WXK_INSERT },
		{ WXK_RETURN, WXK_RETURN },
		{ WXK_REVERSE_SLASH, WXK_REVERSE_SLASH },
		{ WXK_SLASH, WXK_SLASH },
		{ WXK_COLONE, WXK_COLONE },
		{ WXK_QUESTION, WXK_QUESTION },
		{ WXK_DBQUATATION, WXK_DBQUATATION },
		{ WXK_LEFT_M_G, WXK_LEFT_M_G },
		{ WXK_RIGHT_M_G, WXK_RIGHT_M_G },
		{ WXK_V_LINE, WXK_V_LINE },
		{ wxKEY_DOT, wxKEY_DOT },
		{ WXK_WINDOWS_KEY, WXK_WINDOWS_KEY },
	};

	std::copy(skipKeymap.begin(), skipKeymap.end(), std::inserter(m_SkipKeyMap, m_SkipKeyMap.begin()));
}