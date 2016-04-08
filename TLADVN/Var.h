// Var.h: interface for the CVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VAR_H__F135339C_1A30_4F26_A057_49D245F880DC__INCLUDED_)
#define AFX_VAR_H__F135339C_1A30_4F26_A057_49D245F880DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AlgorithmBase.h"
class CVar   : public CAlgorithmBase 
{
public:
	CVar();
	virtual ~CVar();
	CResult * OnVAR(CDataInterface *pDataInterface, string VO);//�ⲿ������Ҫ����
	TIntStruct		m_tVarInfo;//�ع����struct
	TIntStruct		m_tVarInfo_D;//��������struct	
private:
	
	//////////////////////////////////////
	/////		��������б�		//////
	//////////////////////////////////////

	int m_nLagnum;                            //�ӽ������ͺ����
	int m_nForcast;						      //�ӽ������Ƿ����һ��Ԥ�����Ϣ 
	bool m_bXWithconst;				          //�ӽ�����ģ���Ƿ��г�����
	CDoubleMatrix m_mxX;				      //�ӽ����ûع��������
	CDoubleMatrix m_mxExto;					  //�ӽ�����������������


	//////////////////////////////////////
	////		���������������		//
	//////////////////////////////////////
	int				m_nRegre;           //�ع��������
	int				m_nExto;            //������������
	CTStringArray	m_strDepend;        //�ع���������ַ���
	CTStringArray	m_strVarSel;        //�������������ַ���
	bool m_bChartScatterPlot;			//�Ƿ����ɢ��ͼ
	bool m_bChartLine;                  //�Ƿ�������ͼ
	
	//////////////////////////////////////
	///////       �������б�   /////////
	//////////////////////////////////////

	//////////////////////////////////////////ģ��ϵ����
	CDoubleMatrix m_mxestB;					//ģ��ϵ��
	CDoubleMatrix m_mxparattest;			//ģ��ϵ��t����ֵ
	CDoubleMatrix m_mxparastde;				//ģ��ϵ����׼��

	//////////////////////////////////////////�����ͳ����
	CDoubleVector m_vrRsqu;					//ģ�͵�R^2
	CDoubleVector m_vrAdjRsqu;				//ģ�͵ĵ�����R^2
	CDoubleVector m_vrSSE;					//ģ�ͷ����SSE
	CDoubleVector m_vrSST;					//ģ�ͷ����SST
	CDoubleVector m_vrSER;					//ģ�ͷ���Ļع��׼��
	CDoubleVector m_vrFST;					//ģ�ͷ����Fͳ����
	CDoubleVector m_vrLogLG;				//ģ�ͷ���Ķ�����Ȼֵ
	CDoubleVector m_vrAICG;					//ģ�ͷ����AIC׼��
	CDoubleVector m_vrSCG;					//ģ�ͷ����SC׼��
	CDoubleVector m_vrSDD;                  //ģ�ͷ���ı�׼��
	CDoubleVector m_vrMD;					//ģ�ͷ���ľ�ֵ
	CDoubleMatrix m_mxResidul;				//ģ�͵Ĳв����
	double m_dDRC;							//ģ�͵ľ����в�Э����
	double m_dAdjDRC;						//ģ�͵������ľ����в�Э����
	double m_bLogl;					    	//ģ�͵Ķ�����Ȼ
	double m_bAIC;							//ģ�͵�AIC׼��
	double m_bSC;							//ģ�͵�SC׼��
	double m_bSBC;							//ģ�͵�SBC׼��
	double m_bFPE;							//ģ�͵�FPE׼��
	CDoubleMatrix m_mxYhat;					//ģ�͵�Ԥ��ֵ��������ɢ��ͼ

	//////////////////////////////////////Ԥ��
	CDoubleMatrix m_mxForcast;				//Ԥ��ֵ

private:
	bool GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void DrawResult();					//������չʾ					
	bool Varest(CDoubleMatrix& m_mxX,bool m_bXWithconst,CDoubleMatrix& m_mxExto,int lagnum,int m_nForcast);//�����̺������ع�����������������ͺ�������Ƿ�Ԥ��
	void GetMatrixData(CDoubleMatrix& Matrix_X, CDoubleMatrix& Matrix_y);//����������matrix_x[�ع����]��matrix_y[��������]
};

#endif // !defined(AFX_VAR_H__F135339C_1A30_4F26_A057_49D245F880DC__INCLUDED_)



















































































