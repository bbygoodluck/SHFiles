#include "../ginc.h"
#include "SHCD.h"

std::unique_ptr<CDirTree> CDirTree::m_pInstance(nullptr);

CDirTree* CDirTree::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CDirTree());

	return m_pInstance.get();
}

void CDirTree::Initialize()
{
	m_root       = nullptr;
	m_iLastRow   = 0;
	m_iMaxLevel  = -1;
	m_strMCDFile = wxT("";)

	m_mapItems.clear();
}

void CDirTree::SetMCDFile(const wxString& strMCDFile)
{
	m_strMCDFile = strMCDFile;
}

NODE* CDirTree::AddNode(const wxString& strNodeName, int iLevel, NODE* pParentNode)
{
	NODE* pNode   = new NODE();
	pNode->_Name  = strNodeName;

	if (iLevel > m_iMaxLevel)
		m_iMaxLevel = iLevel;

	if (m_root == nullptr || pParentNode == nullptr)
	{
		pNode->_parent = nullptr;
		pNode->_level = iLevel;
		m_root = pNode;
	}
	else
	{
		pNode->_level = iLevel;

		if (pParentNode->_level < iLevel)
		{
			pNode->_level = iLevel;
			pNode->_parent = pParentNode;
			pParentNode->_childNode.push_back(pNode);
		}
		else
		{
			NODE* _parentNode = FindParentNode(iLevel, pParentNode);

			pNode->_parent = _parentNode;
			_parentNode->_childNode.push_back(pNode);
		}
	}

	return pNode;
}

bool CDirTree::FindChildNodeExist(const wxString& strNodeName, int iLevel, NODE* pParentNode)
{
	wxVector<NODE *>::iterator iter = pParentNode->_childNode.begin();
	wxString strName(wxT(""));
	int _childLevel = -1;
	bool bReturn = false;
	int _cmpName;
	bool bCmpLevel;
	while (iter != pParentNode->_childNode.end())
	{
		strName = (*iter)->_Name;
		_childLevel = (*iter)->_level;

		bCmpLevel = (iLevel == _childLevel);
#ifdef __WXMSW__
		_cmpName = strNodeName.CmpNoCase(strName);
#else
		_cmpName = strNodeName.Cmp(strName);
#endif

		if (_cmpName == 0 && bCmpLevel)
		{
			bReturn = true;
			break;
		}

		++iter;
	}

	return bReturn;
}

NODE* CDirTree::GetChildNode(const wxString& strNodeName, int iLevel, NODE* pParentNode)
{
	wxVector<NODE *>::iterator iter = pParentNode->_childNode.begin();
	wxString strName(wxT(""));
	int _childLevel = -1;
	bool bExist = false;
	int _cmpName;
	bool bCmpLevel;
	while (iter != pParentNode->_childNode.end())
	{
		strName = (*iter)->_Name;
		_childLevel = (*iter)->_level;

		bCmpLevel = (iLevel == _childLevel);
#ifdef __WXMSW__
		_cmpName = strNodeName.CmpNoCase(strName);
#else
		_cmpName = strNodeName.Cmp(strName);
#endif

		if (_cmpName == 0 && bCmpLevel)
		{
			bExist = true;
			break;
		}

		++iter;
	}

	if (bExist)
		return (*iter);

	return nullptr;
}


NODE* CDirTree::FindParentNode(int iLevel, NODE* pCurrNode)
{
	NODE* pNode = pCurrNode;

	while (iLevel <= pNode->_level)
		pNode = pNode->_parent;

	return pNode;
}

void CDirTree::Clear(NODE* pNode, bool bTerminateApp)
{
	if (pNode == nullptr)
		return;

	wxVector<NODE *>::iterator iter;
	for (iter = pNode->_childNode.begin(); iter != pNode->_childNode.end(); ++iter)
	{
		if (*iter)
		{
			NODE* _node = *iter;
			Clear(_node);
			delete _node;
		}
	}

	pNode->_childNode.clear();
	if (bTerminateApp)
		wxVector<NODE*>().swap(pNode->_childNode);
}

