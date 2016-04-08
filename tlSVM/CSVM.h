// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSVM_3DD99A250127_INCLUDED
#define _INC_CSVM_3DD99A250127_INCLUDED

#define  KERNFUNC_GAUSS			1 //高斯核函数
#define  KERNFUNC_MULTINORMINAL	2 //多项式核函数
#define  ROLE_TRAIN				1
#define  ROLE_PREDICT			2
#define  DLLEXPORT	__declspec(dllexport)

//add by liytao 20030122
// to store real value of target fieldc
#include "SVMThread.h"

class CSVMVariant {
	public:
	CTString strVal;
	double dbVal;
	int nVal;
	bool bVal;
};
class RealY{
public:
	RealY(){
		m_Vars.RemoveAll();
		m_nCount=0;
	}
	RealY(int cType){
		m_Vars.RemoveAll();
		m_cType=cType;
		m_nCount=0;
	}
	void Empty(){
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			delete pV;
		}
		m_Vars.RemoveAll();
		m_cType=0x00;

		m_nCount=0;
	}
	CTString GetStrVal(int nPredictY){
		CTString strVal;
		int nIndex=-1;
		if ( nPredictY==1)
			nIndex=0;
		if ( nPredictY==-1)
			nIndex=1;
		if ( nIndex<0||nIndex>=m_nCount)
			return _T("");

		switch(m_cType)
		{
			case fDate :
			case fString :
				strVal=((CSVMVariant*)(m_Vars[nIndex]))->strVal;
				break;
			case fInt :
				strVal.Format("%d",((CSVMVariant*)m_Vars[nIndex])->nVal);
				break;
			case fBoolean :
				if (nIndex==0)
					strVal=_T("1");
				else
					strVal=_T("0");
				break;
			case fBlob :
				break;
			case fCurrency :
			case fDouble:

				strVal.Format("%.2f",((CSVMVariant*)m_Vars[nIndex])->dbVal);
				break;
		}
		return strVal;

	}
	void Add(CSVMVariant *pVar){
		m_Vars.Add(pVar);
		m_nCount++;
	}
	void Add(CTString strVar){
		CSVMVariant *pVar=new CSVMVariant ;
		pVar->strVal=strVar;
		m_Vars.Add(pVar);
		m_nCount++;
	}
	void Add(int nVar){
		CSVMVariant *pVar=new CSVMVariant ;
		pVar->nVal=nVar;
		m_Vars.Add(pVar);
		m_nCount++;
	}
	void Add(double dbVar){
		CSVMVariant *pVar=new CSVMVariant ;
		pVar->dbVal=dbVar;
		m_Vars.Add(pVar);
		m_nCount++;
	}
	void Add(bool bVar){
		CSVMVariant *pVar=new CSVMVariant ;
		pVar->bVal=bVar;
		m_Vars.Add(pVar);
		m_nCount++;
	}
	int Find (int nType,CSVMVariant *pVar){
		if( pVar==NULL )
			return -1;
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			switch (nType)
			{
			case fString:
				if ( pVar->strVal==pV->strVal)
					return i;
				break;
			case fInt:
				if ( pVar->nVal==pV->nVal)
					return i;
				break;	
			case fDate:
			case fCurrency:
			case fDouble:
				if ( fabs(pVar->dbVal-pV->dbVal)<0.00001)
					return i;
			case fBoolean:
				if (pVar->bVal==pV->bVal)
					return i;
			case fBlob:
			default:;
			}
		}
		return -1;

	}
	int Find (CTString strVal){
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			if ( strVal==pV->strVal)
					return i;
		}
		return -1;

	}
	int Find (int nVal){
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			if ( nVal==pV->nVal)
					return i;
		}
		return -1;

	}
	int Find (double dbVal){
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			if ( fabs(dbVal-pV->dbVal)<0.00001)
					return i;
		}
		return -1;

	}
	int Find (bool bVal){
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			if ( (bVal&&pV->bVal)||(!bVal&&!pV->bVal))
					return i;
		}
		return -1;

	}
	~RealY(){
		for (int i=0;i<m_nCount;i++)
		{
			CSVMVariant *pV=(CSVMVariant* )m_Vars[i];
			if (pV)
				delete pV;
		}
		m_Vars.RemoveAll();
	}
