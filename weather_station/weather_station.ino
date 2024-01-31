#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 21
#define SCL_PIN 22

const char* ssid        = "jessy";
const char* password    = "hmuj3310";
const char* serverName  = "http://192.168.77.219:3002/weather/";
const int potPin        = 34;
int potValue            = 0;
JsonDocument docx;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
}

String getCityFromPotValue(int value) {
  int segmentSize = 4096 / 6; // Pour six villes
  if (value < segmentSize) return "Paris";
  else if (value < segmentSize * 2) return "London";
  else if (value < segmentSize * 3) return "Lisbon";
  else if (value < segmentSize * 4) return "Rome";
  else if (value < segmentSize * 5) return "Moscow";
  else return "New York";
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
    potValue = analogRead(potPin); // Lire la valeur du potentiomètre
    String city = getCityFromPotValue(potValue); // Obtenir le nom de la ville
    
    HTTPClient http;
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

  String cityName = docx[String("name")];
  String countryName = docx[String("country")];
  String placeTemperature = docx[String("temp_c")];
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);

  display.println("Ville",cityName);
  display.println("Pays",countryName);
  display.println("Temperature",placeTemperature);

  delay(5000);
  display.display();
  display.clearDisplay();
}
