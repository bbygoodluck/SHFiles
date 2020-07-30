#include "ginc.h"
#include "MainFrame.h"
#include "MenuOperationHandler.h"
#include "TabManager.h"
#include "ViewPanel.h"
#include "./view/ListView.h"

#include <wx/aboutdlg.h>
#include <wx/utils.h> 

#include "./dialog/wxMozillaLikeAboutBoxDialog.h"
#include "./dialog/header_png.h"
#include "./dialog/GenericDirDialog.h"
#include "./dialog/DlgAddDir.h"
#include "./dialog/DlgFavoriteManager.h"
#include "./dialog/DlgFind.h"
#include "./dialog/DirectoryManagerDlg.h"
#include "./dialog/DlgEnv.h"

CMenuOperationHandler* CMenuOperationHandler::m_pMenuOPHandlerInstance(nullptr);
CMenuOperationHandler* CMenuOperationHandler::Get()
{
	if (m_pMenuOPHandlerInstance == nullptr)
		m_pMenuOPHandlerInstance = new CMenuOperationHandler();
	
	return m_pMenuOPHandlerInstance;
}

void CMenuOperationHandler::ExecuteMenuOperation(_MENU_EVENT_TYPE _menuType, const wxString& strInfo)
{
	switch(_menuType)
	{	
		case _MENU_FILE_ADD_DIR:
			DoMakeDirectory();
			break;
		//복사/이동
		case _MENU_FILE_COPY:
		case _MENU_FILE_MOVE:
		case _MENU_FILE_COPY_CLIPBOARD:
		case _MENU_FILE_MOVE_CLIPBOARD:
		case _MENU_FILE_PASTE_CLIPBOARD:
			DoCopyMove(_menuType);
			break;
			
		//이름변경
		case _MENU_FILE_RENAME:
			DoRename();
			break;
		//편집
		case _MENU_FILE_EDIT:
			DoFileEdit();
			break;
		//프로그램 종료
		case _MENU_FILE_PROGRAM_TERMINATE:
			DoProgramTerminate();
			break;
		//휴지통또는 완전삭제
		case _MENU_FILE_DEL_TRASH:
		case _MENU_FILE_DEL_COMP:
			DoDeleteTrash(_menuType);
			break;
			
		case _MENU_EDIT_ALL_RELEASE:
		case _MENU_EDIT_ALL_SELECT:
			DoAllSelectOrRelease(_menuType);
			break;
		case _MENU_EDIT_FIND:
			DoFileFind();
			break;
		//컨텍스트 메뉴
		case _MENU_EDIT_CONTEXT_MENU:
			DoDispContextMenu();
			break;
		//새로고침
		case _MENU_PATH_REFRESH:
			DoRefreshDir();
			break;
		//폴더직접이동
		case _MENU_PATH_GOTO_DIR_DIRECTLY:
			DoGotoDirDirectly();
			break;
		case _MENU_PATH_DIR_MANAGER:
			DoDirectoryManager();
			break;
		//전체화면	
		case _MENU_VIEW_FULLSCREEN:
			DoFullScreen();
			break;
		//컬럼변경	
		case _MENU_VIEW_COLUMN_CHANGE:
			DoChageViewColumn();
			break;
			
		case _MENU_TOOL_ENVIRONMENT:
			DoSettings();
			break;
			
		case _MENU_VIEW_FILELIST_CHANGE:
			DoChangeFileList();
			break;
		//환경설정 저장
		case _MENU_TOOL_SAVECONFIG:
			DoSaveConfig();
			break;
			
		case _MENU_FAVORITE_ITEM_ADD:
		case _MENU_FAVORITE_MANAGER:
			DoFavorite(_menuType);
			break;
			
		case _MENU_FAVORITE_ITEM_SELECTED:
			DoFavoriteItemSelected();
			break;
		//이 프로그램은...
		case _MENU_HELP_THIS_PROGRAM:
			DoThisProgramIs();
			break;
#ifdef __WXMSW__
		case _MENU_ETC_ADD_DRIVE:
			DoAddDriveAndRemove();
			break;
			
		case _MENU_DISK_SPACE_UPDATE:
			DoDiskspaceUpdate(strInfo);
			break;
#endif			
		default:
			break;
		
	}
}

