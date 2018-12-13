#include "master.h"
#include "apduBuffer.h"
#include "log.h"

int main(){
	loadLogFromFile();
	uint8_t outdata[200];

	int p = getRecentLogByte(outdata,200);
	// int p = getALogByte(logQueue[0],outdata);

	// printALog(&logQueue[0]);
	print_hex(outdata,p);	
	printf("%d\n", p);
	showRecentLog();
}