#include <Servo.h>
#include "pid.h"
#include "mpu.h"
#include <Wire.h>


mpu gyro;   // p  i  d
//pid PID(10, 0.01, 0.1); //27.9.2020
pid PID(15.0, 0.1, 0.5);
//pid PID(15.0, 0.0, 0.0);

int pinA = 5;
int pinB = 3;
int pinC = 6;
int pinD = 10;

Servo ESCA;
Servo ESCB;
Servo ESCC;
Servo ESCD;


#include <SoftwareSerial.h>
SoftwareSerial bluetooth(7, 8);

unsigned long start_time;
unsigned long last_signal_time;

float Round(float num){
  return float(int(num*10))/10;
}

void setup() {
  Serial.begin(9600);
  bluetooth.begin(19200);
  gyro.initiate();
  ESCA.attach(pinA,1000,2000);
  ESCB.attach(pinB,1000,2000);
  ESCC.attach(pinC,1000,2000);
  ESCD.attach(pinD,1000,2000);
  
  ESCA.write(180);
  ESCB.write(180);
  ESCC.write(180);
  ESCD.write(180);
  delay(2000);
  ESCA.write(0);
  ESCB.write(0);
  ESCC.write(0);
  ESCD.write(0);
  delay(2000);

  //Serial.println("bagr");


start_time = millis();
last_signal_time = millis();

if (bluetooth.available() > 0){
    for (byte x = 0; x<bluetooth.available(); x++){
      bluetooth.read();
    }
  }

}

int strength = 0;
float X = 0;
float Y = 0;
int max_tilt = 5;

byte bytes_available;


bool connection_started = false;
void loop() {
  gyro.refresh();
  bytes_available = bluetooth.available();
  
  if (bytes_available > 0){//dorucene byty
    last_signal_time = millis();
    if (start_time == 0){
      start_time = millis();
    }
    if ((millis() - start_time)>200){//pokud se doruci mene, nez 3 byty behem 100ms, tak se byty zahodi a dron vypne motorky
      for (byte x = 0; x<bytes_available; x++){
        bluetooth.read();
      }
        strength = 0;
        X = 0;
        Y = 0;
        start_time = 0;
    }
     if (bytes_available >= 3) {//spravne doruceni bytu
      strength = map(bluetooth.read(), 0, 255, 0, 500);
      X = map(bluetooth.read(), 0, 256, -max_tilt*100, max_tilt*100)/100.0;
      Y = map(bluetooth.read(), 0, 256, -max_tilt*100, max_tilt*100)/100.0;
      start_time = 0;
  
      if (!connection_started){
        strength = 0;
        X = 0;
        Y = 0;
        connection_started = true;
      }
   }
  }else{
    if (millis()-last_signal_time > 3000){//ztrata signalu
        strength = 0;
        X = 0;
        Y = 0;
        last_signal_time = millis();
    }
  }
    PID.refresh(Round(gyro.angleX)-X+Y, Round(gyro.angleY)-X-Y, gyro.Gyr_rawX, gyro.Gyr_rawY, strength);
  
   
    Serial.print(PID.A);
    Serial.print("  ");
    Serial.print(PID.B);
    Serial.print("  ");
    Serial.print(PID.C);
    Serial.print("  ");
    Serial.println(PID.D);
  
    //Serial.println(Round(gyro.angleX), Round(gyro.angleY));
    /*
    ESCA.write(PID.A);
    ESCB.write(PID.B);
    ESCC.write(PID.C);
    ESCD.write(PID.D);
    */

}
