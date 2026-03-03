#include <Arduino.h>

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
    uint32_t lastMillis; // Para el filtrado
};


Button button1 = {20, 0, false};


void IRAM_ATTR isr() {
    uint32_t now = millis(); //Solo contamos si han pasado más de 200ms desde el último pulso
    if (now - button1.lastMillis > 200) {
  button1.numberKeyPresses += 1;
  button1.pressed = true;
      button1.lastMillis = now;
      }
}

void setup() {
  Serial.begin(115200);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);
}
void loop() {
  if (button1.pressed) {
    Serial.printf("Button 1 has been pressed %u times\n",
    button1.numberKeyPresses);
    button1.pressed = false;
  }

//Detach Interrupt after 1 Minute
  static uint32_t lastMillis = 0;
  if (millis() - lastMillis > 60000) {
  lastMillis = millis();
    detachInterrupt(button1.PIN);
    Serial.println("Interrupt Detached!");
  }
}