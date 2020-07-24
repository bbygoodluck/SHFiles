#include "ginc.h"
#include "TabManager.h"

wxBEGIN_EVENT_TABLE(CTabManager, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, OnPageClose)
wxEND_EVENT_TABLE()

CTabManager::CTabManager(wxWindow* parent, long style)
	: wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)
{
	m_PageBMP = wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_OTHER, wxSize(16, 16));
	this->SetArtProvider(new wxAuiGenericTabArt);
}

CTabManager::~CTabManager()
{
}

void CTabManager::AddTabPage(const wxString& strPath)
{
	Freeze();

	wxString strPathName = GetTabDispPathName(strPath);

	AddPage(new CViewPanel(this, ++wxTABWindowID, strPath), strPathName, false, m_PageBMP);

	int nPageCount = GetPageCount();

//	SetSelection(nPageCount - 1);
	SetPageToolTip(nPageCount - 1, strPath);

	Thaw();
}

wxString CTabManager::GetTabDispPathName(const wxString& strSourcePath)
{
	wxString::size_type n = strSourcePath.Find(SLASH);
	wxString strTarget = (n == wxString::npos ? strSourcePath + SLASH : theCommonUtil->GetPathName(strSourcePath));
	
	return strTarget;
}

CViewPanel* CTabManager::GetActiveViewPanel()
{
	int nSel = GetSelection();
	return (CViewPanel *)GetPage(nSel);
}

void CTabManager::OnPageClose(wxAuiNotebookEvent& event)
{
	int iPageCount = this->GetPageCount();
	if (iPageCount == 1)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DELETE_TAB")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		event.Veto();
		GetActiveViewPanel()->SetActivateView();
		return;
	}
}