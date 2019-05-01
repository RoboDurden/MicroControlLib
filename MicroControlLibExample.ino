
#define _DEMO_ 1
#include "MicroControlLib.h"
// this is the data like you will use in your project
float aGlobal[9] = {0,0,0,0,0,0,0,0,0};
float aSystem[2] = {1556474970,0};
SyncData oData = {1556474000,0,2.7,64.4,0,1.3,43.7,95.5,99,6.5,43.6,4.1};

void setup() 
{
  Serial.begin(115200);
  // now point the calcy data arrays to your true data
  CalcyInit(  (float*)&oData+1,(float*)&aGlobal,(float*)&aSystem);
  String sCalc = "(v2>46.3)+(v10>3.0)=2~s1";  // inside humidity > 46.3) AND (battery_voltage > 3.0) -> store in bit 1 of System array
  String sError;
  float fResult = Calc(sCalc,sError);
  if (sError.length())  Serial.println(sError);

  Serial.println("\nmultiple control:");
  sCalc = "(v2>46.3)+(v10>3.5)=2~s1;v1<3.0*2|s1~s1;v10<3.0*4|s1~s1";
  String sDisplay;
  String sC = sCalc;  // make a copy from which to remove the multiple formulas
  do
  {
    String sError = "";
    String sDo = ShiftValue(sC,";");
    Serial.println("------ do: " + sDo);
    float fResult = Calc(sDo,sError);
    if (sError.length())
    {
      sDisplay += sError;
    }
    else
    {
      uint16_t wSystem = (uint16_t)aSystem[1];
      boolean bCoolerOn = wSystem & 1;
      boolean bFrostWarning = wSystem & 2;
      boolean bBatteryEmpty = wSystem & 4;
      sDisplay += sDo + " -> wSystem=" + String(wSystem) + "\n";
    }
  } while (sC.length());
  Serial.print(sDisplay);
}

void loop() {}

String ShiftValue(String &sLine, char* c)
{
  //Serial.println("ShiftValue:'"+sLine+"'");
  int i = sLine.indexOf(c);
  String s;
  if (i<0)
  {
    s = sLine;
    sLine = "";
  }
  else
  {
    s = sLine.substring(0,i);
    sLine.remove(0,i+1);
  }
  return s;
}
