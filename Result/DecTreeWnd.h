#if !defined(AFX_DECTREEWND_H__B48B608C_25F8_4F76_81CF_22095B3F1744__INCLUDED_)
#define AFX_DECTREEWND_H__B48B608C_25F8_4F76_81CF_22095B3F1744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecTreeWnd.h : header file
//
#pragma  warning(disable:4251)

#include <list>
#include "../Public/TString.h"
#include "../Public/TCLFile.h"
#include "../Public/common.h"
#include "TGraphData.h"
/////////////////////////////////////////////////////////////////////////////
// CDecTreeWnd window
const int ORIGIN_X=1;
const int ORIGIN_Y=10;
const int SIBLING_INTERVAL=100; //ÿ���ľ��ο��Ŀ��
const int SUBTREE_INTERVAL=150; //ÿ�����ο����һ�����ο��ĸ߶�
const int LEFT_XCOOR=20;
const int   FIX_WIDTH  = 120;   //���ο�ˮƽ��ľ����� 
const float WIDTH_X  = 10;      //X������Ը�ֵ
const float HEIGHT_Y = 36;      //Y������ϸ�ֵ
const float HEIGHT_T = 0.33334; //������ʱ��λ��
const float HEIGHT_B = 0.66666; //������ʱ��λ��


class  AFX_EXT_CLASS DecisionTreeNode
{
	typedef std::vector<CTString> LISTSTR;
public:
	DecisionTreeNode();
	DecisionTreeNode(CTString nodeLabel,int arcNum,DecisionTreeNode * parentNode);
	virtual ~DecisionTreeNode();
    void Serialize(TCLFile &file);
	virtual void	Print();

public:
	bool				isRoot;
	int					level;        //�ڵ�ļ���
	int					position;     //1����������2:������,0����������
	int					childNum;
	int					numTH;        //���׵ĵڼ������ӣ���0��ʼ����
	CTString            nodeLabel;    //�ڵ��Label��Ϣ
	int                 arcNum ;      //�ӽڵ����
	DecisionTreeNode*   parent;       //���ڵ�
	CPtrArray  *        childList;
	DecisionTreeNode*   leftNeighbor; //���ھ�  
	DecisionTreeNode*   leftSibling;
	DecisionTreeNode*   rightSibling; //�����ֵ�
	CTString            nodeCode;     //�ڵ����룬���ڵ�����ӵڼ������ӡ�
	LISTSTR*			arcLabelList;
public:
	LISTSTR::iterator   m_list;
	int                 width,height;          //�ڵ���εĸ߶ȺͿ��
	int                 rel_XCoorl,rel_YCoorl; //�������,����ڵ�λ��ʹ��
	int                 abs_XCoorl,abs_YCoorl; //�������꣬����ʱ���ڵ��������е�����
	CTString            nodeID;                //�ڵ��ڲ���ʾ��������
public:
	int               GetBufLen();
	void              Serialize(unsigned char *&pBuf);
	void              setLevel(int num);
	int               getLevel();
	int               getPosition();
	void              setPosition(int num);
	void              setNodeLabel(CTString label);
	CTString          getNodeLabel();
	void              setParent(DecisionTreeNode * parentNode);
	DecisionTreeNode *getParent();
	void              setArcNum(int arcNum);
	int               getArcNum();
	void              setArcLabel(CTString arcLabel,int arcNum);
	CTString          getArcLabel(int arcNum);
	void              setChild(DecisionTreeNode * child,int children);
	DecisionTreeNode *getChild(int children);
	void              setLeftNeighbor(DecisionTreeNode* leftNeighbor);
	DecisionTreeNode *getLeftNeighbor();
	void              setLeftSibling(DecisionTreeNode* leftSibling);
	DecisionTreeNode *getLeftSibling();
	void              setRightSibling(DecisionTreeNode* rightSibling);
	DecisionTreeNode *getRightSibling();
	void              setChildNum(int num);
	int               getChildNum();
	void			  setNodeCode(CTString code);
	CTString		  getNodeCode();
	//λ�ú���
	void  setWidth(int num);
	int   getWidth();
	void  setHeight(int num);
	int   getHeight();
	void  setRelativeCoor(int cx,int cy);
	void  setRelativeCoor(SIZE size);
	int   getXRelativeCoor();
	int   getYRelativeCoor();
	SIZE  getRelativeCoor();
	void  setAbsolateCoor(int cx,int cy);
	void  setAbsolateCoor(SIZE size);
	int   getXAbsolateCoor();
	int   getYAbsolateCoor();
	SIZE  getAbsolateCoor();
};

class  AFX_EXT_CLASS ManagerNode
{
public:
	ManagerNode();
	virtual ~ManagerNode();
	virtual void Serialize(TCLFile &file);
	void Serialize(unsigned char *&pBuf);
	virtual void Print();
	int GetBufLen();
public:
	void GetData(tagDataPointDecTree *pData);
	void SetData(CPtrArray *pArrList);
private:
	DecisionTreeNode *getRoot();
	void Clear(BOOL bReleaseMemory = FALSE); //����
	void CreateTree();                       //��ԭʼ���ݵ�ת��������λ�á����Ƶȵĵ���˳��
	//����λ�õĺ���
private:
	int GetTreeWidth();
	int GetTreeHeight();	
	void setSibling();
	void setSibling(DecisionTreeNode *pNode, int nLevel);//�ݹ齨���ֵܺͽڵ㼶��.
	void setNodeSize(DecisionTreeNode *pNode);
	void setNodeCode();
	void setNodeCode(DecisionTreeNode *pNode);
	void moveChild(DecisionTreeNode *pNode,int nInterval);    //�ƶ�����
	void getAncestorSibling(DecisionTreeNode *pNode, CTString szNodeCode, int nInterval);
	void CalcBlurX(DecisionTreeNode *pNode, int nCoorX, int nCoorY);//ģ���������X����
	void ReCalcParentX(DecisionTreeNode *pNode);                    //���¼��㸸�׵�Xֵ
	void OverlapProcess(DecisionTreeNode *pNode);                   //�ڵ㽻���ص�����
	void setAbsolateCoor();
	void Recursion(DecisionTreeNode *pParentNode);                  //���Ļ���
	void deleteNode(DecisionTreeNode *pNode, int nNum);
public:
	CPtrArray *m_pNodeList;
public:
	typedef struct tagVPoint //��
	{
		double x;
		double y;
	}VPoint;
	typedef struct tagRect   //����
	{
		double l;        //��
		double t;        //��
		double r;        //��
		double b;        //��
		CTString szLabel;//����
	}VRect;
private:
	CTArray<VRect, VRect>m_RectangleList;  //���߿�����
	CTArray<VRect, VRect>m_RectFillList;   //�����߿�����
	CTArray<VPoint, VPoint>m_PointList;    //�ߵ�
private:
	BOOL m_bFixWidth;//ÿ�����ο��Ƿ�Ⱦ�
};

	
class AFX_EXT_CLASS CDecTreeWnd
{
public:
	CDecTreeWnd();
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	void SetNodeTree(CPtrArray *pNodeList);
	ManagerNode m_manager;
	virtual ~CDecTreeWnd();
	virtual void Serialize(TCLFile &file);
	virtual void Print();
};
#pragma  warning(default:4251)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECTREEWND_H__B48B608C_25F8_4F76_81CF_22095B3F1744__INCLUDED_)





