#ifdef __WXMSW__
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
		{
			//캡션에서 Help 삭제
			DWORD dwStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
			::SetWindowLong(hWnd, GWL_EXSTYLE, dwStyle & ~WS_EX_CONTEXTHELP);
			
			HWND hStatic = GetDlgItem(hWnd, 0x00003742);//14146); // 0x00003742(Static Control)
			if(hStatic)
			{
				::EnableWindow(hStatic, FALSE);
				::ShowWindow(hStatic, SW_HIDE);
			}
			
			//지정된 폴더가 선택되도록 한다.
			::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, dwData);

			//대화상자가 중앙에 오도록 한다.
			RECT rc;
			GetClientRect(hWnd, &rc);
			::SetWindowPos(hWnd
				    	 , NULL
					     , (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2
					     , (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2
					     , 0
					     , 0
					     , SWP_NOZORDER | SWP_NOSIZE);

			//Status text에 3D 테두리 첨가
			HWND hwndLabel = ::GetDlgItem(hWnd, 0x3743);
			if (IsWindow(hwndLabel))
			{
				TCHAR szClass[MAX_PATH] = {0};
				::GetClassName(hwndLabel, szClass, MAX_PATH);
				if (lstrcmpi(szClass, __TEXT("static")))
					break;
			}
			else
				break;
				
			dwStyle = ::GetWindowLong(hwndLabel, GWL_EXSTYLE);
			::SetWindowLong(hwndLabel, GWL_EXSTYLE, dwStyle | WS_EX_STATICEDGE);
			::SetWindowPos(hwndLabel, NULL, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);
		}
			break;
	
		case BFFM_SELCHANGED:
		{
			TCHAR szText[MAX_PATH] = {0};
			::SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), szText);
			::SendMessage(hWnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(szText));
		}
			break;
			
		case BFFM_VALIDATEFAILED:
			break;
	}
	
	return 0;
}
#endif

void CMenuOperationHandler::DoCopyMove(_MENU_EVENT_TYPE _menuType)
{
	std::list<wxString> lstSrcList;
	wxString strTargetPath(wxT(""));
		
	if(_menuType != _MENU_FILE_PASTE_CLIPBOARD)
	{
		bool bMove = ((_menuType == _MENU_FILE_MOVE) || (_menuType == _MENU_FILE_MOVE_CLIPBOARD)) ? true : false;
		bool bUseClipboard = ((_menuType == _MENU_FILE_COPY_CLIPBOARD) || (_menuType == _MENU_FILE_MOVE_CLIPBOARD)) ? true : false;
		
		if(!bUseClipboard)
		{	
			strTargetPath = GetTargetDirectory();
			if(strTargetPath.IsEmpty())
				return;
			
		}
			
		if(!theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetSelectedItem(bUseClipboard, bMove, lstSrcList))
			return;
		
		if(!bUseClipboard)
			DoCopyMoveOperation(lstSrcList, strTargetPath, bMove);
		else
			m_strCopyMoveSrcDir = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();
	}	
	else
	{
		wxString strTargetPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
		theClipboard->GetData(lstSrcList);
		
		_MENU_EVENT_TYPE menuType = theClipboard->GetFileOperation();
		bool bMove = menuType == _MENU_FILE_MOVE_CLIPBOARD ? true : false;
		
		DoCopyMoveOperation(lstSrcList, strTargetPath, bMove);
	}
}

