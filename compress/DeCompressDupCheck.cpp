#include "../ginc.h"
#include "DeCompressDupCheck.h"
#include "../dialog/DlgDuplicateCheck.h"

wxBEGIN_EVENT_TABLE(CDeCompressDupCheck, wxEvtHandler)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DECOMPRESS_DUP_CHECK, wxID_ANY, CDeCompressDupCheck::OnDuplicateCheck)
wxEND_EVENT_TABLE()


std::unique_ptr<CDeCompressDupCheck> CDeCompressDupCheck::m_pInstance = nullptr;
CDeCompressDupCheck* CDeCompressDupCheck::Get()
{
	if (m_pInstance.get() == nullptr)
		m_pInstance.reset(new CDeCompressDupCheck());
	
	return m_pInstance.get();
}

void CDeCompressDupCheck::SetDuplicateFile(const wxString& strFullPathName, const wxString& strFileName, unsigned long ulDosDate, unsigned long ulUncompressedSize)
{
	m_strFullPathName = strFullPathName;
	m_strFileName = strFileName;
	m_ulDosDate = ulDosDate;
	m_ulUncompressedSize = ulUncompressedSize;
}

void CDeCompressDupCheck::OnDuplicateCheck(wxCommandEvent& event)
{
	DlgDuplicateCheck dlg(_gMainFrame);
	dlg.SetDuplicateFile(m_strFullPathName, m_strFileName, m_ulDosDate, m_ulUncompressedSize);
	int iRet = dlg.ShowModal();
	dlg.Destroy();
	
	if(iRet == wxID_CANCEL)
		theCompress->DoCancel();
		
	theCompress->SetRelease();
}