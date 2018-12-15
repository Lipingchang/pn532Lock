#include "master.h"
#include "apduBuffer.h"
#include "log.h"
#include <stdlib.h>

int main(){
	//loadLogFromFile();
	uint8_t outdata[200];

	//int p = getRecentLogByte(outdata,200);
	// int p = getALogByte(logQueue[0],outdata);

	// printALog(&logQueue[0]);
	//print_hex(outdata,p);	
	//printf("%d\n", p);
	//showRecentLog();

	// 1. 把　数据　和密码　写入文件　
	// 2. 调用python解密
	// 3. 查看返回值　256 -> 到文件中读取　客人的名字
	//				其他　-> 认证失败
	int  i = system("python3 aesdemo.py");
	if( i == 256 ){
		printf("succ\n");
	}
}