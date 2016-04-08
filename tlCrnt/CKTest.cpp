// CKTest.cpp: implementation of the CCKTest class.
//				������KS����
//					����ׯ 2006/03/20
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "tlcrnt.h"
#include "CKTest.h"
#include "tlcrnt.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCKTest::CCKTest()
{
	m_bCheckBin = TRUE;
	m_Bin_n = 1;//>=1;
	m_Bin_p = 0.5;//0.001< <0.999
	m_bGetVoBin = FALSE;
	m_bCheckNegativBin = FALSE;//������ʽ�ֲ�, û�и�����ʽ�ֲ����溯��, ��ʱ������.
	m_bCheckPoisson = TRUE;
	m_bGetVoPoisson = FALSE;
	m_Poisson_lambda = 1;// >0.001
	m_bCheckGeo = TRUE;
	m_bGetVoGeo = FALSE;
	m_Geo_p = 0.5;// 0.001< <0.999
	
	m_bCheckNormal = TRUE;
	m_bGetVoNormal = FALSE;
	m_Normal_mu = 0;
	m_Normal_sigma = 1;//>0.001
	m_bCheckAverage = TRUE;
	m_bGetVoAverage = FALSE;
	m_Average_low = 0;
	m_Average_up = 1;// m_Average_up>m_Average_low
	m_bCheckExp = TRUE;
	m_bGetVoExp = FALSE;
	m_Exp_lambda = 1;//>0.001
	m_bCheckLogNormal = FALSE;
	m_bGetVoLogNormal = TRUE;
	m_lognormalmu = 0;// == 0
	m_lognormalsigma = 1;// >0.001
	m_bCheckLogist = TRUE;
	m_bGetVoLogist = FALSE;
	m_Logist_m = 0;
	m_Logist_b = 1; //> 0.001
	m_bCheckGam = TRUE;
	m_bGetVoGam = FALSE;
	m_Gam_alpha = 1;// >0.001;
	m_Gam_beta = 1;// > 0.001;
	m_bCheckHalfNor = TRUE;
	m_bGetVoHalfNor = FALSE;
	m_HalfNor_mu = 0;// ==0
	m_HalfNor_sigma = 1;// >0.001
	m_bCheckBeta = FALSE;//Ĭ��ֵδ֪
	m_bGetVoBeta = FALSE;
	m_Beta_alpha = 1;//>0.001
	m_Beta_beta = 1;//>0.001
	m_bCheckWeib = FALSE;
	m_bGetVoWeib = FALSE;
	m_Weib_alpha = 1;//>0.001
	m_Weib_beta = 1;//>0.001
	m_bCheckLaplace = FALSE;
	m_bGetVoLaplace = FALSE;
	m_Laplace_b = 0.5;// >0.001
	m_Logist_m = 0;// ==0;
}

CCKTest::~CCKTest()
{
}

BOOL CCKTest::GetData(CDataInterface *pDataInterface, CDoubleMatrix & Matrix_Data, int index)
{
	CDWordArray arCol;
	if (m_tVarInfo.pValue[index] < 0)
		return FALSE;
	arCol.Add(m_tVarInfo.pValue[index]);
	if(m_bWithWeight)//���Ȩ�ر����±�
       arCol.Add(m_nWeight);
	m_nRow = pDataInterface->GetColsData(arCol, Matrix_Data, 0);//ȡ������
	return TRUE;
}

BOOL CCKTest::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//KSTEST
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/WEIGHT [varname]
//	/TEST [NORMAL] [NORMALPARA({0},{1})] [UNIFORMCON] [UNIFORMCONPARA({0},{1})] [EXP] [EXPPARA({1})]
//	 [GAMMA] [GAMMAPARA({1},{1})] [HALFNOR] [HALFNORPARA({0},{1})] [LOGISTIC] [LOGISTICPARA({0},{1})] 
//	 [LOG] [LOGPARA({0},{1})] [BEAT] [BEATPARA({1,1})]
//	 [WEIB] [WEIBPARA(1,1)] [LAPLACE] [LAPLACE({0,0.5})]
//	 [BIN] [BINPARA({1},{0.5})] [GEO] [GEOPARA({0.5})]
//	 [POISSON] [POISSONPARA({1})] 
//	/METHOD [CUTEND({0.03})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		m_bWithWeight = TRUE;
		CTStringArray ynames;
		AnaSyntax::getVAR("/WEIGHT", VOstr, ynames);
		if ((ynames.GetSize())!=1) 
			return FALSE;
		CTString yname = ynames.GetAt(0);
		m_nWeight = pDataInterface->IndexByName(yname);
	}
	else
		m_bWithWeight = FALSE;
