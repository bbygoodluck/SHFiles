#ifndef __DRIVE_INFO_H__
#define __DRIVE_INFO_H__

class CDriveItem final
{
public:
	explicit CDriveItem() {}
	~CDriveItem() {}

public:
	void SetDriveName(const wxString& strDriveName) { m_strDriveName = strDriveName; }
	const wxString& GetDriveName() { return m_strDriveName; }
	
	void SetDisplayName(const wxString& strDispName) { m_strDisplayName = strDispName; }
	const wxString& GetDisplayName() { return m_strDisplayName; }
	
	void SetSpace(const wxString& strSpace) { m_strSpace = strSpace; }
	wxString GetSpace() { return m_strSpace; }
	
	void SetTypeName(const wxString& strTypeName) { m_strTypeName = strTypeName; }
	const wxString& GetTypeName() { return m_strTypeName; }

	void SetDisplayEtc(const wxString& strDispEtc) { m_strDisplayEtc = strDispEtc; }
	const wxString& GetDisplayEtc() { return m_strDisplayEtc; }

	void SetDriveType(int iType) { m_iDriveType = iType; }
	const int GetDriveType() { return m_iDriveType; }

	void SetNetDriveFlag(bool isNetDrive) { m_bNetDrive = isNetDrive; }
	const bool IsNetDrive() { return m_bNetDrive; }

	void SetIconIndex(int iIconIndex) { m_iIconIndex = iIconIndex; }
	const int GetIconIndex() const { return m_iIconIndex; }

	void SetOverlayIconIndex(int iOverlayIndex) { m_iOverlayIndex = iOverlayIndex; }
	const int GetOverlayIconIndex() const { return m_iOverlayIndex; }

private:
	wxString m_strDriveName     = wxT("");
	wxString m_strDisplayName   = wxT("");
	wxString m_strSpace         = wxT("");
	wxString m_strTypeName      = wxT("");
	wxString m_strDisplayEtc    = wxT("");
	
	int m_iDriveType = wxFS_VOL_MAX;
	bool m_bNetDrive = false;
	int m_iIconIndex = -1;
	int m_iOverlayIndex = -1;
};

class CDriveInfo
{
private:
	CDriveInfo() 
		: m_nDriveCount(0)
		, m_strType(wxT(""))
	{
	}

public:
	~CDriveInfo() {
		std::vector<std::unique_ptr<CDriveItem>>().swap(m_drive);
	}

public:
	static CDriveInfo* Get();
	//드라이브용량가져오기
	static void GetDiskSpace(const wxString& strVolume, double& dblTotalSpace, double& dblFreeSpace);
	//드라이브용량문자열로 가져오기
	static void GetDiskSpaceToString(const wxString& strVolume, wxString& strTotalSpace, wxString& strFreeSpace);

public:
	void Init();
	int GetDriveCount() { return m_nDriveCount; }
	wxString GetDriveName(int iIndex);

	wxString GetDisplayName(int iIndex);
	wxString GetDisplayName(const wxString& strDriveName);
	
	wxString GetSpace(const wxString& strDriveName);
	wxString GetEtcInfo(const wxString& strDriveName);

	const bool IsExistDrive(const wxString& strDriveName);
	CDriveItem* GetDriveItem(int iIndex) { return m_drive.at(iIndex).get(); }
	CDriveItem* GetDriveItem(const wxString& strDriveName);

	int GetDriveType(const wxString& strDriveName);

	void UpdateSpace(const wxString& strDriveName);
	void SetIconIndex(const wxString& strDriveName, int& iIconIndex, int& iOverlayIconIndex);

private:
	wxString GetMakeDisplayName(const wxString& strDisplayName, bool IsNet = false);

private:
	static std::unique_ptr<CDriveInfo> m_pInstance;
	
	std::unordered_map<wxString, CDriveItem> m_driveItems;
	std::vector<std::unique_ptr<CDriveItem>> m_drive;

	int m_nDriveCount;
	wxString m_strType;
	wxString m_strEtcDisplay;
};
#endif