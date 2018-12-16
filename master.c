#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <nfc/nfc.h>
#include <stdlib.h>

#include "nfcTool.h"
#include "PassinControl.h"
#include "log.h"


typedef  struct masterStruct{
	uint8_t masterID;	
	uint8_t masterName[100];
	uint8_t masterPwd[100];
} Master;

#define MAX_MASTER 10

Master masterList[MAX_MASTER];
int masterSum = 0;
const char SAVE_FILE_NAME[] = "/home/pi/Desktop/nfc_code/pn532Lock/master.txt";
const char AES_en_MID_FILE[] = "/home/pi/Desktop/nfc_code/pn532Lock/enAES.txt";
const char AES_de_MID_FILE[] = "/home/pi/Desktop/nfc_code/pn532Lock/deAES.txt";

void initMaster();
bool addMaster(uint8_t,const char[],const char[]);
void showMaster(int i);
void showMasters();
bool checkMasterPwd(int id,const char inputpwd[]);
void save2File();
Master getMasterByIndex(int i);

void initMaster(){

}
bool checkMasterPwd(int id,const char inputpwd[]){
	printf("\tchecking masterID:<%d>\n",id );
	for( int i =0 ;i <masterSum ; i++ ){
		Master* p = &masterList[i];
		if( p->masterID == id){
			if( strcmp(inputpwd,(char*)p->masterPwd ) == 0){
				printf("\tmaster <%s> pwd check succes:%s\n", p->masterName,inputpwd);
				return true;
			}else{
				printf("\tmaster <%s> pwd check fail:%s\n", p->masterName,inputpwd );
				return false;
			}
		}
	}
	printf("\tmaster ID:<%d> not found\n", id);
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
	save2File();
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
	Master master = getMasterByIndex(i);
	printf("No.%d Name:<%s> ID:<%d> PWD:<%s>\n", 
		i,
		master.masterName,
		master.masterID,
		master.masterPwd
	);
}
bool canAcceptMaster(){
	return MAX_MASTER>=(masterSum+1);
}
void fomatMaster(int i,char* outdata){ // TODO 数组边界检查
	if( i>=masterSum ){
		printf("format master No:<%d> fail,out of range\n", i);
		outdata[0] = '\0';
		return;
	}
	int outdatap = 0;
	strcpy(&outdata[outdatap], (char*) masterList[i].masterName);
	outdatap += strlen(  (char*) masterList[i].masterName );
	outdata[outdatap++] = '\n';
	strcpy(&outdata[outdatap], (char*) masterList[i].masterPwd);
	outdatap += strlen(  (char*) masterList[i].masterPwd );
	outdata[outdatap++] = '\n';
	char buffer[33];
	sprintf( buffer ,"%d",masterList[i].masterID);
	strcpy(&outdata[outdatap], (char*) buffer );
}
void save2File(){
	FILE *fp = NULL;
	fp = fopen( SAVE_FILE_NAME ,"w+" );
	fprintf(fp, "%d\n", masterSum);

	char oudata[255];
	for( int i =0; i<masterSum; i++ ){
		fomatMaster( i,oudata );
		fprintf(fp, "%s\n",oudata ); 
	}

	fclose(fp);
}
void loadFromFile(){
	FILE *fp = NULL;

	fp = fopen( SAVE_FILE_NAME , "r");
	char buffer[255];
	fscanf( fp, "%d\n", &masterSum );
	printf("masterLoad sum:%d\n",masterSum );
	
	for ( int i =0; i<masterSum; i++ ){
		size_t ll;
		fgets((char*)masterList[i].masterName,255,fp);
		masterList[i].masterName[strlen((char*)masterList[i].masterName)-1] = '\0';
		//printf("get:<%s>\n", masterList[i].masterName);
		
		// fgets((char*)masterList[i].masterName,255,fp);
		fgets((char*)masterList[i].masterPwd,255,fp);
		masterList[i].masterPwd[strlen((char*)masterList[i].masterPwd)-1] = '\0';
		
		fgets(buffer,255,fp);
		buffer[strlen((char*)buffer)-1] = '\0';
		masterList[i].masterID = atoi(buffer);
	}


}
Master getMasterByIndex(int i){
	if( masterSum <= i ){
		printf("master <%d> not found, masterSum is %d \n", i,masterSum);
		exit(ARRAY_OUTRANGE_ERROR);
	}
	return masterList[i];
}
bool getMasterByID(int id,Master* data){
	for( int i = 0; i<masterSum; i++ ){
		if( masterList[i].masterID == id ){
			*data = masterList[i];
			return true;
		}
	}
	return false;
}
bool checkGuestFromMaster(int id,uint8_t* pwd,int len){
	Master m;
	if( getMasterByID(id,&m) ){
		FILE *fp = NULL;
		fp = fopen( AES_en_MID_FILE ,"w+" );
		for( int i = 0; i < len; i++ ){
			fprintf(fp, "%c", pwd[i]);
		}

		fprintf(fp, "\n%s", m.masterPwd);
		fclose(fp);
		int  re = system("python3 aesdemo.py");
		if( re == 256 ){
			printf("guest pass succ\n");
			fp = fopen( AES_de_MID_FILE,"r");
			char guestname[100];
			fgets(guestname,100,fp);
			addLog(guestname,id,Guest_e,Pass_e);

			//调用开门
			pass();

			return true;
		}
	}
	return false;
}
