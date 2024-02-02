#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 21
#define SCL_PIN 22

#define DHTPIN 4
#define DHTTYPE    DHT11

const char* ssid        = "iPhone de Diégo";
const char* password    = "1234567812";
const char* serverName  = "http://172.20.10.7:3002/weather/";
const int potPin        = 34;
int potValue            = 0;
JsonDocument docx;
HTTPClient http;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(1000);

  if(!display.begin(SSD1306_BLACK, 0x3C)) {
    Serial.println(F("Erreur lors de l'initialisation de l'écran OLED"));
    for(;;);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("Connecté au WiFi");
  display.clearDisplay();
  Serial.println(WiFi.localIP());
}

String getCityFromPotValue(int value) {
  int segmentSize = 4096 / 7; // Pour six villes
  if (value < segmentSize) return "Paris";
  else if (value < segmentSize * 2) return "London";
  else if (value < segmentSize * 3) return "Lisbon";
  else if (value < segmentSize * 4) return "Rome";
  else if (value < segmentSize * 5) return "Moscow";
  else if (value < segmentSize * 6) return "MaPosition";
  else return "New_York";
}

void loop() {
    String contentType = "application/json";

    if (WiFi.status() == WL_CONNECTED) {
    potValue = analogRead(potPin); // Lire la valeur du potentiomètre
    String city = getCityFromPotValue(potValue); // Obtenir le nom de la ville
    http.begin(String(serverName) + city);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {

      String response = http.getString();
      deserializeJson(docx ,response);
 // Traitement de la réponse
    } else {  
      Serial.print("Erreur lors de la requête: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }

  sensors_event_t event;
    dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    Serial.print(F("Temperature de la salle: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }

  if (event.temperature != NULL) {

    DynamicJsonDocument doc(2048);
    doc["temperature"] = event.temperature;

    String json;
    serializeJson(doc, json);
    http.begin("http://172.20.10.7:3002/geolocate");
    http.addHeader("Content-Type", contentType);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    //Serial.print(json);
    http.POST(json);
    //Serial.print(http.getString());
    http.end();
  }

  String cityName = docx[String("name")];
  String countryName = docx[String("country")];
  String placeTemperature = docx[String("temp_c")];
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);

  Serial.print(F("City choosen: "));
  Serial.println(cityName);

  display.print(F("City: "));
  display.println(cityName);
  display.print(F("Country: "));
  display.println(countryName);
  display.print(F("Temperature: "));
  display.println(placeTemperature);
  display.print(F(" C"));

  delay(1000);
  display.display();
  display.clearDisplay();
}
