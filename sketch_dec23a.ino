#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"
#include <math.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= DHT22 =================
#define DHTPIN D4          
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ================= GAS =================
#define GAS_ANALOG A0
#define GAS_THRESHOLD 150

#define RL 10.0  
#define R0 10.0   

// ================= BUZZER =================
#define BUZZER_PIN D7

// ================= LED =================
#define LED_PIN D6

// ================= FIREBASE =================
const char* temperatureURL =
"https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/temperature.json";

const char* humidityURL =
"https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/humidity.json";

const char* gasURL =
"https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/gas.json";

const char* gasLimitURL =
"https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/gasLimit.json";

const char* ledCommandURL =
"https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/ledCommand.json";

// ================= GAS PPM FUNCTION =================
float calculatePPM(int analogValue) {
  float voltage = analogValue * (3.3 / 1023.0);
  if (voltage <= 0.01) return 0; 

  float Rs = ((3.3 * RL) / voltage) - RL;
  float ratio = Rs / R0;

  float ppm = pow(10, ((-0.47 * log10(ratio)) + 1.52));
  return ppm;
}

// ================= FIREBASE FUNCTIONS =================
void sendToFirebase(const char* url, float value) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  String json = String(value, 1);
  int code = http.PUT(json);
  Serial.printf("Firebase response: %d\n", code);
  http.end();
}

void sendGasToFirebase(float gasPPM){
  if(WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, gasURL);
  http.addHeader("Content-Type", "application/json");

  String json = String(gasPPM, 0);

  int code = http.PUT(json);
  Serial.printf("Gas PPM response: %d\n", code);
  http.end();
}

int getGasLimitFromFirebase() {
  if (WiFi.status() != WL_CONNECTED) return GAS_THRESHOLD;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, gasLimitURL);
  int httpCode = http.GET();

  int gasLimit = GAS_THRESHOLD;

  if (httpCode == 200) {
    String payload = http.getString();
    gasLimit = payload.toInt();
    Serial.printf("Preuzet gasLimit: %d\n", gasLimit);
  } else {
    Serial.printf("Greška pri preuzimanju gasLimit: %d\n", httpCode);
  }

  http.end();
  return gasLimit;
}

int getLedCommand() {
  if (WiFi.status() != WL_CONNECTED) return 0;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, ledCommandURL);

  int httpCode = http.GET();
  int command = 0;

  if (httpCode == 200) {
    String payload = http.getString();
    command = payload.toInt();
  }

  http.end();
  return command;
}

// ================= RESET LED COMMAND =================
void resetLedCommand() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, ledCommandURL);
  http.addHeader("Content-Type", "application/json");

  http.PUT("0"); 
  http.end();
}


void blinkSOS() {
  for(int i = 0; i < 3; i++){
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }

  delay(400);

  for(int i = 0; i < 3; i++){
    digitalWrite(LED_PIN, HIGH);
    delay(700);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  dht.begin();
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED nije pronadjen");
    while (true);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Pokretanje...");
  display.display();

  Serial.println("WiFi povezivanje...");
  WiFi.begin(WLAN_SSID, WLAN_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi spojen!");
  Serial.println(WiFi.localIP());
}

// ================= LOOP =================
void loop() {
  int gasLimit = getGasLimitFromFirebase();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int gasValue = analogRead(GAS_ANALOG);
  float gasPPM = calculatePPM(gasValue);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Greska DHT22!");
    return;
  }

  Serial.println("---------------");
  Serial.printf("Temp: %.1f C\n", temperature);
  Serial.printf("Vlaznost: %.1f %%\n", humidity);
  Serial.printf("Plin (PPM): %.0f\n", gasPPM);
  Serial.printf("Trenutni gasLimit: %d\n", gasLimit);

  sendToFirebase(temperatureURL, temperature);
  sendToFirebase(humidityURL, humidity);
  sendGasToFirebase(gasPPM);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("    Sobni monitor\n");

  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.print("Vlaznost: ");
  display.print(humidity);
  display.println(" %");

  display.print("Plin: ");
  display.print(gasPPM, 0);
  display.println(" PPM");

  display.display();

  // ================= GAS ALERT =================
  if (gasPPM > gasLimit) {   
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  int ledCommand = getLedCommand();
  if (ledCommand == 1) {
    blinkSOS();       
    resetLedCommand(); 
  }

  delay(150);
}
