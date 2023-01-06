#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include <Keypad.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "Cooooooooooooooooooool"
#define WIFI_PASSWORD "hhhhyh33"

#define API_KEY "CVZnha3EeEZqdRPeb1mBW08XiOdmExztb3ofAslm"
#define DATABASE_URL "https://wintercapstonedesign-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns
#define RELAY_PIN 26

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedTime; //현재 시간 받아오는 변수 선언
String user;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {18, 5, 17, 16}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {4, 0, 2};  // GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );


void setup()
{
  Serial.begin(115200);
  delay(200);
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

  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
  char key = keypad.getKey();
  
  if(!timeClient.update()) { //현재 시간 가져오기 위해 필요한 코드
      timeClient.forceUpdate();
  

  formattedTime = timeClient.getFormattedTime(); //현재 시간 가져옴
  Serial.println(formattedTime);

  if (key == '1') {
    Serial.println(key);
    digitalWrite(RELAY_PIN, HIGH); // unlock the door
    if (Firebase.ready()) {
    //user = Firebase.get(fbdo,"/QR_lock");
    Firebase.set(fbdo, "/QR_lock/time",formattedTime); //현재 시간 파이어베이스로 전송
    Firebase.set(fbdo, "/toESP_unlock","true");
    //delay(2000);
    } 
  }
  if (key == '2'){
    Serial.println(key);
    digitalWrite(RELAY_PIN, LOW);  // lock the door
    if (Firebase.ready()){
      Firebase.set(fbdo, "/toESP_unlock","false");
    }
  }
  }
}
