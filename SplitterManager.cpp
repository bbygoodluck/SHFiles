#include "ginc.h"
#include "SplitterManager.h"

CSplitterManager* CSplitterManager::m_pSplitterMngInstance(nullptr);

CSplitterManager* CSplitterManager::Get()
{
	if (m_pSplitterMngInstance == nullptr)
		m_pSplitterMngInstance = new CSplitterManager();
	
	return m_pSplitterMngInstance;
}

bool CSplitterManager::Init()
{
	if(!m_pSplitterImpl.get())
		m_pSplitterImpl = std::make_unique<CSplitterManagerImpl>(_gMainFrame);
		
	if(m_pSplitterImpl.get())
	{
		CreateTabManager();
		return true;
	}
		
	return false;
}

void CSplitterManager::CreateTabManager()
{
	if(!m_pTabLeft.get())
	{
		m_pTabLeft = std::make_unique<CTabManager>(m_pSplitterImpl.get(), theJsonConfig->GetTabStyle());
		m_pTabLeft->AddTabPage(theJsonConfig->GetLeftPath());
		
	//	m_pActiveTab = m_pTabLeft.get();
		SetDefaultActiveTab();
	}
}

void CSplitterManager::DoCreateSplitTab()
{
	bool bSplit = theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE ? true : false;
	if (bSplit && !m_pTabRight.get())
	{
		m_pTabRight = std::make_unique<CTabManager>(m_pSplitterImpl.get(), theJsonConfig->GetTabStyle());
		m_pTabRight->AddTabPage(theJsonConfig->GetRightPath());
	}
	else
	{
		if(!bSplit && m_pTabRight.get())
		{
			if (m_pSplitterImpl->IsSplit())
				m_pSplitterImpl->Unsplit();
		
			CTabManager* pRightTab = m_pTabRight.release();
			if(pRightTab)
				delete pRightTab;
		}
	}
}

void CSplitterManager::DoSplitter(const wxSize& _size)
{
	DoCreateSplitTab();
	
	wxSize szClient = _size;
	if (_size.GetWidth() == 0)
		szClient = _gMainFrame->GetClientSize();

	int iCenterWidth = szClient.GetWidth() / 2;
	int iCenterHeight = szClient.GetHeight() / 2;

	int iSplitStyle = theJsonConfig->GetSplitStyle();
	if (m_pSplitterImpl.get() != nullptr)
	{
		if (iSplitStyle == WINDOW_SPLIT_NONE)
		{
		//	if (m_pSplitterImpl->IsSplit())
		//		m_pSplitterImpl->Unsplit();
			
			m_pSplitterImpl->Initialize(m_pTabLeft.get());
		}
		else if (iSplitStyle == WINDOW_SPLIT_HORZ)
		{
			if (m_pSplitterImpl->IsSplit())
				m_pSplitterImpl->Unsplit();

			if (m_pTabRight->GetPageCount() == 0)
				m_pTabRight->AddTabPage(theJsonConfig->GetRightPath());

			m_pTabLeft->Show(true);
			m_pTabRight->Show(true);

			m_pSplitterImpl->SetMinimumPaneSize(iCenterHeight - 40);
			m_pSplitterImpl->SplitHorizontally(m_pTabLeft.get(), m_pTabRight.get());
		}
		else
		{
			if (m_pSplitterImpl->IsSplit())
				m_pSplitterImpl->Unsplit();

			if (m_pTabRight->GetPageCount() == 0)
				m_pTabRight->AddTabPage(theJsonConfig->GetRightPath());

			m_pTabLeft->Show(true);
			m_pTabRight->Show(true);

			m_pSplitterImpl->SetMinimumPaneSize(iCenterWidth);
			m_pSplitterImpl->SplitVertically(m_pTabLeft.get(), m_pTabRight.get());
		}
		
		SetDefaultActiveTab();
	}
}

CSplitterManagerImpl* CSplitterManager::GetSplitter() 
{
	if(m_pSplitterImpl.get())
		return m_pSplitterImpl.get();

	return nullptr;
}

void CSplitterManager::ChangeTabPagePathName(const wxString& strPath)
{
	wxString strName = theCommonUtil->GetPathName(strPath);
	int iSel = GetActiveTab()->GetSelection();
	
	GetActiveTab()->SetPageText(iSel, strName);
	GetActiveTab()->SetPageToolTip(iSel, strPath);
}

void CSplitterManager::SetDefaultActiveTab()
{
	m_pActiveTab = m_pTabLeft.get();
	SetActivatePage();
}

void CSplitterManager::ChangeActiveTab()
{
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		if(m_pActiveTab == m_pTabLeft.get())
		{
			m_pActiveTab = m_pTabRight.get();
			SetActivatePage();
		}
		else
			SetDefaultActiveTab();
	}
	else
		SetDefaultActiveTab();
}

void CSplitterManager::SetActivatePage()
{
	GetActiveTab()->GetActiveViewPanel()->SetActivateView();
}

void CSplitterManager::AddTabPage()
{
	CTabManager* pCurrentTab = GetActiveTab();
	
	wxString strNewPath(wxT(""));
	int iSel = pCurrentTab->GetSelection();
	if(iSel > -1) strNewPath = pCurrentTab->GetPageToolTip(iSel);
	
	if(strNewPath.IsEmpty())
		strNewPath = theJsonConfig->GetLeftPath();
			
	pCurrentTab->AddTabPage(strNewPath);
	
	int iPageCount = pCurrentTab->GetPageCount();
	pCurrentTab->SetSelection(iPageCount - 1);
	pCurrentTab->GetActiveViewPanel()->SetActivateView();
}

CTabManager* CSplitterManager::GetAnotherTab()
{
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
			return m_pActiveTab == m_pTabLeft.get() ? m_pTabRight.get() : m_pTabLeft.get();
		
	return nullptr;
}