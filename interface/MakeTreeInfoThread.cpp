#include "../ginc.h"
#include "../dialog/DlgMakeSHCDInfo.h"
//#include "../dialog/DirectoryManagerDlg.h"
#include "MakeTreeInfoThread.h"
#include "../engine/LocalFileSystem.h"
CMakeTreeInfoThread::CMakeTreeInfoThread(DlgMakeSHCDInfo* pDlg)
	: wxThread(wxTHREAD_JOINABLE)
	, m_pDlg(pDlg)
{
}

CMakeTreeInfoThread::~CMakeTreeInfoThread()
{

}

void CMakeTreeInfoThread::SetSHCDFile(const wxString& strFile, const wxString& strPath, size_t lineCount)
{
	m_strSHCDFile = strFile;
	m_lineCnt = lineCount;
	m_strCurrentPath = strPath;
	
	int iFind = m_strCurrentPath.find(SLASH);
	if (iFind == -1)
		m_strCurrentPath += SLASH;
}

int CMakeTreeInfoThread::GetNumberofDirectory(path p)
{
	int n = 0;
	for (recursive_directory_iterator it(p); it != recursive_directory_iterator{}; ++it)
	{
		if (is_directory(*it))
			n++;
	}

	return n;
}

wxThread::ExitCode CMakeTreeInfoThread::Entry()
{
	if (m_bLoadFromFile)
	{
		if (m_strSHCDFile.IsEmpty())
		{
			wxString strMsg(m_strSHCDFile);
			strMsg += wxT(" is not file exist");

			wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);

			SendThreadTerminateMessage();
			return (wxThread::ExitCode)0;
		}

		if (!m_file.Open(m_strSHCDFile))
		{
			wxMessageBox(wxT("Fai to read the directory infomation file "), wxT("Read directory information"), wxOK | wxICON_ERROR);
			SendThreadTerminateMessage();
			return (wxThread::ExitCode)0;
		}

		int iCount = 0;
		wxString strPath(wxT(""));

		strPath = m_file.GetFirstLine();

		wxVector<wxString> vecSplit = theCommonUtil->Split(strPath, wxT("^"));
		if (vecSplit.size() < 2)
		{
			wxString strMsg(strPath);
			strMsg += wxT("The file is in the wrong format.\n Delete that file and create directory information again!");

			wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		}
		else
		{
			theSHCD->Initialize();
			theSHCD->SetMCDFile(m_strSHCDFile);

			wxString strLevel = vecSplit.at(0);
			wxString strName = vecSplit.at(1);

			unsigned long _lLevel = 0;

			strLevel.ToULong(&_lLevel);
			NODE* pNode = theSHCD->AddNode(strName, wx_static_cast(int, _lLevel), nullptr);

			while (!m_file.Eof())
			{
				strPath = m_file.GetNextLine();
				if (strPath.IsEmpty())
					break;

				iCount++;
			
				vecSplit.clear();
				vecSplit = theCommonUtil->Split(strPath, wxT("^"));

				strLevel = vecSplit.at(0);
				strName = vecSplit.at(1);

				strLevel.ToULong(&_lLevel);
				pNode = theSHCD->AddNode(strName, wx_static_cast(int, _lLevel), pNode);
			}

			m_file.Close();
			theSHCD->SetOrder();
			theSHCD->SetCur(m_strCurrentPath);
		}
	}
	else
	{
		path p(CONVSTR_TO_STD(m_strCurrentPath));
		theSHCD->Clear(m_pCurrNode);
		LoadSubDir(m_strCurrentPath, m_pCurrNode);

		theSHCD->SetOrder();
		theSHCD->SaveMCDFile();
		theSHCD->SetCur(m_strCurrentPath);
	}

	SendThreadTerminateMessage();
	return (wxThread::ExitCode)0;
}

void CMakeTreeInfoThread::SendThreadTerminateMessage()
{
	wxThreadEvent event(wxEVT_THREAD, SCD_MAKE_TREE_INFO_THREAD_EVENT);
	event.SetInt(-1);
	wxQueueEvent(m_pDlg, event.Clone());
}

void CMakeTreeInfoThread::LoadSubDir(const wxString& strPath, NODE* pNode)
{
#ifdef __WXMSW__
	wxString sPath(strPath);
	wxString strFullPath(strPath);

	CLocalFileSystem localSys;
	bool isDir = false;
	unsigned long lattr = 0;

	if (!localSys.BeginFindFiles(sPath, true))
		return;

	int iLevel = pNode->_level + 1;
	NODE* _pNode = nullptr;

	wxString strName(wxT(""));
	while (localSys.GetNextFile(strName))//, isDir, lattr))
	{
		if (IsCancelOrClose())
			break;

		if (std::find(_gVecIgnore.begin(), _gVecIgnore.end(), CONVSTR_TO_STD(strName)) != _gVecIgnore.end())
			continue;

		if (!theSHCD->FindChildNodeExist(strName, iLevel, pNode))
			_pNode = theSHCD->AddNode(strName, iLevel, pNode);

		wxString strNewFullPath(strFullPath);
		strNewFullPath += SLASH;
		strNewFullPath += strName;

		LoadSubDir(strNewFullPath, _pNode);
	}

	if (GetLastError() != ERROR_NO_MORE_FILES)
		wxMessageBox(wxT("Something went wrong during searcing..."), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);

#else
#endif
}

bool CMakeTreeInfoThread::IsCancelOrClose()
{
	return (m_pDlg->IsCancel() || m_pDlg->IsClose());
}