void CMenuOperationHandler::DoCopyMoveOperation(std::list<wxString>& lstDatas, const wxString& strPath, bool bMove)
{	//윈도우의 경우 IFileOperation 인터페이스를 사용해서 파일 복사를 진행하게 되면
    //타겟 폴더가 소스 폴더와 같은경우는 IFileOperation 의 CopyItems(or MoveItems)함수는 작동하지 않음)
	//그래서 IFileOperation의 CopyItem(or MoveItem)함수를 사용해서 처리해야함..
	bool bResult = true;
	bResult = m_strCopyMoveSrcDir.Cmp(strPath) == 0 ? CLocalFileSystem::RecursiveCopyOrMoveSameTarget(lstDatas, strPath, _gMainFrame, bMove)
													: CLocalFileSystem::RecursiveCopyOrMove(lstDatas, strPath, _gMainFrame, bMove);
	
	if(!bResult)
	{
		wxMessageBox(wxT("The file copy or move is failed"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}
		
	m_strCopyMoveSrcDir = wxT("");
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ClearSelectedItems();
}

wxString CMenuOperationHandler::GetTargetDirectory()
{
	wxString strTargetPath(wxT(""));
	
	if(theJsonConfig->GetSplitStyle() == WINDOW_SPLIT_NONE)
	{
		wxString strPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();
		bool bWindowSelectDir = false;
	#ifdef __WXMSW__
		if(theJsonConfig->GetUseSHBrowseForFolder())
		{
			bWindowSelectDir = true;
			LPITEMIDLIST pidlBrowse;
			TCHAR szDir[MAX_PATH] = {0, };
			
			BROWSEINFO brInfo;
			wxZeroMemory(brInfo);
			
			brInfo.hwndOwner = _gMainFrame->GetHWND();
			brInfo.pidlRoot = NULL;
			brInfo.pszDisplayName = szDir;
			brInfo.lpszTitle = _T("Select copy or move target directory");
			brInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;
			brInfo.lpfn = BrowseCallbackProc;
			brInfo.lParam = (LPARAM)strPath.wc_str();
			
			pidlBrowse = ::SHBrowseForFolder(&brInfo);
			if(pidlBrowse != NULL)
			{
				::SHGetPathFromIDList(pidlBrowse, szDir);
				strTargetPath = wxString::Format(wxT("%s"), szDir);
			}
		}
	#endif
		
		if(!bWindowSelectDir)
		{
			wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strPath);
			dlgTargetPath.SetSize(wxSize(800, 600));
			dlgTargetPath.CenterOnScreen();
			if(dlgTargetPath.ShowModal() == wxID_OK)
				strTargetPath = dlgTargetPath.GetPath();
		}
	}
	else
		strTargetPath = theSplitterManager->GetAnotherTab()->GetActiveViewPanel()->GetCurrentViewPath();
	
	return strTargetPath;
}

void CMenuOperationHandler::DoRename()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_DIR_FILE_RENAME);
}

void CMenuOperationHandler::DoMakeDirectory()
{
	wxString strCurrentPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
	wxString strNewPath(wxT(""));
	
	DlgAddDir dlg(_gMainFrame);
	dlg.SetCurrentPath(strCurrentPath);
	if(dlg.ShowModal() == wxID_OK)
	{
		strNewPath = dlg.GetNewPath();
		wxMkdir(strNewPath);
	}
	
	dlg.Destroy();
}

void CMenuOperationHandler::DoFileEdit()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_EXEC_FILE_EDIT);
}

void CMenuOperationHandler::DoDeleteTrash(_MENU_EVENT_TYPE _menuType)
{
	std::list<wxString> lstDatas;
	bool bTrash = (_menuType == _MENU_FILE_DEL_TRASH) ? true : false;
	
	if(!theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetDeleteItems(lstDatas, bTrash))
		return;
		
	int iRet = 0;
	if(!CLocalFileSystem::RecursiveDelete(lstDatas, _gMainFrame, bTrash, iRet))
	{	
		wxMessageBox(wxT("Failed to file delete"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}
	
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ClearSelectedItems();
}

void CMenuOperationHandler::DoProgramTerminate()
{
	wxTheApp->GetTopWindow()->Close();
}

//전체선택/해제
void CMenuOperationHandler::DoAllSelectOrRelease(_MENU_EVENT_TYPE _menuType)
{
	wxEventType evtType = _menuType == _MENU_EDIT_ALL_RELEASE ? wxEVT_ITEM_ALL_RELEASE : wxEVT_ITEM_ALL_SELECT;
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->DoMyEventExecuteToView(evtType);
}

//파일찾기
void CMenuOperationHandler::DoFileFind()
{
	wxString strCurrentPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();
	DlgFind findDlg(_gMainFrame);
	findDlg.setSearchDir(strCurrentPath);
	findDlg.ShowModal();
	findDlg.Destroy();
			
}
	
void CMenuOperationHandler::DoDispContextMenu()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_EXEC_CONTEXT_MENU);
}

void CMenuOperationHandler::DoRefreshDir()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView();
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
		theSplitterManager->GetAnotherTab()->GetActiveViewPanel()->ReloadPathOfView();
}

