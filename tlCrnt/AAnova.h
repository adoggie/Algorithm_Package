// AAnova.h: interface for the CAAnova class.
//                �����ط������
//          ��ע�� ������ 2005��10��21��
//			�޸ģ�����ׯ	2006/03/22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AANOVA_H__BB256B39_09F2_4314_B428_8F7AE456473F__INCLUDED_)
#define AFX_AANOVA_H__BB256B39_09F2_4314_B428_8F7AE456473F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "tlcrnt.h"
class AFX_EXT_CLASS CAAnova //: public CStatMethod  
{
public:
	CAAnova();
	~CAAnova();
	//һԪ��������������������������ṹ������ͬ���û�ʹ�á�
	CResult* OnAnova(CDataInterface *pDataInterface, string VO);
//			struct OneWayANOVAResult 
//			{
//				double m_SquareSum[3];//ƽ���ͣ�����Ϊ ��䡢���ڡ�����
//				int    m_Df[3];		  //���ɶȣ�����Ϊ ��䡢���ڡ�����
//				CTString m_strVarName;//��������
//			};
//
//			//��������ṹ���ṩ��������ݽӿڣ�ʵ�ֹ�����������⡣
//			CTArray<OneWayANOVAResult,OneWayANOVAResult> m_OneWayANOVAResult;

private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	int OneAnova(CDoubleVector& y, CIntVector& x, CDoubleVector& right);
	void OnAAnova(CDataInterface *pDataInterface);
	void OnDescriptive_Plot(CDataInterface* pDataInterface,CResult* &pResult);
	//���ɽ��
	void CreateResult(CDataInterface *pDataInterface,CRsltVariable* &pVariable,int nIndex,CTString strFactName);
	//���ɽ����
	CRsltElementTable* CreateDescriptiveTable(CTString strName,CTString strFactName,int idx);
	//������ͳ��ͼ
	CRsltElementTable* CreateCompareMeansTable(CTString strName,CTString strFactName,int idx);//��������Ƚϵı��
	
	CRsltElementChart* CreateMeadPlot(CTString strDepName,CTString strFactName,int idx);
	//�в�ͼ
	CRsltElementChart* CreateResiduPlot(CTString strDepName,CTString strFactName,int idx);
	CRsltElementChart* CreateRPPlot(CTString strDepName,CTString strFactName,int idx);
	//�в���̬����ͼ
	CRsltElementChart*  CreateRNormlot(CTString strDepName,CTString strFactName,int idx);
	//ֱ��ͼ
	CRsltElementChart*  CreateHistogram(CTString strDepName,CTString strFactName,int idx);
	void GetData(CDataInterface* pDataInterface);

