/*
Author George Chatzisavvidis.
Projects that used for creation of Taranis-Mikrokopter project was
Altastation and MavlinkFrsky.



*/

#include <stdint.h>
#include <Stdio.h>
#include <stdlib.h>

#include "FrSkySPort.h"
#include "altapix.h"
#include "altatools.c"
#include "errorCodes.c"

//#define debugSerial           Serial
#define _MKSerial            Serial2
#define START                   1
#define MSG_RATE            10              // Hertz

// ******************************************
// Message #0  HEARTHBEAT 
uint8_t    ap_type = 0;
uint8_t    ap_autopilot = 0;
uint8_t    ap_base_mode = 0;
uint32_t  ap_custom_mode = 0;
uint8_t    ap_system_status = 0;
uint8_t    ap_mavlink_version = 0;

// Message # 1  SYS_STATUS 
uint16_t  ap_voltage_battery = 0;    // 1000 = 1V
int16_t    ap_current_battery = 0;    //  10 = 1A

// Message #24  GPS_RAW_INT 
uint8_t    ap_fixtype = 3;                  //   0= No GPS, 1 = No Fix, 2 = 2D Fix, 3 = 3D Fix
uint8_t    ap_sat_visible = 0;           // numbers of visible satelites
// FrSky Taranis uses the first recieved lat/long as homeposition. 
int32_t    ap_latitude = 0;              // 585522540;
int32_t    ap_longitude = 0;            // 162344467;
int32_t    ap_gps_altitude = 0;        // 1000 = 1m


// Message #74 VFR_HUD 
int32_t    ap_airspeed = 0;
uint32_t  ap_groundspeed = 0;
uint32_t  ap_heading = 0;
uint16_t  ap_throttle = 0;

// FrSky Taranis uses the first recieved value after 'PowerOn' or  'Telemetry Reset'  as zero altitude
int32_t    ap_bar_altitude = 0;    // 100 = 1m
int32_t    ap_climb_rate=0;        // 100= 1m/s

// Message #27 RAW IMU 
int32_t   ap_accX = 0;
int32_t   ap_accY = 0;
int32_t   ap_accZ = 0;

int32_t   ap_accX_old = 0;
int32_t   ap_accY_old = 0;
int32_t   ap_accZ_old = 0;

// ******************************************
// These are special for FrSky
int32_t   adc2 = 0;               // 100 = 1.0V
int32_t     vfas = 0;                // 100 = 1,0V
int32_t     gps_status = 0;     // (ap_sat_visible * 10) + ap_fixtype
                                             // ex. 83 = 8 sattelites visible, 3D lock 
uint8_t   ap_cell_count = 4;

// ******************************************
uint8_t     MavLink_Connected;
//uint8_t     buf[MAVLINK_MAX_PACKET_LEN];

uint16_t  hb_count;

unsigned long MavLink_Connected_timer;
unsigned long hb_timer;
unsigned long acc_timer;

int led = 13;

///////////////////////////////
char commandLine[maxFrameLen+1];
char debugString[200];
int status = 0;
char cmdStringRequest[30];
unsigned char rawDataDecoded[200];
int checkVersionStatus = 0;
 int statusReadCommandLine = 0;
 
 

int nbCrcError = 0;
int cmdSend = 0;
int currentMenuIndex = 0;
int serialPinTx = 10;
int serialPinRx = 9;
char cmdName[10];
char gpsInfo[11];
int menuIndex; 
char menuName[25];
////////////////////////////////


void setup()
{  
 
  FrSkySPort_Init();
  Serial.begin(57600);
   _MKSerial.begin(57600);
  pinMode(led,OUTPUT);
  pinMode(12,OUTPUT);
  
  pinMode(14,INPUT);
  analogReference(DEFAULT);
 
  redirectUartNc(); 
  sprintf(cmdName,"OSD");
}






