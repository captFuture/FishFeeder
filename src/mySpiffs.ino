void initSpiffs(){
  SPIFFS.begin(true);
  if (SPIFFS.begin()) {
    Serial.println("mounted SPIFFS file system");

    if(!SDpresent){
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading SPIFFS config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened SPIFFS config file");
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

            strcpy(clientID, json["clientID"]);
            strcpy(mqtt_server, json["mqtt_server"]);
            strcpy(mqtt_user, json["mqtt_user"]);
            strcpy(mqtt_pwd, json["mqtt_pwd"]);

            strcpy(out_topic, json["out_topic"]);
            strcpy(in_topic, json["in_topic"]);
            strcpy(config_topic, json["config_topic"]);
            strcpy(reboot_topic, json["reboot_topic"]);

            /*Serial.println(degrees);
            Serial.println(amount);
            Serial.println(startHour);
            Serial.println(endHour);

            Serial.println(clientID);

            Serial.println(mqtt_server);
            Serial.println(mqtt_user);
            Serial.println(mqtt_pwd);

            Serial.println(out_topic);
            Serial.println(in_topic);
            Serial.println(config_topic);
            Serial.println(reboot_topic);*/
        } else {
          Serial.println("failed to load json config from SPIFFS");
        }
        configFile.close();
      }
    }else{
        Serial.println("config.json does not exist in SPIFFS");
    }
    }
  } else {
    Serial.println("failed to mount SPIFFS");
  }
}

void saveConfigToSpiffs(String myconfig){
    Serial.println("mounted SPIFFS file system");
    SPIFFS.begin(true);
    if (SPIFFS.begin()) {
      File file = SPIFFS.open("/config.json", "w");
      
      if (!file) {
        Serial.println("Error opening SPIFFS file for writing");
        return;
      }
    
      int bytesWritten = file.print(myconfig);
    
      if (bytesWritten > 0) {
        Serial.print("\nFile was written to SPIFFS - ");
        Serial.println(bytesWritten);
    
      } else {
        Serial.println("File write to SPIFFS failed");
      }
      file.close();
    }
}