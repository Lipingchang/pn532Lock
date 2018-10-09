#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

#include "nfcTool.h"

#include "apduBuffer.h" 

 


int main(int argc, const char *argv[] ){
	nfc_device *pnd;
	nfc_target nt;
	nfc_context *context;

	cout.set
	initDevice(&context,&pnd,&nt);


	uint8_t capdu[264];
	size_t capdulen;
	uint8_t rapdu[264];
	size_t rapdulen;

	// Send AID
	memcpy(capdu,"\x00\xA4\x04\x00\x07\x00\x66\x88\x66\x88\x66\x88",12);
	capdulen = 12;
	rapdulen = sizeof(rapdu);
	int received_len = CardTransmit(pnd,capdu,capdulen,rapdu,&rapdulen);

	// uint8_t ok[] = {0x90,0x88,0x88};
	// if( Check_Hex(rapdu,received_len,ok,3) )
	// 	printf("ok !!!\n");

	if( rapdu[0] == start_auth_head ){
		// return my Lock_ID to phone
		capdulen = 9;
		apduBuffer reply;
		initApduBuffer(&reply);
		appendApduBufferByte(&reply,Lock_ID_Head);
		appendApduBufferBytes(&reply,Lock_ID,8);
		int length;
		uint8_t* k;
		getApduArray(&reply,&k,&length);
		received_len = CardTransmit(pnd,k,length,rapdu,&rapdulen);

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

