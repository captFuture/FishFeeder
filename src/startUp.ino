void startUp(){
  static uint8_t brightness, pre_brightness;
  M5.Lcd.setBrightness(0);
  M5.Lcd.pushImage(0, 0, 320, 240, (uint16_t *)ma_logo);
  for(int i=0; i<1000; i++) {
      delayMicroseconds(40);
      brightness = (i/157);
      if(pre_brightness != brightness) {
          pre_brightness = brightness;
          M5.Lcd.setBrightness(brightness);
      }
  }

  for(int i=255; i>=0; i--) {
      M5.Lcd.setBrightness(i);
      if(i<=32) {
      }
      delay(2);
  }
  M5.Speaker.mute();

  M5.Lcd.fillScreen(BLACK);
  delay(800);
  for(int i=0; i>=100; i++) {
      M5.Lcd.setBrightness(i);
      delay(2);
  }
  M5.Lcd.setTextSize(2);
  M5.Lcd.setBrightness(display_brightness);
  M5.Lcd.pushImage(0, 0, 320, 240, (uint16_t *)ma_gui);
  Serial.println("");
  Serial.println("************************************************");
  Serial.println("*   Maker Austria 2020                         *");
  Serial.print("*   Firmware Version v"); Serial.print(version); Serial.println("                    *");
  Serial.println("************************************************");
  Serial.println("");
  spr.createSprite(320, 25);
}