//////////////////////////////////////////////////////////////////////////
//��ȡ�Ƿ����ֲ��Ĳ���
////////////////////////////////////////////////////////////////////////////

//��̬
	if (AnaSyntax::findstr("/TEST", "NORMALPARA", VOstr) )
	{
		m_bCheckNormal = TRUE;
		m_bGetVoNormal = TRUE;
		m_Normal_mu = AnaSyntax::getvalue("/TEST", "NORMALPARA", VOstr);
		m_Normal_sigma = AnaSyntax::getvalue("/TEST", "NORMALPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "NORMAL", VOstr) )
		m_bCheckNormal = TRUE;
	else
		m_bCheckNormal = FALSE;
//���ȷֲ�
	if (AnaSyntax::findstr("/TEST", "UNIFORMCONPARA", VOstr) )
	{
		m_bCheckAverage = TRUE;
		m_bGetVoAverage = TRUE;
		m_Average_low = AnaSyntax::getvalue("/TEST", "UNIFORMCONPARA", VOstr);
		m_Average_up = AnaSyntax::getvalue("/TEST", "UNIFORMCONPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "UNIFORMCON", VOstr) )
		m_bCheckAverage = TRUE;
	else
		m_bCheckAverage = FALSE;
//ָ���ֲ�
	if (AnaSyntax::findstr("/TEST", "EXPPARA", VOstr) )
	{
		m_bCheckExp = TRUE;
		m_bGetVoExp = TRUE;
		m_Exp_lambda = AnaSyntax::getvalue("/TEST", "EXPPARA", VOstr);
	}
	else if (AnaSyntax::findstr("/TEST", "EXP", VOstr) )
		m_bCheckExp = TRUE;
	else
		m_bCheckExp = FALSE;
//	 Gamma�ֲ�
	if (AnaSyntax::findstr("/TEST", "GAMMAPARA", VOstr) )
	{
		m_bCheckGam = TRUE;
		m_bGetVoGam = TRUE;
		m_Gam_alpha = AnaSyntax::getvalue("/TEST", "GAMMAPARA", VOstr);
		m_Gam_beta = AnaSyntax::getvalue("/TEST", "GAMMAPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "GAMMA", VOstr) )
		m_bCheckGam = TRUE;
	else
		m_bCheckGam = FALSE;
//	����̬�ֲ�
	if (AnaSyntax::findstr("/TEST", "HALFNORPARA", VOstr) )
	{
		m_bCheckHalfNor = TRUE;
		m_bGetVoHalfNor = TRUE;
		m_HalfNor_mu = AnaSyntax::getvalue("/TEST", "HALFNORPARA", VOstr);
		m_HalfNor_sigma = AnaSyntax::getvalue("/TEST", "HALFNORPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "HALFNOR", VOstr) )
		m_bCheckHalfNor = TRUE;
	else
		m_bCheckHalfNor = FALSE;
//�߼�
	if (AnaSyntax::findstr("/TEST", "LOGISTICPARA", VOstr) )
	{
		m_bCheckLogist = TRUE;
		m_bGetVoLogist = TRUE;
		m_Logist_m = AnaSyntax::getvalue("/TEST", "LOGISTICPARA", VOstr);
		m_Logist_b = AnaSyntax::getvalue("/TEST", "LOGISTICPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "LOGISTIC", VOstr) )
		m_bCheckLogist = TRUE;
	else
		m_bCheckLogist = FALSE;
//������̬
	if (AnaSyntax::findstr("/TEST", "LOGPARA", VOstr) )
	{
		m_bCheckLogNormal = TRUE;
		m_bGetVoLogNormal = TRUE;
		m_lognormalmu = AnaSyntax::getvalue("/TEST", "LOGPARA", VOstr);
		m_lognormalsigma = AnaSyntax::getvalue("/TEST", "LOGPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "LOG", VOstr) )
		m_bCheckLogNormal = TRUE;
	else
		m_bCheckLogNormal = FALSE;
//Beta�ֲ�
	if (AnaSyntax::findstr("/TEST", "BETAPARA", VOstr) )
	{
		m_bCheckBeta = TRUE;
		m_bGetVoBeta = TRUE;
		m_Beta_alpha = AnaSyntax::getvalue("/TEST", "BEATPARA", VOstr);
		m_Beta_beta = AnaSyntax::getvalue("/TEST", "BEATPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "BETA", VOstr) )
		m_bCheckBeta = TRUE;
	else
		m_bCheckBeta = FALSE;

//Weib�ֲ�
	if (AnaSyntax::findstr("/TEST", "WEIBPARA", VOstr) )
	{
		m_bCheckWeib = TRUE;
		m_bGetVoWeib = TRUE;
		m_Weib_alpha = AnaSyntax::getvalue("/TEST", "WEIBPARA", VOstr);
		m_Weib_beta = AnaSyntax::getvalue("/TEST", "WEIBPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "BEAT", VOstr) )
		m_bCheckWeib = TRUE;
	else
		m_bCheckWeib = FALSE;
//Laplace�ֲ�
	if (AnaSyntax::findstr("/TEST", "LAPLACEPARA", VOstr) )
	{
		m_bCheckLaplace = TRUE;
		m_bGetVoLaplace = TRUE;
		m_Laplace_mu = AnaSyntax::getvalue("/TEST", "LAPLACEPARA", VOstr);
		m_Laplace_b = AnaSyntax::getvalue("/TEST", "LAPLACEPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "LAPLACE", VOstr) )
		m_bCheckLaplace = TRUE;
	else
		m_bCheckLaplace = FALSE;

//////////////////////////////////////////////////////////////////////////
//��ɢ��
//////////////////////////////////////////////////////////////////////////

//����ʽ
	if (AnaSyntax::findstr("/TEST", "BINPARA", VOstr) )
	{
		m_bCheckBin = TRUE;
		m_bGetVoBin = TRUE;
		m_Bin_n = AnaSyntax::getvalue("/TEST", "BINPARA", VOstr);
		m_Bin_p = AnaSyntax::getvalue("/TEST", "BINPARA", VOstr, 2);
	}
	else if (AnaSyntax::findstr("/TEST", "BIN", VOstr) )
		m_bCheckBin = TRUE;
	else
		m_bCheckBin = FALSE;
//���ηֲ�
	if (AnaSyntax::findstr("/TEST", "GEOPARA", VOstr) )
	{
		m_bCheckGeo = TRUE;
		m_bGetVoGeo = TRUE;
		m_Geo_p = AnaSyntax::getvalue("/TEST", "GEOPARA", VOstr);
	}
	else if (AnaSyntax::findstr("/TEST", "GEO", VOstr) )
		m_bCheckGeo = TRUE;
	else
		m_bCheckGeo = FALSE;
//����
	if (AnaSyntax::findstr("/TEST", "POISSONPARA", VOstr) )
	{
		m_bCheckPoisson = TRUE;
		m_bGetVoPoisson = TRUE;
		m_Poisson_lambda = AnaSyntax::getvalue("/TEST", "POISSONPARA", VOstr);
	}
	else if (AnaSyntax::findstr("/TEST", "POISSON", VOstr) )
		m_bCheckPoisson = TRUE;
	else
		m_bCheckPoisson = FALSE;
	
	if (AnaSyntax::findstr("/METHOD", "CUTEND", VOstr))
	{
		m_bCheckDel = TRUE;
		m_dDelPer = AnaSyntax::getvalue("/METHOD", "CUTEND", VOstr);
	}
	else
		m_bCheckDel = FALSE;

	return TRUE;
}

CResult * CCKTest::OnCKTest(CDataInterface *pDataInterface, string VO)
{
	CResult *pResult = NULL;
		pResult=new CResult("������KS����");
		m_pResult = pResult;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	for (int index=0; index < m_tVarInfo.iCount; index++)//�����index��ѡ��ı���
	{	
		m_str_x = pDataInterface->GetFieldName(m_tVarInfo.pValue[index]);
		CDoubleMatrix data_read;
		if (pDataInterface->GetWeightNumb() < 0)
			m_bWithWeight = FALSE;
		else
			m_bWithWeight = TRUE;
		if (!GetData(pDataInterface, data_read, index))//����
		{
			CRsltElementText * pPRsltElementText=new CRsltElementText("����");
			pPRsltElementText->SetName("����");
			pPRsltElementText->AddString(m_str_x+" ���ݶ�ȡ��������\r\n");
			m_pResult->Add(pPRsltElementText);
			return pResult;
		}
		if (!CKTestMain(data_read))//������Ҫ�㷨���еĳ���
		{
			CRsltElementText * pPRsltElementText=new CRsltElementText("����");
			pPRsltElementText->SetName("����");
			pPRsltElementText->AddString(m_str_x+" Ȩ��Ϊ�������������ݴ���\r\n�޷�����");
			m_pResult->Add(pPRsltElementText);
			return pResult;
		}
		DrawOutput();
	}//INDEX������
	return pResult;//???
}

BOOL CCKTest::CKTestMain(CDoubleMatrix & data_read)//���Ȩ�س��ָ���ʱ, ����.
{
	if (data_read.mcol() != 1+m_bWithWeight)
		return FALSE;

//������Ԥ����
	CDoubleVector vec_x = data_read(0);
	CDoubleVector vec_weight;
	CIntVector vec_index;
	int i, j;
	double mu = 0;
	double sigma = 1;
	if (m_bWithWeight)
	{
		vec_weight = data_read(1);
		for (i=0; i<m_nRow; i++)
			if (vec_weight(i) < -EPS_ACCUR)
				return FALSE;
	}
	if (m_bWithWeight)
	{
		mu = vec_x.mean_w(vec_weight);
		sigma = sqrt(var_w(vec_x, vec_weight));
		vec_x = vec_x.Sort(vec_index);
		m_totalweight = vec_weight.sum();
	}
	else
	{
		mu = vec_x.mean();
		sigma = sqrt(vec_x.var());
		vec_x.Sort();
		m_totalweight = m_nRow;
	}

	int step = max(pow(m_nRow/1000, 0.7), 1);
	int dCase = (m_nRow - 0.1) / step+1;
	CDoubleVector vec_cum(dCase);
	CDoubleVector vec_id(dCase);
	CDoubleVector vec_x_id(dCase);
	for (i=0; i<dCase; i++)
	{
		vec_id(i) = i*step;
		vec_x_id(i) = vec_x(i*step);
	}

//���㾭�����, �õ�vec_cumΪ�ۼƸ��ʺ���
	if (m_bWithWeight)
	{
		for (i=0; i<dCase-1; i++)
			for (j=i*step; j< (i+1)*step; j++)
				vec_cum(i+1) +=vec_weight(vec_index(j)-1);
		vec_cum = vec_cum / m_totalweight;
	}
	else
		vec_cum = vec_id / m_totalweight;
	CDoubleVector vec_res(dCase);
	int totalfuncs;
	totalfuncs = m_bCheckNormal +m_bCheckExp + m_bCheckHalfNor + m_bCheckLogist + m_bCheckGam + m_bCheckAverage + 
				 m_bCheckLogNormal + m_bCheckBeta + m_bCheckWeib + m_bCheckLaplace +
				 m_bCheckBin + m_bCheckGeo + m_bCheckPoisson;
	CDoubleVector vec_D(totalfuncs);
	int funcount = -1;
	CTString str_temp;
	m_AllFuncInfo.RemoveAll();
/***************************************************/
//	������ֲַ��ܶȺ����뾭���ܶȺ���֮�������ֵ
/***************************************************/

//���㾭���ۼƺ�������̬�ֲ�����֮�������ֵ
	if (m_bCheckNormal)
	{
		funcount++;
		double mu_nor = mu;
		double sigma_nor = sigma;
		if (m_bGetVoNormal)
		{
			mu_nor = m_Normal_mu;
			sigma_nor = m_Normal_sigma;
		}
		m_str_FunInfo = "��̬�ֲ�\r\n";
		str_temp.Format("%.4f", mu_nor);
		m_str_FunInfo += "��ֵ="+str_temp + "\r\n";
		str_temp.Format("%.4f", sigma_nor);
		m_str_FunInfo += "����="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);
		
		//�ֲ����ۼƸ��ʷֲ�������vec_cum�Ĳ�ֵvec_res
		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - normcdf(vec_x_id(i), mu_nor, sigma_nor));
		vec_D(funcount) = vec_res.Max();
	}
//���㾭���ۼƺ����� ָ�� �ֲ�����֮�������ֵ
	if (m_bCheckExp)
	{
		funcount++;
		double lambda = sigma;
		if (m_bGetVoExp)
			lambda = 1/m_Exp_lambda;
		m_str_FunInfo = "ָ���ֲ�\r\n";
		str_temp.Format("%.4f", 1/lambda);
		m_str_FunInfo += "lambda="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);

		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - expcdf(vec_x_id(i), lambda));
		vec_D(funcount) = vec_res.Max();		
	}
//���㾭���ۼƺ����� Gam �ֲ�����֮�������ֵ
	if (m_bCheckGam && (mu > 0.001|| m_bGetVoGam))
	{
		funcount++;
		double alpha_gam = mu*mu/sigma/sigma;
		double beta_gam = (sigma*sigma)/mu;
		if (m_bGetVoGam)
		{
			alpha_gam = m_Gam_alpha;
			beta_gam = 1/m_Gam_beta;
		}
		m_str_FunInfo = "Gamma�ֲ�\r\n";
		str_temp.Format("%.4f", alpha_gam);
		m_str_FunInfo += "��="+str_temp + "\r\n";
		str_temp.Format("%.4f", 1/beta_gam);
		m_str_FunInfo += "��="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);

		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - gamcdf(vec_x_id(i), alpha_gam, beta_gam));
		vec_D(funcount) = vec_res.Max();
	}
	else if (m_bCheckGam)
	{
		funcount++;
		m_AllFuncInfo.Add("Gamma�ֲ�\r\n �����޷�����!!");
		vec_D(funcount) = 1.1E20;
	}
//���㾭���ۼƺ����� ������̬ �ֲ�����֮�������ֵ
	if (m_bCheckLogNormal)
	{
		funcount++;
		m_str_FunInfo = "������̬�ֲ�\r\n";
		str_temp.Format("%.4f", m_lognormalmu);
		m_str_FunInfo += "��̬�ֲ���ֵ="+str_temp + "\r\n";
		str_temp.Format("%.4f", m_lognormalsigma);
		m_str_FunInfo += "��̬�ֲ�����="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);
		
		for (i=0; i<dCase; i++)
			vec_res = fabs(vec_cum(i) - logncdf(vec_x_id(i), m_lognormalmu, m_lognormalsigma));
		vec_D(funcount) = vec_res.Max();
	}
//���㾭���ۼƺ����� ����̬ �ֲ�����֮�������ֵ
	if (m_bCheckHalfNor)
	{
		funcount++;
		double sigma_hn = sigma;
		if (m_bGetVoHalfNor)
			sigma_hn = m_HalfNor_sigma;
		m_str_FunInfo = "����̬�ֲ�\r\n";
		str_temp.Format("%.4f", sigma_hn);
		m_str_FunInfo += "ԭʼ��̬����="+str_temp + "\r\n";
		m_AllFuncInfo.Add(m_str_FunInfo);

		double theta = sqrt((pi-1)/2)/sigma_hn;
		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - halfncdf(vec_x_id(i), theta));
		vec_D(funcount) = vec_res.Max();		
	}
//���㾭���ۼƺ����� �߼� �ֲ�����֮�������ֵ
	if (m_bCheckLogist)
	{
		funcount++;
		double m = mu;
		double b = sigma*sqrt(3)/pi;
		if (m_bGetVoLogist)
		{
			m = m_Logist_m;
			b = m_Logist_b;
		}
		m_str_FunInfo = "�߼��ֲ�\r\n";
		str_temp.Format("%.4f", m);
		m_str_FunInfo += "m="+str_temp + "\r\n";
		str_temp.Format("%.4f", b);
		m_str_FunInfo += "b="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);
		
		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - logistcdf(vec_x_id(i), m, b));
		vec_D(funcount) = vec_res.Max();		
	}
//���㾭���ۼƺ����� ���� �ֲ�����֮�������ֵ
	if (m_bCheckAverage)
	{
		funcount++;
		double range_len = (vec_x(int(m_nRow*0.75-0.5))-vec_x(int(m_nRow*0.25-0.5)));
		double Average_low = mu - range_len;
		double Average_up = mu + range_len;
		if (((Average_low - 1/sqrt(dCase)*range_len) < vec_x(int(sqrt(dCase)))) && ((Average_up + 1/sqrt(dCase)*range_len) > vec_x(m_nRow -1 - int(sqrt(dCase)))))
		{
			Average_up = vec_x(m_nRow -1 - int(sqrt(dCase)));
			Average_low = vec_x(int(sqrt(dCase)));
		}	
		if (m_bGetVoAverage)
		{
			Average_up = m_Average_up;
			Average_low = m_Average_low;
		}
		m_str_FunInfo = "���ȷֲ�\r\n";
		str_temp.Format("%.4f", Average_up);
		m_str_FunInfo += "�ֲ��Ͻ�="+str_temp + "\r\n";
		str_temp.Format("%.4f", Average_low);
		m_str_FunInfo += "�ֲ��½�="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);
		for (i=0; i<dCase; i++)
		{
			if (vec_x_id(i) > Average_low && vec_x_id(i) < Average_up)
				vec_res(i) = fabs(vec_cum(i) - (vec_x_id(i)-Average_low)/(Average_up - Average_low));
			else if (vec_x_id(i) <= Average_low)
				vec_res(i) = fabs(vec_cum(i) -0);
			else 
				vec_res(i) = fabs(vec_cum(i) -1);
		}
		vec_D(funcount) = vec_res.Max();
	}
//���㾭���ۼƺ����� Beta �ֲ�����֮�������ֵ
	if (m_bCheckBeta)
	{
		funcount++;
		m_str_FunInfo = "Beta�ֲ�\r\n";
		str_temp.Format("%.4f", m_Beta_alpha);
		m_str_FunInfo += "��="+str_temp + "\r\n";
		str_temp.Format("%.4f", m_Beta_beta);
		m_str_FunInfo += "��="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);

		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - betacdf(vec_x_id(i), m_Beta_beta, m_Beta_alpha));
		vec_D(funcount) = vec_res.Max();		
	}
//���㾭���ۼƺ����� Weibill �ֲ�����֮�������ֵ
	if (m_bCheckWeib)
	{
		funcount++;
		m_str_FunInfo = "Weibill�ֲ�\r\n";
		str_temp.Format("%.4f", m_Weib_alpha);
		m_str_FunInfo += "��="+str_temp + "\r\n";
		str_temp.Format("%.4f", m_Weib_beta);
		m_str_FunInfo += "��="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);
	
		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - weibcdf(vec_x_id(i), m_Weib_alpha, m_Weib_beta));
		vec_D(funcount) = vec_res.Max();		
	}
//���㾭���ۼƺ����� Laplace �ֲ�����֮�������ֵ
	if (m_bCheckLaplace)
	{
		funcount++;
		m_str_FunInfo = "Laplace�ֲ�\r\n";
		str_temp.Format("%.4f", m_Laplace_b);
		m_str_FunInfo += "b="+str_temp + "\r\n";
		str_temp.Format("%.4f", m_Laplace_mu);
		m_str_FunInfo += "mu="+str_temp;
		m_AllFuncInfo.Add(m_str_FunInfo);
		for (i=0; i<dCase; i++)
			vec_res(i) = fabs(vec_cum(i) - laplacecdf(vec_x_id(i), m_Laplace_b, m_Laplace_mu));
		vec_D(funcount) = vec_res.Max();
	}

/***********************************************************/
//	�����ɢ�ֲ��Ĺ۲�ֵֻ��������������, ���¼���vec_discum
/***********************************************************/
	CDoubleVector vec_discum;
	CDoubleVector vec_DisValue;
	int nVlen = 0;
	if (m_bCheckBin || m_bCheckGeo || m_bCheckPoisson)
	{
		CalcVec_DisCum(vec_x, vec_weight, vec_index, dCase, step, vec_discum, vec_DisValue);
		nVlen = vec_discum.vlen();
		double a_temp = 0;
		for (i=0; i<nVlen; i++)
		{
			a_temp += vec_discum(i);
			vec_discum(i) = a_temp;
		}
	}
	vec_res.resize(nVlen);
	
//���㾭���ۼƺ��������ʽ�ֲ�����֮�������ֵ
	if (m_bCheckBin && mu > 0 && (((1 - sigma*sigma/mu) > 0.001 && vec_x(m_nRow-1) < 10000) || m_bGetVoBin))
	{
		funcount++;
		double p = max((1 - sigma*sigma/mu), 0.003);
		int bin_n = min(max(vec_x(m_nRow-1) + 0.5, mu/p+0.5), 10000);
		p = mu / bin_n;
		if (m_bGetVoBin)//�ӽ��������
		{
			bin_n = m_Bin_n;
			p = m_Bin_p;
		}
		m_str_FunInfo = "����ʽ�ֲ�\r\n";
		str_temp.Format("%d", bin_n);
		m_str_FunInfo += "������� n="+str_temp + "\r\n";
		str_temp.Format("%.4f", p);
		m_str_FunInfo += "���ָ��� p="+str_temp;
		
		double bin_temp = 0;
		for (i=0; i<nVlen-1; i++)
		{
			for (j=vec_DisValue(i); j< vec_DisValue(i+1); j++)
				bin_temp += bindist(p, bin_n, j);
			vec_res(i) = fabs(vec_discum(i) - bin_temp);
		}
		vec_res(nVlen-1) = fabs(vec_discum(nVlen-1) - 1);
		vec_D(funcount) = vec_res.Max();
		if (vec_discum(nVlen-1) < 0.5)
		{
			vec_D(funcount) = 1.1E20;
			m_AllFuncInfo.Add("����ʽ�ֲ�\r\n �۲�ֵΪ������!!\r\n������Ч");
		}
		else
			m_AllFuncInfo.Add(m_str_FunInfo);
	}
	else if (m_bCheckBin)
	{
		funcount++;
		vec_D(funcount) = 1.1E20;
		m_AllFuncInfo.Add("����ʽ�ֲ�\r\n �����޷�����!!\r\n������Ч");
	}
//���㾭���ۼƺ����뼸�ηֲ�����֮�������ֵ
	if (m_bCheckGeo && ((mu > 0.0001 && mu < 10000) || m_bGetVoGeo))
	{
		funcount++;
		double p = 1/(mu+1);
		if (m_bGetVoGeo)//�ӽ��������
			p = m_Geo_p;
		m_str_FunInfo = "���ηֲ�\r\n";
		str_temp.Format("%.4f", p);
		m_str_FunInfo += "����p="+str_temp;

		for (i=0; i<nVlen; i++)
			vec_res(i) = fabs(vec_discum(i) - geocdf(vec_DisValue(i), p));
		vec_D(funcount) = vec_res.Max();
		if (vec_discum(nVlen-1) < 0.5)
		{
			vec_D(funcount) = 1.1E20;
			m_AllFuncInfo.Add("���ηֲ�\r\n �۲�ֵΪ������!!\r\n������Ч");
		}
		else
			m_AllFuncInfo.Add(m_str_FunInfo);
	}
	else if (m_bCheckGeo)
	{
		funcount++;
		vec_D(funcount) = 1.1E20;
		m_AllFuncInfo.Add("���ηֲ�\r\n �����޷�����!!\r\n������Ч");
	}
//���㾭���ۼƺ����벴�ɷֲ�����֮�������ֵ
	if (m_bCheckPoisson && ((mu > 0.001 && mu < 100) || m_bGetVoPoisson))
	{
		funcount++;
		double lambda = mu;
		if (m_bGetVoPoisson)
			lambda = m_Poisson_lambda;
		m_str_FunInfo = "���ɷֲ�\r\n";
		str_temp.Format("%.4f", lambda);
		m_str_FunInfo += "lambda="+str_temp;
		
		for (i=0; i<nVlen; i++)
			vec_res(i) = fabs(vec_discum(i) - poisscdf(vec_DisValue(i), lambda));
		vec_D(funcount) = vec_res.Max();
		if (vec_discum(nVlen-1) < 0.5)
		{
			vec_D(funcount) = 1.1E20;
			m_AllFuncInfo.Add("���ɷֲ�\r\n �۲�ֵΪ������!!\r\n������Ч");
		}
		else
			m_AllFuncInfo.Add(m_str_FunInfo);
	}
	else if (m_bCheckPoisson)
	{
		funcount++;
		vec_D(funcount) = 1.1E20;
		m_AllFuncInfo.Add("���ɷֲ�\r\n �����޷�����!!\r\n������Ч");
	}

/************************************************************************/
/*               ����Zֵ�Լ�sig                                         */
/************************************************************************/
	CIntVector vec_D_index;
	vec_D.Sort(vec_D_index);
	CDoubleVector vec_sig(totalfuncs);
	CDoubleVector vec_z = vec_D*sqrt(m_nRow);
	for (i=0; i<totalfuncs; i++)
	{
		if (vec_D(i) > 1E20)
		{
			vec_sig(i) = 0;
			vec_D(i) = -1;
			vec_z(i) = -1;
		}
		else if (vec_D(i) > -EPS_ACCUR)
			vec_sig(i) = CalcP(sqrt(m_nRow) * vec_D(i));
		else
			return FALSE;
	}

	m_vec_sig = vec_sig;
	m_vec_index = vec_D_index;
	m_vec_D = vec_D;
	m_vec_z = vec_z;
	return TRUE;
}

double CCKTest::CalcP(double Z)//�õ�Z=sqrt(nSize)*maxDiff֮��������KS����sig����
{
	if(Z < -EPS_ACCUR || Z > 10)
		return 0;
	if (Z <0.17)
		return 1.0;
	double sig = 0;
	for (int i=1; i < int(4+9.0/(Z)); i++)
		sig += 2*(pow(-1, i-1) * exp(-2*i*i*Z*Z));
	return sig;
}

void CCKTest::DrawOutput()
{
	CTLTable *pTable11 = NULL;
	CRsltElementTable *pETable11=NULL;
	
	pTable11 = new CTLTable;
	pTable11->CreateTable(2, 2);
	pTable11->SetTitle(m_str_x + "  ����������");
	pTable11->SetRows(2);
	pTable11->SetCols(2);
	pTable11->InsertColumn(0, "���� ����");
	pTable11->InsertColumn(1, "P ֵ");
	pTable11->InsertColumn(2, "D ֵ");
	pTable11->InsertColumn(3, "Z ֵ");

	int length = m_vec_sig.vlen();
	CTString str;
	CTString strValue;

	for(int i=0; i<m_vec_sig.vlen(); i++)
	{
		str.Format("%d", i + 1);
		pTable11->InsertItem(i, m_AllFuncInfo[m_vec_index(i)-1]);
		pTable11->SetItemText(i, 1, m_vec_sig(i));
		pTable11->SetItemText(i, 2, m_vec_D(i));
		pTable11->SetItemText(i, 3, m_vec_z(i));
	}
	pETable11 = new CRsltElementTable(m_str_x + "  ����������", pTable11);//����һ���������Ķ���
	m_pResult->Add(pETable11);
	return;
}

bool CCKTest::IsInt(double fValue)
{
	int nValue = int(fValue);    //����
	return (fValue-nValue >= -DOUBLE_ZERO && fValue-nValue <= DOUBLE_ZERO);
}
//ע��:vec_discum��ø���ֵ0�����������ɲ���Ԥ֪�Ĵ���
void CCKTest::CalcVec_DisCum(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, int nCase, int nStep, CDoubleVector &vec_discum, CDoubleVector &vec_DisValue)
{
	int i=0, j=0, k=0, nIndex=0;
	double fTmpData=0;

	int nVlen   = vec_x.vlen();
	if (vec_discum.vlen() != nCase) //����
	{
		vec_discum.create(nCase,0.0);
		vec_DisValue.create(nCase, 0);
	}

	if (m_bWithWeight)              //�����߼��ж�
	{
		for (i=0; i<nCase; i++)
		{
			if ((i+1)*nStep > nVlen)//�������
			{
				vec_discum(nIndex++) = 0;
				vec_DisValue(i) = int(vec_x(m_nRow - 1));
				continue;
			}
			for (j=i*nStep; j<(i+1)*nStep; j++)
			{
				if(IsInt(vec_x(j))) //�Ƿ�Ϊ����
				{
					vec_discum(nIndex) += vec_weight(vec_index(j)-1);
					vec_DisValue(nIndex) = vec_x(j);
				}	
			}
			if ((i+1)*nStep < nVlen)
			{
				for (j=i+1;j<nCase;j++)         //����һ������ͬ����
				{
					fTmpData = vec_x(j*nStep-1);//��ʱ����
					if ((j+1)*nStep>nVlen)      //����
					{
						vec_discum(nIndex++) = 0;
						vec_DisValue(i) = int(vec_x(m_nRow - 1));
						continue;
					}
					if (fTmpData-vec_x((j+1)*nStep) <= DOUBLE_ZERO && 
						fTmpData-vec_x((j+1)*nStep) >= -DOUBLE_ZERO)
					{
						for (k=j*nStep;k<(j+1)*nStep;k++)
						{
							if(IsInt(vec_x(k))) //�Ƿ�Ϊ����
							{
								vec_discum(nIndex) += vec_weight(vec_index(j)-1);
								vec_DisValue(nIndex) = vec_x(k);
							}	
						}
						i=j; //����һ����ʼ
					}
					else
					{
						break;
					}
				}
			}
			nIndex++;
		}
	}
	else
	{
		for (i=0; i<nCase; i++) 
		{
			if ((i+1)*nStep > nVlen)//�������
			{
				vec_discum(i) = 0;
				vec_DisValue(i) = int(vec_x(m_nRow - 1));
				continue;
			}
			for (j=i*nStep; j<(i+1)*nStep; j++)//��Ϊ������������������Բ������õ�ֵ���Ƚϣ���ֶκ���ͬ����ֵ�����ڲ���ͬһ�Σ��Ӷ���ɼ���Ĵ���
			{
				if(IsInt(vec_x(j))) //�Ƿ�Ϊ����
				{
					vec_discum(nIndex) += 1;
					vec_DisValue(nIndex) = vec_x(j);
				}
			}
			if ((i+1)*nStep < nVlen)
			{
				for (j=i+1;j<nCase;j++)         //����һ������ͬ����
				{
					fTmpData = vec_x(j*nStep-1);//��ʱ����
					if ((j+1)*nStep>nVlen)      //����
					{
						vec_discum(nIndex++) = 0;
						continue;
					}
					if (fTmpData-vec_x((j+1)*nStep-1) <= DOUBLE_ZERO && 
						fTmpData-vec_x((j+1)*nStep-1) >= -DOUBLE_ZERO)
					{
						for (k=j*nStep;k<(j+1)*nStep;k++)
						{
							if(IsInt(vec_x(k))) //�Ƿ�Ϊ����
							{
								vec_discum(nIndex) += 1;
								vec_DisValue(nIndex) = vec_x(k);
							}	
						}
						i=j; //����һ����ʼ
					}
					else
					{
						break;
					}
				}
			}
			nIndex++;
		}
	}
	//ƽ��
	vec_discum.resize(nIndex);
	vec_DisValue.resize(nIndex);
	vec_discum /= m_totalweight;
}
