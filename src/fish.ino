#define RELEASE FRELEASE

#if RELEASE == true
  #include "variables.h"
#else
  #include "variables_dev.h"
#endif

#include <Arduino.h>
#include <PubSubClient.h>
#include <WifiManager.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <FS.h> 
#include <ESP8266httpUpdate.h>
#include "A4988.h"


StaticJsonBuffer<500> jsonBuffer;
A4988 stepper(stepsPerRevolution, dirPin, stepPin, enablePin);

WiFiManager wifiManager;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup() {
    Serial.begin(115200);
    while (!Serial){};

    if(!debugMode){
      readSpiffs();
      Serial.println("\n***************************************");
      Serial.print("*      "); Serial.print(clientID); Serial.print(" - ");  Serial.print(version); Serial.println("       *");
      Serial.println("***************************************");
    }else{
      Serial.println("\n************DEBUG**********************");
      Serial.print("*      "); Serial.print(clientID); Serial.print(" - ");  Serial.print(version); Serial.println("       *");
      Serial.println("***************************************");
    }
    
    stepper.begin(RPM, MICROSTEPS);
    stepper.setEnableActiveState(LOW);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    wifiManager.setConfigPortalTimeout(180);
    wifiManager.autoConnect(clientID);
    Serial.println("Wifi connected :)");

    timeClient.begin();
    timeClient.update();
    currentHour =  timeClient.getHours();
    currentMinute = timeClient.getMinutes();
    currentEpochTime = timeClient.getEpochTime();
    Serial.print("Startup hours:"); Serial.println(currentHour);
    Serial.print("Startup epoch:"); Serial.println(currentEpochTime);

    //DHT dht(tempPin, DHTTYPE); 
}

void loop() {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    timeClient.update();
    loopHour = timeClient.getHours();
    loopMinute = timeClient.getMinutes();
    loopEpochTime = timeClient.getEpochTime();

      // Rotate Motors if interval in minutes is over
      if(currentEpochTime != -1){
        if(loopEpochTime > (currentEpochTime+interval*60)){
          if(loopHour >= startHour && loopHour <= endHour){
            Serial.println("FeedTheFish");
            feedTheFish();
            currentEpochTime = timeClient.getEpochTime();
          }
        }
      }

  delay(1000);
}
