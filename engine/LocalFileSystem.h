#ifndef __LOCAL_FILE_SYSTEM_INCLUDED_H__
#define __LOCAL_FILE_SYSTEM_INCLUDED_H__

#include <list>
class CLocalFileSystem final
{
public:
	explicit CLocalFileSystem();
	~CLocalFileSystem();

	enum local_filetype	
	{
		unknown = -1,
		file,
		dir,
		link,
		link_file = link,
		link_dir
	};
	
public:
	void EndFindFiles();
	bool BeginFindFiles(wxString path, bool dirs_only);
	bool GetNextFile(wxString& strName, bool* isDir = nullptr, unsigned long* latt = nullptr, wxLongLong* llSize = nullptr, wxDateTime* dt = nullptr);
	static bool IsCanReadDir(const wxString& strPath);
	static bool GetAttributeInfo(const wxString& strName, bool& isDir, unsigned long& lattr, wxLongLong* llSize, wxDateTime* dt);
	static bool RecursiveCopyOrMove(std::list<wxString>& dirsToVisit, const wxString& strDest, wxWindow* parent, bool bMove);
#ifdef __WXMSW__
	static bool RecursiveCopyOrMoveSameTarget(std::list<wxString>& dirsToVisit, const wxString& strDest, wxWindow* parent, bool bMove);
#endif	
	static bool IsCheckedFileOpen(const wxString& strFullPathName);
	static bool RecursiveDelete(const wxString& path, wxWindow* parent, bool bGoTrash, int& iRet);
	static bool RecursiveDelete(const std::list<wxString>& dirsToVisit, wxWindow* parent, bool bGoTrash, int& iRet);
	static enum local_filetype GetFileType(const wxString& path);
private:
	bool m_bDirs_only;
	
#ifdef __WXMSW__
	#ifdef _UNICODE
		WIN32_FIND_DATAW m_find_data;
	#else
		WIN32_FIND_DATA m_find_data;
	#endif
	
	HANDLE m_hFind;
	bool m_bfound;
	wxString m_strFind_path;
	
#else
	char* m_raw_path;
	char* m_file_part; // Points into m_raw_path past the trailing slash of the path part
	int m_buffer_length;
	DIR* m_dir;
#endif	

};

#endif