/* 

  https://github.com/RoboDurden/MicroControlLib
  copyright: GNU General Public License v3.0 = GNU GPLv3
  2019/05/01 MicroControlLib v0.21 by Robo Durden

  bug fix 2019/07/17
    case '-': f = o1.fRet - o2.fRet;  break;

example code:
  #define _DEMO_ 1
  #include "MicroControlLib.h"
  // this is the data like you will use in your project
  float aGlobal[9] = {0,0,0,0,0,0,0,0,0};
  float aSystem[2] = {1556474970,0};
  SyncData oData = {1556474000,0,2.7,64.4,95.5,1.3,43.7,95.5,99,6.5,43.6,4.1};
  
  void setup() {
    Serial.begin(115200);
    // now point the calcy data arrays to your true data
    CalcyInit(  (float*)&oData+1,(float*)&aGlobal,(float*)&aSystem);
    String sCalc = "(v2>46.3)+(v10>3.0)=2~s1";  // inside humidity > 46.3) AND (battery_voltage > 3.0) -> store in bit 1 of System array
    String sError;
    float fResult = Calc(sCalc,sError);
    if (sError.length())  Serial.println(sError);
  }
*/

#define CALCY_DEBUG 1

#ifdef CALCY_DEBUG
  int iCALCY_DEBUG_level = 0;
  //#define CEBUGLN(txt, val) Serial.print(txt); Serial.print(": "); Serial.println(val);
  #define CEBUGLN(txt, val){for (int i=iCALCY_DEBUG_level;i>0;i--){Serial.print("\t");}Serial.print(txt); Serial.print(": "); Serial.println(val);}
  #define CEBUGLNU(txt, val){iCALCY_DEBUG_level++;for (int i=iCALCY_DEBUG_level;i>1;i--){Serial.print("\t");}Serial.print(txt); Serial.print(": "); Serial.println(val);}
  #define CEBUGLND(txt, val){for (int i=iCALCY_DEBUG_level--;i>1;i--){Serial.print("\t");}Serial.print(txt); Serial.print(": "); Serial.println(val);}

#endif
#ifndef CALCY_DEBUG
  #define CEBUGLN(txt, val)
  #define CEBUGLNU(txt, val)
  #define CEBUGLND(txt, val)
#endif


float* m_aValue;   // the array of values
float* m_aGlobal;    // the array of global variables
float* m_aSystem;    // the array of system variables, 

void CalcyInit(float* aV,float* aG,float* aS)
{
  m_aValue = aV;
  m_aGlobal = aG;
  m_aSystem = aS;
}

void SetArrayValue(char sType,int i, float f)
{
  switch(sType)
  {
  case 'v': m_aValue[i] = f;  break;
  case 'g': m_aGlobal[i] = f;  break;
  case 's': m_aSystem[i] = f;  break;
  }
  CEBUGLN("SetArrayValue "+String(sType)+","+String(i),String(f));
}

float GetArrayValue(char sType,int i)
{
  switch(sType)
  {
  case 'v': 
    CEBUGLN("GetArrayValue(v,"+String(i)+")",String(m_aValue[i]));
    return m_aValue[i];
  case 'g': return m_aGlobal[i];
  case 's': return m_aSystem[i];
  }
  return 0;
}

typedef struct __attribute((__packed__))
{
  char t;
  int f;    // field if t!=0 a value/global/system field is returend
  int i;
  int  j;
  float  fRet;
} Result;

