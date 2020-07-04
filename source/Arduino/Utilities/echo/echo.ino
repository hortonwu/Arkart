void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  Serial.print(millis());
  Serial.print("\t: ");
  
  if (Serial.available())
    while(Serial.available())
      Serial.print((char)Serial.read());
  else
    Serial.println();
}