void loop() {

 
   digitalWrite(led,LOW);
     delay(10);
     
   currentMenuIndex = 0;
    FrSkySPort_Process(); 
   digitalWrite(led,HIGH);
     //PUSH TO TARANIS
  
 //sprintf(cmdName,"OSD");
  if(!cmdSend){ // we dont need to send the command to mk on each loop , the mk is instructed to output data on regular basis    
    // we need to fetch OSD data  
    if(!checkVersionStatus){
      sprintf(cmdName,"OSD");
    }

    makeCmdString(cmdName,cmdStringRequest);
    Serial.println(cmdStringRequest);
    Serial2.println(cmdStringRequest);
  }

  // Mk is sending the infos for OSD we need to read them and build the GUI
  statusReadCommandLine=readCommandLine(commandLine);
  if(statusReadCommandLine == 1){

    // we have a frame lets check CRC
    int crcStatus = checkCRC(commandLine,strlen(commandLine));
    if(!crcStatus){
      if(nbCrcError>10){
        Serial.println("D: BrokenCRC");
        nbCrcError = 0;
      }
      cmdSend = 0; // we set it to 0 to force the commandIssue on the next Loop
    }
    else{
      nbCrcError = 0;
      // we have a frame with valid CRC lets check the type.
      char dataType = readDataType(commandLine);   

      if(dataType == '\0'){
        cmdSend = 0; // we set it to 0 to force the commandIssue on the next Loop
      } 
      else if (dataType == 'O'){ // we have OSD data its cool its what we want.
        //cmdSend = 1; /// this must be explained later
        // we decode the data
        decode64(commandLine,rawDataDecoded,strlen(commandLine)); //? add decode status..? 
        s_MK_NaviData NaviData;
       
        memcpy((unsigned char *)&NaviData, (unsigned char *)&rawDataDecoded, sizeof(NaviData));

        // cool we are done , we have osdData Struct an we can construct ANY gui we want .
        //1234567890123456 
        //B: 16.17 Sat: 5
        //A: 190 V: +100
        // diviser par 22.5

         //char line1[20];
        // char line2[20];

//        if(NaviData.Errorcode > 0){
//          // error detected
//          char errorCode[40];
//          resolveError((int)  NaviData.Errorcode, errorCode);
//          lcdClearLine(1);
//          Serial.println("_->  ERROR  <-_");
//          Serial.println(errorCode);
//          beepMe(ERROR_BEEP,2);
//        }

//        if(NaviData.UBat < MINIMUM_VOLTAGE_BEFORE_ALERT*10){
          // error detected
//          Serial.println("-> LOW BATT <-");
//          sprintf(line2,"     %2i.%1iv     ",(int)(NaviData.UBat/10), (int) (NaviData.UBat%10));
//          Serial.println(line2);
//          beepMe(ERROR_BEEP,2);
//        }

  
         //-----------------PUSHING DATA FROM MK TO TARANIS------------------....
         //GEORGE CHATZISAVVIDIS 9/7/14...
         
          //ap_voltage_battery=NaviData.UBat/10;
          //ap_bar_altitude=(NaviData.Altimeter / MK_ALTI_FACTOR);
          //ap_groundspeed=(NaviData.GroundSpeed/100);
          //ap_current_battery=NaviData.Current/10;

          ap_voltage_battery=NaviData.UBat;
          ap_bar_altitude=(NaviData.Altimeter );
          ap_groundspeed=(NaviData.GroundSpeed);
          ap_current_battery=NaviData.Current;
          ap_sat_visible=NaviData.SatsInUse;
          ap_heading=NaviData.CompassHeading;
          ap_climb_rate=NaviData.Variometer;
          ap_custom_mode=NaviData.UsedCapacity;
          ap_throttle=NaviData.Gas;
          ap_gps_altitude=NaviData.Altimeter;
          ap_base_mode=NaviData.Errorcode;
      
          if (NaviData.SatsInUse>=5)
          {ap_fixtype=3;
          }
          
           
////  Serial.print("Distance"); Serial.println(Distance.Distance);
//// Serial.print("TargetPositionDeviation"); Serial.println(GPS_PosDev_t.TargetPositionDeviation);
// Serial.print("FCFlags"); Serial.println(NaviData.FCFlags,BIN);
// Serial.print("FCFlags"); Serial.println(NaviData.FCFlags,DEC);
// Serial.print("FCFlags"); Serial.println(NaviData.FCFlags,OCT);
// Serial.print("NCFlags"); Serial.println(NaviData.NCFlags);
// //////////////////////
//           sprintf(line1,"B: %2i.%1iv Sat: %i",(int)(NaviData.UBat/10), (int) (NaviData.UBat%10) ,NaviData.SatsInUse);
//           sprintf(line2,"A: %im  V: %i",(int)(NaviData.Altimeter / MK_ALTI_FACTOR),NaviData.Variometer);
//          
//          Serial.println(line1);
//          Serial.println(line2);      
//          // Main Menu
//          sprintf(line1,"B: %2i.%1iv A:%im",(int)(NaviData.UBat/10), (int) (NaviData.UBat%10) ,(int)(NaviData.Altimeter / MK_ALTI_FACTOR));
//          sprintf(line2,"S:%s[%i]",gpsInfo,NaviData.SatsInUse);
//          Serial.println(line1);
//          Serial.println(line2);
//          sprintf(line1,"Current: %i.%i A",(int)(NaviData.Current/10), (int) (NaviData.Current%10) );
//          sprintf(line2,"UsedC: %i mAh",(int)(NaviData.UsedCapacity));
//          
//          Serial.println(line1);
//          Serial.println(line2);
//          sprintf(line1,"GSpeed: %i m/s",(int)(NaviData.GroundSpeed/100) );
//          sprintf(line2,"Heading: %i ",(int) NaviData.CompassHeading);                    
//           Serial.println(line1);
//          Serial.println(line2);
//           
//          sprintf(line1,"RcQual: %i ",(int)(NaviData.RC_Quality) );          
//          sprintf(line2,"RSSI: %i ",NaviData.RC_RSSI);                    
//          Serial.println(line1);
//          Serial.println(line2);
//           
 
        //------------------------------------------------------------------------------------
        //--------------------------------MK DATA TO SEND-------------------------------------
        //------------------------------------------------------------------------------------
        //          GPS_Pos_t    CurrentPosition;		// see ubx.h for details
        //          GPS_Pos_t    TargetPosition;
        //          GPS_PosDev_t TargetPositionDeviation;
        //          GPS_Pos_t    HomePosition;
        //          GPS_PosDev_t HomePositionDeviation;
        //          WaypointIndex;		// index of current waypoints running from 0 to WaypointNumber-1
        //          WaypointNumber;		// number of stored waypoints
        //          SatsInUse;		// number of satellites used for position solution
        //          Altimeter; 		// hight according to air pressure
        //          Variometer;		// climb(+) and sink(-) rate
        //          FlyingTime;		// in seconds
        //          UBat;			// Battery Voltage in 0.1 Volts
        //          GroundSpeed;		// speed over ground in cm/s (2D)
        //          Heading;		// current flight direction in ° as angle to north
        //          CompassHeading;		// current compass value in °
        //          AngleNick;		// current Nick angle in 1°
        //          AngleRoll;		// current Rick angle in 1°
        //          RC_Quality;		// RC_Quality
        //          FCFlags;		// Flags from FC
        //          NCFlags;		// Flags from NC
        //          Errorcode;		// 0 --> okay
        //          OperatingRadius;               // current operation radius around the Home Position in m
        //          TopSpeed;		// velocity in vertical direction in cm/s
        //          TargetHoldTime;		// time in s to stay at the given target, counts down to 0 if target has been reached
        //          RC_RSSI;		// Receiver signal strength (since version 2 added)
        //          SetpointAltitude;			// setpoint for altitude
        //          Gas;						// for future use
        //          Current;					// actual current in 0.1A steps
        //          UsedCapacity;				// used capacity in mAh
        //
        
        
        //This is the data we send to FrSky, you can change this to have your own
        //set of data
        //******************************************************
        //Data transmitted to FrSky Taranis:
        //Cell           ( Voltage of Cell=Cells/4. [V] This is my LiPo pack 4S ) 
        //Cells         ( Voltage from LiPo [V] )
        //A2             ( Analog voltage from input A0 on Teensy 3.1 )
        //Alt             ( Altitude from baro.  [m] )
        //GAlt          ( Altitude from GPS   [m])
        //HDG         ( Compass heading  [deg])
        //Rpm         ( Throttle when ARMED [%] )
        //AccX         ( AccX m/s ? )
        //AccY         ( AccY m/s ? )
        //AccZ         ( AccZ m/s ? )
        //VSpd        ( Vertical speed [m/s] )
        //Speed      ( Ground speed from GPS,  [km/h] )
        //T1            ( GPS status = ap_sat_visible*10) + ap_fixtype )
        //T2            ( ARMED=1, DISARMED=0 )
        //Vfas          ( same as Cells )
        //Longitud    
        //Latitud
        //Dist          ( Will be calculated by FrSky Taranis as the distance from first received lat/long = Home Position
        //
        //******************************************************
        //delay(200);
        sprintf(cmdName,"D2");

      }
//      else if (dataType == 'V'){ // we have VERSION data we need to display version and set version check and maybe check the version compatibility
//
//
//        decode64(commandLine,rawDataDecoded,strlen(commandLine)); //? add decode status..? 
//        str_VersionInfo VersionInfo;
//        memcpy((unsigned char *)&VersionInfo, (unsigned char *)&rawDataDecoded, sizeof(VersionInfo));
//        char line1[20];
//        char line2[20];
//
//        sprintf(line1,"Version ");
//        sprintf(line2,"Nc: %u.%u",VersionInfo.SWMajor,VersionInfo.SWMinor);
//       // lcdClearLine(1);
//        //Serial.println(line1);
//        //Serial.println(line2);
//
//       // delay(2000);
//        cmdSend = 0; // tel him to resend dataRequest
//        checkVersionStatus = 1; 
//        sprintf(cmdName,"D2");
//
//      } 
//      else if (dataType == 'L'){ 
//        decode64(commandLine,rawDataDecoded,strlen(commandLine)); //? add decode status..? 
//        s_MK_LcdScreen LcdScreen;        
//        memcpy((unsigned char *)&LcdScreen, (unsigned char *)&rawDataDecoded, sizeof(LcdScreen));
//        menuIndex = LcdScreen.Menuitem;        
//        //if(menuIndex == 2){
//          extractGpsInfo(LcdScreen.DisplayText,gpsInfo);
//          Serial.print("gpsInfo=");
//          Serial.println(gpsInfo);
//          
//          sprintf(cmdName,"OSD");
//       // }
//        
//      }
    }

  }
  else if(statusReadCommandLine == 2){
    if(DEBUGME){
      Serial.println("D: Frame Broken");
      cmdSend = 0; // we set it to 0 to force the commandIssue on the next Loop
    }
  }
  else if(statusReadCommandLine == 3){
    if(DEBUGME){
      Serial.println("D: Frame TimeOut");
      cmdSend = 0; // we set it to 0 to force the commandIssue on the next Loop 
    }
  }
  
     digitalWrite(led,HIGH);
     //PUSH TO TARANIS
     FrSkySPort_Process(); 
     
 
     
}
 