	BOOL       m_bDescriptive;
    BOOL       m_bMeanPlot;  //�Ƿ񻭾�ֵͼ
	int        m_nFactorIndex;//���ر���
	int        m_nRightIndex;//Ȩֵ����
	int        m_nMissingSelect;
	TIntStruct m_tVarInfo;//�����ѡ�еı���
	CRsltElementText *m_pRsltElementText;
	/********************************************************
	 *	��ӷ���ͬһ�Լ����ͳ�����ļ���
	 ���� �������� 
	 ʱ��2005��5��19�� 
	 *********************************************************/
	//������� Y Ϊ�۲����������������X Ϊ�������������X��ֵ��Y���з��顣
	//������Ǹ��� ȡ������X == Xval���±��Ӧ��Yֵ����Ϊһ��������
	//������������ıȽϡ�������������Ĺ۲�������ȣ��������ٶ��ô���1��
	//С�ڻ��ߵ���1������û�з���ĸ�����ӱȽϵĶ������޳���ȥ��
	//������X���Ҳ���ָ����Xvalֵ�ĸ�������û���ҵ������� 0 ��ʾʧ�ܡ�
	//ͬʱ���±��ŵ�����Result�С�
	int FindIndex(CDoubleVector Y ,CIntVector X , double Xval,CDoubleVector& Result);
	//�����±�������ȡ������.
	CDoubleVector GetSubVect(CDoubleVector Y ,CDoubleVector IndexVect);
	CDoubleVector GetIndexVal(CIntVector X);//��ȡ�����в�ͬ��ֵ��ɵ��������ҷ���
	//��ȡ��ֵ��Levene test��������Ҫ�õ���
	//index = 0 ����Y�� ����ƽ��ֵ
	//index = 1 ����Y�� ��λ��
	//index = 2 ����Y�� ���½�βpercent/2%��������ľ�ֵ
	double  GetpercentVal(CDoubleVector Y ,double percent);	
	double  GetMean(CDoubleVector Y, int index , double percent);
	double  TrimMean(CDoubleVector Y, double percent);//��ȡ��β�ľ�ֵ
	double  GetBStat(CDoubleVector Y,CIntVector X,int &f);//����BartLett test ͳ����
	double  GetBStat(CDoubleVector* DataSet,int Count, int&f);//����BartLett test ͳ����
	//����Levene test ͳ������Ĭ��ʹ����λ��������β�Ļ���Ĭ��Ϊ10%
	double  GetLStat(CDoubleVector Y,CIntVector X, int &f,int index = 1 , double percent  = 10);  
	double  GetLStat(CDoubleVector* DataSet , int Count,int& f, int index, double percent);
	CDoubleMatrix GetMeansTableStat(CDoubleVector* DataSet,int count);
	double  medium(CDoubleVector Y); //��ȡ��λ��
	//��ȡ�в����ݣ�ͬʱ������Ӧ��Ԥ��ֵ
	CDoubleVector GetResidu(CDoubleVector Y , CIntVector X,CDoubleVector& Residual,int index );//����в� 0-����ģ�Ͳв1-ЧӦģ�Ͳв
	//�������ݣ������ݴ���һ���������У��Ա�������ʹ�á����ص��ǵõ��ķ������ݵ�������
	int GetCleanData(CDoubleVector Y, CIntVector X,CDoubleVector*& ValidData,CDoubleVector& ValidIndex);
	//���ݶ����򣬼�ͬ����
	void SortData(CDoubleVector Y, CIntVector X, CDoubleVector& SortedY, CIntVector& SortedX,int index);
	CDoubleVector GetNormMedians(int n);//������̬������λ�� �����㷨�μ�1.3.22
	CDoubleVector GetNormMedians(double mu,double Std,int n);//������̬������λ�� mu, std ,3.22
	double GetFStat(CDoubleVector X1,CDoubleVector X2 );//����Fͳ�����������������������ķ����Ƿ����
	double GetTStat(CDoubleVector X1,CDoubleVector X2,BOOL Equel = true );//����Tͳ�������Ƚ����������ľ�ֵ��bEqual = 0���������
	double GetLeveneStat(CDoubleVector X1,CDoubleVector X2);//��������������leveneͳ����
	int GetStatList(CDoubleVector &Vect_Ret, CDoubleVector X1,CDoubleVector X2,BOOL bEqual);
	void	Testify(CDoubleVector& Y, CIntVector& X);//���Լ������ȷ��
	CPtrArray       m_arrayResult;
   	CTLTable* briefrTable;
	CDataInterface * m_pDataInterface;

	//�洢ȱʧֵ����������.	
	CPtrArray       m_ptrArrayX;
	CPtrArray       m_ptrArrayY;
	CPtrArray       m_ptrArrayRight;
	//�洢ѡ�����ñ�����ֵ
	//����ͬһ�Լ���
	BOOL m_bBalaiteTest;//�������ؼ���
	BOOL m_bLeveneTest;//�ַ������ leveneTest
	double m_dTailPer;//��β�ٷ���
	int m_nArithmetic;//0:����ƽ��ֵ	1:��λ��  2:��β��ֵ
	
	//ͼ�����
	BOOL m_bResidualScatter;//�в�ɢ��ͼ
	BOOL m_bResidualTable;//�в���̬����ͼ
	BOOL m_bHistogram;    //ֱ��ͼ
	BOOL m_bVars;	//��Ƿ����Ƿ���ڻ�����Ч��>0��
	
	//��ֵ�����Ƚ�
	BOOL	m_bVarianceEQ;//�������
	BOOL	m_bVarianceNE;//������
	BOOL	m_bMeansCompare;////��ֵ�����Ƚ�
	//�������Լ���
	int		m_nVarianceCheck;//0��F���� 1��Leveneͳ����
	/**********************************************************
	 *	����ͬһ�Լ��鹦�ܵ���ӽ��������������ȷ
	 **********************************************************/
};   

#endif // !defined(AFX_AANOVA_H__BB256B39_09F2_4314_B428_8F7AE456473F__INCLUDED_)
