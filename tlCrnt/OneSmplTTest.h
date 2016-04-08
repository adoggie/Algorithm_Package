// OneSmplTTest.h: interface for the COneSmplTTest class.
//                        ��������������
//                    ������ �������ľ�ֵ��������������
//                   ��ע�� ������ 2005��10��21��
//									����ׯ		2006/03/21
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ONESMPLTTEST_H__6CB2C14D_73BD_466D_8BB9_543630921336__INCLUDED_)
#define AFX_ONESMPLTTEST_H__6CB2C14D_73BD_466D_8BB9_543630921336__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class _declspec(dllexport) COneSmplTTest  
{
public:
	COneSmplTTest();
	virtual ~COneSmplTTest();

	TIntStruct m_tVarInfo;				//��������ѡ����Ϣ
	double m_dValue;					//����ֵ��
	double m_dConfidence;					//���Ŷ�

	struct OneSampleResult
	{
		double m_TestStatistics;//����ͳ������ֵ
		int  m_df;//���ɶȣ�����еĻ���
		int  m_df2;//���ɶ�2������еĻ���
		bool m_bSingle; //0��˫�ߣ�1������
		double  m_P;//pֵ
		CTString  m_strName;//��������
	};
	CTArray<OneSampleResult,OneSampleResult> m_OneSampleResult;
private:
	//********************************
	//�������
	bool m_bNE ;// H1 : u �� u0
	bool m_bGT ;// H1 : u �� u0
	bool m_bLT ;// H1 : u �� u0
	bool m_bEquation; //���̷����������������
	
	//���ݷ����Ƿ���֪������
	void CreateTestTable(int index);
	void CreateChiTestTable(int index);
	void CreatePropotionTestTable(int index);
	//��������
	int m_nVarType;//0:����δ֪(t����)  1:������֪ (U����)
	int m_nCheckType;//������ͣ����ޣ����ߵȡ�
	int m_nIsSTD;//0:����  1:��׼��
	double m_dErrValue; //������߱�׼���ֵ
	
	bool m_bNormalTest;//��������̬�Լ���
	bool m_bHistogram;  //ֱ��ͼ
	bool m_bNormalLine; // ��̬����ͼ
	int  m_nDivi;       // ������
	//********************************/
private:
	CDataInterface * m_pDataInterface;//���ݽӿ�ָ��
//	CWnd           * m_pWnd;			//�������ָ��
	CResult		   * m_pResult;			//��������
	CRsltElementText *m_pRsltElementText;//������Ϣ��¼
	bool m_bWeight;						//�Ƿ�ָ����Ȩ��
	int m_nWeight;						//Ȩ���е��±�
	CTArray<int,int> m_RowArray;			//��ɾ�����Ϸ��м�¼
	int m_nFieldIndex	;				//��ǰ�ֶμ�¼

	//����ͳ�Ʊ�
	CTStringArray m_NameStrAr;			//������
	CTStringArray m_FrequencyStrAr;		//Ƶ��
	CTArray<double,double> m_MeanAr;		//��ֵ
	CTArray<double,double> m_StdDAr;		//��׼���
	CTArray<double,double> m_StdEMAr;	//��ֵ��׼��

	//���ڼ����
	CTArray<double,double> m_tAr;		//t
	CTArray<double,double> m_uAr;		//uͳ����
	CTStringArray m_dfStrAr;				//���ɶ�
	CTArray<double,double> m_PAr;		//������
	CTArray<double,double> m_DFAr;		//����
	CTArray<double,double> m_LowerAr;	//����
	CTArray<double,double> m_UpperAr;	//����

	CTArray<double,double> m_ChiSquareAr;	//�����������



	CPtrArray m_ErrorAry;//�в�
public:	
	CResult * COneSmplTTest::OnOneSmplTTest(CDataInterface *pDataInterface, string VO);//������Tjiantyan�ӿں���
	//1�������ĵ���������
	CResult* COneSmplTTest::OnOneSmplPropotionTest(CDataInterface *pDataInterface, string VO);
	//2������ĵ���������
	CResult* COneSmplTTest::OnOneSmplVarianceTest(CDataInterface *pDataInterface, string VO);
	//��Ӽ����������ࡣ
	double GetUStat(double U0,double Xmean,double sigma,int W); //������֪��ʱ�򣬻�ȡUͳ����

	bool Test(CTArray<double,double> &Data,//����
							  CTArray<double,double> &Weight,//Ȩ��
							 CTString strName,int method =0);//�ֶ��� T���飺method =0���������飺method =1;��������:method =2 
private:
	BOOL GetValueFromVoT(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoPro(CDataInterface *pDataInterface, string VO);
	BOOL GetValueFromVoVar(CDataInterface *pDataInterface, string VO);
	void FreeMembers();//ɾ���ַ�������
	void CreateResult(int method=0);//��ʽ�����T���飺method =0���������飺method =1;��������:method =2
	void CreateCharts();//���ͼ��
};

#endif // !defined(AFX_ONESMPLTTEST_H__6CB2C14D_73BD_466D_8BB9_543630921336__INCLUDED_)
