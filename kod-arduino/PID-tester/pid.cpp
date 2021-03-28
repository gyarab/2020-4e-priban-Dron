#include "Arduino.h"
#include "pid.h"
#include <math.h>

float FloatMap(float input, float fromLow, float fromHigh, float toLow, float toHigh){
  float vraceni = ((input-fromLow)*((toHigh-toLow)/(fromHigh-fromLow)) + toLow);
  if (vraceni > 0.0){
    return vraceni;
  }
  return 0.0;
}

pid::pid(float P_low, float I_low, float D_low, float P_high , float I_high , float D_high , byte Pid_low_map = 50){
  p_low = P_low;
  i_low = I_low;
  d_low = D_low;

  p_high = P_high;
  i_high = I_high;
  d_high = D_high;

  pid_low_map = Pid_low_map;
  
}

int maxOutput = 400;
int maxStrength = 500;
int endMap = 2000;
int minValue = 1000;
int maxI = 40;

//vypoctene hodnoty A,B,C,D pro jednotlive motorky jsou v intervalu od 1000 do 2000. jedna se o delku pwm signalu
void pid::refresh(float roll_error, float pitch_error, float raw_roll, float raw_pitch, byte raw_strength){
  strength = map(raw_strength, 0, 255, 0, maxStrength);
  p = FloatMap(raw_strength, pid_low_map, 255, p_low, p_high);
  i = FloatMap(raw_strength, pid_low_map, 255, i_low, i_high);
  d = FloatMap(raw_strength, pid_low_map, 255, d_low, d_high);

  if (strength == 0){
      A = minValue;
      B = minValue;
      C = minValue;
      D = minValue;
  }else{

    roll_p = p * roll_error;

    elapsed_time = micros()-interval_I;
    roll_i = roll_i + ((i*roll_error*elapsed_time)/10000000.0);

    if (roll_i > maxI){roll_i = maxI;}
    else if(roll_i < -maxI){roll_i = -maxI;}

    interval_I = micros();//vynulovani I slozky pred skutecnym startem
    if (strength < 30){
      roll_i = 0;
    }
    
    roll_d = d*raw_roll;
    
    roll = roll_p + roll_i + roll_d;
 
    if(roll < -maxOutput){roll=-maxOutput;}
    if(roll > maxOutput) {roll=maxOutput; }
      
    A  = strength + roll;
    B  = strength - roll;
  
    if(A < 0){A = 0;}
    if(B < 0){B = 0;}
    if(C < 0){C = 0;}
    if(D < 0){D = 0;}

    A += 1000;
    B += 1000;
    C += 1000;
    D += 1000;

  }
  
}