int CDirTree::GetItemDistance(wxVector<NODE *>::iterator it)
{
	return std::distance(_pOrder.begin(), it);
}

void CDirTree::SetOrder()
{
	wxVector<NODE *> vNode;
	NODE*	pNode;

	int index = 0;

	_pOrder.clear();

	vNode.push_back(m_root);
	while (!vNode.empty())
	{
		pNode = vNode.back();
		vNode.pop_back();

		pNode->_index = index;
		_pOrder.push_back(pNode);

		index++;
		std::reverse_copy(std::begin(pNode->_childNode), std::end(pNode->_childNode), std::back_inserter(vNode));
	}

	_pBegin = _pOrder.begin();
	_pCur = _pOrder.begin();

	MakeTreeOrder();

#ifdef _WXDEBUG
//	for (_pCur; _pCur != _pOrder.end(); _pCur++)
//	{
//		theCommonUtil->LogDisplayToConsole(wxT("Name : ") + (*_pCur)->_Name + wxString::Format(wxT(" lv:%d rw:%d idx:%d"), (*_pCur)->_level, (*_pCur)->_row, (*_pCur)->_index));
//	}
#endif
 }

void CDirTree::MakeTreeOrder()
{
	wxVector<NODE *>::iterator iter = _pOrder.begin();
	NODE* pTmpNode = nullptr;
	m_iLastRow = 0;

	m_mapItems.clear();
	wxVector<NODE* > mapData;
	int iPrevRow = 0;

	for (iter; iter != _pOrder.end(); ++iter)
	{
		NODE* pNode = *iter;

		//rootÀÇ °æ¿ì´Â ºÎ¸ð³ëµå°¡ Á¸ÀçÇÏÁö ¾ÊÀ½
		if (m_root == nullptr)
			pNode->_row = m_iLastRow;
		else
		{
			if (pTmpNode != nullptr)
			{
				if (pTmpNode->_level < pNode->_level)
					pNode->_row = m_iLastRow;
				else
					pNode->_row = ++m_iLastRow;// _parentNode->_row + 1;
			}
		}

		if (iPrevRow != pNode->_row)
		{
			std::unordered_map<int, wxVector<NODE* >>::value_type val(iPrevRow, mapData);
			m_mapItems.insert(val);

			mapData.clear();
		}

		mapData.push_back(pNode);
		iPrevRow = pNode->_row;

		pTmpNode = pNode;
	}

	//¸¶Áö¸· µ¥ÀÌÅÍ ÀûÀç
	std::unordered_map<int, wxVector<NODE* >>::iterator iterFind = m_mapItems.find(iPrevRow);
	if (iterFind == m_mapItems.end())
	{
		std::unordered_map<int, wxVector<NODE* >>::value_type val(iPrevRow, mapData);
		m_mapItems.insert(val);
	}

	mapData.clear();
	wxVector<NODE* >().swap(mapData);
}

void CDirTree::MakeRowMap()
{
	//È­¸é¿¡ Ç¥½ÃµÉ Row º°·Î µ¥ÀÌÅÍ¸¦ map¿¡ ÀúÀå
	m_mapItems.clear();
	wxVector<NODE* >::iterator iter = _pOrder.begin();
	wxVector<NODE* > mapData;
	int iPrevRow = 0;

	while (iter != _pOrder.end())
	{
		if (iPrevRow != (*iter)->_row)
		{
			std::unordered_map<int, wxVector<NODE* >>::value_type val(iPrevRow, mapData);
			m_mapItems.insert(val);

			mapData.clear();
		}

		mapData.push_back((*iter));
		iPrevRow = (*iter)->_row;
		++iter;
	}

	//¸¶Áö¸· µ¥ÀÌÅÍ ÀûÀç
	std::unordered_map<int, wxVector<NODE* >>::iterator iterFind = m_mapItems.find(iPrevRow);
	if (iterFind == m_mapItems.end())
	{
		std::unordered_map<int, wxVector<NODE* >>::value_type val(iPrevRow, mapData);
		m_mapItems.insert(val);
	}
}

