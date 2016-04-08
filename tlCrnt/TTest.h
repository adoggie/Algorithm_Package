// TTest.h: interface for the CTTest class.
//               ����˫�����ļ������
//                  ��ע�������� 2005��10��20��
//					�޸ģ�����ׯ 2006/03/28
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TTEST_H__FA5EF87B_8FC9_4BC5_97A2_AEE1BF3425A8__INCLUDED_)
#define AFX_TTEST_H__FA5EF87B_8FC9_4BC5_97A2_AEE1BF3425A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "tlcrnt.h"
class AFX_EXT_CLASS CTTest  
{
public:
	CTTest();
	virtual ~CTTest();
	//��������T����
	CResult *CTTest::OnTTest(CDataInterface *pDataInterface, string VO);
	//˫�����������
	CResult *CTTest::OnDoubleVarTest(CDataInterface *pDataInterface, string VO);
	//˫������������
	CResult *CTTest::OnDoublePropotionTest(CDataInterface *pDataInterface, string VO);
    //*************** ѡ�� ****************
	//������μ���
private:
	//CTString  m_strIndex1;
	BOOL	m_bBartlett; //Bartlett����
	BOOL	m_bF;//F����
	BOOL	m_bLevene;//Levene����
	BOOL	m_bGroup;//���飬�����
	//�������
	BOOL	m_bGT;// H1 : u1 �� u2 �� ��
	BOOL	m_bLT;// H1 : u1 �� u2 �� ��
	BOOL	m_bNE;// H1 : u1 �� u2 �� ��
    BOOL    m_bPooledData ;//�Ƿ�ʹ�û�ϵ��������Ʊ���p
	double	m_dValue;//��
    
	int		m_nCheckType;//0:����δ֪(t����)  1:������֪ (U����) ����������ޡ�����
	
	int		m_nIsStdType;//0:����   1:��׼��
	double	m_dSigma1;//   
	double	m_dSigma2;//����ֵ 
	
	int		m_nIsVarNE;//0:������� 1:�����
	//*************************************

	TIntStruct m_tVarInfo;			//��������ѡ����Ϣ
	int m_nGroupIndex;				//���������Ϣ
	double m_dValue1;				//����ֵ1
	double m_dValue2;				//����ֵ2
	int m_nGroupMethod;				//���鷽ʽ��0��ʾ���鷽ʽ��1��ʾ�ֽ�㷽ʽ����ο�����
	double m_dBoundary;				//�����ʽΪ1ʱ���ֽ���ֵ��
	double m_dConfidence;

	CDataInterface * m_pDataInterface;//���ݽӿ�ָ��
	CResult		   * m_pResult;		//��������
	CRsltElementText *m_pRsltElementText;//������Ϣ��¼
	BOOL m_bWeight;					//�Ƿ�ָ����Ȩ��
	int m_nWeight;					//Ȩ���±�
	CTArray<int,int> m_RowArray;		//��ɾ�����Ϸ��м�¼
	int m_nFieldIndex;				 //��ǰ�ֶ�����

	//����ͳ�����ݱ����ݴ洢
	CTStringArray m_NameStrAr;		//������
	
	CTStringArray m_GroupStrAr;		//�����־
	CTStringArray m_FrequencyStrAr;	//Ƶ��
	CTStringArray m_MeanStrAr;		//��ֵ
	CTStringArray m_StdDStrAr;		//��׼���
	CTStringArray m_StdMStrAr;		//��ֵ��׼��

	//������ͳ�����ݱ����ݵĴ洢
	//���龡����Ҫʹ���ַ�����������������ʧ���ȵġ�
	CTStringArray m_CommentStrAr;	//��ڶ���˵��
	
	CTStringArray m_LStrAr;	//Leveneֵ
	CTStringArray m_LdfStrAr;	//Leveneֵ ���ɶ�
	CTStringArray m_PStrAr;	//������

	CTStringArray m_FStrAr;	//Fֵ	
	CTStringArray m_FdfStrAr;//Fֵ���ɶ�
	CTStringArray m_FPStrAr;//������

	CTStringArray m_BdfStrAr; //bartlett ���ɶ�
	CTStringArray m_BStrAr;	//Bartlettֵ
	CTStringArray m_BPStrAr;	//Bartlettֵ������
	 
	CTStringArray m_ZStrAr;	//U����ͳ����ֵ
	CTStringArray m_EtStrAr;	//T����ͳ����ֵ//�������ʱ
	CTStringArray m_EtdefStrAr;	//�������ʱ���ɶ�

	CTStringArray m_UtStrAr;	//T����ͳ����ֵ//�����
	CTStringArray m_UtdefStrAr;//�������ʱ���ɶ�
	
	
	CTStringArray m_tStrAr;			//tֵ
	CTStringArray m_dfStrAr;			//���ɶ�
	CTStringArray m_P2StrAr;			//�����ԣ�˫β��
	CTStringArray m_MeanDStrAr;		//��ֵ��
	CTStringArray m_StdEDStrAr;		//��ı�׼��
	CTStringArray m_LowerStrAr;		//��������
	CTStringArray m_UpperStrAr;		//��������
	CTString m_strPair;
protected:
	BOOL GetValueFromVoT(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoVar(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoPro(CDataInterface *pDataInterface, string VO);
	double GetVar(CTArray<double,double> &Data);
	void FreeMemebers();
	void CreatHomoTestTable(int index);//���������μ���
	void CreatHypoTestTable(int index);//�����������
	void CreatVarTestTable(int index);//�������ļ������
	void CreatPropotionTestTable(int index);//��������ļ������
	void CreatStatTable(int index);//�������Ϊ���ʱ���ͳ�Ʊ�
	BOOL TestMain(CTArray<double,double> &Data1,		//��1����
						  CTArray<double,double> &Data2,		//��2����
						  CTArray<double,double> &Weight1,	//��1���ݶ�Ӧ��Ȩ��
						  CTArray<double,double> &Weight2,	//��2���ݶ�Ӧ��Ȩ��
						  int nIndex,                       //��ǰ�����ֶε��к�
						  int method = 0 );                 //0-T���飬1- ������飬2-��������						

void GroupIntoTwo(CTArray<double,double> &ar1,				//��1����
									 CTArray<double,double> &ar2,		//��2����
									 CTArray<double,double> &Weight1,	//��1���ݶ�Ӧ��Ȩ��
									 CTArray<double,double> &Weight2,	//��2���ݶ�Ӧ��Ȩ��
									 double dValue1,double dValue2,		//����ֵ1������ֵ2
									 int nVarCol,						//��ǰ��Ӧ�ķ����к�
									 TDataSet *pWeightArray,			//�洢��Ȩ������
									 TDataSet *pGroupArray);			//�洢�˷����������

void CreateResult(int method = 0); //0-T���飬1- ������飬2-��������						

};

#endif // !defined(AFX_TTEST_H__FA5EF87B_8FC9_4BC5_97A2_AEE1BF3425A8__INCLUDED_)
