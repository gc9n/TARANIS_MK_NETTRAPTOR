#include <Stdio.h>
#include "string.h"


void extractGpsInfo(char* menuStr,char* gpsInfo){
   strncpy(gpsInfo,menuStr+7,10);
   gpsInfo[10]='\0';
}

void decode64(char *RxdBuffer,unsigned char *ptrOut,int len) {
  unsigned char a,b,c,d; 
  unsigned char ptr = 0; 
  unsigned char x,y,z; 
	
  int ptrIn;
  ptrIn=0;
  ptrIn = 3; // skip first 3 chrs (#)[TYPE][Addr] 1234567893
  len=len-5;

  while(len) { 
	//printf("ptr : %d =>  chr: %d \n",ptrIn,RxdBuffer[ptrIn]);
    a = RxdBuffer[ptrIn++] - '='; 
    b = RxdBuffer[ptrIn++] - '='; 
    c = RxdBuffer[ptrIn++] - '='; 
    d = RxdBuffer[ptrIn++] - '='; 		
    x = (a << 2) | (b >> 4);
    y = ((b & 0x0f) << 4) | (c >> 2);
    z = ((c & 0x03) << 6) | d;
 // if(ptrIn > len - 2) break;
    if(len--) ptrOut[ptr++] = x; else break;
    if(len--) ptrOut[ptr++] = y; else break;
    if(len--) ptrOut[ptr++] = z; else break;
  }
}

void encode64(char *Data,char *TX_Buff, int Length){
    unsigned int pt = 0;
    unsigned char a,b,c;
    unsigned char ptr = 0;
    
    while(Length > 0){
      if(Length) { a = Data[ptr++]; Length--;} else a = 0;
      if(Length) { b = Data[ptr++]; Length--;} else b = 0;
      if(Length) { c = Data[ptr++]; Length--;} else c = 0;
      
      TX_Buff[pt++] = '=' + (a >> 2);
      TX_Buff[pt++] = '=' + (((a & 0x03) << 4) | ((b & 0xf0) >> 4));
      TX_Buff[pt++] = '=' + (((b & 0x0f) << 2) | ((c & 0xc0) >> 6));
      TX_Buff[pt++] = '=' + ( c & 0x3f);
    }
    TX_Buff[pt] = 0;
}

void addCRC(char *TXBuff,char *CRC){
    unsigned int tmpCRC = 0;
    int i;
    
    for(i = 0; i < strlen(TXBuff); i++){
        tmpCRC += TXBuff[i];
    }

    tmpCRC %= 4096;

    CRC[0] = '=' + tmpCRC / 64;
    CRC[1] = '=' + tmpCRC % 64;
    CRC[2] = '\r';
    CRC[3] = '\n';
    CRC[4] = '\0';
}

int checkCRC(char *t_InData, int Length){
    int CRC = 0;
    int i;
		
    if (t_InData[1] == 127)
        t_InData[1] = 0;

    for(i=0; i < Length-2; i++){
        CRC+=t_InData[i];
    }

    CRC = CRC % 4096;

    if(t_InData[Length - 2] != ('=' + (CRC / 64))){
        return 0;
    }

    if(t_InData[Length - 1] != ('=' + CRC % 64)){
        return 0;
    }

    return 1;
}




//char *ftoa(char *a, double f, int precision)
//{
//  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
//  
//  char *ret = a;
//  long heiltal = (long)f;
//  _itoa(heiltal, a, 10);
//  while (*a != '\0') a++;
//  *a++ = '.';
//  long desimal = abs((long)((f - heiltal) * p[precision]));
//  _itoa(desimal, a, 10);
//  return ret;
//} 



