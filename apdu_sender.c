#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

char const *mynfc_device_name = "pn532_uart:/dev/ttyUSB0";
 
int CardTransmit(nfc_device *pnd,const uint8_t *sendapdu,const size_t sendapdulen, 
								 uint8_t *recapdu,const size_t *recapdulen){
	int res = 0,timeout=500; 
	size_t szPos;
	printf("=> ");
	for( szPos = 0; szPos<sendapdulen; szPos++ ){		// capdu -> send data
		printf("%02x ", sendapdu[szPos]);				
	}
	printf("\n");
	res = nfc_initiator_transceive_bytes(pnd,sendapdu,sendapdulen,recapdu,*recapdulen,timeout);
	if( res == NFC_EOVFLOW){
		printf("receive data is too large, need a larger recapdu[] array..\n");
		return -1;
	}else if( res <= 0 ){ // the byte count of received apdu data
		printf("send fail.error code:%d\n",res);
		return -1;
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

static void print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

int main(int argc, const char *argv[] ){
	nfc_device *pnd;
	nfc_target nt;
	nfc_context *context;
	nfc_init(&context);
	if( context == NULL ){
		printf("init libnfc failure\n");
		exit(EXIT_FAILURE);
	}
	const char *my_version = nfc_version();
	printf("libnfc version:%s\n", my_version);

	pnd = nfc_open(context,mynfc_device_name);
	if( pnd == NULL ){
		printf("can't open device %s\n", mynfc_device_name );
		exit(EXIT_FAILURE);
	}
	if( nfc_initiator_init(pnd) <0 ){
		nfc_perror(pnd,"initiator device failure\n");
		exit(EXIT_FAILURE);
	}

	printf("device %s opened and init to initiator successful!\n", mynfc_device_name);
	const nfc_modulation nmMifare = {
		.nmt = NMT_ISO14443A,
		.nbr = NBR_106
	};

	// select a tag like nmMifare and store the data into nt
	int selected_card_count = 0;
	while( 	nfc_initiator_select_passive_target(pnd,nmMifare,NULL,0,&nt) != 1 );
	printf("Target detected: uid:");
	print_hex(nt.nti.nai.abtUid,nt.nti.nai.szUidLen);


	uint8_t capdu[264];
	size_t capdulen;
	uint8_t rapdu[264];
	size_t rapdulen;

	memcpy(capdu,"\x00\xA4\x04\x00\x07\x88\x66\x88\x66\x88\x66\x88",12);
	capdulen = 12;
	rapdulen = sizeof(rapdu);
	int received_len = CardTransmit(pnd,capdu,capdulen,rapdu,&rapdulen);

	uint8_t ok[] = {0x90,0x88,0x88};
	if( Check_Hex(rapdu,received_len,ok,3) )
		printf("ok !!!\n");

	memcpy(capdu,"\x00\x00\x00",3);
	capdulen = 3;
	rapdulen = sizeof(rapdu);
	received_len = CardTransmit(pnd,capdu,capdulen,rapdu,&rapdulen);
	


	nfc_close(pnd);
	nfc_exit(context);
	exit(EXIT_SUCCESS);


}

