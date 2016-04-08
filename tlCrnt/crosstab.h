//////////////////////////////////////////////////////////////////////
// CrossTab.h                                                       //
//																	//
// Define:        交叉表crosstabulation算法类定义                   //
// Creator:       夏东山                                              //
// Date:          2006.03.20                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _CROSSTAB_H_
#define _CROSSTAB_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"
const int CALC_COLFLD = 0;
const int CALC_ROWFLD = 1;
const int CALC_TMPFLD = 2;
//public:
class CCrossTab  
{
public:
	CCrossTab();
	virtual ~CCrossTab();
	CResult * OnCrossTab(CDataInterface *pDataInterface, string VO); // 界面消息响应函数

private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	//内部入口函数
	void CrossTab();
	//通过值得到相应的标签
	CTString GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue);
	//创建汇总表
	void CreateSummaryTable(CDoubleMatrix &dataMatrix, int nRowIndex, int nColIndex);
	//创建交叉表
	void CreateCrossTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol);
	//创建统计量表
	void CreateStatisticTable(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol);
	//对列向量进行统计
	//BOOL CalcColInfo(CDoubleMatrix &dataMatrix, CDoubleVector &vecCol, CPtrArray *pArrColInf);
	//对行向量进行统计
	//int CalcRowInfo(CDoubleMatrix &dataMatrix, CIntVector &vecRowIndex, CPtrArray *pArrRowInf);
	//得到显示列的索引
	CFldInfo* GetFldInfByValue(double fValue, CPtrArray *pArrColInf);
	//插入数据到交叉表
	BOOL InsertCrossTab(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CPtrArray *pArrRow, CPtrArray *pArrCol, CTLTable * pCrossTable, int nRow);
	//插入某一行数据到交叉表
	void InsertCorssTabOfRow(CMapManager &dataMap, CTLTable * pCrossTable, CFldInfo* pRowInf, CPtrArray *pArrCol, int nRowStart, int nRow, double fTotalWeight, BOOL bLast = FALSE);
	//计算得到指针
	void CalcFldInf(double fValue, double fVarValue, double fWeight, CPtrArray *pArrFld, int nSize, int nType);
	//计算域信息
	BOOL CalcFldInf(  CDoubleVector &v,CDoubleVector &vc, CDoubleVector &w, CPtrArray *pArrFld, int nType);
	//计算相关系数及显著性
	BOOL CalcCorrlation(CDoubleMatrix &dataMatrix, int nRow, int nCol);
	//计算卡方及自由度
	BOOL CalcChiSquare(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol);
	//释放内存
	void Release(CPtrArray *pArrFld);
private:
	TIntStruct m_tColInfo;    //列
	TIntStruct m_tRowInfo;    //行
	int m_nCrossTableCnt;     //交叉表个数
	int m_nType;              //控制输出
	__int64 m_n64Weight;      //权重和或总记录数
	int m_nWeight;			  //权重索引
	int  m_nColSize;          //列总数
	int  m_nCurRows;          //当前总行数
	BOOL m_bWeight;           //是否带权重
	BOOL m_bOutputChiSquare;  //是否输出卡方
	BOOL m_bOutputCorrelation;//是否输出相关系数
	BOOL m_bOutputExpCount;   //是否输出交叉表
	double m_fPChiSquare;     //皮尔逊卡方
	double m_fPChiSquareSig;  //皮尔逊卡方显著性
	double m_fLChiSquare;     //似然卡方
	double m_fLChiSquareSig;  //似然卡方显著性
	int    m_nDF;             //自由度
	double m_fPearsonCorr;    //皮尔逊相关系数
	double m_fSig;            //显著性
	//
	CTLTable *m_pSummaryTable;//汇总表
	CTLTable *m_pCorrelationTable;//相关系数表
	CTLTable *m_pChiSquareTable;  //卡方表
	CResult        *m_pResult;    //数据集指针
	CDataInterface *m_pDataInterface; //外部接口
};

#endif // !defined(_CROSSTAB_H_)
