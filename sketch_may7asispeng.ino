#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- KONFIGURASI PIN ---
#define DHTPIN 4
#define DHTTYPE DHT22
const int pinLDR_Matahari = 34; 
const int pinLDR_#include <WiFi.h>
#include <FirebaseESP32.h> 
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// ═══════════════════════════════════
// KONFIGURASI PIN & FIREBASE
// ═══════════════════════════════════
#define WIFI_SSID "nabillanatha"
#define WIFI_PASSWORD "1sampai10"
#define DATABASE_URL "https://hidroponik-caisim-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "RzMiD8xNWUEZm8Tvrjdzfm6Oku7Fweqlur6QUA0s"

#define DHTPIN 15         
#define DHTTYPE DHT22
#define WATER_L1 32       
#define WATER_L2 33       
#define LDR_MATAHARI 34   
#define LDR_GROW 35       

#define RELAY_FAN 18        // IN1
#define RELAY_POMPA 19      // IN2
#define RELAY_GROWLIGHT 23  // IN3

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define SUHU_THRESHOLD 27.5
#define LDR_THRESHOLD 3000   

#define SD_SCK  14
#define SD_MISO 27        
#define SD_MOSI 13
#define SD_CS   4         

// ═══════════════════════════════════
// OBJEK & VARIABEL
// ═══════════════════════════════════
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
SPIClass hspi(HSPI);

float suhu = 0, kelembaban = 0;
int ldrMat = 0, ldrGrow = 0, w1 = 0, w2 = 0;
unsigned long prevBaca = 0;
const unsigned long INTERVAL = 5000;

// ═══════════════════════════════════
// FUNGSI LOGIKA (SEPERTI TEMAN KAMU)
// ═══════════════════════════════════

void bacaSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && t > 0) suhu = t;
  if (!isnan(h) && h > 0) kelembaban = h;
  
  ldrMat = analogRead(LDR_MATAHARI);
  ldrGrow = analogRead(LDR_GROW);
  w1 = digitalRead(WATER_L1);
  w2 = digitalRead(WATER_L2);
}

void logikaOtomatis() {
  // 1. Kipas (Fan)
  if (suhu > SUHU_THRESHOLD) digitalWrite(RELAY_FAN, RELAY_ON);
  else digitalWrite(RELAY_FAN, RELAY_OFF);

  // 2. Growlight
  if (ldrMat > LDR_THRESHOLD) digitalWrite(RELAY_GROWLIGHT, RELAY_ON);
  else digitalWrite(RELAY_GROWLIGHT, RELAY_OFF);

  // 3. Pompa (Lantai 2)
  if (w2 == LOW) digitalWrite(RELAY_POMPA, RELAY_ON);
  else digitalWrite(RELAY_POMPA, RELAY_OFF);
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembaban);
  lcd.setCursor(0,1); lcd.printf("LDR:%d G:%d", ldrMat, ldrGrow);
  lcd.setCursor(0,2); lcd.printf("W1:%s W2:%s", w1==LOW?"KOSONG":"AMAN", w2==LOW?"ISI..":"OK");
  lcd.setCursor(0,3); lcd.printf("F:%s P:%s L:%s", 
    digitalRead(RELAY_FAN)==RELAY_ON?"ON":"OFF",
    digitalRead(RELAY_POMPA)==RELAY_ON?"ON":"OFF",
    digitalRead(RELAY_GROWLIGHT)==RELAY_ON?"ON":"OFF");
}

void kirimFirebase() {
  if (Firebase.ready()) {
    FirebaseJson json;
    json.set("suhu", suhu); 
    json.set("kelembaban", kelembaban);
    json.set("ldr_matahari", ldrMat);
    json.set("ldr_grow", ldrGrow);
    json.set("water_lvl1", w1);
    json.set("water_lvl2", w2);
    Firebase.setJSON(fbdo, "/sensor", json);
  }
}

// ═══════════════════════════════════
// SETUP & LOOP
// ═══════════════════════════════════

