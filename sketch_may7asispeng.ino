#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- KONFIGURASI PIN ---
#define DHTPIN 4
#define DHTTYPE DHT22
const int pinLDR_Matahari = 34; 
const int pinLDR_Growlight = 35;
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