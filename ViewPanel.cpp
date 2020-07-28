#include "ginc.h"

#ifdef __WXMSW__
	#include "./view/msw/LocalFileListView.h"
#endif

#include "ViewPanel.h"

wxBEGIN_EVENT_TABLE(CViewPanel, wxPanel)
	EVT_ERASE_BACKGROUND(CViewPanel::OnErase)
wxEND_EVENT_TABLE()

CViewPanel::CViewPanel(wxWindow* parent, int iID, const wxString& strPath)
	: wxPanel(parent, iID, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE)
	, m_Mainsizer(nullptr)
	, m_strPath(strPath)
{
	Init(m_strPath);
}

CViewPanel::~CViewPanel()
{
	
}

void CViewPanel::Init(const wxString& strPath, bool bRemote)
{
	wxString _strPath(strPath);
	m_Mainsizer = new wxBoxSizer(wxVERTICAL);
	//패스뷰 등록
	m_pPathView = std::make_unique<CPathView>(this, wxPATHVIEW_ID++, wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), 20));
	//디렉토리 정보뷰
	m_pDirInfoView = std::make_unique<CDirInfoView>(this, ++wxDIRINFOVIEW_ID, wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), 21));
	//파일 리스트뷰 생성
	m_pFileListView = std::make_unique<CLocalFileListView>(this, ++wxFILFELIST_VIEWID, wxDefaultSize);
	//상세설명 뷰 생성
	m_pDetailInfoView = std::make_unique<CDetailInfoView>(this, ++wxINFOVIEW_ID, wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), 21));
	//생성한 뷰 Sizer등록
	m_Mainsizer->Add(m_pPathView.get(), 0, wxEXPAND, 0);
	m_Mainsizer->Add(m_pDirInfoView.get(), 0, wxEXPAND, 0);
	m_Mainsizer->Add(m_pFileListView.get(), wxSizerFlags(1).Expand().Border(wxALL, 0));
	m_Mainsizer->Add(m_pDetailInfoView.get(), 0, wxEXPAND, 0);
	
	if (!wxDirExists(_strPath))
		_strPath = wxT("C:\\");
	
	m_pFileListView->LoadDir(_strPath);
	
	SetSizer(m_Mainsizer);
	m_Mainsizer->SetSizeHints(this);
}

void CViewPanel::OnErase(wxEraseEvent& WXUNUSED(event))
{

}

void CViewPanel::SetActivateView()
{
	m_pFileListView->SetFocus();
}

void CViewPanel::SetActivateColumnChanged()
{
	m_pFileListView->m_bSizeOrColumnChanged = true;
	theCommonUtil->RefreshWindow(m_pFileListView.get(), m_pFileListView->m_viewRect);
}

bool CViewPanel::GetSelectedItem(bool bUseClipboard, bool bMove, std::list<wxString>& lstItems)
{
	m_pFileListView->MakeCopyOrMoveList(bUseClipboard, bMove, lstItems);
	if(lstItems.size() == 0)
		return false;
	
	if(bUseClipboard)
	{
		theClipboard->Clear();
		theClipboard->SetFileOperation(bMove ? _MENU_FILE_MOVE_CLIPBOARD : _MENU_FILE_COPY_CLIPBOARD);
		theClipboard->AddData(lstItems);
		theClipboard->CopyToClipboard();
	}
	
	return true;
}

bool CViewPanel::GetDeleteItems(std::list<wxString>& lstDatas, bool bTrash)
{
	if(!m_pFileListView->MakeTrashOrDeleteData(lstDatas, bTrash))
		return false;
		
	if(lstDatas.size() == 0)
		return false;
		
	return true;
}

void CViewPanel::DoMyEventExecuteToView(wxEventType evtType, const wxString& strInfo)
{
	wxCommandEvent evt(evtType);
	evt.SetString(strInfo);
	wxPostEvent(m_pFileListView.get(), evt);
}

void CViewPanel::TransferInfomation(TRANSFER_PATH_TYPE _transferType, const wxString& strInfo)
{
	switch(_transferType)
	{
		case TRANSFER_LISTVIEW_TO_PATHVIEW:
			m_pPathView->AddPath(strInfo);
			break;
			
		case TRANSFER_PATH_VIEW_TO_LISTVIEW:
		{
			theSplitterManager->ChangeTabPagePathName(strInfo);
			ReloadPathOfView(strInfo);
			SetActivateView();
		}
			break;
		
		case TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW:
			m_pDirInfoView->SetDirInfo(m_pFileListView->GetDirInfo());
			break;
			
		case TRANSFER_LISTVIEW_DETAILINFO_TO_DETAILVIEW:
			m_pDetailInfoView->SetDetailInfo(strInfo);
			break;
			
		default:
		
			break;
	}
}

void CViewPanel::ClearSelectedItems()
{
	m_pFileListView->DoSelectedItemsClear();
	theCommonUtil->RefreshWindow(m_pFileListView.get(), m_pFileListView->m_viewRect);
}

void CViewPanel::ReloadPathOfView(const wxString& strPathInfo)
{
	wxString strCurrentPath = strPathInfo.IsEmpty() ? m_pFileListView->GetCurrentPath() : strPathInfo;
	m_pFileListView->LoadDir(strCurrentPath);
	
	theCommonUtil->RefreshWindow(m_pFileListView.get(), m_pFileListView->m_viewRect);
}