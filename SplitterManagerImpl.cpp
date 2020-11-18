#include "ginc.h"
#include "SplitterManagerImpl.h"

wxBEGIN_EVENT_TABLE(CSplitterManagerImpl, wxSplitterWindow)
	EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, CSplitterManagerImpl::OnPositionChanged)
	EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, CSplitterManagerImpl::OnPositionChanging)
	EVT_SPLITTER_DCLICK(wxID_ANY, CSplitterManagerImpl::OnDClick)
	EVT_SPLITTER_UNSPLIT(wxID_ANY, CSplitterManagerImpl::OnUnsplitEvent)
wxEND_EVENT_TABLE()

CSplitterManagerImpl::CSplitterManagerImpl(wxWindow* parent)
	: wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, /*wxSP_3D*/ wxSP_NOBORDER | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxSP_NO_XP_THEME)
	, m_pParentWindow(parent)
{
}

CSplitterManagerImpl::~CSplitterManagerImpl()
{
}

void CSplitterManagerImpl::OnPositionChanged(wxSplitterEvent& event)
{
	event.Skip();
}

void CSplitterManagerImpl::OnPositionChanging(wxSplitterEvent& event)
{
	event.Skip();
}

void CSplitterManagerImpl::OnDClick(wxSplitterEvent& event)
{
	event.Skip();
}

void CSplitterManagerImpl::OnUnsplitEvent(wxSplitterEvent& event)
{
	event.Skip();
}
