// StatiMethod.h: interface for the CStatiMethod class.
//
//		CDescriptive ��ֵ
//			CFrequency Ƶ��
//			CExplore ����
//				�Ķ�ɽ		2006/03/29
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
	void ParseStatMethod(AnaWord VOstr);//�����õ�ͳ�Ʒ���
	void CalcParam(CDoubleVector &v, CDoubleVector &w, BOOL bCalcMaxMin = TRUE);
	void CalcFldInfo(double fValue, double fVarValue, CPtrArray *pArrFld, int nSize);//��������
	void CalcFldInfo(double fValue, double fVarValue, double fWeight, CPtrArray *pArrFld, int nSize);//��������
	void CalcFldInfo(double fValue, double fWeight, CPtrArray *pArrFld);//��������
	void CalcRangeInfo(double fValue, double fWeight, CPtrArray *pArrFld);//��������
	virtual void Release(CPtrArray *pArrFld); //�ͷ��ڴ�
	BOOL CalcFrequency(CDoubleVector &v, CDoubleVector &w, int nColIndex, CPtrArray &arrFld, int nDispMax, BOOL bCalc);//����Ƶ������Ȩ�أ�
	BOOL CalcFrequency(CDoubleVector &v, CDoubleVector &w, CIntVector &vIndex, int nColIndex, CPtrArray &arrFld, int nDispMax);
	CTString GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue);//�õ���ǩ
	virtual void Free(CPtrArray * par, BOOL bLayer = FALSE);//modify by xds 20060106.for delete...

