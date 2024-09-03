/*
  Repository library
  https://github.com/dancol90/ESP8266Ping
  https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
  https://github.com/bblanchon/ArduinoJson
*/

#include <ESP8266Ping.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Variable dan value untuk ssid dan password WiFi
const char* ssid = "";
const char* password = "";

// Define variable ip dan url www.reqres.in untuk check ping
// IPAddress ip (192, 168, 110, 1); 
const char* serverName = "www.reqres.in";

// Membuat instance WiFiClientSecure
WiFiClientSecure client;

// Variabel untuk menyimpan waktu terakhir request
unsigned long previousMillis = 0;
// Interval waktu untuk jeda antara request dalam milidetik
const long interval = 10000;

void setup() {
  Serial.begin(115200);

  connection();

  // Set waktu untuk validasi sertifikat SSL
  client.setInsecure(); // Menggunakan ini untuk melewati verifikasi sertifikat SSL (jika tidak diperlukan sertifikat yang valid)
}

void loop() {
  // Serial.println(Ping.averageTime()); // averageTime Ping

  // Mendapatkan waktu saat ini
  unsigned long currentMillis = millis();

  // Mengecek apakah interval waktu telah berlalu
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Memperbarui waktu terakhir request

    getHttp();
  }
}

// Method for Connection WiFi
void connection() {
  WiFi.begin(ssid, password);

  // Checking connection WiFi
  Serial.println("Connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi Connected!");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());

  bool ret = Ping.ping(serverName);

  if (ret) {
    Serial.println("Ping success\n");
  }
}

// Method GET HTTP DATA
const char* apiDetailUser = "https://reqres.in/api/users/1";
void getHttp() {
  // Pastikan telah terhubung ke Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, apiDetailUser);
    
    int httpCode = http.GET(); // Kirimkan GET request

    // Jika request berhasil (kode HTTP 200)
    if (httpCode > 0) {
        Serial.printf("HTTP GET... code: %d\n", httpCode);
        // Ambil respon dari server
        if (httpCode == HTTP_CODE_OK) {
          String response = http.getString();
          // Serial.print("Response: ");
          // Serial.println(response); // Menampilkan respon API

          /*
            GAMBARAN BENTUK RESPONSE SUCCESS
            {
              "data":{"id":1,"email":"george.bluth@reqres.in","first_name":"George","last_name":"Bluth","avatar":"https://reqres.in/img/faces/1-image.jpg"},
              "support":{"url":"https://reqres.in/#support-heading","text":"To keep ReqRes free, contributions towards server costs are appreciated!"}
            }
          */

          JsonDocument doc;
          // convert string to object
          deserializeJson(doc, response);

          // extract the data
          JsonObject object = doc.as<JsonObject>();

          String data = object["data"];
          String support = object["support"];
          String email = object["data"]["email"];
          
          Serial.println(data);
          Serial.println(support);
          Serial.println(email);
        }
      } else {
        // Jika request gagal
        Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

    http.end(); // Akhiri koneksi HTTP
  } else {
    Serial.println("Wi-Fi not connected");
  }
}
