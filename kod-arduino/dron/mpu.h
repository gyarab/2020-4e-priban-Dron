
#ifndef mpu_h
#define mpu_h

#include <Arduino.h>
#include <math.h>

class mpu{
  public:
    mpu();
    void initiate();
    void refresh();
    float angleX, angleY;
    float Gyr_rawX, Gyr_rawY, Gyr_rawZ;
    float elapsedTime, actualTime, timePrev;
    float Gyro_raw_error_x, Gyro_raw_error_y;
    float Acc_angle_error_x, Acc_angle_error_y;
  private:
    float Gyro_angle_x, Gyro_angle_y;
    
    float gyroFix_X = 4.0;
    float gyroFix_Y = 0.0;

    float rad_to_deg = 180/3.141592654;
    float Acc_rawX, Acc_rawY, Acc_rawZ;
    float Acc_angle_x, Acc_angle_y;

};

#endif
