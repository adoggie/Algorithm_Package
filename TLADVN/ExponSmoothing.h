// ExponSmoothing.h: interface for the CExponSmoothing class.
//						指数平滑模型
//                    功能：指数平滑模型的共同方法和属性
//					  标注：  余卫军
//					  时间：2005年10月20日
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPONSMOOTHING_H__E951CC7F_A07A_492F_A4CD_DE93790E01D9__INCLUDED_)
#define AFX_EXPONSMOOTHING_H__E951CC7F_A07A_492F_A4CD_DE93790E01D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConfTypes.h"
#include "../Public/TString.h"


class AFX_EXT_CLASS CExponSmoothing  
{
public:
	CResult* OnExponSmoothing(CDataInterface *pDataInterface, const string VO);
	CExponSmoothing();
	virtual ~CExponSmoothing();

	void InsertSSECols(int nSeason,int nTrend,CTLTable* pSSETable);
	void SetSSEItem(int nSeason,int nTrend,CTLTable* pSSETable,int nItem,SmoothSetting *pSetting,double dbSSE);
	double GetSSE(CDoubleVector ErrVec);
	double Mean(CDoubleVector Vec);
	int Mean(CDoubleVector Vec,int nPeriodicity,int nPNO,double* pdbSum);
	bool ExponSmooth(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,double dbalpha,double dbdelta,double dbgamma,double dbphi,CResult* pResult,double *pSSE);
	bool GridExponSmooth(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,CResult* pResult);
	void GridSearch(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,SmoothConf * palphaconf,SmoothConf * pdeltaconf,SmoothConf * pgammaconf,SmoothConf * pphiconf,CResult* &pResult);
	bool GetValueFromVO(CDataInterface *pDataInterface, string VO);

	CDataInterface* m_pDataInterface;
	bool m_bShowBestModels;//只显示10个最佳模型
	bool m_bShowPredict;//是否显示预测步数
	int m_nPredictNum;//预测步数
	int m_nSeason;//季节选择	//0-无季节变量 ，1－加法  ，2 －乘法
	int m_nTrend;//趋势选择		//0－无趋势；1－线性趋势；2－指数趋势 ；3－衰减趋势
	int m_nAlpha;//0-给定 ，1－网格搜寻
	int m_nDelta;//0-给定 ，1－网格搜寻
	int m_nGamma;//0-给定 ，1－网格搜寻
	int  m_nPhi;//0-给定 ，1－网格搜寻

	int m_nPeriodicity;
	double m_dbalpha;
	SmoothConf m_alphaconf;
	double m_dbdelta;
	SmoothConf m_deltaconf;
	double m_dbgamma;
	SmoothConf m_gammaconf;
	double m_dbphi;
	SmoothConf m_phiconf;
	TIntStruct	m_LstSelVar;//选入变量下标集
	CTString strDBName;
	CTStringArray m_FNames;

};

//add by liytao 20030410
#define NOSEASONAL    0 //无季节变量
#define ADDITIVE	1	//加法
#define MULTIPLY	2	//乘法

#define STRNOSEASONAL    _T("无季节变量")
#define STRADDITIVE		_T("加法")
#define STRMULTIPLY		_T("乘法")


#define NOTREND		0	//无趋势
#define LINEAR		1	//线性
#define EXPONENTIAL 2	//指数
#define DAMPED		3	//衰减

#define STRNOTREND		_T("无趋势")
#define STRLINEAR		_T("线性")
#define STREXPONENTIAL 	_T("指数")
#define STRDAMPED		_T("衰减")
 	
//add end



#endif // !defined(AFX_EXPONSMOOTHING_H__E951CC7F_A07A_492F_A4CD_DE93790E01D9__INCLUDED_)
