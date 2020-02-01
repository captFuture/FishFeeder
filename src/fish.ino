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

#define BUTTON_PIN 0 // flash button

EasyButton button(BUTTON_PIN);
StaticJsonBuffer<200> jsonBuffer;

A4988 stepper(stepsPerRevolution, dirPin, stepPin, enablePin);

WiFiManager wifiManager;
const char* mqtt_server = "makeradmin.ddns.net";
const char* mqtt_user = "linaro";
const char* mqtt_pwd = "Che11as!1";
const char* out_topic = "fish/001/out";
const char* in_topic = "fish/001/in";
const char* config_topic = "fish/001/config";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

long lastMsg = 0;
char msg[50];
int degrees = 0;
int amount = 0;
int startHour = 0;
int endHour = 0;
int turned = 1;
int currentHour = -1; 

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

void onPressed() {
    /*Serial.println("Erase config and start Wifimanager");
    WiFiManager wifiManager;      
    wifiManager.resetSettings();
    delay(1000);
    ESP.reset();*/

    // For debugging use this below -> flash button rotates motor
    feedTheFish();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "FishFeeder_001";
    
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

    button.begin();
    button.onPressed(onPressed);

    wifiManager.autoConnect("FishFeeder_001");
    Serial.println("Wifi connected :)");
    // let's say 100 complete revolutions (arbitrary number)
    //stepper.startMove(1 * MOTOR_STEPS * MICROSTEPS);     // in microsteps
    // stepper.startRotate(1 * 360);                     // or in degrees
    timeClient.begin();
    currentHour =  timeClient.getHours();
    Serial.println(currentHour);
}
void loop() {
  //turnMotor();
  //delay(2000);
  //feedTheFish();
  //delay(2000);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(turned == 0){
    turnMotor();
  }

  timeClient.update();
  //Serial.println(timeClient.getFormattedTime());
  if(timeClient.getHours() > currentHour ){
    if(currentHour != -1){
      Serial.println("FeedTheFish");
      feedTheFish();
      currentHour = timeClient.getHours();
    }
  }
  button.read();
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
  Serial.println("Start");
  
  stepper.rotate(360);
  snprintf (msg, 50, "turned: %ld", degrees);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(out_topic, msg);
  turned = 1;
  stepper.disable();
}