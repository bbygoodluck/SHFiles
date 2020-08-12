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
	void LoadDir(const wxString& strPath) override;
	void DoCreate(const wxString& strName) override;
	void DoModify(const wxString& strName) override;
	void DoDelete(const wxString& strName) override;
	void DoRename(const wxString& strOldName, const wxString& strNewName) override;

protected:
	void Renderer(wxDC* pDC) override;
	void DoRenameFromMenu(const wxString& strOldPathName, const wxString& strNewPathName) override;
	void DoUpdateModificationTimeOfDir() override;
	
private:
	bool ReadDirectory();
	void AddDrive();
		
	void ExecuteExternalProgramForEdit(int iIndex) override;
	
private:
	std::unique_ptr<CLocalWatcherMonitorHandler> m_pWatcherHandler;
	
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	
	void OnDriveAddOrRemove(wxCommandEvent& event);
	void OnUpdateDriveSpace(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
#endif