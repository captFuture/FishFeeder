void feedTheFish(){
  //do something to turn the motor
  //degrees = ;
  turned = 0;
  turnMotor();
}

void turnMotor(){
  if(stepMotorModule){
    Serial.print("Start: "); Serial.println(degrees);

    snprintf (grbl, 50, "G1 X%dY20Z20 F800", degrees);

    SendCommand(STEPMOTOR_I2C_ADDR, grbl);
    snprintf (msg, 50, "StepperModule turned: %s", grbl);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(out_topic, msg);
    turned = 1;
  }else{
    stepper.enable();
    Serial.print("Start: "); Serial.println(degrees);
    stepper.rotate(degrees);
    
      snprintf (msg, 50, "Motor turned: %d degrees", degrees);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(out_topic, msg);
    
    turned = 1;
    stepper.disable();
  }

}

void SendByte(byte addr, byte b) {
  Wire.beginTransmission(addr);
  Wire.write(b);
  Wire.endTransmission();
}

void SendCommand(byte addr, char *c) {
  Wire.beginTransmission(addr);
  while ((*c) != 0) {
    Wire.write(*c);
    c++;
  }
  Wire.write(0x0d);
  Wire.write(0x0a);
  Wire.endTransmission();
}