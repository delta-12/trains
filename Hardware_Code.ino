void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {

      String input = Serial.readStringUntil('\n');

      int commaIndex = input.indexOf(',');

      if (commaIndex != -1) 
      {
        String kp_str = input.substring(0, commaIndex);
        String ki_str = input.substring(commaIndex + 1);
        double kp_term = kp_str.toDouble();
        double ki_term = ki_str.toDouble();


        Serial.println(kp_term+ki_term);

      }
        digitalWrite(LED_BUILTIN, HIGH);
        delay(10);
        digitalWrite(LED_BUILTIN, LOW);
    }
}