// StatiMethod.h: interface for the CStatiMethod class.
//
//		CDescriptive 均值
//			CFrequency 频率
//			CExplore 描述
//				夏东山		2006/03/29
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATIMETHOD_H__A9465ACD_92CE_4A69_968A_D5782972CB5D__INCLUDED_)
#define AFX_STATIMETHOD_H__A9465ACD_92CE_4A69_968A_D5782972CB5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"
const double MAX_VALUE = 1e300;
const double MIN_VALUE = -1e300;
class AFX_EXT_CLASS CStatiMethod
{
public:
	CStatiMethod();
	virtual ~CStatiMethod();
	virtual void MoveTable(int nHor,int nVer){};
	virtual void OnClose() {};
protected:
	void ParseStatMethod(AnaWord VOstr);//解析得到统计方法
	void CalcParam(CDoubleVector &v, CDoubleVector &w, BOOL bCalcMaxMin = TRUE);
	void CalcFldInfo(double fValue, double fVarValue, CPtrArray *pArrFld, int nSize);//查找索引
	void CalcFldInfo(double fValue, double fVarValue, double fWeight, CPtrArray *pArrFld, int nSize);//查找索引
	void CalcFldInfo(double fValue, double fWeight, CPtrArray *pArrFld);//查找索引
	void CalcRangeInfo(double fValue, double fWeight, CPtrArray *pArrFld);//查找索引
	virtual void Release(CPtrArray *pArrFld); //释放内存
	BOOL CalcFrequency(CDoubleVector &v, CDoubleVector &w, int nColIndex, CPtrArray &arrFld, int nDispMax, BOOL bCalc);//计算频数（或权重）
	BOOL CalcFrequency(CDoubleVector &v, CDoubleVector &w, CIntVector &vIndex, int nColIndex, CPtrArray &arrFld, int nDispMax);
	CTString GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue);//得到标签
	virtual void Free(CPtrArray * par, BOOL bLayer = FALSE);//modify by xds 20060106.for delete...

protected:
	BOOL    m_bCalcVar;    //是否能计算方差等
	BOOL    m_bWeight;     //是否带权重
	BOOL    m_bOutApex;    //是否输出峰值
	BOOL    m_bOutApexE;   //是否输出峰值误差
	BOOL    m_bOutPartial; //是否输出偏值
	BOOL    m_bOutPartialE;//是否输出偏值误差
	int     m_nWeight;     //权重索引
	CResult        *m_pResult;       //结果集
	CDataInterface *m_pDataInterface;//数据接口
	TIntStruct m_tStatMethod;                 //表示选中了哪些方法
	TIDName  m_tStatMethodS[macDESCRIPMETHOD];//描述分析的所有方法
	struct CFreOut		   //定义输出变量结构
	{
	public:
		double  m_fMin;    //最小值
		double  m_fMax;    //最大值
		double  m_fSum;    //有效权重和或有效记录数
		double  m_fSmy;    //总和
		double  m_fMid;    //中位数
		double  m_fMany;   //众位数
		double m_fMean;    //平均值
		double m_fMe;      //均值标准误
		double m_fS;       //标准差
		double m_fS2;      //方差
		double m_fApex;    //峰值
		double m_fApexE;   //峰值误差
		double m_fPartial; //偏值
		double m_fPartialE;//偏值误差
		double m_fTrimMean;//截尾均值
	public:
		void Init()
		{
			m_fMe  = 0;
			m_fSum = 0;
			m_fSmy = 0;
			m_fMid = 0;
			m_fMany = 0;
			m_fMean = 0;
			m_fTrimMean  = 0;
			m_fS = m_fS2 = 0;
			m_fMin = m_fMax = 0;
			m_fApex = m_fApexE = 0;
			m_fPartial = m_fPartialE = 0;
		};
	} m_tOut;
};

