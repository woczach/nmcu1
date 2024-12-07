#include <Arduino.h>
#include "DFRobot_MICS.h"
#include <Wire.h>

//zielony d1
#define SDA D1
//niebieski d2
#define SCL D2
//Wire.begin(4,5);
#define CALIBRATION_TIME   3       
#define Mics_I2C_ADDRESS 0x75
DFRobot_MICS_I2C mics(&Wire, Mics_I2C_ADDRESS);


void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  Wire.begin(D1,D2);

  byte error, address;
  for(address = 1; address < 127; address++ )
  {
  Serial.println(address,HEX);
  delay(10);
  
  Wire.beginTransmission(address);
  error = Wire.endTransmission(true);
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
    else
    {
    Serial.print("ssome error");
      Serial.println(error,HEX);
    }
    
  }
  while(!mics.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  } Serial.println("Device connected successfully !");  
  uint8_t mode = mics.getPowerState();
  if(mode == SLEEP_MODE){
    mics.wakeUpMode();
    Serial.println("wake up sensor success!");
  }else{
    Serial.println("The sensor is wake up mode");
  }

  /**!
     Do not touch the sensor probe when preheating the sensor.
     Place the sensor in clean air.
     The default calibration time is 3 minutes.
  */
  while(!mics.warmUpTime(CALIBRATION_TIME)){
    Serial.println("Please wait until the warm-up time is over!");
    delay(10000);
  }

}

void loop() {
  float gasdata = mics.getGasData(C2H5OH);
  Serial.print("C2H5OH ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  delay(1000);
  float gd2 = mics.getGasData(H2);
  Serial.print("H2 ");
  Serial.print(gd2,1);
  Serial.println(" PPM");
  delay(1000);
  float gd3 = mics.getGasData(CO);
  Serial.print("CO ");
  Serial.print(gd3,1);
  Serial.println(" PPM");
  delay(1000);  

}