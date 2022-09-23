#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#define WIFI_SSID "MEGACABLE-949E"
#define WIFI_PASSWORD "kNF9QSnu"
#define API_KEY "AIzaSyD_Q2D66p4Myrl9oDAV7vsB6W6XBD-VAik"
#define DATABASE_URL "https://upiiz-freertos-default-rtdb.firebaseio.com"
#define USER_EMAIL "rahernandezm246@gmail.com"
#define USER_PASSWORD "Contraseña123"
#include <HTTPClient.h>
#include <time.h>

const char WiFi_SSID[]="MEGACABLE-949E";
const char WiFi_PASSWORD[]= "kNF9QSnu";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

/*Ultrasonido*/
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
const int trigPin = 14;
const int echoPin = 2;

long duration;
float distanceCm;
float distanceIn;
/*-----------------------*/
String fecha = "0";

/*FireBase*/
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
/*-----------------------*/

void GetDate(void);
void ReadUltrasonico(void);
void FireBase(void);

int control=0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  control = 1;
  Serial.println("");
  Serial.print("Conectando con la siguiente direccion IP: ");
  Serial.println(WiFi.localIP());
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin,LOW);
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  // put your setup code here, to run once:
}

void loop() {
  while(1){
    GetDate();
    ReadUltrasonico();
    FireBase();
    delay(5000);
  }
}

void GetDate(void) {
  struct tm timeinfo;
  //Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char timeYear[5];
  strftime(timeYear,5, "%Y", &timeinfo);
  char timeMonth[15];
  strftime(timeMonth,15, "%B", &timeinfo);
  char timeDay[10];
  strftime(timeDay,10, "%d", &timeinfo);
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  char timeMin[3];
  strftime(timeMin,3, "%M", &timeinfo);
  fecha = String(timeYear) + String(timeMonth) + String(timeDay) + String(timeHour) + String(timeMin);
  Serial.println(fecha);
  }
  
  void ReadUltrasonico(void) { 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  distanceCm = duration * SOUND_SPEED/2;

  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  }

  void FireBase(void) {
    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();
        FirebaseJson json;
        json.setDoubleDigits(3);
        json.add("value", count);

        Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, "/test/json", &json) ? "ok" : fbdo.errorReason().c_str());
        Serial.printf("Get json... %s\n", Firebase.RTDB.getJSON(&fbdo, "/test/json") ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());

        FirebaseJson jVal;
        Serial.printf("Get json ref... %s\n", Firebase.RTDB.getJSON(&fbdo, "/test/json", &jVal) ? jVal.raw() : fbdo.errorReason().c_str());

        FirebaseJsonArray arr;
        arr.setFloatDigits(2);
        arr.setDoubleDigits(4);
        arr.add(fecha, distanceCm);

        Serial.printf("Set array... %s\n", Firebase.RTDB.setArray(&fbdo, "/test/array", &arr) ? "ok" : fbdo.errorReason().c_str());
        Serial.printf("Get array... %s\n", Firebase.RTDB.getArray(&fbdo, "/test/array") ? fbdo.to<FirebaseJsonArray>().raw() : fbdo.errorReason().c_str());
        Serial.printf("Push json... %s\n", Firebase.RTDB.pushJSON(&fbdo, "/test/push", &json) ? "ok" : fbdo.errorReason().c_str());

        json.set("value", count + 0.29745);
        Serial.printf("Update json... %s\n\n", Firebase.RTDB.updateNode(&fbdo, "/test/push/" + fbdo.pushName(), &json) ? "ok" : fbdo.errorReason().c_str());
        count++;
    }
  }
