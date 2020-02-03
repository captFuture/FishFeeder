void drawValues(){
    M5.Lcd.setTextSize(3);
    M5.Lcd.setBrightness(display_brightness);
    M5.Lcd.setCursor(3, 3);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println(clientID);
}

void drawText(){
    M5.Lcd.setTextSize(3);
    M5.Lcd.setBrightness(display_brightness);
    M5.Lcd.setCursor(3, 3);
    M5.Lcd.setTextColor(WHITE);
    
}

