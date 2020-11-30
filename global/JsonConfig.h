#ifndef __JSON_CONFIG_H__
#define __JSON_CONFIG_H__

#include "JsonBase.h"
class CJsonConfig : public CJsonBase
{
private:
	CJsonConfig();

public:
	~CJsonConfig();
	static CJsonConfig* Get();

public:
	virtual bool Load();
	bool IsExistMember(const wxString& strGroup, const wxString& strMember);

	const wxRect& GetDefaultRect()  { return m_rcDefault; }
	const int GetWidth()            const { return _jsonDoc["globalsettings"]["Width"].GetInt(); } // m_nWidth;}
	const int GetHeight()			const { return _jsonDoc["globalsettings"]["Height"].GetInt();} // m_nHeight;}
	const int GetXPos()				const { return _jsonDoc["globalsettings"]["XPos"].GetInt(); }  // m_nXPos;}
	const int GetYPos()				const { return _jsonDoc["globalsettings"]["YPos"].GetInt(); }  // m_nYPos;}
	const int GetSplitStyle()		const { return _jsonDoc["globalsettings"]["SplitWindowStyle"].GetInt(); } //m_nSplitWindowStyle;}

	const int GetViewToolbar()		const { return _jsonDoc["globalsettings"]["ViewToolBar"].GetInt(); } //m_nViewToolBar;}
	const int GetViewFavoritebar()	const { return _jsonDoc["globalsettings"]["ViewFavoriteBar"].GetInt(); }// m_nViewFavoriteBar;}
	const int GetSortType()			const { return _jsonDoc["globalsettings"]["SortType"].GetInt(); } //m_nSortType;}
	const int GetViewFileList()		const { return _jsonDoc["globalsettings"]["ViewFileList"].GetInt(); } //m_nViewFileList;}

	bool IsViewFloppy()				const { return m_nDispFloppy == 1 ? true : false; }
	bool IsViewSystemFile()			const { return (m_nDispSystemFile == 1 ? true : false); }
	bool IsViewHiddenFile()			const { return (m_nDispHiddenFile == 1 ? true : false); }
	bool IsViewAllFile()			const { return (m_nDispAllFile == 1 ? true : false); }
	bool IsViewHeader()				const { return (m_nDispHeader == 1 ? true : false); }
	bool IsDriveListView()			const { return m_nDispDriveListView == 1 ? true : false; }

	const int GetColumnCount()		const { return _jsonDoc["globalsettings"]["DispColumn"].GetInt(); } // m_nDispColumn;}
	const bool IsDispMsgBoxDirWatcher() { return (m_nDispMsgBoxDirWatcher == 1 ? true : false); }

	void SetFontName(const wxString& strFontName) { m_strFontName = strFontName; }
	const wxString GetFontName() { return _jsonDoc["globalsettings"]["fontname"].GetString(); }  //m_strFontName;}

	void SetLeftPath(const wxString& strPath) { m_strLeftPath = strPath; }
	const wxString& GetLeftPath()
	{
		if (!wxDirExists(m_strLeftPath + SLASH))
			m_strLeftPath = wxT("C:");

		return  m_strLeftPath;
	}

	void SetRightPath(const wxString& strPath) { m_strRightPath = strPath; }
	const wxString& GetRightPath()
	{
		if (!wxDirExists(m_strRightPath + SLASH))
			m_strLeftPath = wxT("C:");

		return  m_strRightPath;
	}

	void SetEditProgram(const wxString& strProgram) { m_strFileEditProgram = strProgram; }

	const int GetFontSize()			     const { return _jsonDoc["globalsettings"]["fontsize"].GetInt();                                      } // m_nFontSize;}
	const int GetMinCountInThread()	     const { return _jsonDoc["globalsettings"]["MinThreadRead"].GetInt();                                 } // m_nMinThreadRead; }
	const int IsFileInfoView()		     const { return _jsonDoc["globalsettings"]["FileInfoView"].GetInt();                                  } // m_nFileInfoView; }
	const int GetTimeFMT()		         const { return _jsonDoc["globalsettings"]["TimeFormat"].GetInt();                                    } // m_nTimeFormat;}
	const int GetPreIconReadCount()	     const { return _jsonDoc["globalsettings"]["preIconRead"].GetInt();                                   } // m_nPreIconReadCount;}
	const int GetPreRead()                const { return _jsonDoc["globalsettings"]["preRead"].GetInt();                                       } // m_iPreRead;}
	const int GetCopyMoveSamePath()       const { return _jsonDoc["globalsettings"]["CopyMoveSamePath"].GetInt();                              } // m_iCopyMoveSamePath; }
	const int GetCopyMoveUseWindowShell() const { return _jsonDoc["globalsettings"]["CopyMoveUseWindowShell"].GetInt();                        } // m_iCopyMoveUseWindowShell; }
	const int IsActiveWndToNonActiveWndCopyMove() const { return _jsonDoc["globalsettings"]["CopyMoveActvieWindowToNonActiveWindow"].GetInt(); } // m_iCopyMoveActiveWindowToNonActiveWindow; }
	const int IsDirectoryWatcherAsync() const { return m_iDirectoryWatcherAsync; }
	const int GetUseSHBrowseForFolder() const { return m_iUseSHBrowseForFolder; }
	const wxColour& GetDefaultColor()      { return m_defaultColor; }
	const wxColour& GetDirColor()          { return m_dirColor; }
	const wxColour& GetDriveColor()        { return m_drvColor; }
	const wxColour& GetReadOnlyColor()     { return m_readOnlyColor; }
	const wxColour& GetSelectItemColor()   { return m_selectItemColor; }
	const wxColour& GetBackgroundColor()   { return m_backgroundColor; }
	const wxColour& GetAttributeColor()    { return m_attributeColor; }
	const wxColour& GetDescriptionColor()  { return m_descColor; }
	const wxColour& GetColumnColor()       { return m_columnColor; }
	const wxColour& GetInfoViewBackColor() { return m_infoviewColor; }
	const wxColour& GetInfoViewTextColor() { return m_infoviewtxtColor; }
	const wxColour& GetMatchColor()        { return m_matchColor; }