void redirectUartNc(void){
  Serial.print(0x1b);
  Serial.print(0x55);
  Serial.print(0xaa);
  Serial.print(0x00);
}

void makeCmdString(char *typeCmd,char *cmdStringRequest){
  char cmdData[20];
  char toEncode[1];
  char cmdFrame[30];
  char cmdHeader[5];
 
 

  if(strcmp(typeCmd, "OSD")  == 0){
    // we want osdData
    //#cO=DzdMHTAAe[ype>==A=================================s]A=====eM====@?====n==>=I]=====s]AR>M@S=M==xe
    cmdHeader[0]='#'; // frame start
    cmdHeader[1]='a'; // navi Addr = C
    cmdHeader[2]='o'; // osdData = o
    cmdHeader[3]='\0';
    toEncode[0] = 200; // frequence of refresh of the OSD data => 1000ms
    encode64(toEncode,cmdData,sizeof(toEncode));
    sprintf(cmdFrame,"%s%s",cmdHeader,cmdData);
    char CRC[5];
    addCRC(cmdFrame,CRC);

    sprintf(cmdStringRequest,"%s%s%s",cmdHeader,cmdData,CRC);
  }
if(strcmp(typeCmd, "VER")  == 0){
    // we want versionINfo
    //#c
    cmdHeader[0]='#'; // frame start
    cmdHeader[1]='a'; // navi Addr = C
    cmdHeader[2]='v'; // osdData = o
    cmdHeader[3]='\0';
    sprintf(cmdFrame,"%s",cmdHeader);
    char CRC[5];
    addCRC(cmdFrame,CRC);
    sprintf(cmdStringRequest,"%s%s",cmdHeader,CRC);

  }  
   if(strcmp(typeCmd, "D2")  == 0){
    // we want osdData
       cmdHeader[0]='#'; // frame start
    cmdHeader[1]='a'; // navi Addr = C
    cmdHeader[2]='l'; // osdData = o
    cmdHeader[3]='\0';

    toEncode[0] = 2;
    
    encode64(toEncode,cmdData,sizeof(toEncode));
    sprintf(cmdFrame,"%s%s",cmdHeader,cmdData);
    char CRC[5];
    addCRC(cmdFrame,CRC);

    sprintf(cmdStringRequest,"%s%s%s",cmdHeader,cmdData,CRC);
 
  }

}
int readCommandLine(char *commandLine){
  int hasLine = 0;
  int charIndex = 0;
  char incomingChar;
  const char startChar='#';
  const char endChar='\r';
  int inFrame = 0;
  int brokenFrame = 0;
  int timeOut = 0;
  unsigned long timeStart = millis();
  unsigned long timeNow = 0;
  while(hasLine == 0){
    // implementer le timeout
    timeNow = millis();
    if(timeNow - timeStart > SERIAL_READ_TIMEOUT){
      timeOut =1;
      hasLine = 1;
    }


 
      // read the incoming byte:
 
     if (_MKSerial.available() > 0) {
      incomingChar = _MKSerial.read();

       

      if(incomingChar == startChar){
        inFrame = 1;
      }
      if(inFrame && (incomingChar == endChar)){


        inFrame = 0;
        hasLine = 1;
      }

      if(inFrame){
        commandLine[charIndex] = incomingChar;
        charIndex++;
        if(charIndex > maxFrameLen){
          brokenFrame = 1;
          hasLine=1;
        }
      }
    }
     
  }
  if(brokenFrame){
    commandLine[0] = '\0';
    return 2;
  }
  else if(timeOut){
    commandLine[0] = '\0';
    return 3;
  }
  else{
    commandLine[charIndex] = '\0';
  }
  return 1;
}

char readDataType(char *mkLine){
  char dataType = mkLine[2];
  if(dataType == 'D'){
    return dataType;
  }
  if(dataType == 'L'){
    return dataType;
  }
  if(dataType == 'O'){
    return dataType;
  }
  if(dataType == 'l'){
    return dataType;
  }
  if(dataType == 'V'){
    return dataType;
  }
  if(dataType == 'H'){
    return dataType;
  }

  return '\0';
}

//This beep used for altastastion for grount station perposes

void beepMe(int beepMode,int beepDuration){

//  if (beepMode == ERROR_BEEP){
//    tone(beepPin, 18000); //plays a 18kHz tone on digital I/O pin 13
//  }
//
//  delay(beepDuration*1000);
//  noTone(beepPin);

}


