#ifndef __MSW_CONTEXT_MENU_HANDLER_H__
#define __MSW_CONTEXT_MENU_HANDLER_H__

#include "../ContextMenuHandler.h"
class CMSWContextMenuHandler : public CContextMenuHandlerBase
{
public:
	CMSWContextMenuHandler();
	virtual ~CMSWContextMenuHandler();

public:
	void SetObject(wxArrayString& strArray);
	void ShowContextMenu(wxWindow* pWnd, const wxPoint& pt);
	void Clear();

private:
	bool GetContextMenu(void ** ppContextMenu, int& iMenuType);
	LPITEMIDLIST CopyPIDL(LPCITEMIDLIST pidl, int cb = -1);
	UINT GetPIDLSize(LPCITEMIDLIST pidl);
	void FreePIDLArray();

private:
	IShellFolder* m_psfFolder;
	PIDLIST_ABSOLUTE* m_pidlArray;
	LPCONTEXTMENU m_pContextMenu;

	int	m_iItem;
	bool m_bDelete;

private:
	void OnInvokeCommand(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
typedef CMSWContextMenuHandler CContextMenuHandler;
#endif