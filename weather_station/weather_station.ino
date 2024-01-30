#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Votre_SSID";
const char* password = "Votre_Mot_de_Passe";
const char* serverName = "http://adresse_ip_du_serveur:3000/weather/";

const int potPin = 34; // Remplacer par le pin de votre potentiomètre

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("Connecté au WiFi");
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
    int potValue = analogRead(potPin); // Lire la valeur du potentiomètre
    String city = getCityFromPotValue(potValue); // Obtenir le nom de la ville

    HTTPClient http;
    http.begin(String(serverName) + city);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response); // Traitement de la réponse
    } else {
      Serial.print("Erreur lors de la requête: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  delay(60000); // Faire une requête toutes les 60 secondes
}