	const long GetTabStyle()  const { return m_tabStyle; }
	const int GetMonitorPos() const { return _jsonDoc["globalsettings"]["DispMonitor"].GetInt(); } // m_nCurrentMonitor; }

	const bool IsShift_IME_KOR_MoveDrive() { return _jsonDoc["globalsettings"]["Shift_IME_KOR_MoveDrv"].GetInt() ? true : false; }

	const int IsMaximized() const { return _jsonDoc["globalsettings"]["Maximized"].GetInt(); }     // m_nMaximized; }
	const bool IsSaveLastSession() { return _jsonDoc["globalsettings"]["LastSessionSave"].GetInt() ? true : false; }

	void SetViewFileKind(enum FILE_VIEW_OPTION fvo);//int iViewFileKind);
	void SetColumnCount(const int iColumnCnt);
	void SetSplitStyle(const int iViewStyle);
	const wxColour GetExtColor(const wxString& strExt);

	void SaveConfig();

	void SetChangeGlobalVal(const wxString& strMember, int iVal);
	void SetChangeGlobalVal(const wxString& strMember, const wxString& strVal);
	void SetChangeColorVal(const wxString& strKey, const wxString& strMember, const wxString& strVal);
	bool IsCenterOnScreen() { return _jsonDoc["globalsettings"]["DisplayCenterOnScreen"].GetInt() ? true : false; }
	bool IsSelectedSCDItemToParent() {	return _jsonDoc["globalsettings"]["SelectSCDItem_toparent"].GetInt() ? true : false; }

	void SetExternalEditProgram(wxVector<EXTERNAL_EDIT_PROGRAM>& vecExternal);
//	bool GetExternalEditProgram(wxVector<EXTERNAL_EDIT_PROGRAM>& vecExternal);
	int GetExternalPGCount() {
		return _gExternalPGList.size();//m_mapExternalProgram.size();
	}

	void SetDefaultColor();

private:
	void SetGlobalConfig();
	unsigned long ConvertStringToColor(wxString& strColor);

private:
	static std::unique_ptr<CJsonConfig> m_pInstance;
//	Document _jsonDoc;

//	wxString m_strJsonPath;
	//globalsettings
private:
	int m_nWidth;
	int m_nHeight;
	int m_nXPos;
	int m_nYPos;
	int m_nSplitWindowStyle;
	int m_nViewToolBar;
	int m_nViewFavoriteBar;
	int m_nSortType;
	int m_nViewFileList;
	int m_nDispColumn;
	int m_nDispMsgBoxDirWatcher;
	int m_nDispFloppy;
	int m_nDispSystemFile;
	int m_nDispHiddenFile;
	int m_nDispAllFile;
	int m_nDispHeader;
	int m_nDispDriveListView;
	int m_nMinThreadRead;
	int m_nFileInfoView;
	int m_nTimeFormat;
	int m_nFontSize;
	int m_nPreIconReadCount;
	int m_nCurrentMonitor;
	int m_nMaximized;
	int m_nLastSessionSave;
	int m_iPreRead;
	int m_iCopyMoveSamePath;
	int m_iCopyMoveUseWindowShell;
	int m_iCopyMoveActiveWindowToNonActiveWindow;
	int m_iShift_IME_KOR_MoveDrv;
	int m_iDirectoryWatcherAsync;
	int m_iUseSHBrowseForFolder;
	//2019.04.17 ADD
	int m_iItemDisplayFromBack;
	wxString m_strFontName;
	wxString m_strFileEditProgram;
	wxString m_strLeftPath;
	wxString m_strRightPath;

	long m_tabStyle;
	wxRect m_rcDefault;
	//defaultColor
private:
	wxColour m_defaultColor;
	wxColour m_dirColor;
	wxColour m_drvColor;
	wxColour m_readOnlyColor;
	wxColour m_selectItemColor;
	wxColour m_backgroundColor;
	wxColour m_attributeColor;
	wxColour m_descColor;
	wxColour m_columnColor;
	wxColour m_infoviewColor;
	wxColour m_infoviewtxtColor;
	wxColour m_matchColor;
/*
#if defined(_MSC_VER) && (_MSC_VER <= 1800) //VS2013
	stdext::hash_map<int, EXTERNAL_EDIT_PROGRAM> m_mapExternalProgram;
#else
	std::unordered_map<int, EXTERNAL_EDIT_PROGRAM> m_mapExternalProgram;
#endif
 */
};
#endif