public:
	CPtrArray m_Vars;
	char m_cType;
	int m_nCount;
};
//add end
#define  MAX_DATASIZE				1000000

class DLLEXPORT CSVM 
{
public:
	int m_SVNum;
	RealY m_RealYs;
	VarIndexs m_TrainVarIndex;
	VarIndexs m_PredictVarIndex; //add by liytao 20030124 
	int m_nMissingRecNum;	//add by liytao 20021230
	int* m_pRecIndexs;
	bool* m_pbCheck;

//	CDoubleVector m_dbVj;
//	CDoubleVector m_dbVi;

private:
	double m_tolerance;
	double m_b;
	double m_dbCorrectRate;
	double m_dbCeilalpha;
	double m_equalprecision;
	double m_eps;
	int m_nOrder;
	int m_nKernFuncKind;
	int m_twosigmasquared;
	int m_nEndSupporti; 
	bool m_bIsLinear;
// 	bool	m_bModeSet;
// 	CDoubleVector m_weight;
	
	int m_nRowNum;
	int m_nColNum;

	CDoubleVector pVec;
	CIntVector pY;
	CDoubleVector pAlpha;
	CDoubleVector pWeight;

	CDataAccess* m_pDataSource;
	CDataAccess* m_pPredictAccess;
	
	CDataAccess* m_pAlphaAccess;
	
public:
	void SetPrgWnd(/*CWnd *pPrgWnd,UINT prgMsg*/);
	bool GetCheck();
	void SetStopCheck(bool *bCheck);
	double GetCorrectRate();
	void SetCorrectRate(double dbVal);
	int GetPredictY(int nIndex);
	void SetPredict(CDataAccess *pAccess);
	CDataAccess* GetSourceAccess();
	void SetSourceAccess(CDataAccess *pDataAccess);
	double GetEps();
	CTString GetKernFunc();
	void SetStoreMode(bool  bMemDB);
	double DotWeight(int nIndex);
	double DotProduct(int nIndexi,int nIndexj);
	double DotProduct(double *pVi, double *pVj);

	double GetTolerance();
	double GetCeilAlpha();
	bool IsLinear();
	double GetB();
	double GetWeight(int nIndex);
	void SetAlphaTableName(CTString strTName);
	void SetParam(int nLinearMethod,int nKernFuncKind,int nSigmasquared,int nOrder,double dbCeilAlpha,double dbTolerance,double dbEps);
	int GetRowNum();
	int GetColNum();
	bool EmptyData();
	double GetAlpha(int nIndex);
	void SetAlpha(int nIndex, double dbValue);
	double GetError(int nIndex);
	void	SetError(int nIndex,double dbValue);
	CDoubleVector GetVector(int nIndex);
	void GetVector(int nIndex, CDoubleVector& dbVector);
	int CSVM::GetY(int nIndex);
	CSVM();
	CSVM(CDataAccess *pSourceAccess);
	~CSVM();
	CTString m_strAlphaTableName;
	bool InitData();
	//##ModelId=3DD99DBE028A
	int Examine(int nIndex);

	//##ModelId=3DD99EC803A4
	int TakeStep(int nIndexi, int nIndexj);
	void ComputeLH(int nIndexi,int nIndexj,double* pLVal,double *pHVal);
	//##ModelId=3DD99F2B01F8
	double SMOCalc();
	
	double Learn(int nIndexi);
	//##ModelId=3DD99FAF01E4
	double Evaluate();
	bool EvaluateVector(int nIndex);
	bool EvaluateVector(int nIndex,double *pdbRs);
	int FindNextSVectorIndex(int nCur);
	double Kernel(int nIndexi, int nIndexj);
	double Kernel(double *pVi, double *pVj);
	double Kernel(CDoubleVector vi, CDoubleVector vj);
// 	void   SetTrainDataParam(TSVMTrainVO *pVO);
	//模型
	virtual bool Export(TCLFile &theFile);
	virtual bool Import(TCLFile &theFile);
};

#endif /* _INC_CSVM_3DD99A250127_INCLUDED */