void CMenuOperationHandler::DoGotoDirDirectly()
{
	wxString strCurrPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();
	wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strCurrPath);
	dlgTargetPath.SetSize(wxSize(600, 450));
	dlgTargetPath.CenterOnScreen();
	
	if(dlgTargetPath.ShowModal() == wxID_OK)
	{
		wxString strTargetDir(dlgTargetPath.GetPath());
		theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(strTargetDir);
	}
}

void CMenuOperationHandler::DoDirectoryManager()
{
	wxString strCurrPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();
	wxString strDriveName = theCommonUtil->GetVolume(strCurrPath);

	wxString strTreeDataPath = theCommonUtil->GetWorkDir() + SLASH + wxT("treedata");
	if (!wxDirExists(strTreeDataPath))
		wxMkDir(strTreeDataPath);

	wxString strSHCDFilePath = strTreeDataPath + SLASH + strCurrPath.Left(1) + wxT(".SCD");
	DirectoryManagerDlg dirMng(_gMainFrame);
	dirMng.SetSCDFileInfo(strSHCDFilePath, strCurrPath);
	dirMng.ShowModal();
	dirMng.Destroy();
}

//전체화면
void CMenuOperationHandler::DoFullScreen()
{
	_gMainFrame->Maximize();
}

//컬럼 변경
void CMenuOperationHandler::DoChageViewColumn()
{
	CTabManager* pCurrentTabManager = theSplitterManager->GetActiveTab();
	CTabManager* pAnotherTabManager = nullptr;
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
		pAnotherTabManager = theSplitterManager->GetAnotherTab();
	
	SetViewColumnInTabs(pCurrentTabManager);
	SetViewColumnInTabs(pAnotherTabManager);
}

void CMenuOperationHandler::DoChangeFileList()
{
	DoRefreshDir();
}

void CMenuOperationHandler::SetViewColumnInTabs(CTabManager* pTabManager)
{
	if(pTabManager)
	{
		int iPageCountInTab = pTabManager->GetPageCount();
		int iPageIndex = 0;
		
		while(iPageIndex < iPageCountInTab)
		{
			((CViewPanel *)pTabManager->GetPage(iPageIndex))->SetActivateColumnChanged();
			iPageIndex++;
		}
	}
}

void CMenuOperationHandler::DoSettings()
{
	DlgEnv dlg(_gMainFrame);
	dlg.ShowModal();

	dlg.Destroy();
}

//환경설정저장
void CMenuOperationHandler::DoSaveConfig()
{
	int xPos = 0;
	int yPos = 0;

	int nCurrDisp = wxDisplay::GetFromWindow(_gMainFrame);

	wxDisplay disp(nCurrDisp);
	wxRect rcMonitor = disp.GetGeometry();

	wxTopLevelWindow* pTopWindow = (wxTopLevelWindow *)wxTheApp->GetTopWindow();
	pTopWindow->GetScreenPosition(&xPos, &yPos);
	wxRect rcScreen = pTopWindow->GetScreenRect();

	int iMaximized = _gMainFrame->IsMaximized() ? 1 : 0;

	if (rcMonitor.GetLeft() != 0)
		xPos = rcMonitor.GetLeft();

	int iWidth = rcScreen.GetWidth();
	int iHeight = rcScreen.GetHeight();

	theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), iMaximized);
	theJsonConfig->SetChangeGlobalVal(wxT("DispMonitor"), nCurrDisp);
	theJsonConfig->SetChangeGlobalVal(wxT("XPos"), xPos);
	theJsonConfig->SetChangeGlobalVal(wxT("YPos"), yPos);
	theJsonConfig->SetChangeGlobalVal(wxT("Width"), iWidth);
	theJsonConfig->SetChangeGlobalVal(wxT("Height"), iHeight);

	if (theJsonConfig->IsSaveLastSession())
	{
		CTabManager* pCurrentTabManager = theSplitterManager->GetActiveTab();
		CTabManager* pAnotherTabManager = nullptr;
		
		if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
			pAnotherTabManager = theSplitterManager->GetAnotherTab();
		
		wxString strActiveKey(wxT(""));
		wxString strAnotherKey(wxT(""));
		
		bool bLeftTab = pCurrentTabManager == theSplitterManager->GetLeftTab() ? true : false;
		if(bLeftTab)
		{
			strActiveKey = wxT("LeftPath");
			strAnotherKey = wxT("RightPath");
		}
		else
		{
			strActiveKey = wxT("RightPath");
			strAnotherKey = wxT("LeftPath");
		}
		
		wxString strCurrentPath = pCurrentTabManager->GetActiveViewPanel()->GetCurrentViewPath();
		theJsonConfig->SetChangeGlobalVal(strActiveKey, strCurrentPath);
		
		if(pAnotherTabManager)
		{
			strCurrentPath = pAnotherTabManager->GetActiveViewPanel()->GetCurrentViewPath();
			theJsonConfig->SetChangeGlobalVal(strAnotherKey, strCurrentPath);
		}
	}

	theJsonConfig->SaveConfig();
	wxMessageBox(wxT("The Current settings have been saved."), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
}

