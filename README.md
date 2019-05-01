# MicroControlLib
An Arduino / Javascript micro control language: "(v2>46.3)+(v10>3.0)=2~s1" -> cooler on/off :-)

.ino example includes multiple calculations like "(v2>46.3)+(v10>3.5)=2~s1;v1<3.0*2|s1~s1;v10<3.0*4|s1~s1"

calculation is down from left to right !  
'~' has the meaning of '=:' and asigns the left value to the right variable

the calculation operates on three float arrays:  

    m_aValue;     // the array of values  
    m_aGlobal;    // the array of global variables  
    m_aSystem;    // the array of system variables,  

point these data arrays to your true data with  

    void CalcyInit(float* aV,float* aG,float* aS);

execute the calculation with:  

    float fResult = Calc(sCalc,sError);

  
copyright: GNU General Public License v3.0 = GNU GPLv3

