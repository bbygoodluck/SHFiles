#ifndef __LOCAL_FILE_LISTVIEW_INCLUDED_H__
#define __LOCAL_FILE_LISTVIEW_INCLUDED_H__

#include "../ListView.h"
class CLocalWatcherMonitorHandler;

//class CLocalFileListView;
//typedef CLocalFileListView CFileListView;

class CLocalFileListView : public CListView
{
public:
	explicit CLocalFileListView(wxWindow* parent, const int nID, const wxSize& sz);
	virtual ~CLocalFileListView();
	
public:
	virtual void LoadDir(const wxString& strPath) override;
	virtual void DoCreate(const wxString& strName) override;
	virtual void DoModify(const wxString& strName) override;
	virtual void DoDelete(const wxString& strName) override;
	virtual void DoRename(const wxString& strOldName, const wxString& strNewName) override;

protected:
	virtual void Renderer(wxDC* pDC) override;
	virtual void DoRenameFromMenu(const wxString& strOldPathName, const wxString& strNewPathName) override;
	virtual void DoUpdateModificationTimeOfDir() override;
	
private:
	bool ReadDirectory();
	void AddDrive();
	virtual void ExecuteExternalProgramForEdit(int iIndex) override;
	
private:
	std::unique_ptr<CLocalWatcherMonitorHandler> m_pWatcherHandler;
	
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	wxDECLARE_EVENT_TABLE();
};
#endif