#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

#include "nfcTool.h"

#include "apduBuffer.h"
#include "master.h" 

 


int main(int argc, const char *argv[] ){
	nfc_device *pnd;
	nfc_target nt;
	nfc_context *context;

	
	initDevice(&context,&pnd,&nt);


	uint8_t capdu[264];
	size_t capdulen;
	uint8_t rapdu[264];
	size_t rapdulen;

	// Send AID
	memcpy(capdu,"\x00\xA4\x04\x00\x07\x00\x66\x88\x66\x88\x66\x88",12);
	capdulen = 12;
	rapdulen = sizeof(rapdu);
	CardTransmit(pnd,capdu,capdulen,rapdu,&rapdulen);

	// uint8_t ok[] = {0x90,0x88,0x88};
	// if( Check_Hex(rapdu,received_len,ok,3) )
	// 	printf("ok !!!\n");

	// 
	if( rapdu[0] == start_auth_head ){
		// return my Lock_ID to phone
		capdulen = 9;
		apduBuffer reply;
		initApduBuffer(&reply);
		appendApduBufferByte(&reply,Lock_ID_Head);
		appendApduBufferBytes(&reply,Lock_ID,8);
		int length;
		uint8_t replydata[256];
		getApduArray(&reply,replydata,&length);
		CardTransmit(pnd,replydata,length,rapdu,&rapdulen);
		
		// get the password which match the Lock_ID;
		if( rapdu[0] == Access_Request_Head ){
			uint8_t Mode = rapdu[1];
			uint8_t MasterID = rapdu[2];

			if( Mode == Master_Mode ){
				printf("user is master\n" );
				char pwd[100];
				getPwdFromAccessRequest(rapdu,rapdulen,pwd);

			}else if( Mode == Guest_Mode ){
				printf("user is guest\n");
			}else{
				printf("user is undefine,<Mode:%d>\n",Mode);
			}

		}

		// memcpy(capdu,Lock_ID,8);


	}

	// memcpy(capdu,"\x00\x00\x00",3);
	// capdulen = 3;
	// rapdulen = sizeof(rapdu);
	// received_len = CardTransmit(pnd,capdu,capdulen,rapdu,&rapdulen);


	ByebyeDevice(&context,&pnd);
	// nfc_close(pnd);
	// nfc_exit(context);
	exit(EXIT_SUCCESS);


}

