#ifndef pid_h
#define pid_h

#include <Arduino.h>
#include <math.h>

class pid{
  public:
    pid(float P_low, float I_low, float D_low, float P_high , float I_high , float D_high , byte Pid_low_map = 50);
    void refresh(float roll_error, float pitch_error, float raw_roll, float raw_pitch, byte raw_strength);
    int A = 0;
    int B = 0;
    int C = 0;
    int D = 0;

    float roll = 0;
    float pitch = 0; 

    float strength;
    
    unsigned long interval_I = 0;
    unsigned long elapsed_time = 0;
  private:
      
    int currentXD = 0;
    int currentYD = 0;

    byte pid_low_map;
    
    float p_low;
    float i_low;
    float d_low;

    float p_high;
    float i_high;
    float d_high;

    float p;
    float i;
    float d;
    
    float roll_p;
    float pitch_p;

    float roll_i;
    float pitch_i;

    float roll_d;
    float pitch_d;

    float previous_roll_error;
    float previous_pitch_error;

};

#endif
