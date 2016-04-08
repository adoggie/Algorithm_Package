/*// RegLinearPredict.h: interface for the CRegLinearPredict class.
//                          ���Իع�Ԥ��ģ��
//                   ���ܣ� ���лع�����õ��ع鷽�̺�����������µĹ۲��Ա������ݣ�
//                          �õ�Ԥ��ֵ������������
//                   ���ߣ� ������
//                   ʱ�䣺 2005��10��26��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGLINEARPREDICT_H__D78B81F7_7899_4863_8A8E_2E317C046D21__INCLUDED_)
#define AFX_REGLINEARPREDICT_H__D78B81F7_7899_4863_8A8E_2E317C046D21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "RegressionLinear.h"
//Ԥ��ģ���ѡ�����ýṹ��
class AFX_EXT_CLASS CRegLinearPredictVo
{
public:
	CRegLinearPredictVo()
	{
		m_bConfidInterval = false;
		pRegressLinearResult = NULL;//Ĭ��Ϊ��
	}
	bool   m_bConfidInterval;//�Ƿ���Ҫ��������0������Ҫ��1����Ҫ��Ĭ��0����Ҫ
	int m_nDependIndex;//ָ��������±�
	TIntStruct m_tVarInfo;//�Ա����±꼯������ģ�͵ı���˳����д��
    TRegressLinearResult*  pRegressLinearResult;//�ع����ģ��
protected:
private:
};

class AFX_EXT_CLASS CRegLinearPredict  : public CAlgorithmBase
{
public:
	CRegLinearPredict();
	virtual ~CRegLinearPredict();
	//����һ������
	//���û��Ҫ������������䣬��ô�õ��ľ���һ��n��1�ľ��󣬷���Ϊn��3�ľ�������nΪ�µĹ۲����
	CDoubleMatrix OnRegLinearPredict(CDataInterface* pDataInterface,CRegLinearPredictVo* pVo);
public:
	bool	m_bConfidInterval;//�Ƿ�����������䡣Ĭ��ֵΪ 1��
	double	m_dConfidence;//����ˮƽ��ȡֵ0��1֮�䣬Ĭ��ֵΪ0.95
	int		m_nDependIndex;//��������µ������ļ��е��±�
private:
	CDoubleMatrix GetPredictY(CDoubleMatrix vect_NewX//�µ��Ա����۲�ֵ
			       , TRegressLinearResult* pRegResult);//���Իع�ģ��
	
	//��ȡ���ݣ�����ɹ�������1�����򷵻�0
    bool GetData(CDataInterface* pDataInterface,TRegressLinearResult* pRegResult,CDoubleMatrix& Matrix_NewX ,CDoubleMatrix& Matrix_Y);
};

#endif // !defined(AFX_REGLINEARPREDICT_H__D78B81F7_7899_4863_8A8E_2E317C046D21__INCLUDED_)
*/
