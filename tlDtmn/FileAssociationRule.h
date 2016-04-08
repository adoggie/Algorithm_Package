// FileAssociationRule.h: interface for the CFileAssociationRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEASSOCIATIONRULE_H__A57FE163_6FA5_4A32_881F_9531C460C6F2__INCLUDED_)
#define AFX_FILEASSOCIATIONRULE_H__A57FE163_6FA5_4A32_881F_9531C460C6F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

#include "SingleRuleReduction.h"
struct TTransaction
{
	long lCustomerID;
	long lProductID;
	TTransaction()
	{
		memset(this,0,sizeof(TTransaction));
	}
};

class GroupingInfo{
public :
	int dataType;
	int first;
	int last;
	GroupingInfo(){
		first=-1;
		last=-1;
	}
	GroupingInfo(int dataType,int first,int last){
		GroupingInfo();
		this->dataType=dataType;
		this->first=first;
		this->last=last;
	}
};
class GroupingIDs{
private:
	CPtrArray Vals;
	CPtrArray grpInfos;
	std::vector<int> IDs;
	int maxID;
	void increaseMaxID(){
		maxID++;
	}
public:
	int getMaxID(){
		if (maxID<0)
			maxID=0;
		return maxID;
	}

	void reset(){
		int i;
		for (i=0;i<grpInfos.GetSize();i++)
			delete (GroupingInfo*)grpInfos.GetAt(i);
		for (i=0;i<Vals.GetSize();i++)
			delete (TDataSet*)Vals.GetAt(i);

		Vals.RemoveAll();
		grpInfos.RemoveAll();
		IDs.clear();
	}

	void setGrpCount(int count){
		grpInfos.SetSize(count);
		for (int i=0;i<getGrpCount();i++){
			GroupingInfo* grpInfo=new GroupingInfo();
			grpInfo->first=-1;grpInfo->last=-1;grpInfo->dataType=-1;
			grpInfos.SetAt(i,grpInfo);
		}
			
	}
	int getGrpCount(){
		return grpInfos.GetSize();
	}
	void setDataType(int grpIndex,int dataType){
		if (grpIndex<getGrpCount())
			((GroupingInfo*)grpInfos.GetAt(grpIndex))->dataType=dataType;
	}
	int getIDIndex(int grpIndex,TDataSet data){
		if (grpIndex<getGrpCount()){
			GroupingInfo* grpInfo=(GroupingInfo*)grpInfos.GetAt(grpIndex);
			for (int i=grpInfo->first;i<=grpInfo->last&&i>=0;i++){
				if (*((TDataSet*)Vals.GetAt(i))==data)
					return i;
			}
		}
		return -1; 

	}
	int getID(int IDIndex){

		return IDIndex<0?(-1):IDs.at(IDIndex);
	}
	int getIDIndex(int ID){
		for (int i=0;i<IDs.size();i++){
			//if (IDs.at(IDs.begin()+i)==ID) //--UNIX PORT
			if (IDs.at(i)==ID) //--UNIX PORT
				return i;

		}
		return -1;

	}

	int getGrpIndex(int ID){
		int index=getIDIndex(ID);
		for (int i=0;i<getGrpCount();i++){
			GroupingInfo* grpInfo;
			grpInfo=(GroupingInfo*)grpInfos.GetAt(i);
			if (index>=grpInfo->first&&index<=grpInfo->last)
				return i;
		}
		return -1;

	}
	TDataSet* getVal(int index){
		return (TDataSet*)Vals.GetAt(index);
	}
	int getGrpDataType(int grpindex){
		
		return grpindex<0?(-1):((GroupingInfo*)grpInfos.GetAt(grpindex))->dataType; 
	}

	void add(int grpIndex,int dataType,TDataSet data){
		if (grpIndex>=0&&grpIndex<getGrpCount()){
			TDataSet* newData=new TDataSet();
			*newData=data;
			GroupingInfo* grpInfo=(GroupingInfo*)grpInfos.GetAt(grpIndex);
			if (grpInfo->first<0||grpInfo->last<0){
				if (grpIndex<=0){
					grpInfo->first=0;
					grpInfo->last=0;
				}
				else{
					grpInfo->first=((GroupingInfo*)grpInfos.GetAt(grpIndex-1))->last+1;
					grpInfo->last=grpInfo->first;				
				}
			}
			else
				grpInfo->last++;

			grpInfo->dataType=dataType;
			Vals.InsertAt(grpInfo->first, newData);
			IDs.insert(IDs.begin()+grpInfo->first,getMaxID()+1);
			increaseMaxID();
			//first one
			if (Vals.GetSize()==1){
				return;
			}
			//adjust groups
			for (int i=grpIndex+1;i<getGrpCount();i++){
				GroupingInfo* grpInfo=(GroupingInfo*)grpInfos.GetAt(i);
				if (grpInfo->first>=0&&grpInfo->last>=0){
					grpInfo->first++;
					grpInfo->last++;
				}
			}

		}
	}


};

