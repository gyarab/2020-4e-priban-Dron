
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
    float elapsedTime, actualTime, timePrev;        //Variables for time control
    float Gyro_raw_error_x, Gyro_raw_error_y; //Here we store the initial gyro data error
    float Acc_angle_error_x, Acc_angle_error_y; //Here we store the initial Acc data error
  private:
    int gyro_error=0;                         //We use this variable to only calculate once the gyro data error
    float Gyro_angle_x, Gyro_angle_y;         //Here we store the angle value obtained with Gyro data


    float gyroFix_X = 4.0;
    float gyroFix_Y = 0.0;
    //Acc Variables 
    int acc_error=0;                         //We use this variable to only calculate once the Acc data error
    float rad_to_deg = 180/3.141592654;      //This value is for pasing from radians to degrees values
    float Acc_rawX, Acc_rawY, Acc_rawZ;    //Here we store the raw data read 
    float Acc_angle_x, Acc_angle_y;          //Here we store the angle value obtained with Acc data

};

#endif
