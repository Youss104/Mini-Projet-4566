#define BLYNK_TEMPLATE_ID "TMPL2_CS6H2Gv"
#define BLYNK_TEMPLATE_NAME "Smart Parking System"
#define BLYNK_AUTH_TOKEN "E1fbU-_XMXK705xXVS58LufiSPyPTd9U"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define DHTPIN 14       // Pin du capteur DHT11
#define DHTTYPE DHT11   // Type du capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Informations WiFi
char ssid[] = "ouffa";   // Remplace avec ton réseau WiFi
char pass[] = "Youss104"; // Remplace avec ton mot de passe WiFi

// Fonction pour envoyer les données du capteur vers Blynk
void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Erreur de lecture du capteur DHT11 !");
    return;
  }

  Serial.print("Humidité : "); Serial.print(humidity); Serial.print("%\t");
  Serial.print("Température : "); Serial.print(temperature); Serial.println("°C");

  // Envoi des valeurs à Blynk
  Blynk.virtualWrite(V0, temperature); // Température sur V0
  Blynk.virtualWrite(V8, humidity);    // Humidité sur V1
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connexion à Blynk...");
}

void loop() {
  Blynk.run(); // Maintenir la connexion avec Blynk
  sendSensorData();
  delay(2000); // Attendre 2 secondes entre chaque lecture
}