protected:
	BOOL    m_bCalcVar;    //�Ƿ��ܼ��㷽���
	BOOL    m_bWeight;     //�Ƿ��Ȩ��
	BOOL    m_bOutApex;    //�Ƿ������ֵ
	BOOL    m_bOutApexE;   //�Ƿ������ֵ���
	BOOL    m_bOutPartial; //�Ƿ����ƫֵ
	BOOL    m_bOutPartialE;//�Ƿ����ƫֵ���
	int     m_nWeight;     //Ȩ������
	CResult        *m_pResult;       //�����
	CDataInterface *m_pDataInterface;//���ݽӿ�
	TIntStruct m_tStatMethod;                 //��ʾѡ������Щ����
	TIDName  m_tStatMethodS[macDESCRIPMETHOD];//�������������з���
	struct CFreOut		   //������������ṹ
	{
	public:
		double  m_fMin;    //��Сֵ
		double  m_fMax;    //���ֵ
		double  m_fSum;    //��ЧȨ�غͻ���Ч��¼��
		double  m_fSmy;    //�ܺ�
		double  m_fMid;    //��λ��
		double  m_fMany;   //��λ��
		double m_fMean;    //ƽ��ֵ
		double m_fMe;      //��ֵ��׼��
		double m_fS;       //��׼��
		double m_fS2;      //����
		double m_fApex;    //��ֵ
		double m_fApexE;   //��ֵ���
		double m_fPartial; //ƫֵ
		double m_fPartialE;//ƫֵ���
		double m_fTrimMean;//��β��ֵ
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
struct TIndStep //��Ҫ�������������������->�����ݲ�����
{
public:
	TIndStep()
	{
		nStartInd = 0;
		nStep = 0;
		nType = 2;
		arrData.RemoveAll();
	};
	int nStartInd; //��ʼ������
	int nStep;     //��������ʼ�Ĳ���
	int nType;     //��ʾ���ͣ�0-���֣�1-������2-�հ�
	CPtrArray arrData;//����
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
	int          m_nColIndex;     //��ǰ�е�����
	int          m_nTotalDisp;    //��ʾ�ı�������
	int          m_nCurRows;      //���ĵ�ǰ����
	int          m_nCounter;      //��ǰ����������,2*3*5
	CIntVector   m_FirVecInd;     //��ŵ�һ������
	CDWordArray  m_nArrInd;       //������
	CTStringArray m_szArrLayerName;//�������Ų����Ϣ������:����1+����2+��ĩ
private:
	CPtrArray m_arrLayerConf;
	TIntStruct m_tVarSel;    //��ʾ�Ա���
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
	//�����������
	void OnExplore(); 
	//����ٷ�λ��.dataMatrix:���ݾ���,vecInd:����������
	void CalcPercent(CDoubleMatrix &dataMatrix, CIntVector &vecInd);
	//�����βƽ��ֵ.dataMatrix:���ݾ���,vecInd:����������
	BOOL CalcTrimMean(CDoubleMatrix &dataMatrix, CIntVector &vecInd);
	//����ƫֵ,��ֵ�����.dataMatrix:���ݾ���,vecInd:����������
	void CalcPartial_Apex(CDoubleMatrix &dataMatrix, CIntVector &vecInd);
	//��Ӻ�״ͼ.dataMatrix:���ݾ���,pVariable:�����,szFldName:������ֶ�����
	void AddBoxPlot(CDoubleMatrix &dataMatrix, CRsltVariable *pVariable, CTString szFldName);
	//��Ӿ�Ҷͼ.dataMatrix:���ݾ���,vecInd:����������,pVariable:�����,szFldName:������ֶ�����,szGroup:��������ʱ�ķ�������
	void AddStemPlot(CDoubleMatrix &dataMatrix, CIntVector &vecInd, CRsltVariable *pVariable, CTString szFldName, CTString szGroup = _T(""));
	//���������.dataMatrix:���ݾ���,vecInd:����������,pVariable:�����,szFldName:������ֶ�����
	void CreateResult(CDoubleMatrix &dataMatrix, CIntVector &vecInd, CRsltVariable *pVariable, CTString szFldName);
	//�����״ͼ������.dataMatrix:���ݾ���
	TPlotReco GetCoordinate(CDoubleMatrix &dataMatrix);
	//���㾥Ҷ�ֲ�.dataMatrix:���ݾ���,vecInd:����������,pStemLeaf:��Ž��ָ��,fStemLeaf:֦Ҷ�Ŀ��,����ֵ:��Ҷ��
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
/////// ���������Ŀ��Ʋ���
public:
	double  *pWeight;
	int     m_nCurRows;    //������
	__int64 m_n64Weight;   //�ܵ�Ȩ�ػ�����
	int     m_nConfidence; //���Ŷ�
	BOOL    m_bBoth;       //�Ƿ���ʾͳ��ͼ����
	BOOL    m_bStatistics; //�Ƿ���ʾͳ�Ʊ�
	BOOL    m_bPlots;      //�Ƿ����ʾͳ��ͼ
	BOOL    m_bPercentiles;//�Ƿ���ʾ�ٷ���
	BOOL    m_bMEstimators;//�Ƿ���ʾ��ֵ����
	BOOL    m_bDescriptive;//�Ƿ���ʾ����
	BOOL    m_bOutliers;   //�Ƿ���ʾ�����С�����ֵ
	double m_fPercentile[DISP_PERCENT][2];//�ٷ�λ��
	TIntStruct m_tDependent;//��ʾѡ�е��Ա����ֶ�
	TIntStruct m_tFactor;   //��ʾѡ�е�������ֶ�
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
	//�Ƿ񳬳�������ʾ��Χ.
	BOOL IsOverDisp(int nColIndex);
	//��ʾƵ��ͳ��.
	void DispFrequency();
	//��ǰ�����Ƿ���ڷ�Χ
	BOOL IsExistRange(int nColIndex);
	//��������
	void CreatePtrArray(CPtrArray &arrFld, int nColIndex);
	//��������ݣ�������
	void InsertTable(double fValueFrom, double fValueTo, double fFreValue, double fFreTotal , CTLTable* pTable, int nType = 0);
	//���������.dataMap:����ӳ���,��Ӧ��������,nColIndex:������,fMapValue:ӳ���ֵ,fFreValue:��ǰ��Ƶ�ʺ�,fFreTotal:�ܵ�Ƶ�ʺ�,pTable:�����ָ��,nType:��������,bValid:�Ƿ�Ϊ�Ϸ�������
	void InsertTable(CMapManager &dataMap, int nColIndex, double fMapValue, double fFreValue, double fFreTotal , CTLTable* pTable ,int nType ,BOOL bValid = TRUE);
	//����Ƶ�ʱ�.dataMatrix:���ݾ���,nColIndex:������,dataMap:����ӳ���,pArrFld:�����ֵ��Ȩ��,nType:��ʾ����
	void CreateFreqTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, int nColIndex, CPtrArray *pArrFld, int nType = 0);
	//������ֵ����������λ��
	void CalcOther(CPtrArray *pArrFld, int nColIndex);
	//������ֵ����������λ��
	void CalcOther(CDoubleVector &v, int nColIndex);
	//�ͷ��ڴ�
	virtual void Free(CPtrArray *par);//add by xds 20060118
public:
	int     m_nCurRows;    //������
	__int64 m_n64Weight;   //�ܵ�Ȩ�ػ�����
	CPtrArray m_ValConfArray;
	TIntStruct m_tFrequency; //��ʾѡ�е��ֶ�
};

#endif // !defined(AFX_STATIMETHOD_H__A9465ACD_92CE_4A69_968A_D5782972CB5D__INCLUDED_)
