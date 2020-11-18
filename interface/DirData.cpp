#include "../ginc.h"
#include "DirData.h"
#include <wx/bitmap.h>

CDirData::CDirData()
	: m_strName(wxT(""))
	, m_strPath(wxT(""))
	, m_strDriveName(wxT(""))
	, m_strDisp(wxT(""))
	, m_isDir(false)
	, m_isFile(false)
	, m_isDrive(false)
	, m_bSelected(false)
	, m_bMatched(false)
	, m_isCut(false)
	, m_isShortDisp(false)
	, m_ulattr(999999999)
	, m_llSize(0)
	, m_iIcon(-1)
	, m_dt(0.0)
	, m_iType(item_type::unknown)
	
{
	Init();
}

CDirData::~CDirData()
{
}

void CDirData::Init()
{
	m_strName		= wxT("");
	m_strPath		= wxT("");
	m_strDriveName	= wxT("");
	m_isDir			= false;
	m_isFile		= false;
	m_isDrive		= false;
	m_bSelected		= false;
	m_isCut			= false;
	m_ulattr		= 999999999;
	m_llSize		= 0;
	m_dt			= 0.0;
	m_iType			= item_type::unknown;
	m_iIcon			= -1;
}

void CDirData::SetName(const wxString& strName)
{
	m_strName = strName;
}

void CDirData::SetPath(const wxString& strPath)
{
	m_strPath = strPath;
}

void CDirData::SetType(item_type iType)
{
	m_iType = iType;
}

void CDirData::SetAttribute(unsigned long ulAttr)
{
	m_ulattr = ulAttr;
}

void CDirData::SetSize(const wxLongLong& llSize)
{
	m_llSize = llSize;
	if (m_iType == item_type::file)
	{
		wxString strSizeType;
		wxString strSize;
		m_colorType = wxColour(192, 192, 192);

		double dblFileSize = llSize.ToDouble();
		theCommonUtil->GetSizeInfo(dblFileSize, m_strSize, m_strSizeType, m_colorType);
	}
}

void CDirData::SetDateTime(const wxDateTime& dt)
{
	m_dt = 0.0;
	m_dt = dt;
}

void CDirData::SetItemSelected(const bool bSelected)
{
	m_bSelected = bSelected;
}


void CDirData::SetIconIndex(int iIcon)
{
	m_iIcon = iIcon;
}

void CDirData::SetIcon(const wxIcon& ico)//wxIcon icon)
{
	m_Icon = ico;
}

void CDirData::SetDriveName(const wxString& strDriveName)
{
	m_strDriveName = strDriveName;
}

void CDirData::SetDisp(const wxString& strDisp)
{
	m_strDisp = strDisp;
}

void CDirData::SetTypeName(const wxString& strTypeName)
{
	m_strTypeName = strTypeName;
}

const wxString CDirData::GetFullPath()
{
	return ((theCommonUtil->Compare(SLASH, m_strPath.Right(1)) == 0) ? m_strPath + m_strName :  m_strPath + SLASH + m_strName);
}