float Calc(String s, String& sError);   // recursive programming -> declaration neccessary
Result NextNum(String s,int i, int j, String& sError)
{
  while(i<j)
  {
    char c = s.charAt(i++);
    if (c == '(')
    {
      int iOpen = 1;
      int j2=i;
      while(j2<j)
      {
        char c = s.charAt(j2++);
        if (c == '(')
        {
          iOpen++;
        } 
        if (c == ')')
        {
          iOpen--;
          break;
        } 
      }
      if (iOpen>0)
      {
        sError += s + " : syntax error too many '('\n";
        return {0,0,i,j2,0};
      }
      return {0,0,i,j2,Calc(s.substring(i,j2-1),sError)};
    }
    char cType = 0;  // expecting this to be a constant number
    int j2=--i;
    while(j2<j)
    {
      switch(c)
      {
      case '+': case '-': 
        if (j2>i) // we already parsed a number
        {
            if (cType)  // we already parsed a param
            {
              int iPos = s.substring(i,j2).toInt();
              return {cType,iPos,i,j2,GetArrayValue(cType,iPos)};
            }
            return {0,0,i,j2,s.substring(i,j2).toFloat()};
        }
      case '.':
        if (cType)  // syntax error
        {
          sError += s + " : syntax error #2\n";
          return {0,0,i,j2,0};
        }
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        break;
      default: 
        if (cType)
        {
          int iPos = s.substring(i,j2).toInt();
          return {cType,iPos,i,j2,GetArrayValue(cType,iPos)};
        }
        if (j2>i) // we already parsed a number
        {
          return {0,0,i,j2,s.substring(i,j2).toFloat()};
        }
        cType = c;  // seems to be something linke "v1"
        i++;
      }
      j2++;
      c = s.charAt(j2);
    }
    if (cType)
    {
        int iPos = s.substring(i,j2).toInt();
        return {cType,iPos,i,j2,GetArrayValue(cType,iPos)};
    }
    return {cType,0,i,j2,s.substring(i,j2).toFloat()};
  }
}

float Calc(String s, String& sError)
{
  CEBUGLNU("Calc()",s);
  int iLen = s.length();
  Result o1 = NextNum(s,0,iLen,sError);
  float f = 0;
  while(1)
  {
    char sOp = s.charAt(o1.j);
    Result o2 = NextNum(s,o1.j+1,iLen,sError);

    switch(sOp)
    {
    case '>': f = (o1.fRet > o2.fRet) ? 1 : 0;  break;
    case '<': f = (o1.fRet < o2.fRet) ? 1 : 0;  break;
    case '=': f = (o1.fRet == o2.fRet) ? 1 : 0; break;
    case '+': f = o1.fRet + o2.fRet;  break;
    case '-': f = o1.fRet - o2.fRet;  break;
    case '*': f = o1.fRet * o2.fRet;  break;
    case '/': 
      if (o2.fRet == 0)
      {
        sError += s + " : division by zero " + String(o2.t)+String(o2.f)+"\n";
        return 0;
      }
      f = o1.fRet / o2.fRet;  
      break;
    case '|': f = (uint16_t)o1.fRet | (uint16_t)o2.fRet;  break;
    case '&': f = (uint16_t)o1.fRet & (uint16_t)o2.fRet;  break;
    case '~': 
    {
      if (!o2.t)  // syntax error: can not set o1.f to a constant variable
      {
        sError += s + " : can not set " + String(o1.t)+String(o1.fRet)+" to constant.\n";
        return 0;
      }
      SetArrayValue(o2.t,(int)o2.f,o1.fRet);
      break;
    }
    default:  // unkown operator
      sError += s + " : unkown operator '"+sOp+"'\n";
      return 0;
    }
    if (o2.j >= iLen)
      break;
    //o1 = {"t":0,"i":o2.i,"j":o2.j,"fRet":f};
    o1.t=0; o1.i=o2.i; o1.j=o2.j; o1.fRet=f; 
  }
  CEBUGLND("returning",String(f));
  return f;
}

#ifdef _DEMO_
typedef struct __attribute((__packed__))
{
  uint32_t  iTime;
  uint32_t  wData;  // bits 0..7 = iAlarms
  float fInT; // temperature
  float fInH; // humidity
  float fInP; // preassure
  float fOutT;
  float fOutH;
  float fOutP;
  float fSoilH; // moisture
  float fSoilA; // acid
  float fSoilL; // light
  float fBatt;  // battery voltage
} SyncData;
#endif
