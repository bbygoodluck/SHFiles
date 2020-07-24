#include "../../ginc.h"
#include "MSWCopyMoveClipboard.h"

std::unique_ptr<CMSWCopyMoveClipboard> CMSWCopyMoveClipboard::m_pInstance(nullptr);

/*
CMSWCopyMoveClipboard::CMSWCopyMoveClipboard()
{
}

CMSWCopyMoveClipboard::~CMSWCopyMoveClipboard()
{
	Clear();
}
*/

CMSWCopyMoveClipboard* CMSWCopyMoveClipboard::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CMSWCopyMoveClipboard());

	return m_pInstance.get();
}

void CMSWCopyMoveClipboard::Clear()
{
	m_aDatas.clear();
}

void CMSWCopyMoveClipboard::AddData(const wxString& strData)
{
	m_aDatas.push_back(strData);
}

void CMSWCopyMoveClipboard::AddData(const std::list<wxString>& lstDatas)
{
	m_aDatas = lstDatas;
}

void CMSWCopyMoveClipboard::CopyToClipboard()
{
	wxTheClipboard->UsePrimarySelection();
	if (wxTheClipboard->Open())
	{
		wxFileDataObject* pFileDataObject = new wxFileDataObject();
		for (auto item : m_aDatas)
			pFileDataObject->AddFile(item);
		
		wxTheClipboard->AddData(pFileDataObject);
		wxTheClipboard->Close();
	}
}

void CMSWCopyMoveClipboard::GetData(std::list<wxString>& lstDatas)
{
	wxTheClipboard->UsePrimarySelection();
	wxDataFormat fmt(wxDF_FILENAME);

	if (wxTheClipboard->IsSupported(fmt))
	{
		if (wxTheClipboard->Open())
		{
			wxArrayString arrDatas;
			wxFileDataObject fileDataObject;
			if (wxTheClipboard->GetData(fileDataObject))
				arrDatas = fileDataObject.GetFilenames();

			wxTheClipboard->Close();

			for (auto item : arrDatas)
				lstDatas.push_back(item);
		}
	}
}