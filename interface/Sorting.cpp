#include "../ginc.h"
#include "DirData.h"
#include "Sorting.h"

CSorting::CSorting() 
{
}

CSorting::~CSorting() 
{
}

bool CSorting::DirSortOfName(CDirData& lhs, CDirData& rhs)
{
	if (lhs.IsDrive())
	{
		if (!rhs.IsDrive())
			return false;
	}
	else
	{
		if (rhs.IsDrive())
			return true;
	}
		
	int iRes = CmpDir(lhs, rhs);
	if (iRes < 0)
		return true;
	else if (iRes > 0)
		return false;

	wxString strLName(lhs.GetName());
	wxString strRName(rhs.GetName());

	iRes = CmpName(strLName, strRName);

	if (iRes < 0)
		return true;
	else
		return false;
}

bool CSorting::DirSortOfSize(const CDirData& lhs, const CDirData& rhs)
{
	return false;
}

bool CSorting::DirSortOfTime(const CDirData& lhs, const CDirData& rhs)
{
	return false;
}

bool CSorting::DirSortOfType(const CDirData& lhs, const CDirData& rhs)
{
	return false;
}

int CSorting::CmpDir(CDirData& lhs, CDirData& rhs)
{
	if (lhs.IsDir())
	{
		if (!rhs.IsDir())
			return -1;
		else
			return 0;
	}
	else
	{
		if (rhs.IsDir())
			return 1;
		else
			return 0;
	}
}

int CSorting::CmpName(const wxString& strLeft, const wxString& strRight)
{
	wxString strLName(strLeft);
	wxString strRName(strRight);
	
	int cmp = strLName.CmpNoCase(strRName);
	
	if (strLName.Cmp(wxT("..")) == 0)
		return -1;
	else if (strRName.Cmp(wxT("..")) == 0)
		return 1;
	else
		return cmp;

}
