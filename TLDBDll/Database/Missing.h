#ifndef  ERICDONG_CMISSING_0925
#define	 ERICDONG_CMISSING_0925

typedef byte			CMissingType;
#define mtNoMissing		0x01
#define mtDiscrete		0x02
#define msRange			0x03

class CField;
class CValueLabel;

class AFX_EXT_CLASS CMissing
{
private:
	CMissingType		m_byteType;

	double				m_dDataA;
	double				m_dDataB;
	double				m_dDataC;

	CTString				m_strDataA;
	CTString				m_strDataB;
	CTString				m_strDataC;

public:
	int					m_nCount;
	CField*				m_pField;
	CMissing(CField * pField);
	~CMissing();
	CMissingType		GetMissingType() ; 
	int					GetCount() ; 
	bool				SetMissingType( CMissingType nType) ; 
	bool				LoadFromBuffer(LPBYTE& lpData) ; 
	int					SaveToBuffer(LPBYTE lpData) ; 
	int					QueryLength() ; 
	void				Clone(CMissing * pMissing) ; 

	int					GetDoubleData(double & DataA,double & DataB,double & DataC) ; 
  
	bool				SetDoubleData(int nCount,double DataA,double DataB,double DataC) ; 

	int					GetStrData(CTString &DataA,CTString &DataB,CTString &DataC); 

	bool				SetStrData(int nCount,CTString DataA,CTString DataB,CTString DataC); 

};

#endif
