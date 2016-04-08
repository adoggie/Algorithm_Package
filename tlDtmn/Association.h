// Association.h: interface for the CAssociation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSOCIATION_H__68C7C38C_F7F0_47DF_8557_ACE166A16D5B__INCLUDED_)
#define AFX_ASSOCIATION_H__68C7C38C_F7F0_47DF_8557_ACE166A16D5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../TLDBDll/Database/DataAccess.h"
#include "../DataInterface/DataInterface.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/AnaSyntax.h"
#include "../Public/AnaWord.h"

//��ά��������Ľ����������
class CRuleTrainVO
{
public:
	//	CPtrArray	arFeildInfo;//TFieldInfo�����������ֶε���Ϣ
	CTStringArray strInputName;
	CTStringArray strDestName;
	bool	bSingle;			//true��ά��������,false��ά��������
	int		nSupport;			//��С֧�ֶ�
	double	dbConfidence;		//���Ŷ�
	CRuleTrainVO()
	{
		bSingle	= true;
		nSupport = 0;
		dbConfidence = 0;
	}
	bool Compile(string strVO)
	{
	//	��������VO����
	//	DTMNRUN
	//	/DATA = FILE
	//	/VARINPUT arglistg
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/ASSOTYPE [{SINGLE},{MULTI**}]
	//	/MINSUPPORT SUPPORT(0.1**)
	//	/ MINCREDIT CREDIT (0.5**)		



		AnaWord VOstr;
		VOstr.Import(strVO);
		
		bSingle = AnaSyntax::findstr("/ASSOTYPE","SINGLE",VOstr);
		if (AnaSyntax::findstr("/MINSUPPORT","SUPPORT",VOstr))
			nSupport = AnaSyntax::getvalue("/MINSUPPORT","SUPPORT",VOstr);
		if (AnaSyntax::findstr("/MINCREDIT","CREDIT",VOstr))
			dbConfidence = AnaSyntax::getvalue("/MINCREDIT","CREDIT",VOstr);
		
		AnaSyntax::getVAR("/VARINPUT", VOstr, strInputName);
		AnaSyntax::getVAR("/VARDEST", VOstr, strDestName);
		return true;
	}
};

//��������������������Թ�ѭ��������
//Ԥ��ģ�͵Ĳ���
class CRuleOutput : public StructBase
{
public:
	CRuleOutput(){;}
	virtual ~CRuleOutput(){;}
	virtual bool Import(string szFileName);
	virtual	bool Export(string szFileName);
protected:
	//�����������
	CPtrArray	arFeildInfo;//TFieldInfo�����������ֶε���Ϣ
	bool	bSingle;			//true��ά��������,false��ά��������
	int		nSupport;			//��С֧�ֶ�
	double	dbConfidence;		//���Ŷ�
	bool	bStopWork;			//�Ƿ�ֹͣ����
	bool	bSort;				//����
	//����
	CPtrArray	arFreSet;//Ƶ����������TFreSet
	CPtrArray	arRuleDmn;//���������ǿ����TRuleDmn
};




class CAssociation  
{
public:
	CAssociation();
	virtual ~CAssociation();

	CResult *RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO, string strModel);
};

#endif // !defined(AFX_ASSOCIATION_H__68C7C38C_F7F0_47DF_8557_ACE166A16D5B__INCLUDED_)
