#ifndef __SPLITTER_MANAGER_H__
#define __SPLITTER_MANAGER_H__

class CSplitterManagerImpl : public wxSplitterWindow
{
public:
	CSplitterManagerImpl(wxWindow* pWindow);
	virtual ~CSplitterManagerImpl();

private:
	wxWindow* m_pParentWindow;

private:
	// event handlers
	void OnPositionChanged(wxSplitterEvent& event);
	void OnPositionChanging(wxSplitterEvent& event);
	void OnDClick(wxSplitterEvent& event);
	void OnUnsplitEvent(wxSplitterEvent& event);
	wxDECLARE_EVENT_TABLE();

};

#endif