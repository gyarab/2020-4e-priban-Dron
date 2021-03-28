#include "Arduino.h"
#include "mpu.h"
#include <Wire.h>

mpu::mpu(){
}

void mpu::initiate(){
  Wire.begin();                          
  Wire.beginTransmission(0x68);                      
  Wire.write(0x6B);                       //reset modulu
  Wire.write(0x00);
  Wire.endTransmission(true);

  delay(20);//tento delay se zda byt nezbytny, (min 6ms)
  
  //Nastaveni gyroskopu
  Wire.beginTransmission(0x68);           
  Wire.write(0x1B);                       
  Wire.write(0x00);                       //±250°/s = 0x00, ±500°/s = 0x01, ±1000°/s = 0x10, ±2000°/s =  0x11
  Wire.endTransmission(true);
               
  //nastaveni akcelerometru
  Wire.beginTransmission(0x68);           
  Wire.write(0x1C);                       
  Wire.write(0x00);                       // ±2g = 0x00 ±4g=0x01 ±8g=0x10, ±16g = 0x11
  Wire.endTransmission(true); 
  
  actualTime = millis();

  //kalibrace akcelerometru
    for(int a=0; a<200; a++){
      Wire.beginTransmission(0x68);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,6,true); 
      Acc_rawX=(Wire.read()<<8|Wire.read())/2048.0 ; //±2g: 16384.0, ±4g: 8192.0, ±8g: 4096.0, ±16g: 2048.0
      Acc_rawY=(Wire.read()<<8|Wire.read())/2048.0 ;
      Acc_rawZ=(Wire.read()<<8|Wire.read())/2048.0 ;
      Acc_angle_error_x = Acc_angle_error_x + ((atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg));
      Acc_angle_error_y = Acc_angle_error_y + ((atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg)); 
    }
    Acc_angle_error_x = Acc_angle_error_x/200;
    Acc_angle_error_y = Acc_angle_error_y/200;

    for(int i=0; i<200; i++){
      Wire.beginTransmission(0x68);
      Wire.write(0x43);
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,4,true);
      Gyr_rawX=Wire.read()<<8|Wire.read();
      Gyr_rawY=Wire.read()<<8|Wire.read();
      Gyro_raw_error_x = Gyro_raw_error_x + (Gyr_rawX/131.0); //±250°/s: 131.0, ±500°/s: 65.5, ±1000°/s: 32.8, ±2000°/s: 16.4
      Gyro_raw_error_y = Gyro_raw_error_y + (Gyr_rawY/131.0);
    }
    Gyro_raw_error_x = Gyro_raw_error_x/200;
    Gyro_raw_error_y = Gyro_raw_error_y/200;

  angleX = Acc_angle_error_x;
  angleY = Acc_angle_error_y;
}

void mpu::refresh(){
  timePrev = actualTime;                        // cas predchoziho mereni
  actualTime = millis();                        // soucasne mereni
  elapsedTime = (actualTime - timePrev) / 1000; //prevod na sekundy

  //cteni dat z gyroskopu
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,4,true);
  Gyr_rawX=Wire.read()<<8|Wire.read();
  Gyr_rawY=Wire.read()<<8|Wire.read();
  Gyr_rawX = (Gyr_rawX/131.0) - Gyro_raw_error_x; //±250°/s: 131.0, ±500°/s: 65.5, ±1000°/s: 32.8, ±2000°/s: 16.4
  Gyr_rawY = (Gyr_rawY/131.0) - Gyro_raw_error_y;
  Gyro_angle_x = Gyr_rawX*elapsedTime;
  Gyro_angle_y = Gyr_rawY*elapsedTime;

  //cteni dat z akcelerometru
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,6,true);
  
  Acc_rawX=(Wire.read()<<8|Wire.read())/2048.0 ; //±2g: 16384.0, ±4g: 8192.0, ±8g: 4096.0, ±16g: 2048.0
  Acc_rawY=(Wire.read()<<8|Wire.read())/2048.0 ;
  Acc_rawZ=(Wire.read()<<8|Wire.read())/2048.0 ; 
  
  Acc_angle_x = (atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg);
  Acc_angle_y = (atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg);  
 
  angleX = 0.98 *(angleX + Gyro_angle_x) + 0.02*Acc_angle_x;
  angleY = 0.98 *(angleY + Gyro_angle_y) + 0.02*Acc_angle_y;
}
