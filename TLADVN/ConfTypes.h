
class SmoothConf
{
public:
	double m_dbFrom;
	double m_dbTo;
	double m_dbStep;
	SmoothConf()
	{
		m_dbFrom=0;
		m_dbTo=0;
		m_dbStep=0;
	}
	SmoothConf(double dbFrom,double dbTo,double dbStep)
	{
		m_dbFrom=dbFrom;
		m_dbTo=dbTo;
		m_dbStep=dbStep;
	}
	SmoothConf& operator=(SmoothConf& t)
	{
		m_dbFrom = t.m_dbFrom;
		m_dbTo = t.m_dbTo;
		m_dbStep = t.m_dbStep;

		return *this;
	}
	

};

class SmoothSetting
{
public:
	double m_dbalpha;
	double m_dbdelta;
	double m_dbgamma;
	double m_dbphi;
	SmoothSetting(double dbalpha,double dbdelta,double dbgamma,double dbphi)
	{
		m_dbalpha=dbalpha;
		m_dbdelta=dbdelta;
		m_dbgamma=dbgamma;
		m_dbphi=dbphi;

	}



};
