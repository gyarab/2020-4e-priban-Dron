#ifndef pid_h
#define pid_h

#include <Arduino.h>
#include <math.h>

class pid{
  public:
    pid(float p, float i, float d);
    void refresh(float roll_error, float pitch_error, float raw_roll, float raw_pitch, float strength);
    float A = 0;
    float B = 0;
    float C = 0;
    float D = 0;

    float roll = 0;
    float pitch = 0; 
  private:
    int currentXD = 0;
    int currentYD = 0;

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

    //long elapsedTime, time, timePrev;


};

#endif
