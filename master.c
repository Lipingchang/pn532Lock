#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <nfc/nfc.h>

#include "nfcTool.h"

typedef  struct masterStruct{
	uint8_t masterID;	
	char masterName[100];
	uint8_t masterPwd[100];
} Master;

#define MAX_MASTER 10

Master masterList[MAX_MASTER];
int masterSum = 0;

void initMaster();
bool addMaster(uint8_t,const char[],const char[]);
void showMaster(int i);
void showMasters();
bool checkMasterPwd(int id,const char inputpwd[]);


void initMaster(){

}
bool checkMasterPwd(int id,const char inputpwd[]){
	for( int i =0 ;i <masterSum ; i++ ){
		Master* p = &masterList[i];
		if( p->masterID == id){
			if( strcmp(inputpwd,(char*)p->masterPwd ) == 0){
				printf("master <%s> pwd check succes:%s\n", p->masterName,inputpwd);
				return true;
			}else{
				printf("master <%s> pwd check fail:%s\n", p->masterName,inputpwd );
				return false;
			}
		}
	}
	printf("master ID:<%d> not found\n", id);
	return false;
}
bool addMaster(uint8_t id,const char name[],const char pwd[]){
	if( masterSum+1 > MAX_MASTER ){
		printf("too much master!\n");
		return false;
	}

	Master* mm = &masterList[masterSum];
	mm->masterID = id;
	strcpy((char*)mm->masterName,name);
	strcpy((char*)mm->masterPwd,pwd);
	printf("master add:\t");
	showMaster(masterSum++);
	return true;
}
int generateID(){
	return masterSum+1;
}
void showMasters(){
	printf("show masters:\n");
	for( int i = 0; i<masterSum; i++){
		printf("\t");
		showMaster(i);
	}
	printf("\n");
}
void showMaster(int i){
	if( masterSum <= i ){
		printf("master <%d> not found, masterSum is %d \n", i,masterSum);
		return;
	}
	printf("No.%d Name:<%s> ID:<%d> PWD:<%s>\n", 
		i,
		masterList[i].masterName,
		masterList[i].masterID,
		masterList[i].masterPwd
	);
}
bool canAcceptMaster(){
	return MAX_MASTER>=(masterSum+1);
}
