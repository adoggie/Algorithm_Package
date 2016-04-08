// EnvParam.h: interface for the CEnvParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVPARAM_H__F0BE7E6F_CCE6_4947_BA44_D076FA84AD6A__INCLUDED_)
#define AFX_ENVPARAM_H__F0BE7E6F_CCE6_4947_BA44_D076FA84AD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../Public/common.h"
#include "../Public/TString.h"
#include "../cpknl/cpknl.h"
#include  "../TLDBDll/Database/DataAccess.h"
#include "../TLDBDll/Database/fieldlist.h"
#include "../TLDBDll/Database/cfield.h"
#include <string>
//using namespace std;
/*
*�㷨����綯̬�������Ƿ���Ҫ���������Ϣ���Ƿ���ֹ
*/
struct Interaction
{
	bool bNeedFeedback;//���룺�����㷨�Ƿ���Ҫд������Ϣ��true�����뷴����false������
	bool bAbort;//���룺true�㷨Ӧ����ֹ��ǰ����false���������С�Ĭ��false
	char BufMsg[100];//����������ǰ������Ϣ	
	Interaction()
	{
		bNeedFeedback = false;
		bAbort = false;
		memset(BufMsg,0,100);
	}
};


/*
����Ŀ�����Ϣ�Ļ��࣬����Щ������Ϊ���������Щ��Ϊ�Ա��������������Щ��Ҫ���
��Ҫ���ƽ������ʾ���⣬ÿһ���㷨����һ����
*/
class VOBase{
public:
	VOBase();
	virtual ~VOBase();
};

/*
�㷨ģ����Ϣ����Ҫ�����㷨ѵ��ʱ�������ģ����Ϣ��������һ�ε�Ԥ�⡣
ÿһ���㷨����һ����
*/
class StructBase{
public :
	StructBase();
	virtual ~StructBase();
	virtual bool Export(std::string szFileName)=0;//add by xds 20060712
	virtual bool Import(std::string szFileName)=0;//add by xds 20060712
};


/*
���������࣬����ϵͳ���������·���ΪC�㷨�ڲ����õķ���
*/
class CEnvParam  
{
public:
	CEnvParam();
	virtual ~CEnvParam();
public:
	//******  ָ����Щ������Ҫ���뵽������  *** 
	bool HasOutParm(CTString sName);//�ж��Ƿ���Ҫ���ĳ����������yresid
	CTString GetOutParamName(CTString sName);//
	//*************���û�������************
	bool SetVar(CTString sVarName,double value);
	bool SetVar(CTString sVarName,CDoubleVector* value);//value �ڴ�ռ����㷨�Լ��ͷš�
	bool SetVar(CTString sVarName,CDoubleMatrix* value);//value �ڴ�ռ����㷨�Լ��ͷš�
	bool SetVar(CTString sVarName,CDataAccess* value);//value �ڴ�ռ����㷨�Լ��ͷš�

};

class AnaWord;
class AnaSyntax;
//������������,Ԥ��ģ���������

typedef struct  tagTPredVO//�������������繲�õ�
{
	tagTPredVO()
	{
		bEvaluate = false;
		strNewFileName = "";
	}
	bool Compile(std::string strVO)
	{
//	NEURALPRED
//		/DATA=[FILE]
//		/VARINPUT [varlist]
//		/MODELFROMFILE [FILE]

		AnaWord wrd;
		wrd.Import(strVO);
		
		strNewFileName = wrd.GetAt(3);
		//�����ֶ�
		if (AnaSyntax::exist("/VARINPUT", wrd))
			AnaSyntax::getVAR("/VARINPUT",wrd,arFieldInfo);
		else
			return false;
		//�滻�ֶ�
		if (AnaSyntax::exist("/REPLACEVARNAME",wrd))
		{
			AnaSyntax::getVAR("/REPLACEVARNAME", wrd, szArrReplace);
			if (szArrReplace.GetSize()%2 != 0)
				return false;
		}
		//Ŀ����ʾ�ֶ�
		if (AnaSyntax::exist("/PREDNAME", wrd))
			szOutput = AnaSyntax::getSingleStr("/PREDNAME", wrd);
		//ģ������-add by xds 20060717
		CTStringArray arrOut;
		AnaSyntax::getVAR("/OUTPUT",wrd,arrOut);
		int i=0, nSize = arrOut.GetSize();
		for (i=0; i<nSize; i++)
		{
			CTString szTemp = arrOut.GetAt(i);
			if (szTemp == "EVALUATION")
			{
				bEvaluate = true;   //�Ƿ�ֻ������������ļ�
			}
		}
		//ģ��·��
		if (AnaSyntax::exist("/MODELFROMFILE", wrd))
			m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", wrd);
		else 
			return false;
		return true;
	};
public:
	bool        bEvaluate;      //�Ƿ�Ϊģ������
	CTString	strNewFileName; //���������Դ�ļ��У�����Ҫ�½�һ���ļ����ڴ��Ԥ���ֵ������Ԥ���һЩ����
	CTStringArray	arFieldInfo;//��Ҫ���뵽���ļ���ȥ�ֶε���Ϣ
	CTString		m_ModelFile;		//�����ģ��
public:
	CTStringArray   szArrReplace;//�滻�����ֶ�����
	CTString        szOutput;    //Ŀ�����-��ʾ��,�������ܻ����!
}TPredVO;



/*
���ӣ�
����ʱ�������ࣺ1���������������ʾ�ã����Ʋ�����VO����
                2������������pEnv����Ҫ������������
				3������ṹ��Ϣ��ѵ����Ľṹ������Ԥ��
�ع�ѵ��ʱ��
CDataInterface *pData �����롿 ��������ݼ�
CEnvParam *pEnv       ����������� ��������������
Interaction *itt      ����������� ��̬������

VOReg *VO             �����롿 ����Ŀ�����Ϣ
StructReg *pOutStruct ������� ���������Ϣ
˵�������еĲ����ڴ��������������ͷţ��㷨�ڲ�����Ҫ����

CResult * RegTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
				   VOReg *VO, StructReg *pOutStruct)
{
	//.....����
	if (pEnv->HasOutParm("yresid"))
	{//���ָ��Ҫ�������yresid��
		pEnv->SetVar(pEnv->GetOutParamName("yresid"),nY);//nYΪϵͳ������Ĳ���
	}

}*/
/*
���ӣ�
����ʱ�������ࣺ1���������������ʾ�ã����Ʋ�����VO����
                2������������pEnv����Ҫ������������
�ع�Ԥ��ʱ��
CDataInterface *pData �����롿 ��������ݼ�
CEnvParam *pEnv       ����������� ��������������
Interaction *itt      ����������� ��̬������

VORegPredict *VO             �����롿 ����Ŀ�����Ϣ��ѵ��ʱ���ܲ�һ��
StructReg *pInStruct �����롿 ����Ŀ�����Ϣ ���˴���Ϊ���룩
˵�������еĲ����ڴ��������������ͷţ��㷨�ڲ�����Ҫ����
CResult * RegPredict(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
				   VORegPredict *VO, StructReg *pInStruct)
{

}
*/

#endif // !defined(AFX_ENVPARAM_H__F0BE7E6F_CCE6_4947_BA44_D076FA84AD6A__INCLUDED_)
