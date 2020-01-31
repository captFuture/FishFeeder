#include <Arduino.h>
#include <PubSubClient.h>
#include <WifiManager.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <FS.h> 

#define dirPin D4
#define stepPin D3
#define stepsPerRevolution 200

#include "A4988.h"
#define RPM 120
#define MICROSTEPS 1

StaticJsonBuffer<200> jsonBuffer;

A4988 stepper(stepsPerRevolution, dirPin, stepPin);
WiFiManager wifiManager;
const char* mqtt_server = "makeradmin.ddns.net";
const char* mqtt_user = "linaro";
const char* mqtt_pwd = "Che11as!1";
const char* out_topic = "fish/001/out";
const char* in_topic = "fish/001/in";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

long lastMsg = 0;
char msg[50];
int degrees = 360;
int turned = 1;
int currentHour = -1;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char buff_p[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    buff_p[i] = (char)payload[i];
  }
  Serial.println();
  JsonObject& root = jsonBuffer.parseObject(buff_p);
  int vali = root["value"];
  Serial.println(vali); 


  buff_p[length] = '\0';
  String msg_p = String(buff_p);
  degrees = msg_p.toInt(); // to Int
  turned = 0;

  if (!root.success()) {
    Serial.println("parseObject() failed");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "FishFeeder_001";
    
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      client.subscribe(in_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      delay(5000);
    }
  }
}

void setup() {
    SPIFFS.begin(); 
    Serial.begin(115200);
    stepper.begin(RPM, MICROSTEPS);
    //stepper.enable();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(turned == 0){
    turnMotor();
  }

  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  if(timeClient.getHours() > currentHour && currentHour != -1){
    Serial.println("FeedTheFish");
    feedTheFish();
    currentHour = timeClient.getHours();
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
  Serial.println("Start");
  stepper.rotate(degrees);
  snprintf (msg, 50, "turned: %ld", degrees);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(out_topic, msg);
  turned = 1;
  stepper.disable();
}