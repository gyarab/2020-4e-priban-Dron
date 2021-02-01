#include "Arduino.h"
#include "pid.h"
#include <math.h>

pid::pid(float P, float I, float D){
  p = P;
  i = I;
  d = D;
  
}
int maxOutput = 300;
int maxStrength = 500;
int endMap = 2000;
int minValue = 1000;
void pid::refresh(float roll_error, float pitch_error, float raw_roll, float raw_pitch, float strength){
  
  if (strength == 0){
      A = minValue;
      B = minValue;
      C = minValue;
      D = minValue;
  }else{
    roll_p = p * roll_error;
    pitch_p = p * pitch_error;

    elapsed_time = micros()-interval_I;
    
    roll_i = roll_i + ((i*roll_error*elapsed_time)/1000000.0);
    pitch_i = pitch_i + ((i*pitch_error*elapsed_time)/1000000.0);

    Serial.println(roll_error, 1);
    
    interval_I = micros();
    if (strength < 200){
      roll_i = 0;
      pitch_i = 0;
    }
    
    roll_d = d*raw_roll;
    pitch_d = d*raw_pitch;
    
    roll = roll_p + roll_i + roll_d;
    pitch = pitch_p + pitch_i + pitch_d;
  
    if(roll < -maxOutput){roll=-maxOutput;}
    if(roll > maxOutput) {roll=maxOutput; }
    if(pitch < -maxOutput){pitch=-maxOutput;}
    if(pitch > maxOutput) {pitch=maxOutput;}
      
    A  = strength + roll;
    B  = strength - roll;
    C  = strength + pitch;
    D  = strength - pitch;
  
    if(A < 0){A = 0;}
    if(B < 0){B = 0;}
    if(C < 0){C = 0;}
    if(D < 0){D = 0;}
    
    A = map(A, 0, maxOutput+maxStrength, minValue, endMap);
    B = map(B, 0, maxOutput+maxStrength, minValue, endMap);
    C = map(C, 0, maxOutput+maxStrength, minValue, endMap);
    D = map(D, 0, maxOutput+maxStrength, minValue, endMap);
  }
  
}
