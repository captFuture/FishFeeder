#include "variables.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <WifiManager.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <FS.h> 
#include <EasyButton.h>

#define dirPin D4
#define stepPin D3
#define enablePin D2
#define stepsPerRevolution 200

#include "A4988.h"
#define RPM 120
#define MICROSTEPS 1

StaticJsonBuffer<200> jsonBuffer;
//A4988 stepper(stepsPerRevolution, dirPin, stepPin);
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
  String configTopic = String( config_topic);
  if(mqttopic == inTopic){
    char buff_p[length];
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      buff_p[i] = (char)payload[i];
    }
    Serial.println();
    buff_p[length] = '\0';
    String msg_p = String(buff_p);
    degrees = msg_p.toInt(); // to Int
    turned = 0;

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
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      client.subscribe(in_topic);
      client.subscribe(config_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      delay(5000);
    }
  }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) 
    { 
      ; // Wait for serial to connect 
    }

    //read configuration from FS json
    Serial.println("mounting FS...");

    if (SPIFFS.begin()) {
      Serial.println("mounted file system");
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
            amount = json["amount"];
            startHour = json["start"];
            endHour = json["end"];

            Serial.println(degrees);
            Serial.println(amount);
            Serial.println(startHour);
            Serial.println(endHour);
            //strcpy(output, json["output"]);
          } else {
            Serial.println("failed to load json config");
          }
        }
      }
    } else {
      Serial.println("failed to mount FS");
    }
    //end read

    stepper.begin(RPM, MICROSTEPS);
    stepper.setEnableActiveState(LOW);
    //stepper.enable();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    wifiManager.autoConnect("FishFeeder_001");
    Serial.println("Wifi connected :)");
    timeClient.begin();
    timeClient.update();
    currentHour =  timeClient.getHours();
    Serial.print("hours:"); Serial.println(currentHour);
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(turned == 0){
    turnMotor();
  }

  timeClient.update();
  //Serial.println(timeClient.getFormattedTime());
  //Serial.println(timeClient.getHours());
  if(timeClient.getHours() > currentHour ){
    if(currentHour != -1){
      Serial.println("FeedTheFish");
      feedTheFish();
      currentHour = timeClient.getHours();
    }
  }
  delay(1000);
}

void feedTheFish(){
  //do something to turn the motor
  //degrees = ;
  turned = 0;
  turnMotor();
}

void turnMotor(){
  stepper.enable();
  Serial.print("Start: "); Serial.println(degrees);
  stepper.rotate(degrees);
  snprintf (msg, 50, "turned: %d", degrees);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(out_topic, msg);
  turned = 1;
  stepper.disable();
}
