void performUpdate(Stream &updateSource, size_t updateSize) {
   if (Update.begin(updateSize)) {
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize) {
         Serial.println("Written : " + String(written) + " successfully");
      }
      else {
         Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end()) {
         Serial.println("OTA done!");
         if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting.");
         }
         else {
            Serial.println("Update not finished? Something went wrong!");
         }
      }
      else {
         Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }

   }
   else
   {
      Serial.println("Not enough space to begin OTA");
   }
}

// check given FS for valid update.bin and perform update if available
void updateFromFS(fs::FS &fs) {
   File updateBin = fs.open("/firmware.bin");
   if (updateBin) {
      if(updateBin.isDirectory()){
         Serial.println("Error, firmware.bin is not a file");
         updateBin.close();
         return;
      }

      size_t updateSize = updateBin.size();

      if (updateSize > 0) {
         Serial.println("Starting Update from SD");
         performUpdate(updateBin, updateSize);
      }
      else {
         Serial.println("Error, file is empty");
      }
      updateBin.close();
      fs.remove("/firmware.bin");
   } else {
      Serial.println("Could not load firmware.bin from sd root");
   }
}

void initSD(){
  if(!SD.begin()){
      Serial.println("No SD card present");
      SDpresent = false;
      return;
  }else{
    updateFromFS(SD);
    Serial.println("SD card present");
    SDpresent = true;
  }
  uint8_t cardType = SD.cardType();
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }
  readSDconfig();
}

void readSDconfig(){
  if(SDpresent){
    if (SD.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading SD config file");
      File configFile = SD.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened SD config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        String msg_p;

        json.printTo(msg_p);
        json.printTo(Serial);
        // prepare SD config for writing to spiffs
        //Serial.println("save to SPIFFS ----------------");
        saveConfigToSpiffs(msg_p);
        //Serial.println("save to SPIFFS END ----------------");
        if (json.success()) {
          Serial.println("\nparsed json from SD");

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

            //saveConfigToSpiffs(json);
            //buff_p[length] = '\0';
            //String msg_p = String(json);

        } else {
          Serial.println("failed to load json config from SD");
        }
        Serial.println("close json config from SD");
        configFile.close();
      }
    }else{
        Serial.println("config.json does not exist on SD");
    }
  }
}

