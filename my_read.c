#include <stdlib.h>
#include <nfc/nfc.h>

char const *mynfc_device_name = "pn532_uart:/dev/ttyUSB0";
// mynfc... is a pointer to const char


static void print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}
int main(int argc, const char *argv[]){
	nfc_device *pnd = NULL; 
	nfc_target nt;
	nfc_context *context;

	nfc_init(&context);
	if( context == NULL ){
		printf("nfc_init fail!!");
		exit(EXIT_FAILURE);
	}else{
		printf("nfc_init_success\n");
	}

	char const *acLibnfcVersion = nfc_version();
	printf("libnfc version:%s\n",acLibnfcVersion);

	pnd = nfc_open(context,mynfc_device_name);
	if( pnd == NULL ){
		printf("can't find nfc device:%s\n", mynfc_device_name);
		exit(EXIT_FAILURE);
	}else{
		printf("find and open nfc device:%s\n",mynfc_device_name);
	}

	// Initialize NFC device as initiator(reader)
	int return_code = nfc_initiator_init(pnd);
	printf("initialize device return code:%d, success:%d\n",return_code,return_code==0);

	// define what kind of tag you want to read:
	const nfc_modulation nmMifare = {
		.nmt = NMT_ISO14443A,				// enum nfc_modulation_type 
		.nbr = NBR_106						// enum nfc_baud_rate 
	};
	// out data 
	nfc_target ant[2];
	const size_t targetsize = 2;

	int success_find_tag_number = nfc_initiator_list_passive_targets(pnd,nmMifare,ant,targetsize);

	printf("success find tag number: %d\n", success_find_tag_number);
	if( success_find_tag_number>0 ){
		int i = 0;
		for(; i<success_find_tag_number; i++){
			printf("showing tag%d:\n\ttag uid:",i);
			nfc_target_info *info = &(ant[i].nti);		// nfc_target_info is union, and we specific NMT_ISO14443A, so zhe nfc_target_info is store nai which is type nfc_ios1443a_info
			//typedef struct {
			//  uint8_t  abtAtqa[2];
			//  uint8_t  btSak;
			//  size_t  szUidLen;
			//  uint8_t  abtUid[10];
			//  size_t  szAtsLen;
			//  uint8_t  abtAts[254]; // Maximal theoretical ATS is FSD-2, FSD=256 for FSDI=8 in RATS
			//} nfc_iso14443a_info;			
			// Atqa Sak Uid Ats -> confirm between Mifaretag and pn532 during the anti-collision
			// i dont't know what is "atqa sak ats" is used for , so just print the "uid" out..  
			print_hex(info->nai.abtUid,info->nai.szUidLen);
		}
	}

	nfc_close(pnd);
	nfc_exit(context);
	exit(EXIT_SUCCESS);
 
}