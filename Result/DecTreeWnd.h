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
const int SIBLING_INTERVAL=100; //每层间的矩形框间的宽度
const int SUBTREE_INTERVAL=150; //每个矩形框和下一个矩形框间的高度
const int LEFT_XCOOR=20;
const int   FIX_WIDTH  = 120;   //矩形框水平间的距离间隔 
const float WIDTH_X  = 10;      //X方向乘以该值
const float HEIGHT_Y = 36;      //Y方向加上该值
const float HEIGHT_T = 0.33334; //画折线时上位置
const float HEIGHT_B = 0.66666; //画折线时下位置


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
	int					level;        //节点的级数
	int					position;     //1：左子树，2:右子树,0：其他类型
	int					childNum;
	int					numTH;        //父亲的第几个孩子，从0开始计数
	CTString            nodeLabel;    //节点的Label信息
	int                 arcNum ;      //子节点个数
	DecisionTreeNode*   parent;       //父节点
	CPtrArray  *        childList;
	DecisionTreeNode*   leftNeighbor; //左邻居  
	DecisionTreeNode*   leftSibling;
	DecisionTreeNode*   rightSibling; //左右兄弟
	CTString            nodeCode;     //节点内码，父节点内码加第几个孩子。
	LISTSTR*			arcLabelList;
public:
	LISTSTR::iterator   m_list;
	int                 width,height;          //节点矩形的高度和宽度
	int                 rel_XCoorl,rel_YCoorl; //相对坐标,计算节点位置使用
	int                 abs_XCoorl,abs_YCoorl; //绝对坐标，绘制时，节点在容器中的坐标
	CTString            nodeID;                //节点内部标示，测试用
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
	//位置函数
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
	void Clear(BOOL bReleaseMemory = FALSE); //回收
	void CreateTree();                       //由原始数据的转换、计算位置、绘制等的调用顺序
	//计算位置的函数
private:
	int GetTreeWidth();
	int GetTreeHeight();	
	void setSibling();
	void setSibling(DecisionTreeNode *pNode, int nLevel);//递归建立兄弟和节点级数.
	void setNodeSize(DecisionTreeNode *pNode);
	void setNodeCode();
	void setNodeCode(DecisionTreeNode *pNode);
	void moveChild(DecisionTreeNode *pNode,int nInterval);    //移动子孙
	void getAncestorSibling(DecisionTreeNode *pNode, CTString szNodeCode, int nInterval);
	void CalcBlurX(DecisionTreeNode *pNode, int nCoorX, int nCoorY);//模糊计算相对X坐标
	void ReCalcParentX(DecisionTreeNode *pNode);                    //重新计算父亲的X值
	void OverlapProcess(DecisionTreeNode *pNode);                   //节点交叉重叠处理
	void setAbsolateCoor();
	void Recursion(DecisionTreeNode *pParentNode);                  //树的绘制
	void deleteNode(DecisionTreeNode *pNode, int nNum);
public:
	CPtrArray *m_pNodeList;
public:
	typedef struct tagVPoint //线
	{
		double x;
		double y;
	}VPoint;
	typedef struct tagRect   //矩形
	{
		double l;        //左
		double t;        //上
		double r;        //右
		double b;        //下
		CTString szLabel;//内容
	}VRect;
private:
	CTArray<VRect, VRect>m_RectangleList;  //带边框＋文字
	CTArray<VRect, VRect>m_RectFillList;   //不带边框＋文字
	CTArray<VPoint, VPoint>m_PointList;    //线点
private:
	BOOL m_bFixWidth;//每个矩形框是否等距
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





















