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
	double dCredit;  //����ˮƽ��Ҳ�൱�ڽڵ������
	double yPosition;//�ýڵ��������
	int    elemsCode;//�ýڵ��Ԫ�ش���
	bool   bLeaf;    //�Ƿ�Ϊ��ײ�ڵ�
	CPtrArray NodePtrArray;//ָ����һ��ڵ��ָ������
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
	double m_fMinX;//X����Сֵ
	double m_fMaxX;//X�����ֵ
	double m_fMinY;//Y����Сֵ
	double m_fMaxY;//Y�����ֵ
	int    m_nCount;//�ܸ���
	BOOL   m_bDataType;         //TRUE���ֲ���ࣻFALSE��Ϊģ������
	PedigreeTreeNode m_treeNode;//���ͽṹͼ
	CPtrArray m_arrNumY;        //ԭʼ��Ԫ���
	CTArray<YValue,YValue>m_PointArray;
	CTArray<VPoint,VPoint>m_LinePoint;
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTLINE_H__A66DA0D5_9A3B_42DB_8C43_49C7341252FD__INCLUDED_)
