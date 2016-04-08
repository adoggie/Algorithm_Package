// SurvivalCox.h: interface for the CSurvivalCox class.
//         ������� ��������ģ�� 
//			��ע�� ������
//          ʱ�䣺 2005��10��20��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURVIVALCOX_H__7E177F30_E0F9_42D5_9B80_9993FB4AD2F8__INCLUDED_)
#define AFX_SURVIVALCOX_H__7E177F30_E0F9_42D5_9B80_9993FB4AD2F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
		
#include "AlgorithmBase.h"

//��������ģ�� 
class AFX_EXT_CLASS CSurvivalCox : public CAlgorithmBase  
{
public:
	CResult *OnSurvivalCox(CDataInterface* pDataInterface, string VO);
	int DelCol(double *pData,int nRowNum,int nColNum, int nColNO);
	CSurvivalCox();
	virtual ~CSurvivalCox();

private:
	CResult			* m_pResult;
	CDataInterface	* m_pDataInterface;

	int* m_pCovarValid;
	bool m_bDispBaseLn;
	double m_dbW;
	int m_nWeightFNO;
	bool m_bWeighted;
	double* m_pWeight;

	double m_dbPrecision;
	double m_Eps ;
	// get from db
	int m_nRowNum;
	int m_nColNum;
	double* m_pData;
	double *m_pSurvTime;
	int *m_pStatus;

	// get from dialog
	bool m_bSingleEventValue;//�¼��ĵ�ֵ���
	double m_dSingleValue; //������ֵ
	double m_dRangeFrom;//���䷶Χ����ʼֵ
	double m_dRangeTo;  //���䷶Χ���յ�ֵ
	int m_nStatusFNO;   //״̬�����±�
	int m_nTimeFNO;		//ʱ������±�
	int* m_pCovarFNOs;	//Э�����±�

	bool GetValueFromVO(CDataInterface * pDataIngerface, string VO);
	bool Initialize();
	CResult  * Analyze();
	double Sigdot(double x);
};

#endif // !defined(AFX_SURVIVALCOX_H__7E177F30_E0F9_42D5_9B80_9993FB4AD2F8__INCLUDED_)
