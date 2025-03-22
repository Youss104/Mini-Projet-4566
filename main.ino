#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <DHT.h>

//-----------------------------------
//         CONFIGURATIONS
//-----------------------------------
// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA=21, SCL=22

// Servo Configuration
Servo servoEntry;
Servo servoExit;
const int servoEntryPin = 13;
const int servoExitPin = 15;
const int minPulseWidth = 500;
const int maxPulseWidth = 1500;

const int IR_ENTREE = 25;
const int IR_SORTIE = 26;
const int detectionThreshold = 5; // Distance en cm

// IR Sensors Configuration
const int irPins[6] = {34, 35, 36, 39, 27, 18};

// DHT11 Configuration
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//-----------------------------------
//    PROTOTYPES DE FONCTIONS
//-----------------------------------
void showLCDMessage(String message, int delayTime = 0);
void handleEntry();
void handleExit();
bool isCarPresent(int trigPin, int echoPin);
void emergencyOpen();

//-----------------------------------
//        VARIABLES GLOBALES
//-----------------------------------
bool slots[6] = {false};
unsigned long lastLCDUpdate = 0;
unsigned long lastTempCheck = 0;
float temperature = 0.0;

void setup() {
  Serial.begin(115200);
  
  // LCD Initialization
  lcd.init();
  lcd.backlight();
  showLCDMessage("Parking Intelligent", 2000);

  // Servo Initialization
  servoEntry.attach(servoEntryPin, minPulseWidth, maxPulseWidth);
  servoExit.attach(servoExitPin, minPulseWidth, maxPulseWidth);
  servoEntry.write(90);
  servoExit.write(90);

  // IR Sensors Setup
  pinMode(IR_ENTREE, INPUT);
  pinMode(IR_SORTIE, INPUT);

  // IR Sensors Setup
  for(int i=0; i<6; i++) {
    pinMode(irPins[i], INPUT);
  }

  // DHT11 Initialization
  dht.begin();
}

void loop() {
  handleEntry();
  handleExit();
  updateParkingSlots();
  checkTemperature();
  updateLCD();
  delay(50);
}

//-----------------------------------
//  DETECTION IR ENTRÉE/SORTIE
//-----------------------------------
bool isCarAtEntry() { return digitalRead(IR_ENTREE) == LOW; }
bool isCarAtExit() { return digitalRead(IR_SORTIE) == LOW; }

//-----------------------------------
//     FONCTIONS SERVO
//-----------------------------------
void openBarrier(Servo &servo) {
  servo.write(90);
  delay(3000);
  servo.write(0);
}

//-----------------------------------
//     FONCTIONS IR
//-----------------------------------
bool isSlotOccupied(int irPin) {
  return digitalRead(irPin) == LOW;
}

void updateParkingSlots() {
  for(int i=0; i<6; i++) {
    slots[i] = isSlotOccupied(irPins[i]);
  }
}

int countFreeSlots() {
  int count = 0;
  for(bool slot : slots) if(!slot) count++;
  return count;
}

//-----------------------------------
//     FONCTIONS DHT11
//-----------------------------------
void checkTemperature() {
  if(millis() - lastTempCheck > 2000) {
    temperature = dht.readTemperature();
    
    if(isnan(temperature)) {
      Serial.println("Erreur DHT11!");
      return;
    }

    if(temperature > 40.0) {
      emergencyOpen();
    }
    
    lastTempCheck = millis();
  }
}

void emergencyOpen() {
  openBarrier(servoEntry);
  openBarrier(servoExit);
  showLCDMessage("ALERTE INCENDIE!", 1000);
}

//-----------------------------------
//     FONCTIONS LCD
//-----------------------------------
void showLCDMessage(String message, int delayTime) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  if(delayTime > 0) delay(delayTime);
}

void updateLCD() {
  if(millis() - lastLCDUpdate > 1000) {
    String line1 = "Libres:" + String(countFreeSlots());
    line1 += " Temp:" + String(temperature, 1) + "C";
    
    String line2 = "Places:";
    for(int i=0; i<6; i++) {
      if(!slots[i]) line2 += String(i+1) + " ";
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));
    
    lastLCDUpdate = millis();
  }
}

//-----------------------------------
//    GESTION ENTREE/SORTIE
//-----------------------------------
void handleEntry() {
  if(isCarAtEntry() && countFreeSlots() > 0) {
    openBarrier(servoEntry);
    Serial.println("Entree autorisee");
  }
}

void handleExit() {
  if(isCarAtExit()) {
    openBarrier(servoExit);
    Serial.println("Sortie effectuee");
  }
}
