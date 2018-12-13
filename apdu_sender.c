#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

#include "nfcTool.h"

#include "apduBuffer.h"
#include "master.h" 
#include "ERRORCode.h"
#include "log.h"

void startAuth();
void startRelate(uint8_t rapdu[],int len);


#define MAX_RAPDU_LEN 255 
uint8_t sapdu[ MAX_RAPDU_LEN ];
uint8_t rapdu[ MAX_RAPDU_LEN ];
int sendApduLen = 0;
int receiveApduLen = 0;

nfc_device *pnd;
nfc_target nt;
nfc_context *context;


#define ACCEPT_RELATE_BTN 1 

int main(int argc, const char *argv[] ){
	initDevice(&context,&pnd,&nt);
	initMaster();
	loadFromFile();
	loadLogFromFile();
	showRecentLog();
	
	while(1){
		waitPhoneCome(&pnd,&nt);

		// Send AID to choose an Android Service!
		sendApduLen = 12;
		memcpy(sapdu,"\x00\xA4\x04\x00\x07\x00\x66\x88\x66\x88\x66\x88",sendApduLen);
		receiveApduLen = CardTransmit(pnd,sapdu,sendApduLen,rapdu,MAX_RAPDU_LEN);

		// dispatch return data:
		if( rapdu[0] == start_auth_head ){
			startAuth();
		}else if( rapdu[0] == ByeBye_Head ){
			printf("device say bye byte ~\n");
		}else if( rapdu[0] == Start_Relate_Head ){
			startRelate( rapdu, receiveApduLen );
		}
	}

	ByebyeDevice(&context,&pnd);
	exit(EXIT_SUCCESS);
}

// 检查来的人的pwd
void startAuth(){

	// return     my Lock_ID to phone
	apduBuffer reply;
	initApduBuffer(&reply);
	appendApduBufferByte(&reply,Lock_ID_Head);
	appendApduBufferBytes(&reply,Lock_ID,Lock_ID_Len);
	getApduArray(&reply,sapdu,&sendApduLen);
	receiveApduLen = CardTransmit(pnd,sapdu,sendApduLen,rapdu,MAX_RAPDU_LEN);
	
	// check    the password which need to be matched with the Lock_ID;
	if( rapdu[0] == Access_Request_Head ){
		uint8_t Mode = rapdu[1];
		uint8_t MasterID = rapdu[2];

		if( Mode == Master_Mode ){
			printf("\tuser say he is a master\n" );
			char pwd[100];
			getPwdFromAccessRequest(rapdu,receiveApduLen,pwd);
			if( checkMasterPwd(MasterID,pwd) ){
				// send something back ..
				Master data;
				getMasterByID(MasterID,&data);
				addLog((char*)data.masterName,MasterID,Master_e,Pass_e);
			}else{
				// TODO not a master!
			}

		}else if( Mode == Guest_Mode ){
			printf("user is guest\n");
			exit(NOT_IMPLEMENT_ERROR);

		}else{
			printf("user is undefine,<Mode:%d>\n",Mode);
			addLog("unknow",0,Unknow_e,Reject_e);
		}

	}else{
		exit(ABNOREMAL_ROUTINE);
	}
}
// 
void startRelate(uint8_t rapdu[],int len){

	uint8_t pwdlen = rapdu[1];
	char pwd[100]; char name[100];
	if( pwdlen+2 < len){
		uint8_t i;
		for(  i = 0; i<pwdlen; i++ ){
			pwd[i] = rapdu[i+2];
		}
		pwd[i] = '\0';
		for(  i = 0; i<len-pwdlen-2 ; i++ ){
			name[i] = rapdu[i+pwdlen+2];
		}
		name[i] = '\0';
		printf("\trelate:pwd:<%s>,name:<%s>\n", pwd,name);
	}else{
		printf("Relate data struct error\n");
		exit( DATA_STRUCT_ERROR );
	}

	apduBuffer reply;
	initApduBuffer(&reply);

	if ( ACCEPT_RELATE_BTN ){
		if ( canAcceptMaster() ){
			uint8_t id = (uint8_t)generateID();
			if( addMaster(id,name,pwd) ){
				appendApduBufferByte(&reply,Relate_Accept_Head);
				appendApduBufferByte(&reply,id);
				appendApduBufferBytes(&reply,Lock_ID,Lock_ID_Len);
				getApduArray(&reply,sapdu,&sendApduLen);
				receiveApduLen = CardTransmit(pnd,
					sapdu,sendApduLen,
					rapdu,MAX_RAPDU_LEN);

			}else{
				exit(NOT_IMPLEMENT_ERROR);
			}
		}else{
			exit(NOT_IMPLEMENT_ERROR);
		}
	}else{
		exit(NOT_IMPLEMENT_ERROR);
	}
	
}

