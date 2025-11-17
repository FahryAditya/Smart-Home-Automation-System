#include <DHT.h>
#include <IRremote.h>

// ------------------- Hardware Config ------------------
#define RELAY_PIN 8
#define DHT_PIN 2
#define DHT_TYPE DHT22
#define IR_PIN 7
#define LDR_PIN A0
#define MOTION_PIN 4
#define MANUAL_SWITCH_PIN 5

// ------------------- Objects & Globals ------------------
DHT dht(DHT_PIN, DHT_TYPE);
IRrecv irrecv(IR_PIN);
decode_results results;

bool lampState = false;
bool manualOverride = false;

enum Mode { NORMAL, ENERGY_SAVER, NIGHT_MODE, SECURITY_MODE };
Mode currentMode = NORMAL;

unsigned long lampTimer = 0;
unsigned long lastCheck = 0;
unsigned long securityAlarmTimer = 0;

// ------------------- Setup ------------------
void setup() {
  Serial.begin(9600);
  dht.begin();
  irrecv.enableIRIn();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(MOTION_PIN, INPUT);
  pinMode(MANUAL_SWITCH_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, LOW);

  Serial.println("=== ADVANCED SMART HOME SYSTEM STARTED ===");
}

// ------------------- Main Loop ------------------
void loop() {
  readManualSwitch();
  handleIR();
  handleBluetooth();
  environmentLogic();
  modeLogic();
  securityLogic();
  timedLampCutoff();
  displayStatus();
}

// ------------------- Manual Switch Override ------------------
void readManualSwitch() {
  if (!digitalRead(MANUAL_SWITCH_PIN)) {
    manualOverride = true;
    lampState = !lampState;
    digitalWrite(RELAY_PIN, lampState);
    Serial.println("[MANUAL] Toggle pressed!");
    delay(300);
  }
}

// ------------------- IR Remote Control ------------------
void handleIR() {
  if (irrecv.decode(&results)) {
    unsigned long code = results.value;

    if (code == 0xFFA25D) { // POWER = toggle lamp
      lampState = !lampState;
      digitalWrite(RELAY_PIN, lampState);
    }

    if (code == 0xFF629D) { currentMode = NORMAL; }
    if (code == 0xFFA857) { currentMode = ENERGY_SAVER; }
    if (code == 0xFF22DD) { currentMode = NIGHT_MODE; }
    if (code == 0xFF02FD) { currentMode = SECURITY_MODE; }

    irrecv.resume();
  }
}

// ------------------- Bluetooth Commands ------------------
void handleBluetooth() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');

    if (cmd == "ON") {
      lampState = true;
      digitalWrite(RELAY_PIN, HIGH);
    }
    if (cmd == "OFF") {
      lampState = false;
      digitalWrite(RELAY_PIN, LOW);
    }
    if (cmd == "MODE:NORMAL") currentMode = NORMAL;
    if (cmd == "MODE:ES") currentMode = ENERGY_SAVER;
    if (cmd == "MODE:NIGHT") currentMode = NIGHT_MODE;
    if (cmd == "MODE:SECURITY") currentMode = SECURITY_MODE;
  }
}

// ------------------- Sensor-Based Environment Logic ------------------
void environmentLogic() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int lightLevel = analogRead(LDR_PIN);

  // If manual override is active, skip auto logic
  if (manualOverride) return;

  // Too hot → turn ON lamp with cooling fan logic (concept)
  if (t > 30) {
    lampState = true;
    lampTimer = millis();
  }

  // Too humid → lamp ON for airflow
  if (h > 75) {
    lampState = true;
    lampTimer = millis();
  }

  // Too dark → auto-night illumination
  if (lightLevel < 200 && currentMode == NIGHT_MODE) {
    lampState = true;
  }

  digitalWrite(RELAY_PIN, lampState ? HIGH : LOW);
}

// ------------------- Mode Logic (Adaptive) ------------------
void modeLogic() {
  float t = dht.readTemperature();
  int lightLevel = analogRead(LDR_PIN);

  if (currentMode == ENERGY_SAVER) {
    if (t < 20) { lampState = false; }
    if (lightLevel > 700) { lampState = false; }
  }

  if (currentMode == NIGHT_MODE) {
    if (lightLevel < 150) lampState = true;
  }
}

// ------------------- Security Mode ------------------
void securityLogic() {
  bool motion = digitalRead(MOTION_PIN);

  if (currentMode == SECURITY_MODE) {
    if (motion) {
      lampState = true;
      lampTimer = millis();
      Serial.println("[SECURITY] Motion detected!");

      securityAlarmTimer = millis();
    }
  }

  // Auto-off lamp after alarm burst
  if (millis() - securityAlarmTimer > 5000) {
    if (currentMode == SECURITY_MODE) lampState = false;
  }

  digitalWrite(RELAY_PIN, lampState);
}

// ------------------- Auto Lamp Cut-Off Timer ------------------
void timedLampCutoff() {
  if (lampState && millis() - lampTimer > 15 * 60 * 1000) {
    lampState = false;
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("[TIMER] Lamp auto-off after 15 minutes.");
  }
}

// ------------------- Status Monitoring ------------------
void displayStatus() {
  if (millis() - lastCheck > 1000) {
    lastCheck = millis();

    Serial.print("Mode: ");
    switch (currentMode) {
      case NORMAL:        Serial.print("NORMAL"); break;
      case ENERGY_SAVER:  Serial.print("ENERGY SAVER"); break;
      case NIGHT_MODE:    Serial.print("NIGHT"); break;
      case SECURITY_MODE: Serial.print("SECURITY"); break;
    }

    Serial.print(" | Lamp: ");
    Serial.print(lampState ? "ON" : "OFF");

    Serial.print(" | Temp: ");
    Serial.print(dht.readTemperature());

    Serial.print(" | Hum: ");
    Serial.print(dht.readHumidity());

    Serial.print(" | Light: ");
    Serial.print(analogRead(LDR_PIN));

    Serial.println();
  }
}
