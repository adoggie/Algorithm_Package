
/*******************************************************
 	name: 	alosvc.cpp
	func:	算法库独立服务入口
			多进程算法调度
	author:	zhangbin
	reversion:
			2006.10.16	zhangbin 
				1. created
*********************************************************/

#include "Interface.h"
#include "../Public/Packet.h"
#include "Manager.h"
#include <error.h>

#include "algsvc.h"
int sockSvc;

void RecvFromClient(int sock,DataBlock& result){
	int len = 1024;
	int r;
	char *buff = new char[len];
	result.clear();
	//back_insert_iterator<DataBlock> result;
	while( (r = recv(sock,buff,len,0))>=0 ){ // if return 0,indicated half-shutdown 
		result.resize( result.size()+ r);
		memcpy( &result.at(result.size()),buff,r);
	}
	delete[] buff;	
}

void SendBackClient(int sock,DataBlock& resp){
	int r;
	int len = resp.size();
	if( !len ) 	return;
	const unsigned char * first = &resp.at(0);
	while(1){
		r = send(sock,(void*)&first,len,0);
		if( r <= 0){
			return ;		
		}else{
			len -=r;
			first+=r;
		}
	}
}

void process(int sock){
	DataBlock   req,resp;
	RecvFromClient(sock,req);
	if(req.size()==0){
		close(sock);
		return ;
	}
	std::string command;		
	command.assign((char*)&req.at(0),req.size());
	
	TCPacket pgIn;
	TCManager mg;
	TCPacket pgOut;
	bool forked = false;
	pgIn.Import(( char*)command.c_str());
	mg.DealPacket(pgIn,pgOut,forked);
	if( forked ){	// parent space
		;
	}else{//child space or parent space
		int nRstLen = pgOut.GetPacketLength();
		unsigned char *pSour =new unsigned char[nRstLen];
		pgOut.Export((char*)pSour,nRstLen);	
		resp.assign(pSour,pSour+nRstLen);
		SendBackClient(sock,resp);
		close(sock);
	}
	
}


int main(int argc,char** argv){
	int ret;
	int sockClt;
	sockaddr_in sa;
	sockSvc = socket(AF_INET,SOCK_STREAM,0);
	sa.sin_family = AF_INET;
	sa.sin_port = htons( (unsigned short) AlgoServerPort);
	sa.sin_addr.s_addr = 0;
	//-- set sock_reused
	ret = bind(sockSvc,(sockaddr*)&sa,sizeof(sa));
	if( ret ){
		printf("service port:%d binded by other proecess!\n",AlgoServerPort);
		exit(-1);
	}
	ret = listen(sockSvc,5);
	if( ret ){
		perror("listen failed!");
		exit(-1);
	}
	socklen_t len;	
	while( 1 ){
		len = sizeof( sa);
		printf("waiting for request...\n");
		sockClt = accept(sockSvc,(sockaddr*)&sa,&len);
		if( sockClt <0 ){
			perror("service ocurred exception!\nSystem shutdown...");
			break;
		}
		printf("request from %s:%d\n","",0);		
		process(sockClt);
	}
	
	return 0;
}

