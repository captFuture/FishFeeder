#include "variables.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <WifiManager.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <FS.h> 
#include <ESP8266httpUpdate.h>

#define dirPin D4
#define stepPin D3
#define enablePin D2
#define stepsPerRevolution 200

#include "A4988.h"
#define RPM 120
#define MICROSTEPS 1

StaticJsonBuffer<500> jsonBuffer;
A4988 stepper(stepsPerRevolution, dirPin, stepPin, enablePin);

WiFiManager wifiManager;

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String mqttopic = String(topic);
  String inTopic = String(in_topic);
  String configTopic = String(config_topic);
  String rebootTopic = String(reboot_topic);
  if(mqttopic == inTopic){
    char buff_p[length];
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      buff_p[i] = (char)payload[i];
    }
    Serial.println();
    buff_p[length] = '\0';
    String msg_p = String(buff_p);
    int manualdegrees = msg_p.toInt(); // to Int
    turnMotor(manualdegrees, 1);

  }else if(mqttopic == configTopic){
    char buff_p[length];
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      buff_p[i] = (char)payload[i];
    }
    Serial.println();
    buff_p[length] = '\0';
    String msg_p = String(buff_p);
    Serial.println(msg_p); 

    SPIFFS.begin();
    File file = SPIFFS.open("/config.json", "w");
 
    if (!file) {
      Serial.println("Error opening file for writing");
      return;
    }
  
    int bytesWritten = file.print(msg_p);
  
    if (bytesWritten > 0) {
      Serial.println("File was written");
      Serial.println(bytesWritten);
  
    } else {
      Serial.println("File write failed");
    }
  
    file.close();
  }else if(mqttopic == rebootTopic){
    char buff_p[length];
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      buff_p[i] = (char)payload[i];
    }
    Serial.println();
    buff_p[length] = '\0';
    String msg_p = String(buff_p);
    int command = msg_p.toInt(); // to Int
    if(command == 0){
      Serial.println("update firmware");
      httpUpdate();
    }else{
      ESP.restart();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to - ");
    Serial.print(mqtt_server);
    Serial.println(" - ");
    Serial.print(mqtt_user);
    Serial.println(" - ");
    Serial.print(mqtt_pwd);
    Serial.println(" - ");
    Serial.print("in_topic: "); Serial.println(in_topic);
    Serial.print("out_topic: "); Serial.println(out_topic);
    Serial.print("config_topic: "); Serial.println(config_topic);
    
    
    if (client.connect(clientID,mqtt_user, mqtt_pwd)) {
    //if (client.connect(clientID,"megara", "Che11as!1")) {
      Serial.println("connected");
      client.subscribe(in_topic);
      client.subscribe(config_topic);
      client.subscribe(reboot_topic);
      snprintf (msg, 50, "Connected");
      
      postTelemetry(msg);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      delay(5000);
    }
  }
}

void readSpiffs(){
  //read configuration from FS json
    Serial.println("\nmounting FS...");
    if (SPIFFS.begin()) {
      Serial.println("mounted file system");

      //Serial.println("formatting file system");
      //SPIFFS.format();

      if (SPIFFS.exists("/config.json")) {
        //file exists, reading and loading
        Serial.println("reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) {
          Serial.println("opened config file");
          size_t size = configFile.size();
          // Allocate a buffer to store contents of the file.
          std::unique_ptr<char[]> buf(new char[size]);

          configFile.readBytes(buf.get(), size);
          DynamicJsonBuffer jsonBuffer;
          JsonObject& json = jsonBuffer.parseObject(buf.get());
          json.printTo(Serial);
          if (json.success()) {
            Serial.println("\nparsed json");
            degrees = json["degrees"];
            interval = json["interval"];
            startHour = json["start"];
            endHour = json["end"];

            strcpy(clientID, json["clientID"]);
            strcpy(mqtt_server, json["mqtt_server"]);
            strcpy(mqtt_user, json["mqtt_user"]);
            strcpy(mqtt_pwd, json["mqtt_pwd"]);

            strcpy(out_topic, json["out_topic"]);
            strcpy(in_topic, json["in_topic"]);
            strcpy(config_topic, json["config_topic"]);
            strcpy(reboot_topic, json["reboot_topic"]);

            Serial.println(degrees);
            Serial.println(interval);
            Serial.println(startHour);
            Serial.println(endHour);

            Serial.println(clientID);
            Serial.println(mqtt_server);
            Serial.println(mqtt_user);
            Serial.println(mqtt_pwd);

            Serial.println(out_topic);
            Serial.println(in_topic);
            Serial.println(config_topic);
            Serial.println(reboot_topic);
            
          } else {
            Serial.println("failed to load json config");
          }
        }
      }else{
        Serial.println("confignot present");
      }
    } else {
      Serial.println("failed to mount FS");
    }
    //end read
}

void setup() {
    Serial.begin(115200);
    while (!Serial){};
    readSpiffs();
    stepper.begin(RPM, MICROSTEPS);
    stepper.setEnableActiveState(LOW);
      client.setServer(mqtt_server, 1883);
      client.setCallback(callback);
      wifiManager.autoConnect(clientID);
      Serial.println("Wifi connected :)");
      timeClient.begin();
      timeClient.update();
      currentHour =  timeClient.getHours();
      currentMinute = timeClient.getMinutes();
      currentEpochTime = timeClient.getEpochTime();
      Serial.print("Startup hours:"); Serial.println(currentHour);
      Serial.print("Startup epoch:"); Serial.println(currentEpochTime); 
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

void feedTheFish(){
  //Feed the fish automatically
  turned = 0;
  turnMotor(degrees, 0);
}

void turnMotor(int variable, int turnmode){
  Serial.print("Start: "); Serial.print(variable);
  stepper.enable();
  delay(1000);
  stepper.rotate(-45);
  delay(1000);
  stepper.rotate(45);
  stepper.rotate(variable);
  snprintf (msg, 50, "Motor turned:%d:%d", variable, turnmode);
  postTelemetry(msg);
  turned = 1;
  stepper.disable();
  Serial.println(" : End");
}

void postTelemetry(char msg[]){
  String payload = "{";
  payload += "\"clientID\":"; payload += "\""; payload += clientID; payload += "\"";
  payload += ",";
  payload += "\"start\":"; payload += startHour;
  payload += ",";
  payload += "\"end\":"; payload += endHour;
  payload += ",";
  payload += "\"interval\":"; payload += interval;
  payload += ",";
  payload += "\"weight\":"; payload += degrees;
  payload += ",";
  payload += "\"message\":"; payload; payload += "\""; payload += msg; payload += "\"";
  payload += "}";

  char attributes[200];
  payload.toCharArray( attributes, 200 );
  client.publish(out_topic, attributes);
}

void httpUpdate(){
  char updateString[] = "";
  snprintf (updateString, 150, "http://tarantl.com/fishfeeder/%s/firmware.bin", clientID);
  Serial.println(updateString);
  t_httpUpdate_return ret = ESPhttpUpdate.update(updateString); 
  
  
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
  
}