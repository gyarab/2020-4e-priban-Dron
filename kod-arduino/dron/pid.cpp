#include "Arduino.h"
#include "pid.h"
#include <math.h>

//jako funkce map(), ale pro float
float FloatMap(float input, float fromLow, float fromHigh, float toLow, float toHigh){
  float vraceni = ((input-fromLow)*((toHigh-toLow)/(fromHigh-fromLow)) + toLow);
  if (vraceni > 0.0){
    return vraceni;
  }
  return 0.0;
}
//inicializace pro body [Pid_low_map, slozky_low] a [255, slozky_high]
pid::pid(float P_low, float I_low, float D_low, float P_high , float I_high , float D_high , byte Pid_low_map = 50){
  p_low = P_low;
  i_low = I_low;
  d_low = D_low;

  p_high = P_high;
  i_high = I_high;
  d_high = D_high;

  pid_low_map = Pid_low_map;
}

int maxOutput = 400; // maximalni kolektivni hodnota PID
int maxStrength = 500; // maximalni hodnota sily, na kterou se vstupni sila zmapuje
int minValue = 1000; // hodnota pro minimalni rychlost motorku
int maxI = 40; // maximalni hodnota pro slozku I

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
    //vypocet P slozky
    roll_p = p * roll_error;
    pitch_p = p * pitch_error;
    
    //vypocet I slozky
    elapsed_time = micros()-interval_I;
    
    roll_i = roll_i + ((i*roll_error*elapsed_time)/1000000.0);
    pitch_i = pitch_i + ((i*pitch_error*elapsed_time)/1000000.0);

    if (roll_i > maxI){roll_i = maxI;}
    else if(roll_i < -maxI){roll_i = -maxI;}

    if (pitch_i > maxI){pitch_i = maxI;}
    else if(pitch_i < -maxI){pitch_i = -maxI;}

    interval_I = micros();//vynulovani I slozky pred skutecnym startem
    if (strength < 70){
      roll_i = 0;
      pitch_i = 0;
    }
    //vypocet D slozky
    roll_d = d*raw_roll;
    pitch_d = d*raw_pitch;


    //secteni slozek
    roll = roll_p + roll_i + roll_d;
    pitch = pitch_p + pitch_i + pitch_d;

    //nesmi presahovat interval
    if(roll < -maxOutput){roll=-maxOutput;}
    if(roll > maxOutput) {roll=maxOutput; }
    if(pitch < -maxOutput){pitch=-maxOutput;}
    if(pitch > maxOutput) {pitch=maxOutput;}

    //rozdeleni slozek mezi motory
    A  = strength + roll;
    B  = strength - roll;
    C  = strength + pitch;
    D  = strength - pitch;
    
    //opet nesmi presahovat interval
    if(A < 0){A = 0;}
    if(B < 0){B = 0;}
    if(C < 0){C = 0;}
    if(D < 0){D = 0;}
    
    //posun z intervalu <0, 1000> do intervalu <1000, 2000>
    A += 1000;
    B += 1000;
    C += 1000;
    D += 1000;

  }
  
}