void setup() {
  Serial.begin(115200);

  // Samakan urutan setup relay dengan teman (Off dulu baru pinMode)
  digitalWrite(RELAY_FAN, RELAY_OFF);
  digitalWrite(RELAY_POMPA, RELAY_OFF);
  digitalWrite(RELAY_GROWLIGHT, RELAY_OFF);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_POMPA, OUTPUT);
  pinMode(RELAY_GROWLIGHT, OUTPUT);
  pinMode(WATER_L1, INPUT);
  pinMode(WATER_L2, INPUT);

  dht.begin();
  lcd.begin();
  lcd.backlight();
  lcd.print("Warm-up System...");

  hspi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  SD.begin(SD_CS, hspi);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 15) { delay(500); attempt++; }

  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  delay(2000); // Warm-up sensor
  lcd.clear();
}

void loop() {
  unsigned long now = millis();
  if (now - prevBaca >= INTERVAL) {
    prevBaca = now;
    
    bacaSensor();      // 1. Ambil data
    logikaOtomatis();  // 2. Eksekusi relay
    kirimFirebase();   // 3. Update awan
    updateLCD();       // 4. Update layar
    
    // Simpan ke SD Card
    File dataFile = SD.open("/datalog.txt", FILE_APPEND);
    if (dataFile) {
      dataFile.printf("%.1f,%.1f,%d,%d\n", suhu, kelembaban, ldrMat, w2);
      dataFile.close();
    }
  }
}Growlight = 35;
const int pinWater_Lt1 = 32;   
const int pinWater_Lt2 = 33;   
const int relayFan = 18;
const int relayPompa = 19;
const int relayGrowlight = 23;

// --- PARAMETER AMBANG BATAS ---
const int ambangGelapMatahari = 2500; 
const int ambangAir = 150; // DITURUNKAN ke 150 karena pembacaanmu sangat rendah

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(115200);
  
  lcd.begin();
  lcd.backlight();

  pinMode(relayFan, OUTPUT);
  pinMode(relayPompa, OUTPUT);
  pinMode(relayGrowlight, OUTPUT);

  digitalWrite(relayFan, HIGH);
  digitalWrite(relayPompa, HIGH);
  digitalWrite(relayGrowlight, HIGH);

  dht.begin();
}

void loop() {
  // 1. Baca Sensor
  float suhu = dht.readTemperature();
  int valMatahari = analogRead(pinLDR_Matahari);
  int valGrowlight = analogRead(pinLDR_Growlight);
  int valAirLt1 = analogRead(pinWater_Lt1); 
  int valAirLt2 = analogRead(pinWater_Lt2);

  // 2. Logika Pompa
  // Jika Lt2 < 150 (Kering) DAN Lt1 > 150 (Ada Air)
  if (valAirLt2 < ambangAir && valAirLt1 > ambangAir) {
    digitalWrite(relayPompa, LOW); 
  } else {
    digitalWrite(relayPompa, HIGH); 
  }

  // 3. Serial Monitor (TETAP MENAMPILKAN LDR)
  Serial.println("--- CEK SENSOR ---");
  Serial.print("Mthri: "); Serial.print(valMatahari);
  Serial.print(" | Grow: "); Serial.println(valGrowlight);
  Serial.print("W_Lt1: "); Serial.print(valAirLt1);
  Serial.print(" | W_Lt2: "); Serial.println(valAirLt2);
  Serial.println("------------------");

  // 4. LCD
  lcd.setCursor(0,0);
  lcd.print("T:"); lcd.print(suhu,1); lcd.print(" L1:"); lcd.print(valAirLt1);
  lcd.setCursor(0,1);
  lcd.print("Sun:"); lcd.print(valMatahari); lcd.print(" L2:"); lcd.print(valAirLt2);
  lcd.setCursor(0,2);
  lcd.print("W1:"); lcd.print(valAirLt1 > ambangAir ? "ADA" : "HBS");
  lcd.print(" W2:"); lcd.print(valAirLt2 > ambangAir ? "ADA" : "HBS");
  
  delay(2000); 
}
