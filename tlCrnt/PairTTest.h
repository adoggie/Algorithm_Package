// PairTTest.h: interface for the CPairTTest class.
//                    ���������T����
//					����ׯ    2006/03/21
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAIRTTEST_H__9FF9DF3D_2C57_4D65_BD07_880B2F3A82A9__INCLUDED_)
#define AFX_PAIRTTEST_H__9FF9DF3D_2C57_4D65_BD07_880B2F3A82A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CPairTTest  
{
public:
	CPairTTest();
	virtual ~CPairTTest();
	
private:
	CPtrArray m_PtrArrayIndex;//��Ա�����Ϣ
	TIntStruct m_tVarInfo;
	double m_dConfidence;		//���Ŷ�
	CDataInterface * m_pDataInterface;//���ݽӿ�ָ��
//	CWnd           * m_pWnd;		//�������ָ��
	CResult		   * m_pResult;		//��������
	CRsltElementText *m_pRsltElementText;//������Ϣ��¼
	BOOL m_bWeight;					//�Ƿ�ָ����Ȩ��
	int m_nWeight;					//Ȩ���±�
	CTArray<int,int> m_RowArray;		//��ɾ�����Ϸ��м�¼
	int m_nPairs;

	bool m_bLT;
	bool m_bNEQ;
	bool m_bGT;

	bool m_bNormalTest;//�Ƿ������������̬�Լ���
	bool m_bHistogram; //ֱ��ͼ
	int  m_nDivi;      //������
	bool m_bNormalLine; //��̬�ֲ�����

	//ͳ�Ʊ�����������±����洢
	CTStringArray m_NameAr;			//������
	CTArray<double,double> m_MeanSAr;			//��ֵ
	CTStringArray m_FrequencySAr;	//Ƶ��
	CTArray<double,double> m_StdDSAr;			//��׼���
	CTArray<double,double> m_StdEMSAr;		//��ֵ��׼��

	//����Ա�
	CTStringArray m_PairNameAr;		//������(���)
	CTStringArray m_FrequencyPAr;	//Ƶ��
	CTStringArray m_CorrAr;			//�����
	CTStringArray m_P2Ar;			//������

	//�����
	CTArray<double,double> m_MeanPAr;			//��ֵ
	CTArray<double,double> m_StdDPAr;			//��׼���
	CTArray<double,double> m_StdEMPAr;			//��ֵ��׼��
	CTArray<double,double> m_LowerAr;			//����
	CTArray<double,double> m_UpperAr;			//����
	CTArray<double,double> m_tAr;				//tֵ

	
	CPtrArray m_ErrorAry;//�в�

	CTStringArray m_dfStrAr;				//���ɶ�
	CTArray<double,double> m_PAr;				//������(˫β)
	CTArray<double,double> m_PArGT;			//�������Ϊ����
	CTArray<double,double> m_PArLT;			//�������ΪС��
public:
	CResult *CPairTTest::OnPairTTest(CDataInterface *pDataInterface, string VO);

protected:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	void CPairTTest::GetTwoRelatedData(CTArray<double,double> &Data1,
									  CTArray<double,double> &Data2,
									  CTArray<double,double> &Weight,
									  TDataSet *pWeightArray,
									  int nCol1,int nCol2);
	void CPairTTest::FreeAllMemember();
	BOOL CPairTTest::PairTTest(CTArray<double,double> &Data1,//��Ա���1����
							   CTArray<double,double> &Data2,//��Ա���2����
							   CTArray<double,double> &Weight,//��Ӧ��Ȩ��
							   int column1,int column2,//��Ӧ��������
							   CTString strName);//��Ա�����
	void CPairTTest::CreateResult();


};

#endif // !defined(AFX_PAIRTTEST_H__9FF9DF3D_2C57_4D65_BD07_880B2F3A82A9__INCLUDED_)
