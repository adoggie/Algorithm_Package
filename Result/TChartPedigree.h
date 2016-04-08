// TChartLine.h: interface for the CTChartPedigree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTPEDIGREE_H__09F34FAB_CC90_4A44_950F_A2DE2F03FA2D__INCLUDED_)
#define AFX_TCHARTPEDIGREE_H__09F34FAB_CC90_4A44_950F_A2DE2F03FA2D__INCLUDED_
#pragma  warning(disable:4251)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"
#include "TChart1.h"

typedef struct regPedigreeTreeNode
{
	double dCredit;  //置信水平，也相当于节点横坐标
	double yPosition;//该节点的纵坐标
	int    elemsCode;//该节点的元素代码
	bool   bLeaf;    //是否为最底层节点
	CPtrArray NodePtrArray;//指向下一层节点的指针数组
}PedigreeTreeNode;

class AFX_EXT_CLASS CTChartPedigree : public CTChart  
{
public:
	CTChartPedigree();
	CTChartPedigree(BOOL bDataType);
	virtual ~CTChartPedigree();
private:
	void Release(CPtrArray *pArr);
	void DeleteTree(PedigreeTreeNode &node);
	void CalcPedigree(PedigreeTreeNode& node);
	void CalcMaxTreeNode(PedigreeTreeNode& node);
	void PrepareData(TDataPointPedigree *pPredigree);
public:
	void SetValue(double fMinX, double fMaxX, double fMinY, double fMaxY, const PedigreeTreeNode *pNode, CPtrArray &arrNumY);
protected:
	typedef struct tagYValue
	{
		int leaf;
		double ypos;
	}YValue;
	typedef struct tagVPoint
	{
		double x;
		double y;
	}VPoint;
	double m_fMinX;//X轴最小值
	double m_fMaxX;//X轴最大值
	double m_fMinY;//Y轴最小值
	double m_fMaxY;//Y轴最大值
	int    m_nCount;//总个数
	BOOL   m_bDataType;         //TRUE－分层聚类；FALSE－为模糊聚类
	PedigreeTreeNode m_treeNode;//树型结构图
	CPtrArray m_arrNumY;        //原始单元编号
	CTArray<YValue,YValue>m_PointArray;
	CTArray<VPoint,VPoint>m_LinePoint;
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTLINE_H__A66DA0D5_9A3B_42DB_8C43_49C7341252FD__INCLUDED_)
