#include "master.h"
#include "apduBuffer.h"

int main(){
	//printf("%d,%d,%d\n", sizeof(uint8_t),sizeof(char),sizeof(int)); <uint8_t> is 8bytes equals <char>'s size  
	initMaster();
	addMaster(1,"lipc","AABBCC");

	showMasters();
	checkMasterPwd(1,"AACC");
	checkMasterPwd(1,"AABBCC");
	
	uint8_t capdu[264];
	size_t capdulen;
	uint8_t rapdu[264];
	int rapdulen;

	// memcpy(rapdu,"\x02\x01\x01AABBCC",9);
	// memcpy(rapdu,"\x2\x1\x1AABBCC",9);
	apduBuffer buffer;
	initApduBuffer(&buffer);
	appendApduBufferByte(&buffer,Access_Request_Head);
	appendApduBufferByte(&buffer,Master_Mode);
	appendApduBufferByte(&buffer,1);
	appendApduBufferBytes(&buffer,(uint8_t*)"AABBCC",6);


	getApduArray( &buffer,rapdu,&rapdulen);
	char pwd[100];

	getPwdFromAccessRequest(rapdu,rapdulen,pwd);
	printf("%s\n", pwd);
	checkMasterPwd(rapdu[2],pwd);
}