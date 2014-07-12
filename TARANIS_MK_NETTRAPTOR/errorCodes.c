#include "errorCodes.h"

void resolveError(int errorNum,char *errorBuf){

  
  switch(errorNum){
    case 0:
      strcpy(errorBuf,ERROR_CODE_0);
      break;
    case 1:
      strcpy(errorBuf,ERROR_CODE_1);
      break;
    case 2:
      strcpy(errorBuf,ERROR_CODE_2);
      break;
    case 3:
      strcpy(errorBuf,ERROR_CODE_3);
      break;
    case 4:
      strcpy(errorBuf,ERROR_CODE_4);
      break;
    case 5:
      strcpy(errorBuf,ERROR_CODE_5);
      break;
    case 6:
      strcpy(errorBuf,ERROR_CODE_6);
      break;
    case 7:
      strcpy(errorBuf,ERROR_CODE_7);
      break;
    case 8:
      strcpy(errorBuf,ERROR_CODE_8);
      break;
    case 9:
      strcpy(errorBuf,ERROR_CODE_9);
      break;
    case 10:
      strcpy(errorBuf,ERROR_CODE_10);
      break;
    case 11:
      strcpy(errorBuf,ERROR_CODE_11);
      break;
    case 12:
      strcpy(errorBuf,ERROR_CODE_12);
      break;
    case 13:
      strcpy(errorBuf,ERROR_CODE_13);
      break;
    case 14:
      strcpy(errorBuf,ERROR_CODE_14);
      break;
    case 15:
      strcpy(errorBuf,ERROR_CODE_15);
      break;
    case 16:
      strcpy(errorBuf,ERROR_CODE_16);
      break;
    case 17:
      strcpy(errorBuf,ERROR_CODE_17);
      break;
    case 18:
      strcpy(errorBuf,ERROR_CODE_18);
      break;
    case 19:
      strcpy(errorBuf,ERROR_CODE_19);
      break;
    case 20:
      strcpy(errorBuf,ERROR_CODE_20);
      break;
    default:
      errorBuf[0]='\0';
  }

}

