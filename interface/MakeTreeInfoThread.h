#ifndef __MAKE_TREE_INFO_THREAD_H__
#define __MAKE_TREE_INFO_THREAD_H__

#include <wx/textfile.h>
#include <filesystem>

using namespace std::filesystem;

class DlgMakeSHCDInfo;
class CMakeTreeInfoThread : public wxThread
{
public:
	CMakeTreeInfoThread(DlgMakeSHCDInfo* pDlg);
	~CMakeTreeInfoThread();

	virtual wxThread::ExitCode Entry() wxOVERRIDE;

	void SetLoadFromFile(bool bLoadFromFile) {
		m_bLoadFromFile = bLoadFromFile;
	}

	void SetSHCDFile(const wxString& strFile, const wxString& strPath, size_t lineCount = 0);
	void LoadSubDir(const wxString& strPath, NODE* pNode);
	void SetCurrentLevel(NODE* pNode) {
		m_pCurrNode = pNode;
	}
private:
	void SendThreadTerminateMessage();
	bool IsCancelOrClose();
	int GetNumberofDirectory(path p);

private:
	DlgMakeSHCDInfo* m_pDlg;
//	DirectoryManagerDlg* m_pDlg;
	wxTextFile m_file;
	wxString m_strSHCDFile;
	wxString m_strCurrentPath;
	size_t m_lineCnt;

	bool m_bLoadFromFile;
	int m_iLevel;
//	int m_iCount;
	NODE* m_pCurrNode;
};
#endif
