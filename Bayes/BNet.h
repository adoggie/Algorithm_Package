// BNet.h: interface for the CBNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BNET_H__9FF56EFD_AF1E_4A66_B6DA_55725937228D__INCLUDED_)
#define AFX_BNET_H__9FF56EFD_AF1E_4A66_B6DA_55725937228D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BNode.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"
#include <string>
using namespace std;

class CBayesVO
{
public:
	CTStringArray	strInputField;
	CTStringArray	strCauIndex;
	CBayesVO()
	{
	}
	bool Compile(string strVO)
	{
	//	��Ҷ˹VO����
	//	BAYESNET
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/MISSMETHOD DELRECORD
	//	/INDEX arglist


	//	/BAYESNET
	//	/VARINPUT arglist
	//	/INDEX ��ˮ ���� ���� ���� ���� ����
		
		AnaWord VOstr;
		VOstr.Import(strVO);
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputField);
		AnaSyntax::getVAR("/INDEX", VOstr, strCauIndex);
		return true;
	}
};


class CBayesOutput : public StructBase
{
public:
	CTStringArray	strInputField;
	CTStringArray	strCauIndex;
	CIntMatrix		mxCauseMatrix;
	CPtrArray		arValueLabel;
	CPtrArray		arProbility;
	CBayesOutput()
	{
	}
	virtual string Export();
	virtual bool   Import(string strModel);
};


class CBNet  
{
public:
	CBNet();
	CBNet(CDataInterface*);
	virtual ~CBNet();
	CBNet(int nn,char** namelist,CIntMatrix & ct);
	CBNet(CBNet& ns);

	CDataInterface * m_pDataInterface;
	CTStringArray	m_arFieldName;		//�ڵ���ֶ�����
	int				m_nNodeNum;	       //�ڵ����
	CTString		m_NetName;			//��������
	bool			m_NeedSave;		//��ű��������־��0Ϊ����Ҫ��1Ϊ��Ҫ
	CPtrList		m_NodePtrList;     //����б�
	CIntMatrix		m_CausTbl;	   //�ڵ��ϵ
	long             m_nSimNum;     //Monte Carloģ�⽫Ҫ��������������
	int             m_nGSimNum;    //ȱʧ�����е�Gibbs��������
	CIntMatrix	    m_DataMatrix;   //�������Ͼ����ж�Ӧ��¼���ж�Ӧ�ֶΣ�������
	CIntMatrix      m_InitMatrix;    //�趨�û������������ʼ����
	CIntVector      m_AimVector;     //�趨�û�����������Ŀ�����
	//CIntVector		m_OldAimVector;	//��������ǰ�Ĺ۲�ֵ����
	CIntMatrix		m_SimDataMatrix;  //MCģ����������
	CIntVector      m_ChangeTbl;      //���ձ�
	CIntVector      m_StateNum;       //���״̬����
	CDoubleVector	m_ProbVector;		//��������Ŀ�����ȡ��״̬�ĸ���
	bool			m_bErrorFlag;		//�����־
	CTString			m_sErrorInfo;		//������Ϣ
public:
	//���ݽ������ݳ�ʼ������
	void InitiateNet(CDataInterface *pDataInterface,CBayesVO*pBayesVO);

	//ȡ���ݲ���ʼ�����н��
	CIntMatrix  CBNet::GetData();

	//�����ݾ�����ͳ�ƽ���״̬����
	//CIntVector CBNet::CalculateStateNum();

	//�ӽڵ��б��л�ý���״̬����
	bool CBNet::GetStateNum();

	//ͳ�ƽ���״̬ȡֵ
	//void CBNet::GetState();

	//�����ڵ��ϵ��
	void CBNet::CreateCausTbl();
	
	CResult * CBNet::CreateResult();

	//�ڵ����������Լ�����ṹ�޻�ͼ�ļ�飬�õ�˳��仯���±���ձ�
	bool CBNet::Bayesnodessort();
	
	//�����µĽڵ����򣬱任�ڵ��ϵ���ڵ�״̬���������������Լ�һֱ������Ŀ������Ľڵ�˳��
	void CBNet::newCausTbl(CIntMatrix& CausTbl,CIntVector ChangeTbl,CIntVector& StateNum,CIntMatrix& DataMatrix);//,CIntMatrix& InitMatrix,CIntVector& AimVector);
	
	//ȱʧ���ݴ���
	CDoubleMatrix CBNet::DataMatrixSetCompletion(int NodeNum,int GSimNum,CIntMatrix CausTbl,CIntVector StateNum,CDoubleMatrix DataMatrix);
	
	//��������ṹ���ɽڵ�ĸ��ڵ����������
	void CBNet::SetParntArray();

	//��ʼ�����ڵ���������ʱ������ȷֲ�
	void CBNet::CPTInitiate();
	//��������ṹ���������ϼ���������������������ʣ���
	void CBNet::CPTCalculate();//,CIntMatrix DataMatrix,CIntVector StateNum,CIntMatrix CausTbl); 
	
	//����Monte Carloģ�����ģ����������Sim_Samples
	bool CBNet::BayesNetMCSim(/*CShowData*pDialogData*/);

	//���ݹ۲�ֵX�������������õ�Ŀ������ĺ�����ʣ���ֱ�����
	bool CBNet::BayesNetInference(/*CShowData*pDialogData*/);
	
	bool CBNet::FindRow(CIntVector StateTbl,CIntVector DataMatrix,CIntVector parntVector,int parntNum);
   
	double CBNet::random(double mi,double ma);    //�������������

	void CBNet::CreateStateTbl(CIntMatrix& StateTbl,CIntVector parntVector,int num);
public:
	CResult* CBNet::Train(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
		CBayesVO*pBayesVO, CBayesOutput &ppOutStruct);
	CResult *CBNet::Pred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
		CBayesVO*pBayesVO, CBayesOutput &ppOutStruct);
	bool CBNet::LearnParam();//(CIntMatrix InitMatrix,CIntVector AimVector,CIntMatrix CausTbl,int SimNum,int GSimNum);		
	//���������
	CResult* Output(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
		TPredVO *pVO, CBayesOutput &ppOutStruct);
	//�洢����ṹ
	bool CBNet::SaveNet();
	//��ȡ����ṹ
	bool CBNet::ReadNet();	
};


#endif // !defined(AFX_BNET_H__9FF56EFD_AF1E_4A66_B6DA_55725937228D__INCLUDED_)





















