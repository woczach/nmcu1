#include <Arduino.h>
#include "DFRobot_MICS.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

//zielony d1
#define SDA D1
//niebieski d2
#define SCL D2
//Wire.begin(4,5);
#define CALIBRATION_TIME   3       
#define Mics_I2C_ADDRESS 0x75
DFRobot_MICS_I2C mics(&Wire, Mics_I2C_ADDRESS);

const char* ssid = "Dom";
const char* password = "Listo2#4";

WiFiClient client;
MQTTClient Mclient;


void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("\nconnecting...");
  Mclient.begin("192.168.0.230", 11883, client);
  Mclient.onMessage(messageReceived);






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


  Serial.print("\nconnecting...");
  while (!Mclient.connect("arduino", "", "")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");


}

void loop() {
  Mclient.loop();
  
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

  char str[32];
  dtostrf(gasdata, 8, 2, str);
  Mclient.subscribe("C2H5OH");
  //Mclient.publish("/hello", str);
  dtostrf(gd2, 8, 2, str);
  Mclient.publish("/hello1", str);
  dtostrf(gasdata, 8, 2, str);
  Mclient.publish("/hello2", "gd3");

}