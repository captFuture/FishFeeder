#include "variables.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <WifiManager.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <FS.h>
#include <SD.h>
#include "SPIFFS.h"
#include "A4988.h"
#include <M5Stack.h>
#include <Wire.h>



StaticJsonBuffer<200> jsonBuffer;
A4988 stepper(stepsPerRevolution, dirPin, stepPin, enablePin);
WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

TFT_eSprite spr = TFT_eSprite(&M5.Lcd);

void setup() {
    Serial.begin(115200);
    while (!Serial){}
    M5.begin();
    
    startUp();
    initSD();
    initSpiffs();
    initSensors();
    
    //if(stepMotorModule){
      Wire.begin();
    //}else{
      stepper.begin(RPM, MICROSTEPS);
      stepper.setEnableActiveState(LOW);
    //}

    if(enableWifi){
      client.setServer(mqtt_server, 1883);
      client.setCallback(callback);
      wifiManager.autoConnect(clientID);
      Serial.println("Wifi connected :)");
      timeClient.begin();
      timeClient.update();
      currentHour =  timeClient.getHours();
      currentMinute = timeClient.getMinutes();
      Serial.print("Startup hours:"); Serial.println(currentHour);
    }
}

void loop() {
  getSensorData();

  if(enableWifi){
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    timeClient.update();
    loopHour = timeClient.getHours();
    loopMinute = timeClient.getMinutes();
    //Serial.print("Loop hours:"); 
    //Serial.println(loopHour);

    if(debugMode){
      // Rotate Motors if one minute is over
      if(currentMinute != -1){
        if(loopMinute > currentMinute){
          //if(currentMinute >= startHour && currentHour <= endHour){
            Serial.println("FeedTheFish Minutes");
            feedTheFish();
            currentMinute = timeClient.getMinutes();
          //}
        }
      }
    }else{
      // Rotate Motors if one hour is over
      if(currentHour != -1){
        if(loopHour > currentHour){
          if(currentHour >= startHour && currentHour <= endHour){
            Serial.println("FeedTheFish Hours");
            feedTheFish();
            currentHour = timeClient.getHours();
          }
        }
      }
    }

    }else{
      // TODO - manual mode without timeserver 
    }

  if(turned == 0){
    Serial.println("Turning in loop");
    turnMotor();
  }


  if (M5.BtnA.wasReleased()) {
    Serial.println("A button was pressed.");
    
  } 
  
  if (M5.BtnB.wasReleased()) {
    Serial.println("B button was pressed.");
    if(btnFocus == 0){
      feedTheFish();
    }
  } 

  if (M5.BtnC.wasReleased()) {
    Serial.println("C button was pressed.");
  }

  if(M5.BtnB.pressedFor(3000)) {
      Serial.println("Longpress B - debug");
      debugMode = !debugMode;
      delay(1000);
  }

  if(M5.BtnA.pressedFor(3000)) {
      Serial.println("Longpress A - stepModule");
      stepMotorModule = !stepMotorModule;
      delay(1000);
  }
/*
  if(M5.BtnC.pressedFor(3000)) {
      Serial.println("Longpress C");
      delay(1000);
  }*/
  if(stepMotorModule){
    // Get Data from Module.
    Wire.requestFrom(STEPMOTOR_I2C_ADDR, 1);
    if (Wire.available() > 0) {
      int u = Wire.read();
      if (u != 0) Serial.write(u);
    }
    delay(1);
    // Send Data to Module.
    while (Serial.available() > 0) {
      int inByte = Serial.read();
      SendByte(STEPMOTOR_I2C_ADDR, inByte);
    }
  }

  delay(10);
  M5.update();
  
}