void CDirTree::SetCur(const wxString& strFullName)
{
	if (strFullName.IsEmpty())
	{
		_pCur = _pOrder.begin();
		return;
	}

	if (CompareNodeName(strFullName, m_root->_Name) ||
		strFullName.size() < m_root->_Name.size())
	{
		_pCur = _pOrder.begin();
		return;
	}

	wxString strTmpPath(strFullName);
	wxString strTmpPath2(m_root->_Name);

	wxStringTokenizer _tToken(strTmpPath, SLASH);
	NODE *pNode = m_root;
	pNodeIterator i = _pOrder.begin();

	if (_tToken.HasMoreTokens())
		_tToken.NextToken();

	while (_tToken.HasMoreTokens())
	{
		const wxString& t = _tToken.NextToken();
		for (i = pNode->_childNode.begin(); i != pNode->_childNode.end(); i++)
		{
			if (CompareNodeName((*i)->_Name, t))
				break;
		}

		if (i == pNode->_childNode.end())
		{
			_pCur = _pOrder.begin();
			return;
		}

		pNode = (*i);
	}

	if (i)
		_pCur = _pOrder.begin() + (*i)->_index;
	else
		_pCur = _pOrder.begin();
}

bool CDirTree::CompareNodeName(const wxString& str1, const wxString& str2)
{
#ifdef __WXMSW__
	return (str1.CmpNoCase(str2) == 0);
#else
	return (str1.Cmp(str2) == 0);
#endif
}

void CDirTree::DeleteDir(NODE* pNode)
{
	NODE* pParent = pNode->_parent;
	wxString strFullPath = GetFullPath(pParent);

	Clear(pNode);
	wxVector<NODE *>::iterator iter = pParent->_childNode.begin();
	int iChildIndex = 0;
	for (iter; iter != pParent->_childNode.end(); ++iter)
	{
		if ((*iter) == pNode)
			break;

		iChildIndex++;
	}

	pParent->_childNode.erase(pParent->_childNode.begin() + iChildIndex);
	delete pNode;

	SetOrder();
	SaveMCDFile();

//	MakeRowMap();
	SetCur(strFullPath);
}

void CDirTree::RenameDir(NODE* pNode, const wxString& strNewName)
{
	wxString strOldName = pNode->_Name;
	wxString strParent = GetFullPath(pNode->_parent);
	wxString strFullOld = strParent + SLASH + strOldName;
	wxString strFullNew = strParent + SLASH + strNewName;
	wxRename(strFullOld, strFullNew);

	pNode->_Name = strNewName;
	SaveMCDFile();
}

void CDirTree::SaveMCDFile()
{
	wxFile fn(m_strMCDFile, wxFile::write);
	wxVector<NODE *>::iterator iter = _pOrder.begin();

	wxString strSave(wxT(""));
	for (iter; iter != _pOrder.end(); ++iter)
	{
		NODE* pNode = (*iter);
		strSave = wxString::Format(wxT("%d^"), pNode->_level) + pNode->_Name + wxT("\n");
		fn.Write(strSave);
	}

	fn.Close();
}

int CDirTree::FindMaxIndex(NODE * pNode)
{
	int iLastIndex = -1;
	return FindLastChild(pNode)->_index;
}

NODE* CDirTree::FindLastChild(NODE* pNode)
{
	int iChildCount = pNode->_childNode.size();
	_pLastChild = pNode->_childNode.begin() + (iChildCount - 1);

	return (*_pLastChild);
}

wxString CDirTree::GetFullPath(NODE* pCurrNode)
{
	NODE* pNode = pCurrNode;
	wxString strFullPath = pNode->_Name;
	NODE* pParent = nullptr;

	while (true)
	{
		pParent = pNode->_parent;
		if (pParent == nullptr)
			break;

		if (pParent == m_root)
		{
			strFullPath = pParent->_Name + strFullPath;
			break;
		}

		strFullPath = pParent->_Name + SLASH + strFullPath;
		pNode = pParent;
	}

	return strFullPath;
}
