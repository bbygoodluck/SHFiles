#ifndef __CLIP_BOARD_H__
#define __CLIP_BOARD_H__

class CClipBoardBase
{
public:
	explicit CClipBoardBase() {}
	~CClipBoardBase() {}

public:
	virtual void AddData(const wxString& strData) = 0;
	virtual void AddData(const std::list<wxString>& lstDatas) = 0;
	virtual void GetData(std::list<wxString>& lstDatas) = 0;
	void SetFileOperation(_MENU_EVENT_TYPE _menuType) {
		m_menuType = _menuType;
	}

	_MENU_EVENT_TYPE GetFileOperation() {
		return m_menuType;
	}

	virtual void CopyToClipboard() = 0;

private:
	_MENU_EVENT_TYPE m_menuType = _MENU_EVENT_NONE;
};

#endif