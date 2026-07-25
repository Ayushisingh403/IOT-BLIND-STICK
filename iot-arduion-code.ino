
/*
SMART BLIND STICK - LED BLINKING FIXED
*/

// ============================
// PIN DEFINITIONS
// ============================
const int trigFront = 9;
const int echoFront = 10;
const int trigDown = 11;
const int echoDown = 12;
const int buzzerObstacle = 7;
const int buzzerAlert = 6;
const int waterPin = A0;
const int ldrPin = A1;
const int ledPin = 5;

// ============================
// THRESHOLD SETTINGS
// ============================
int OBSTACLE_DISTANCE = 30;
int STAIR_DISTANCE = 400;
int WATER_WET_THRESHOLD = 400;
int DARK_THRESHOLD = 500;  // CHANGED: Lowered from 800

// ============================
// GLOBAL VARIABLES
// ============================
int frontDistance = 0;
int groundDistance = 0;
int waterLevel = 0;
int lightLevel = 0;

bool isDark = false;
bool ldrCalibrated = false;
bool ldrToGND = true;

unsigned long lastPrintTime = 0;
unsigned long lastObstacleBeep = 0;
unsigned long lastStairBeep = 0;
unsigned long lastWaterBeep = 0;
const unsigned long PRINT_INTERVAL = 1000;

// ============================
// SETUP FUNCTION
// ============================
void setup() {
  Serial.begin(9600);
  Serial.println("Smart Blind Stick - LED Fixed");
  
  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigDown, OUTPUT);
  pinMode(echoDown, INPUT);
  pinMode(buzzerObstacle, OUTPUT);
  pinMode(buzzerAlert, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  digitalWrite(buzzerObstacle, LOW);
  digitalWrite(buzzerAlert, LOW);
  digitalWrite(ledPin, LOW);
  
  // Simple calibration
  simpleCalibration();
  Serial.println("System Ready!");
}

// ============================
// MAIN LOOP - SIMPLIFIED
// ============================
void loop() {
  unsigned long currentMillis = millis();
  
  // Read sensors
  readSensors();
  
  // Process sensors
  processObstacle(currentMillis);
  processStairs(currentMillis);
  processWater(currentMillis);
  processLight();  // SIMPLIFIED FUNCTION
  
  // Display readings
  if (currentMillis - lastPrintTime >= PRINT_INTERVAL) {
    showReadings();
    lastPrintTime = currentMillis;
  }
}

// ============================
// SIMPLIFIED LIGHT PROCESSING
// ============================
void processLight() {
  // Simple logic: LED ON only when VERY dark
  // Adjust DARK_THRESHOLD based on your environment
  
  if (lightLevel > DARK_THRESHOLD) {
    // Dark environment - LED ON (steady, not blinking)
    digitalWrite(ledPin, HIGH);
    isDark = true;
  } else {
    // Bright environment - LED OFF
    digitalWrite(ledPin, LOW);
    isDark = false;
  }
}

// ============================
// SIMPLIFIED SENSOR FUNCTIONS
// ============================
void readSensors() {
  frontDistance = getDistance(trigFront, echoFront);
  delay(30);
  groundDistance = getDistance(trigDown, echoDown);
  waterLevel = analogRead(waterPin);
  lightLevel = analogRead(ldrPin);
}

long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  long duration = pulseIn(echo, HIGH, 10000);
  if (duration <= 0) return 400;
  
  return duration * 0.034 / 2;
}

void processObstacle(unsigned long currentMillis) {
  if (frontDistance < OBSTACLE_DISTANCE && frontDistance > 2) {
    if (currentMillis - lastObstacleBeep > 500) {
      tone(buzzerObstacle, 1000, 200);
      lastObstacleBeep = currentMillis;
    }
  }
}

void processStairs(unsigned long currentMillis) {
  if (groundDistance > STAIR_DISTANCE && groundDistance < 150) {
    if (currentMillis - lastStairBeep > 500) {
      tone(buzzerAlert, 800, 300);
      lastStairBeep = currentMillis;
    }
  }
}

void processWater(unsigned long currentMillis) {
  if (waterLevel < WATER_WET_THRESHOLD) {
    if (currentMillis - lastWaterBeep > 1000) {
      tone(buzzerAlert, 1200, 100);
      delay(150);
      tone(buzzerAlert, 1200, 100);
      lastWaterBeep = currentMillis;
    }
  }
}

// ============================
// SIMPLE CALIBRATION
// ============================
void simpleCalibration() {
  Serial.println("Calibrating...");
  
  // Get current LDR reading
  lightLevel = analogRead(ldrPin);
  Serial.print("Current LDR: ");
  Serial.println(lightLevel);
  
  // Auto-set threshold based on current light
  if (lightLevel > 900) {
    DARK_THRESHOLD = 800;  // Very dark environment
    Serial.println("Very dark - High threshold");
  } else if (lightLevel > 600) {
    DARK_THRESHOLD = 600;  // Dim environment
    Serial.println("Dim - Medium threshold");
  } else if (lightLevel > 300) {
    DARK_THRESHOLD = 400;  // Normal light
    Serial.println("Normal - Low threshold");
  } else {
    DARK_THRESHOLD = 200;  // Bright environment
    Serial.println("Bright - Very low threshold");
  }
  
  Serial.print("Set DARK_THRESHOLD to: ");
  Serial.println(DARK_THRESHOLD);
  
  // Quick test
  if (lightLevel > DARK_THRESHOLD) {
    Serial.println("LED should be ON (dark)");
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
  } else {
    Serial.println("LED should be OFF (bright)");
  }
}

// ============================
// DISPLAY FUNCTION
// ============================
void showReadings() {
  Serial.println("======================");
  Serial.print("Front: ");
  Serial.print(frontDistance);
  Serial.println("cm");
  
  Serial.print("Ground: ");
  Serial.print(groundDistance);
  Serial.println("cm");
  
  Serial.print("Water: ");
  Serial.print(waterLevel);
  Serial.println(waterLevel < WATER_WET_THRESHOLD ? " WET" : " Dry");
  
  Serial.print("Light: ");
  Serial.print(lightLevel);
  Serial.print(" (Threshold: ");
  Serial.print(DARK_THRESHOLD);
  Serial.print(") - LED: ");
  Serial.println(digitalRead(ledPin) ? "ON" : "OFF");
  Serial.println("======================");
}

// ============================
// MANUAL THRESHOLD ADJUSTMENT
// ============================
/*
Add this to loop() if you want manual control:

if (Serial.available()) {
  char c = Serial.read();
  if (c == '+') DARK_THRESHOLD += 50;
  if (c == '-') DARK_THRESHOLD -= 50;
  Serial.print("New DARK_THRESHOLD: ");
  Serial.println(DARK_THRESHOLD);
}
*/