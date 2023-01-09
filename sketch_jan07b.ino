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

char* secretCode = "1234";  // 비밀번호를 설정(여기선 1234)
int position = 0; 
int wrong = 0;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate; //현재 시간 받아오는 변수 선언
String dayStamp;
String timeStamp;
String user;

int count = 1; 

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {18, 5, 17, 16}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {4, 0, 2};  // GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

String otp_bool = "";


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
  Serial.println(key);
  Serial.print("secret");
  Serial.println(secretCode[position]);
  Serial.println("wrong");
  if(!timeClient.update()) { //현재 시간 가져오기 위해 필요한 코드
      timeClient.forceUpdate();
  
  if (key == secretCode[position]) {
    Serial.println(key);
    digitalWrite(RELAY_PIN, HIGH); // unlock the door
    position++;
    wrong = 0;
    /*
    if (Firebase.ready()) {
    //user = Firebase.get(fbdo,"/QR_lock");
    String count_str = String(count);
    count = count + 1;
    Firebase.set(fbdo, "/QR_lock/time" + count_str,timeStamp); //현재 시간 파이어베이스로 전송
    Firebase.set(fbdo, "/QR_lock/date" + count_str,dayStamp);//현재 날짜 파이어베이스로 전송
    Firebase.set(fbdo, "/toESP_unlock","true");
    //delay(2000);
    } */
  }
  if (key != secretCode[position]){
    Serial.println(key);
    Serial.println(secretCode[position]);
    position = 0; // 비밀번호를 맞았을 경우를 0으로 만듬
    wrong++; // 비밀번호 오류 값을 늘려준다
    Serial.println(key);
    Serial.println("wrong!!");
    digitalWrite(RELAY_PIN, LOW);  // lock the door
    if (Firebase.ready()){
      Firebase.set(fbdo, "/toESP_unlock","false");
    }
  }
  if(position == 4){
    if (Firebase.ready()) {
    //user = Firebase.get(fbdo,"/QR_lock");
    String count_str = String(count);
    count = count + 1;
    formattedDate = timeClient.getFormattedDate(); //현재 날짜, 시간 가져옴
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    Firebase.set(fbdo, "/QR_lock/time" + count_str,timeStamp); //현재 시간 파이어베이스로 전송
    Firebase.set(fbdo, "/QR_lock/date" + count_str,dayStamp);//현재 날짜 파이어베이스로 전송
    Firebase.set(fbdo, "/toESP_unlock","true");
    //delay(2000);
    } 
  }
  if(wrong == 5){
    while(1){
      key = '*';
      Serial.println(key);
      if(Firebase.ready()){
        otp_bool = Firebase.get(fbdo,"/input_otp_unlock");
        if (otp_bool == "true"){
          wrong = 0;
          break;
        }
      }
    }
  }
  }
}
