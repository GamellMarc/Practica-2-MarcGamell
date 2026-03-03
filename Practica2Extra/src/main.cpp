#include <Arduino.h>

// --- Definición de Pines ---
const int PIN_LED = 35;          
const int PIN_BTN_UP = 40;      
const int PIN_BTN_DOWN = 20;    

// --- Variables de Configuración ---
hw_timer_t *timer = NULL;
const int TIMER_INTERVAL_US = 1000; 

const int MAX_PERIOD_MS = 2000; 
const int MIN_PERIOD_MS = 50;   
const int STEP_MS = 100;        
const int DEBOUNCE_TIME_MS = 50; 

// --- Variables Volátiles (Compartidas con la ISR) ---
volatile int currentPeriodMs = 500; 
volatile int blinkCounter = 0;
volatile bool ledState = false;

// --- CRITICAL SECTION MUX (ESTA ES LA LÍNEA QUE FALTABA) ---
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Estructura para manejar el estado de los botones
struct ButtonState {
    int pin;
    int stableCount;      
    bool lastReading;     
    bool state;           
    bool actionPending;   
};

volatile ButtonState btnUp = {PIN_BTN_UP, 0, true, true, false};
volatile ButtonState btnDown = {PIN_BTN_DOWN, 0, true, true, false};

// --- Declaración de funciones ---
void IRAM_ATTR onTimer(); 
void handleButtonLogic(volatile ButtonState &btn);

// --- Configuración Inicial (Setup) ---
void setup() {
    Serial.begin(115200);

    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BTN_UP, INPUT_PULLUP);
    pinMode(PIN_BTN_DOWN, INPUT_PULLUP);

    // Configurar Timer 0 (Prescaler 80, CountUp true)
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, TIMER_INTERVAL_US, true);
    timerAlarmEnable(timer);

    Serial.println("Sistema iniciado. Timer corriendo a 1ms.");
}

// --- Bucle Principal (Loop) ---
void loop() {
    // CASO 1: Botón SUBIR Frecuencia
    if (btnUp.actionPending) {
        // Bloqueamos interrupciones momentáneamente para leer/escribir variable compartida
        portENTER_CRITICAL(&timerMux); 
        currentPeriodMs -= STEP_MS;
        if (currentPeriodMs < MIN_PERIOD_MS) currentPeriodMs = MIN_PERIOD_MS;
        btnUp.actionPending = false; 
        portEXIT_CRITICAL(&timerMux);
        
        Serial.printf("Frecuencia Aumentada (Periodo menor). Periodo actual: %d ms\n", currentPeriodMs);
    }

    // CASO 2: Botón BAJAR Frecuencia
    if (btnDown.actionPending) {
        portENTER_CRITICAL(&timerMux); 
        currentPeriodMs += STEP_MS;
        if (currentPeriodMs > MAX_PERIOD_MS) currentPeriodMs = MAX_PERIOD_MS;
        btnDown.actionPending = false;
        portEXIT_CRITICAL(&timerMux);
        
        Serial.printf("Frecuencia Disminuida (Periodo mayor). Periodo actual: %d ms\n", currentPeriodMs);
    }
    
    delay(10); 
}

// --- Rutina de Servicio de Interrupción (ISR) ---
void IRAM_ATTR onTimer() {
    
    // 1. Lógica del LED
    blinkCounter++;
    if (blinkCounter >= currentPeriodMs) {
        blinkCounter = 0;
        ledState = !ledState;
        digitalWrite(PIN_LED, ledState);
    }

    // 2. Lógica de Botones
    handleButtonLogic(btnUp);
    handleButtonLogic(btnDown);
}

void IRAM_ATTR handleButtonLogic(volatile ButtonState &btn) {
    bool currentRead = digitalRead(btn.pin);

    if (currentRead != btn.lastReading) {
        btn.stableCount = 0;
    } else {
        btn.stableCount++;
    }

    btn.lastReading = currentRead;

    if (btn.stableCount >= DEBOUNCE_TIME_MS) {
        if (btn.state != currentRead) {
            btn.state = currentRead; 

            // Detectar Flanco de Bajada (Pulsación en PULLUP)
            if (btn.state == LOW) {
                btn.actionPending = true; 
            }
        }
    }
}