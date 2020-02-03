// Getting commands from MQTT and save to spiffs if config is sent
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
    saveConfigToSpiffs(msg_p);
    
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
      Serial.println("TODO: http update firmware");
    }else{
      ESP.restart();
    }
    
  }
}

// Reconnect MQTT if disconnected
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to -");
    Serial.print(mqtt_server);
    Serial.println("- ");
    Serial.print("in_topic: "); Serial.println(in_topic);
    Serial.print("out_topic: "); Serial.println(out_topic);
    Serial.print("config_topic: "); Serial.println(config_topic);
    if (client.connect(clientID,mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      client.subscribe(in_topic);
      client.subscribe(config_topic);
      client.subscribe(reboot_topic);

      snprintf (msg, 50, "Connected: %s", clientID);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(out_topic, msg);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      delay(5000);
    }
  }
}