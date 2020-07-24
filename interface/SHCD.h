/*
 * CSHCDBase
 * µð·ºÅä¸® º¯°æÀ» À§ÇÑ TreeÅ¬·¡½º
 */
#ifndef __SHCD_H__
#define __SHCD_H__

typedef struct _NODE {
	wxString _Name = wxT("");
	wxString _DispName = wxT("");
	wxVector<_NODE *> _childNode;
	_NODE* _parent;
	int _index = 0;
	int _level = 0;
	int _row = 0;

	wxRect _rect;
	wxRect _rectName;
} NODE, *PNODE;

class CDirTree
{
	typedef wxVector<NODE *> pNodeContainer;
	typedef wxVector<NODE *>::iterator pNodeIterator;

private:
	CDirTree()
		: m_root(nullptr)
		, m_iLastRow(0)
		, m_strMCDFile(wxT(""))
		, m_iMaxLevel(-1)
	{

	}

public:
	~CDirTree() {
		wxVector<NODE*>().swap(_pOrder);
	}
	static CDirTree* Get();

	void Initialize();
	void SetMCDFile(const wxString& strMCDFile);
	//Tree Functions
	NODE* GetRoot() { return m_root; }
	pNodeIterator& GetBegin() { return _pBegin; }
	pNodeIterator GetEnd() { return _pOrder.end(); }

	pNodeIterator& GetCur() { return _pCur; }
	pNodeIterator GetLast() { return (_pOrder.end() - 1); }
	
	NODE* AddNode(const wxString& strNodeName, int iLevel, NODE* pParentNode);

	NODE* FindParentNode(int iDepth, NODE* pCurrNode);
	NODE* FindLastChild(NODE* pNode);

	NODE* GetChildNode(const wxString& strNodeName, int iLevel, NODE* pParentNode);
	bool FindChildNodeExist(const wxString& strNodeName, int iLevel, NODE* pParentNode);
	
	void Clear(NODE* pNode, bool bTerminateApp = false);

	void Delete() {
		Clear(m_root, true);
		delete m_root;

		m_mapItems.clear();
	};

	void SetOrder();
	void SetCur(const wxString& strFullName);
	void DeleteDir(NODE* pNode);
	void RenameDir(NODE* pNode, const wxString& strNewName);
	bool CompareNodeName(const wxString& str1, const wxString& str2);
	
	void MakeTreeOrder();
	void MakeRowMap();

	int GetItemDistance(wxVector<NODE* >::iterator it);
	int GetMaxLevel() { return m_iMaxLevel; }
	int FindMaxIndex(NODE * pNode);

	wxString GetFullPath(NODE* pCurrNode);
	std::unordered_map<int, wxVector<NODE *>>& GetDatas() { return m_mapItems; }

	void SaveMCDFile();

private:
	static std::unique_ptr<CDirTree> m_pInstance;

	NODE* m_root;
	pNodeIterator	_pLastChild;    // 마지막 자식
	int m_iLastRow;
	int m_iMaxLevel;

	pNodeContainer	_pOrder;  		///< 순회 테이블
	pNodeIterator	_pCur;			///< 현 위치 index
	pNodeIterator   _pBegin;

	std::unordered_map<int, wxVector<NODE *>> m_mapItems;

	wxString m_strMCDFile;
};

#endif