class CFileAssociationRule : public CSingleRuleReduction  
{
	int m_nCustomCount;//客户个数   //#_S 2004-11-25 zlq
public:
	CFileAssociationRule();
	virtual ~CFileAssociationRule();
	
	//	void StartCalculate();		
	void	SetDataParam(CRuleTrainVO	*pVO);	
	bool	CFileAssociationRule::SetDataParam(CTString StrVO);
	void	SetDataInterface(CDataInterface *pDataInterface, CPtrArray *parField);
	void CreateFrequentSet(CRuleTrainVO * pt,bool bString);
	CResult* CreateRule(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt);
	CResult* Predict(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt);
	//	int GetValidData(CDataInterface *pDataInterface,int nCustCol,int nProductCol,TDataSet *&pCustArray,TDataSet *&pProductArray);
//	int GetValidData(CDataInterface *pDataInterface,int nCustCol,int nProductCol,TDataSet *&pCustArray,TDataSet *&pProductArray,bool bString);
	int GetValidData(CDataInterface *pDataInterface,int nCustCol,int nProductCol,TDColAry &CustomArray,TDColAry &ProductArray);
	
//	void prshl(TDataSet *p, int n,bool reSort);   //实数的shell（希尔）排序
//	void prshl(TDColAry &ColAry, int n,bool reSort);   //实数的shell（希尔）排序
	void MergeSort(TDColAry &ColAry, int nTotalValidCount);
	
//	int RemoveDuplicate(TDataSet *pCustom,TDataSet *pProduct,int count);
//	int RemoveDuplicate(TDColAry &CustomArray,TDColAry &ProductArray,int count);
//	int RemoveDuplicate_quick(TDColAry &CustomArray,TDColAry &ProductArray,int count);
	// int RemoveDuplicate_quick_test(TDColAry &CustomArray,TDColAry &ProductArray,int count,void * pt);
	int RemoveDuplicate_quick_test(TDColAry &CustomArray,TDColAry &ProductArray,int count);
	
	
//	bool Aproiri_gen12(TDataSet* pCust,TDataSet * pProduct,int nRecCount, CPtrArray *par, int nk, int mk, CPtrArray *parCk,void * lp,bool * pStop,int nMiniSupport);
	bool Aproiri_gen12(TDColAry &CustomArray,TDColAry &ProductArray,int nRecCount, CPtrArray *par, int nk, int mk, CPtrArray *parCk/*,void * lp*/,bool * pStop,int nMiniSupport);
	
//	void InXthenCount(TDataSet* pCust,TDataSet * pProduct,int nRecCount,CPtrArray *par,void * lp = NULL,bool * pbStop = NULL );
	void InXthenCount(TDColAry &CustomArray,TDColAry &ProductArray,int nRecCount,CPtrArray *par/*,void * lp = NULL*/,bool * pbStop = NULL );
	//	void InXthenCount(CPtrArray * par,void * lp = NULL,bool * pbStop = NULL);
	

	GroupingIDs grouper;
	void WritePtrData(CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult,bool bString);
//	void WritePtrData(CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult=NULL);
//	void Confidence(CPtrArray *par,double mini_confidence,CResult * pResult,bool * pbStop);
	void Confidence(CPtrArray *par,double mini_confidence,CResult * pResult,bool * pbStop,bool bString);

private:
	inline void CountItem(CTArray<long,long> *parClient,long* pValArray,int * pFreqArray,int nRowNum,bool* bStop);
//	inline void CountItem(CPtrArray * parClient,CPtrArray * par,bool *pStop);
	inline void CountItem(CTArray<long,long> *parClient, CPtrArray *par,bool *pStop);
	int FindLongVal(long lVal,long * pVal,int nCount);

//	CWinThread * m_pPrcoessThread;
	CTString m_strFileName;
	
public: bool IsInOneFrequentSet(CPtrArray * par,TTransaction * pRecord);
	CPtrArray m_arProduct;
	void SortItem(CPtrArray * par);


protected:
//	int GetDifferCount(TDataSet *pCustom,int nCount,int nCustCol ,CDataInterface *pDataInterface);
};

#endif // !defined(AFX_FILEASSOCIATIONRULE_H__A57FE163_6FA5_4A32_881F_9531C460C6F2__INCLUDED_)