class AFX_EXT_CLASS  CDescriptive : public CStatiMethod
{
struct TIndStep //主要保存变量的索引及步长->仅传递参数用
{
public:
	TIndStep()
	{
		nStartInd = 0;
		nStep = 0;
		nType = 2;
		arrData.RemoveAll();
	};
	int nStartInd; //开始的索引
	int nStep;     //从索引开始的步长
	int nType;     //显示类型：0-数字，1-总数，2-空白
	CPtrArray arrData;//数据
};
public:
	CDescriptive();
	virtual ~CDescriptive();
	CResult*  OnDescriptive(CDataInterface* pDataInterface, string VO);
	BOOL Descriptive(CDataInterface *pDataInterface, int *pSelFeildArray,int nSelFldCount,double *pOutArray,int nMethod);
private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	/************************Begin add by xds 20060105*************************/
	void DispResultOfNoLayer();
	void DispResultOfHaveLayer();
	void SortDataSet(CPtrArray *pArrDataSet);
	void SortVector(CDoubleVector &dataVector, int *pVectorIndex);
	inline void Swap(double &f1, double &f2, int &n1, int &n2);
	void QSortVector(CDoubleVector& dataVector, int *pVectorIndex, int nLow, int nHigh);
	inline void Swap(TDataSet *p1, TDataSet *p2);
	void QSortDataSet(CPtrArray *pArrDataSet, int nLow, int nHigh);
	void ParseLayerName(CTStringArray *pArrRet, int nLayer);
	int  ParseLayerIndex(int nLayerIndex, int nCurLayer=0);
	void Output(CTLTable *pTable, int nCol);
	void Output(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable);
	void Output1(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable);
	void Output2(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable);
	void Output3(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable);
	void InsertTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable, TIndStep* pT1, TIndStep *pT2 = NULL, TIndStep *pT3 = NULL);
	void InsertTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CFldInfo *pFld, CTLTable *pTable, TIndStep* pT1, TIndStep *pT2 = NULL, TIndStep *pT3 = NULL);
	virtual void Free(CPtrArray *pArr, BOOL bLayer = FALSE);
	/************************End add by xds 20060105**************************/
	BOOL IsOverDisp();
	void GetLayerName(CTStringArray *pArrName, int nLayerDisp);
	CTString GetValueString(CMapManager* pDataMap,int nLayerIndex ,double fValue);
private:
	int          m_nColIndex;     //当前列的索引
	int          m_nTotalDisp;    //显示的表总数；
	int          m_nCurRows;      //表单的当前行数
	int          m_nCounter;      //当前层数计数器,2*3*5
	CIntVector   m_FirVecInd;     //存放第一层索引
	CDWordArray  m_nArrInd;       //层索引
	CTStringArray m_szArrLayerName;//解析后存放层的信息，诸如:期中1+期中2+期末
private:
	CPtrArray m_arrLayerConf;
	TIntStruct m_tVarSel;    //表示自变量
};

