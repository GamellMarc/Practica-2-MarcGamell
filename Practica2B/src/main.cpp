#include <Arduino.h>

// --- 1. ESTRUCTURA Y VARIABLES DEL BOTÓN ---
struct Button {
  const uint8_t PIN;
  bool pressed;
  uint32_t lastMillis; // Para el filtrado de rebotes
};

// Configurado en el Pin 20 (ajústalo si usas otro)
Button button1 = {20, false, 0};

// --- 2. VARIABLES DEL TIMER ---
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
bool contadorActivo = true; // Para saber el estado y no imprimir mensajes repetidos

// --- 3. ISR DEL BOTÓN (Interrupción por hardware) ---
void IRAM_ATTR isrButton() {
  uint32_t now = millis();
  // Debounce: Solo aceptamos pulsaciones si pasaron 200ms desde la última
  if (now - button1.lastMillis > 200) {
    button1.pressed = true;
    button1.lastMillis = now;
  }
}

// --- 4. ISR DEL TIMER (Interrupción por hardware) ---
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);

  // A. Configuración del Botón
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isrButton, FALLING);

  // B. Configuración del Timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); // 1 segundo
  timerAlarmEnable(timer);

  Serial.println("--- Sistema Iniciado: Contando ---");
}

void loop() {
  // --- A. GESTIÓN DEL BOTÓN (PARADA) ---
  if (button1.pressed) {
    button1.pressed = false; // Resetear la bandera del botón

    if (contadorActivo) {
      // AQUÍ OCURRE LA MAGIA: Desactivamos la alarma del timer
      timerAlarmDisable(timer);
      
      contadorActivo = false;
      Serial.println("\n!!! BOTÓN PULSADO !!!");
      Serial.println("El temporizador ha sido detenido.");
      Serial.printf("Conteo final: %d segundos.\n", totalInterruptCounter);
    } else {
      Serial.println("El temporizador ya estaba detenido.");
    }
  }

  // --- B. GESTIÓN DEL CONTEO ---
  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;
    Serial.print("Tiempo: ");
    Serial.println(totalInterruptCounter);
  }
}