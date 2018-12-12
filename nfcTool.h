#ifndef _NFC_TOOL_H_
#define _NFC_TOOL_H_ 1

#include "ERRORCode.h"

char const *mynfc_device_name = "pn532_uart:/dev/ttyUSB0";
uint8_t const *Lock_ID = ( uint8_t* ) "\x88\x01\x02\x03\x04\x05\x06\x88"; // 8 byte Lock ID
const int Lock_ID_Len = 8;
const uint8_t start_auth_head =  			0x90;
const uint8_t Access_Request_Head =         0x02;
const uint8_t Lock_ID_Head =                0x01;
const uint8_t Access_Reply_Head =           0x03;
const uint8_t Get_Recent_Record_Head =       0x04;
const uint8_t Send_Recent_Access_Record =    0x05;
const uint8_t Send_Recent_Refuse_Record =    0x06;
const uint8_t Send_Recent_Key_Access_Record = 0x07;
const uint8_t ByeBye_Head =                	0x08;

const uint8_t Start_Auth_Head =             0x90;
const uint8_t Receive_Fuck_Head =           0x99;
const uint8_t Receive_OK_Head   =           0x66;
const uint8_t  AID_Head  =                 0x00;

const uint8_t Master_Mode = 0x01;
const uint8_t Guest_Mode = 0x02;
const uint8_t Start_Relate_Head = 0x78;
const uint8_t Relate_Accept_Head = 0x79;
const uint8_t Relate_Result_Head = 0x80;

// 返回接受到的长度
int CardTransmit(nfc_device *pnd,
	const uint8_t *sendapdu, const int sendapdulen,  
	uint8_t 	  *recapdu,  int 	   maxreceivelen)
{
	int res = 0,timeout=1000; 
	size_t szPos,slen = sendapdulen,rlen = maxreceivelen;
	printf("=> ");
	for( szPos = 0; szPos<sendapdulen; szPos++ ){		// capdu -> send data
		printf("%02x ", sendapdu[szPos]);	
	}
	printf("\n");

	res = nfc_initiator_transceive_bytes(pnd,
		sendapdu, slen,
		recapdu,  rlen,
		timeout);

	if( res == NFC_EOVFLOW){
		printf("receive data is too large, need a larger recapdu[] array..\n");
		exit(RECEIVE_OVERFLOW);
	}else if( res <= 0 ){ // the byte count of received apdu data
		printf("send fail error code:%d\n",res);
		exit(SEND_APDU_FAIL);
	}else{
		printf("<= ");
		for( szPos=0; szPos<res; szPos++ ){
			printf("%02x ",recapdu[szPos]);
		}
		printf("\n");
		return res;
	}
}
bool Check_Hex(const uint8_t *input,const size_t input_len,const uint8_t *checkdata,const size_t checkdatalen){
	// printf("%d,%d\n",input_len,checkdatalen);
	if( checkdatalen != input_len )
		return false;
	for( int i = 0; i<checkdatalen; i++){
		// printf("|%02x <-> %02x|",input[i],checkdata[i]);
		if( input[i]!=checkdata[i] ){
			// printf("fuck");
			return false;
		}
		// printf("\n");
	}
	return true;
}
static void print_hex(const uint8_t *pbtData, const size_t szBytes){
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}
void print_hex(char *data){
	char* p = data;
	while(*p != '\0'){
		printf("%02x ",*p);
		p++;
	}
	printf("\n");
}

void initDevice(nfc_context **context_addr,nfc_device **pnd_addr,nfc_target *nt_addr){

	nfc_init(context_addr);
	if( *context_addr == NULL ){
		printf("init libnfc failure\n");
		exit(EXIT_FAILURE);
	}
	const char *my_version = nfc_version();
	printf("libnfc version:%s\n", my_version);

	*pnd_addr = nfc_open(*context_addr,mynfc_device_name);
	if( *pnd_addr == NULL ){
		printf("can't open device %s\n", mynfc_device_name );
		exit(EXIT_FAILURE);
	}
	if( nfc_initiator_init(*pnd_addr) <0 ){
		nfc_perror(*pnd_addr,"initiator device failure\n");
		exit(EXIT_FAILURE);
	}

	printf("device %s opened and init to initiator successful!\n", mynfc_device_name);

}
void ByebyeDevice(nfc_context **context_addr,nfc_device **pnd_addr ){
	nfc_close(*pnd_addr);
	nfc_exit(*context_addr);
	//exit(EXIT_SUCCESS);
}
void waitPhoneCome(nfc_device **pnd_addr, nfc_target *nt_addr){	
	const nfc_modulation nmMifare = {
		.nmt = NMT_ISO14443A,
		.nbr = NBR_106
	};

	// select a tag like nmMifare and store the data into nt
	int selected_card_count = 0;
	while( 	nfc_initiator_select_passive_target(*pnd_addr,nmMifare,NULL,0,nt_addr) != 1 );
	printf("Target detected: uid:");
	print_hex((*nt_addr).nti.nai.abtUid,(*nt_addr).nti.nai.szUidLen);

}
int getPwdFromAccessRequest(uint8_t* input,int input_len,char *pwd ){
	// pwd must big enough to contain the pwd!!
	int count = 0;
	for( int i = 3; i<input_len;i++ ){
		pwd[count++] = input[i];
	}
	pwd[count] = '\0';
	printf("find pwd:<%s> len:<%d>\n", pwd,count);
	return count;
}

#endif