class  AFX_EXT_CLASS CExplore : public CStatiMethod
{
public:
	CExplore();
	virtual ~CExplore();
	CResult* OnExplore(CDataInterface* pDataInterface, string VO);
	CResult* Explore(CDataInterface* pDataInterface,CTString strScript);
private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	//描述函数入口
	void OnExplore(); 
	//计算百分位数.dataMatrix:数据矩阵,vecInd:排序后的索引
	void CalcPercent(CDoubleMatrix &dataMatrix, CIntVector &vecInd);
	//计算截尾平均值.dataMatrix:数据矩阵,vecInd:排序后的索引
	BOOL CalcTrimMean(CDoubleMatrix &dataMatrix, CIntVector &vecInd);
	//计算偏值,峰值及误差.dataMatrix:数据矩阵,vecInd:排序后的索引
	void CalcPartial_Apex(CDoubleMatrix &dataMatrix, CIntVector &vecInd);
	//添加盒状图.dataMatrix:数据矩阵,pVariable:结果集,szFldName:具体的字段名称
	void AddBoxPlot(CDoubleMatrix &dataMatrix, CRsltVariable *pVariable, CTString szFldName);
	//添加茎叶图.dataMatrix:数据矩阵,vecInd:排序后的索引,pVariable:结果集,szFldName:具体的字段名称,szGroup:对有因子时的分组名称
	void AddStemPlot(CDoubleMatrix &dataMatrix, CIntVector &vecInd, CRsltVariable *pVariable, CTString szFldName, CTString szGroup = _T(""));
	//创建结果集.dataMatrix:数据矩阵,vecInd:排序后的索引,pVariable:结果集,szFldName:具体的字段名称
	void CreateResult(CDoubleMatrix &dataMatrix, CIntVector &vecInd, CRsltVariable *pVariable, CTString szFldName);
	//计算盒状图的坐标.dataMatrix:数据矩阵
	TPlotReco GetCoordinate(CDoubleMatrix &dataMatrix);
	//计算茎叶分布.dataMatrix:数据矩阵,vecInd:排序后的索引,pStemLeaf:存放结果指针,fStemLeaf:枝叶的跨度,返回值:茎叶数
	BOOL CalcStemLeaf(CDoubleMatrix &dataMatrix, CIntVector &vecInd ,TStemLeaf *&pStemLeaf, double &fStemLeaf, int &nRow_num);
private:
	int GetMethodS(int* &pMethodS);
	int GetValidData(CDataInterface *pDataInterface,TDataSet *&pDataArray, CTString strSelName,CTString strFactName);
	void OnExplore(CDataInterface* pDataInterface);
	CTLTable* CreateTableS(int which,CTString strTitle,CTString strHead,int nMaxMin[2]=NULL);
    void AddStemPlot(CRsltVariable* pVariable,TDataSet *pDataArray,int count,CTString strName,CTString strGroup);
    void AddBoxPlot(CRsltVariable* pVariable,TDataSet *pDataArray,int count,CTString strName);
	void CreateResult(CTString strTitle,CTString strHead,TDataSet* pDataArray,int nSumData,int nData,int *pMethodS,int nMethodCount,CRsltVariable* &pVariable,double weight[]);
	TStatBoxPlot GetPlotBox(TDataSet *pDataArray,int count,CTString strName);
/////// 描述分析的控制参数
public:
	double  *pWeight;
	int     m_nCurRows;    //总行数
	__int64 m_n64Weight;   //总的权重或行数
	int     m_nConfidence; //置信度
	BOOL    m_bBoth;       //是否都显示统计图及表
	BOOL    m_bStatistics; //是否显示统计表
	BOOL    m_bPlots;      //是否仅显示统计图
	BOOL    m_bPercentiles;//是否显示百分数
	BOOL    m_bMEstimators;//是否显示均值评估
	BOOL    m_bDescriptive;//是否显示描述
	BOOL    m_bOutliers;   //是否显示最大最小的五个值
	double m_fPercentile[DISP_PERCENT][2];//百分位数
	TIntStruct m_tDependent;//表示选中的自变量字段
	TIntStruct m_tFactor;   //表示选中的因变量字段
};

class  AFX_EXT_CLASS CFrequency : public CStatiMethod
{
public:
	CFrequency();
	virtual ~CFrequency();
	CResult*  OnFrequency(CDataInterface* pDataInterface, string VO);
	CResult*  Frequency(CDataInterface* pDataInterface,CTString strScript);
private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	//是否超出最大的显示范围.
	BOOL IsOverDisp(int nColIndex);
	//显示频率统计.
	void DispFrequency();
	//当前区间是否存在范围
	BOOL IsExistRange(int nColIndex);
	//创建区间
	void CreatePtrArray(CPtrArray &arrFld, int nColIndex);
	//插入表数据－分区段
	void InsertTable(double fValueFrom, double fValueTo, double fFreValue, double fFreTotal , CTLTable* pTable, int nType = 0);
	//插入表数据.dataMap:数据映射表,对应的列索引,nColIndex:列索引,fMapValue:映射的值,fFreValue:当前的频率和,fFreTotal:总的频率和,pTable:表对象指针,nType:插入类型,bValid:是否为合法的数据
	void InsertTable(CMapManager &dataMap, int nColIndex, double fMapValue, double fFreValue, double fFreTotal , CTLTable* pTable ,int nType ,BOOL bValid = TRUE);
	//创建频率表.dataMatrix:数据矩阵,nColIndex:列索引,dataMap:数据映射表,pArrFld:排序后值及权重,nType:显示类型
	void CreateFreqTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, int nColIndex, CPtrArray *pArrFld, int nType = 0);
	//计算最值，众数及中位数
	void CalcOther(CPtrArray *pArrFld, int nColIndex);
	//计算最值，众数及中位数
	void CalcOther(CDoubleVector &v, int nColIndex);
	//释放内存
	virtual void Free(CPtrArray *par);//add by xds 20060118
public:
	int     m_nCurRows;    //总行数
	__int64 m_n64Weight;   //总的权重或行数
	CPtrArray m_ValConfArray;
	TIntStruct m_tFrequency; //表示选中的字段
};

#endif // !defined(AFX_STATIMETHOD_H__A9465ACD_92CE_4A69_968A_D5782972CB5D__INCLUDED_)
