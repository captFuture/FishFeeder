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

StaticJsonBuffer<200> jsonBuffer;
A4988 stepper(stepsPerRevolution, dirPin, stepPin, enablePin);
WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup() {
    Serial.begin(115200);
    while (!Serial){}
    M5.begin();
    //gslc_InitDebug(&DebugOut);
    //createGui();
    initSD();
    initSpiffs();
    

    stepper.begin(RPM, MICROSTEPS);
    stepper.setEnableActiveState(LOW);

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

  //updateGui();
  delay(10);
  M5.update();
}



