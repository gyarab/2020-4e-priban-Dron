//dron_05
//kód inspirován:
//https://howtomechatronics.com/tutorials/arduino/arduino-and-hc-05-bluetooth-module-tutorial/
//https://www.arduino.cc/en/reference/wire
//https://www.arduino.cc/reference/en/libraries/servo/

#include <Servo.h>
#include "pid.h"
#include "mpu.h"
#include <Wire.h>

mpu gyro;
//   Low(p    i    d) High(p    i    d)
//pid PID(1.15, 0.003, 0.5, 7.0, 0.01, 2.1, 64);
pid PID(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

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

  Serial.begin(115200);
  bluetooth.begin(19200);
  
  ESCA.attach(pinA);
  ESCB.attach(pinB);
  ESCC.attach(pinC);
  ESCD.attach(pinD);
  ESCA.writeMicroseconds(2000);
  ESCB.writeMicroseconds(2000);
  ESCC.writeMicroseconds(2000);
  ESCD.writeMicroseconds(2000);
  delay(2000);
  ESCA.writeMicroseconds(1000);
  ESCB.writeMicroseconds(1000);
  ESCC.writeMicroseconds(1000);
  ESCD.writeMicroseconds(1000);
  delay(2000);

  gyro.initiate();
  
  start_time = millis();
  last_signal_time = millis();
  PID.interval_I = micros();
  
  if (bluetooth.available() > 0){
      for (byte x = 0; x<bluetooth.available(); x++){
        bluetooth.read();
      }
    }
}

byte strength = 0;
float X = 0;
float Y = 0;
int max_tilt = 5;

byte bytes_available;

bool connection_started = false;

void loop() {
  
  bytes_available = bluetooth.available();
  
  if (bytes_available > 0){//dorucene byty
    last_signal_time = millis();
    if (start_time == 0){
      start_time = millis();
    }
    if ((millis() - start_time)>200){//pokud se doruci mene, nez 3 byty behem 200ms, tak se byty zahodi a dron vypne motorky
      for (byte x = 0; x<bytes_available; x++){
        bluetooth.read();
      }
        strength = 0;
        X = 0;
        Y = 0;
        start_time = 0;
    }
     if (bytes_available >= 3) {//spravne doruceni bytu
      strength = bluetooth.read();
      X = map(bluetooth.read(), 0, 255, -max_tilt*100, max_tilt*100)/100.0;
      Y = map(bluetooth.read(), 0, 255, -max_tilt*100, max_tilt*100)/100.0;
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
    gyro.refresh();
    
    PID.refresh(Round(gyro.angleX)-X+Y, Round(gyro.angleY)-X-Y, gyro.Gyr_rawX, gyro.Gyr_rawY, strength);

    //uzitecne vypisy pro debuging
    /*
    Serial.print(Round(gyro.angleX));
    Serial.print("  ");
    Serial.println(Round(gyro.angleY));
    */
    
    /*
    Serial.print(PID.A);
    Serial.print("  ");
    Serial.print(PID.B);
    Serial.print("  ");
    Serial.print(PID.C);
    Serial.print("  ");
    Serial.println(PID.D);
    */
    
    ESCA.writeMicroseconds(PID.A);
    ESCB.writeMicroseconds(PID.B);
    ESCC.writeMicroseconds(PID.C);
    ESCD.writeMicroseconds(PID.D);
}
