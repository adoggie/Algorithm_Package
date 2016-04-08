//////////////////////////////////////////////////////////////////////
// Survival.h:                                                      //
//																	//
// Define:        生存分析基类定义                                  //
// Creator:       屈韬                                              //
// Date:          2002.11.25                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _SURVIVAL_H_
#define _SURVIVAL_H_
#include <vector>
#include <string>


typedef struct tagFieldInfo{
	CTString strLable;
	CTString strName;
	DWORD nFieldNo;
} TFieldInfo;

class AFX_EXT_CLASS CSurvivalVO
{
	// Construction
public:
	bool	m_bBaseLn;
	int m_nTimeField;
	int m_nStatusField;
	TIntStruct m_aCovariatesField;
	bool m_bSingleEventValue;
	double m_dSingleValue;
	double m_dRangeFrom;
	double m_dRangeTo;
	bool m_bSetEvent;
	bool m_bField2Time;
	bool m_bField2Status;
	bool m_bField2Covariates;
	CTArray< TFieldInfo, TFieldInfo > m_aFieldInfo;
	CTString m_strDBName;
	int m_nFactorField;
	bool m_bField2Factor;
	double	m_dMaxTime;
	double	m_dStep;
	
};

class AFX_EXT_CLASS CSurvival
{
public:
	CSurvival();
	virtual ~CSurvival();

public:
	virtual CResult * OnSurvival( CDataInterface* pDataInterface, CSurvivalVO &SurvivalVO);

protected:
	virtual bool GetParamSetting() = 0;
	virtual bool Initialize() = 0;
	virtual CResult * Analyze() = 0;

public:
	CSurvivalVO * pSurvivalVO;
	CResult			* m_pResult;
	CDataInterface	* m_pDataInterface;
};

#endif // !defined(_SURVIVAL_H_)
