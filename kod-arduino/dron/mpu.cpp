
 
#include "Arduino.h"
#include "mpu.h"
#include <Wire.h>

mpu::mpu(){
}



void mpu::initiate(){
  //Wire.begin();
    //begin the wire comunication
  Wire.beginTransmission(0x68);           //begin, Send the slave adress (in this case 68)              
  Wire.write(0x6B);                       //make the reset (place a 0 into the 6B register)
  Wire.write(0x00);
  Wire.endTransmission(true);             //end the transmission
  
  delay(100);// nezbytne, minimum 6ms, jinak probehne inicializace spatne (pouze az po resetu)
  //Gyro config
  Wire.beginTransmission(0x68);           //begin, Send the slave adress (in this case 68) 
  Wire.write(0x1B);                       //We want to write to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                       //Set the register bits as 00010000 (1000dps full scale)
  Wire.endTransmission(true);             //End the transmission with the gyro
  //Acc config
  Wire.beginTransmission(0x68);           //Start communication with the address found during search.
  Wire.write(0x1C);                       //We want to write to the ACCEL_CONFIG register
  Wire.write(0x10);                       //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true); 
  
  actualTime = millis();
  
  if(acc_error==0){//kalibrace akcelerometru
    for(int a=0; a<200; a++){
      Wire.beginTransmission(0x68);
      Wire.write(0x3B);                       //Ask for the 0x3B register- correspond to AcX
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,6,true); 
      Acc_rawX=(Wire.read()<<8|Wire.read())/4096.0 ; //each value needs two registres
      Acc_rawY=(Wire.read()<<8|Wire.read())/4096.0 ;
      Acc_rawZ=(Wire.read()<<8|Wire.read())/4096.0 ;
      Acc_angle_error_x = Acc_angle_error_x + ((atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg));
      Acc_angle_error_y = Acc_angle_error_y + ((atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg)); 
    }
    Acc_angle_error_x = Acc_angle_error_x/200;
    Acc_angle_error_y = Acc_angle_error_y/200;
    acc_error=1;
    
  }
  
  if(gyro_error==0){//kalubrace gyroskopu
    for(int i=0; i<200; i++){
      Wire.beginTransmission(0x68);            //begin, Send the slave adress (in this case 68) 
      Wire.write(0x43);                        //First adress of the Gyro data
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,4,true);           //We ask for just 4 registers 
      Gyr_rawX=Wire.read()<<8|Wire.read();     //Once again we shif and sum
      Gyr_rawY=Wire.read()<<8|Wire.read();
      Gyro_raw_error_x = Gyro_raw_error_x + (Gyr_rawX/32.8); 
      Gyro_raw_error_y = Gyro_raw_error_y + (Gyr_rawY/32.8);
    }
    Gyro_raw_error_x = Gyro_raw_error_x/200;
    Gyro_raw_error_y = Gyro_raw_error_y/200;
    //Gyro_raw_error_x = 0;
    //Gyro_raw_error_y = 0;
    gyro_error=1;
  
  }

  angleX = Acc_angle_error_x;
  angleY = Acc_angle_error_y;
}
void mpu::refresh(){
  timePrev = actualTime;                        // the previous time is stored before the actual time read
  actualTime = millis();                        // actual time read
  elapsedTime = (actualTime - timePrev) / 1000; //divide by 1000 in order to obtain seconds

  Wire.beginTransmission(0x68);            //begin, Send the slave adress (in this case 68) 
  Wire.write(0x43);                        //First adress of the Gyro data
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,4,true);           //We ask for just 4 registers
  Gyr_rawX=Wire.read()<<8|Wire.read();     //Once again we shif and sum
  Gyr_rawY=Wire.read()<<8|Wire.read();
  Gyr_rawX = (Gyr_rawX/32.8) - Gyro_raw_error_x; 
  Gyr_rawY = (Gyr_rawY/32.8) - Gyro_raw_error_y;
  Gyro_angle_x = Gyr_rawX*elapsedTime;
  Gyro_angle_y = Gyr_rawY*elapsedTime;
  
  Wire.beginTransmission(0x68);     //begin, Send the slave adress (in this case 68) 
  Wire.write(0x3B);                 //Ask for the 0x3B register- correspond to AcX
  Wire.endTransmission(false);      //keep the transmission and next
  Wire.requestFrom(0x68,6,true);    //We ask for next 6 registers starting withj the 3B  
  
  Acc_rawX=(Wire.read()<<8|Wire.read())/4096.0 ; //each value needs two registres
  Acc_rawY=(Wire.read()<<8|Wire.read())/4096.0 ;
  Acc_rawZ=(Wire.read()<<8|Wire.read())/4096.0 ; 
  
 Acc_angle_x = (atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg);
 Acc_angle_y = (atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg);  
 
 angleX = 0.98 *(angleX + Gyro_angle_x) + 0.02*Acc_angle_x;
 angleY = 0.98 *(angleY + Gyro_angle_y) + 0.02*Acc_angle_y;
}
