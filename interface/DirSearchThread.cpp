#include "../ginc.h"
#include "../dialog/DlgSearchDir.h"
#include "DirSearchThread.h"
#include "../engine/LocalFileSystem.h"

CDirSearchThread::CDirSearchThread(DlgSearchDir* pDlg)
	: wxThread(wxTHREAD_JOINABLE)
	, m_pDlg(pDlg)
{
}

CDirSearchThread::~CDirSearchThread()
{

}

void CDirSearchThread::SetSearchCurrentDirectory(const wxString& strDir, const wxString& strFile)
{
	m_strDir = strDir;
	m_strFile = strFile;
	m_strDrive = theCommonUtil->GetVolume(strDir);
	m_strDrive += wxT(":");
	m_strDrive += SLASH;
	m_pDlg->SetCurrentDrive(m_strDrive);
}

wxThread::ExitCode CDirSearchThread::Entry()
{
	wxMilliSleep(50);
	m_depth = 0;
	m_dirCount = 0;
	wxString strSHCDFilePath = m_strDrive + m_strDir.Left(1) + wxT(".SCD");
	
	if (!wxFileName::FileExists(strSHCDFilePath))
	{
		if (!m_file.Create(m_strFile))
		{
			wxMessageBox(wxT("Failed to create file!"), wxT("Search Directory"), wxOK | wxICON_ERROR);
			return (wxThread::ExitCode)0;
		}
	}

	m_file.Open(m_strFile, wxFile::OpenMode::read_write);

	m_strSave = wxString::Format(wxT("%d^"), m_depth);
	m_strSave += m_strDrive.Left(m_strDrive.Length() - 1) + SLASH;
	m_strSave += wxT("\n");

	m_file.Write(m_strSave);

	DoSearchDir(m_strDrive, 0);

	m_file.Close();
	wxThreadEvent event(wxEVT_THREAD, SCD_DIRECTORY_SEARCH_THREAD_EVENT);
	event.SetInt(-1);
	wxQueueEvent(m_pDlg, event.Clone());

	return (wxThread::ExitCode)0;
}

void CDirSearchThread::DoSearchDir(const wxString& strPath, int depth)
{
	m_pDlg->SetCurrentDir(strPath);

#if defined(__WXMSW__)
	CLocalFileSystem localSys;
	bool isDir = false;
	unsigned long lattr = 0;

	if (!localSys.BeginFindFiles(strPath, true))
		return;

	int iLen = strPath.Len();
	wxString strNewPath = strPath.at(iLen - 1) == SLASH ? strPath.Left(iLen - 1) : strPath;
	wxString strName(wxT(""));
	++depth;

	while (localSys.GetNextFile(strName))//, isDir, lattr))
	{
		if (m_pDlg->IsCancel())
			break;

		if (std::find(_gVecIgnore.begin(), _gVecIgnore.end(), strName.ToStdString()) != _gVecIgnore.end())
			continue;

		m_strSave = wxString::Format(wxT("%d^"), depth) + strName + wxT("\n");
		m_file.Write(m_strSave);

		wxString strDirCount = wxString::Format(wxT("%d"), m_dirCount++);
		m_pDlg->SetReadDirectory(m_dirCount++, strName);// strFullPath);

		wxString strFullPath(strNewPath);
		strFullPath += SLASH + strName;

		DoSearchDir(strFullPath, depth);
	}
}
#else
#endif