//즐겨찾기
void CMenuOperationHandler::DoFavorite(_MENU_EVENT_TYPE _menuType)
{
	DlgFavoriteManager dlg(_gMainFrame);
	if (_menuType == _MENU_FAVORITE_ITEM_ADD)
	{
		wxString strCurrPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
		dlg.SetAddPath(strCurrPath, true);
	}
	
	dlg.ShowModal();
	dlg.Destroy();
}

void CMenuOperationHandler::SetFavoriteItem(const wxString& strItem)
{
	m_strFavoriteItem = strItem;
}

void CMenuOperationHandler::DoFavoriteItemSelected()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(m_strFavoriteItem);
}

void CMenuOperationHandler::DoThisProgramIs()
{
	// Create About box 
	wxMozillaLikeAboutBoxDialog * dlg = new wxMozillaLikeAboutBoxDialog(_gMainFrame);
	// Set application name
	dlg->SetAppName(PROGRAM_NAME);
	// Set application version
	dlg->SetVersion(PROGRAM_MAJOR_VER + wxT(".") + PROGRAM_MINOR_HI_VER + wxT(".") + PROGRAM_MINOR_LO_VER);
	// Set copyright message
	dlg->SetCopyright(theMsgManager->GetMessage(wxT("MSG_PROGRAMM_COPYRIGHT")));
	// Set header bitmap
	//	dlg->SetHeaderBitmap(wxGetBitmapFromMemory(header_png, sizeof(header_png)));

	wxString strBuildDisp(wxT(""));
	wxString strBuildInfo(wxMozillaLikeAboutBoxDialog::GetBuildInfo(wxMozillaLikeAboutBoxDialog::wxBUILDINFO_LONG).GetData());
	wxString strDeveloper(wxT("Developed by "));
	strDeveloper += theMsgManager->GetMessage(wxT("MSG_PROGRAMM_DEVELOPER"));

	strBuildInfo = wxString::Format(wxT("%s. \n%s"), strBuildInfo, strDeveloper);
	// Set build info message. This is optional step. If you don't specify build info message then
	// default one will be used
	dlg->SetCustomBuildInfo(wxT("Advanced File Manager ") + strBuildInfo);
	// Apply changes
	dlg->ApplyInfo();
	// Show dialog
	dlg->ShowModal();
	// Destroy dialog
	dlg->Destroy();
}

#ifdef __WXMSW__
void CMenuOperationHandler::DoAddDriveAndRemove()
{
	CTabManager* pCurrentTabManager = theSplitterManager->GetActiveTab();
	pCurrentTabManager->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_DRIVE_ADD_REMOVE);
			
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		pAnotherTabManager->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_DRIVE_ADD_REMOVE);
	}	
}

void CMenuOperationHandler::DoDiskspaceUpdate(const wxString& strDrive)
{
	//현재 드라이브의 용량을 갱신한다.
	theDriveInfo->UpdateSpace(strDrive);
	CTabManager* pCurrentTabManager = theSplitterManager->GetActiveTab();
	pCurrentTabManager->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_DISK_SPACE_UPDATE, strDrive);
	
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		pAnotherTabManager->GetActiveViewPanel()->DoMyEventExecuteToView(wxEVT_DISK_SPACE_UPDATE, strDrive);
	}
}
#endif