#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "Cooooooooooooooooooool"
#define WIFI_PASSWORD "hhhhyh33"

#define API_KEY "CVZnha3EeEZqdRPeb1mBW08XiOdmExztb3ofAslm"
#define DATABASE_URL "https://wintercapstonedesign-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedTime; //현재 시간 받아오는 변수 선언
String user;

void setup()
{
  Serial.begin(115200);
  delay(2000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  timeClient.begin();
  timeClient.setTimeOffset(32400); 
  
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(DATABASE_URL, API_KEY);
  Firebase.setDoubleDigits(5);
}

void loop()
{
  while(!timeClient.update()) { //현재 시간 가져오기 위해 필요한 코드
      timeClient.forceUpdate();
  }

  formattedTime = timeClient.getFormattedTime(); //현재 시간 가져옴
  Serial.println(formattedTime);
  
  if (Firebase.ready()) 
  {
    //user = Firebase.get(fbdo,"/QR_lock");
    Firebase.set(fbdo, "/QR_lock/time",formattedTime); //현재 시간 파이어베이스로 전송
    delay(2000);
  }
}
