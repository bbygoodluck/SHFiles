#ifndef VIEWPANEL_H_INCLUDED
#define VIEWPANEL_H_INCLUDED

#include <list>
#include "./view/ListView.h"
#include "./view/PathView.h"
#include "./view/DirInfoView.h"
#include "./view/DetailInfoView.h"

class CViewPanel : public wxPanel
{
public:
	explicit CViewPanel(wxWindow* parent, int iID, const wxString& strPath);
	virtual ~CViewPanel();

public:
	void Init(const wxString& strPath, bool bRemote = false);
	void SetActivateView();
	void SetActivateColumnChanged();

	wxString GetCurrentViewPath() { return m_pFileListView->m_strCurrentPath; }
	wxString GetCurrentItem();

	void ClearSelectedItems(bool bDeleted = false);
	int GetSelectedItemCount()
	{
		int iSelectedCount = m_pFileListView->m_hashSelectedItem.size();
		return iSelectedCount == 0 ? 1 : iSelectedCount;
	}

	bool GetSelectedItemForCopyOrMove(bool bUseClipboard, bool bMove, std::list<wxString>& lstItems);
	bool GetDeleteItems(std::list<wxString>& lstDatas, bool bTrash);

	CListView* GetListView() { return m_pFileListView.get(); }

	void DoMyEventExecuteToView(wxEventType evtType, const wxString& strInfo = wxT(""));
	void TransferInfomation(TRANSFER_PATH_TYPE _transferType, const wxString& strInfo = wxT(""));
	void ReloadPathOfView(const wxString& strPathInfo = wxT(""));
	void ShowCompressMenu();

private:
	wxBoxSizer* m_Mainsizer;
	wxString	m_strPath;
	wxSize		m_size;

	// 폴더/파일 리스트뷰
	std::unique_ptr<CListView>    m_pFileListView = nullptr;
	//패스정보
	std::unique_ptr<CPathView>    m_pPathView = nullptr;
	//디렉토리 정보
	std::unique_ptr<CDirInfoView> m_pDirInfoView = nullptr;
	// 상세설명
	std::unique_ptr<CDetailInfoView> m_pDetailInfoView;

private:
	void OnErase(wxEraseEvent& event);
	wxDECLARE_EVENT_TABLE();

};

#endif // VIEWPANEL_H_INCLUDED
