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