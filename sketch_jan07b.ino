#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <Keypad.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "Cooooooooooooooooooool"
#define WIFI_PASSWORD "hhhhyh33"

#define API_KEY "CVZnha3EeEZqdRPeb1mBW08XiOdmExztb3ofAslm"
#define DATABASE_URL "https://wintercapstonedesign-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns
#define RELAY_PIN 25
#define LED_PIN 27

String secretCode;  // 비밀번호를 설정(여기선 1234)
int position = 0; 
int wrong = 0;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate; //현재 시간 받아오는 변수 선언
String user;

int count = 1; 

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {21, 19, 18, 5}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {17, 16, 4};  // GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

String otp_bool=""; //앱에서 올바른 otp 값이 입력되었는가 판별
String check_qr="";
String input_password="";
int f = 0;
String enter_num="";


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
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{ 
  if(Firebase.ready()){ //db에서 실제 비밀번호 값 가져오기
    if(Firebase.getString(fbdo,"/real_password")) {
        secretCode = fbdo.stringData();
     }
    if(Firebase.getString(fbdo,"/input_password")){
      input_password = fbdo.stringData();
    }
    if(Firebase.getString(fbdo,"/check_qr")){
      check_qr = fbdo.stringData();
    }
  }

  char key = keypad.getKey();
  if (key == '#'){   //도어락 키패드에 비밀번호 입력이 들어왔을 때
    Serial.print("key : ");
    Serial.println(key);
    while(1){
      key = keypad.getKey();
      if (key){
        f = f + 1;
        enter_num.concat(key);
        Serial.print("key : ");
        Serial.println(key);
        continue;
      }
      if (f == 4){
        f = 0;
        Serial.println(enter_num);
        break;
      }
    }

    if (enter_num == secretCode){
      digitalWrite(RELAY_PIN, HIGH); // unlock the door
      digitalWrite(LED_PIN, HIGH);
      if (Firebase.ready()){
        if(!timeClient.update()) { //현재 시간 가져오기 위해 필요한 코드
         timeClient.forceUpdate();
        }
        String count_str = String(count);
        count = count + 1;
        formattedDate = timeClient.getFormattedDate(); //현재 날짜, 시간 가져옴
        Firebase.set(fbdo, "/lock_log/" + count_str,formattedDate); //현재 시간 파이어베이스로 전송
      }
      delay(2000);
      digitalWrite(RELAY_PIN, LOW); //도어락 잠금 해제
      digitalWrite(LED_PIN, LOW);
    }
    else if(enter_num != "" && enter_num != secretCode){ //비밀번호 5회 이상 틀린 경우 키패드 비활성화
      Serial.println(wrong);
      while(1){
        key = '*';
        Serial.println(key);
        if(Firebase.ready()){
          if(Firebase.getString(fbdo,"/check_otp")) {
            otp_bool = fbdo.stringData();
            if (otp_bool == "true"){
                wrong = 0;
                Firebase.set(fbdo, "/check_otp", "false");
                break;
            } else {
              Serial.println("false otp");
            }
          }
        }
      }
    }
  }//key

   //라즈베리파이에서 qr 인식 판별이 완료된 후 도어락 잠금 해제 요청이 들어온 경우
  if(check_qr == "true") {
    if(!timeClient.update()) { //현재 시간 가져오기 위해 필요한 코드
         timeClient.forceUpdate();
    }
    digitalWrite(RELAY_PIN, HIGH); //도어락 잠금 해제
    digitalWrite(LED_PIN, HIGH);
    String count_str = String(count);
    count = count + 1;
    formattedDate = timeClient.getFormattedDate(); //현재 날짜, 시간 가져옴
    Firebase.set(fbdo, "/lock_log/" + count_str,formattedDate); //현재 시간 파이어베이스로 전송 
    Firebase.set(fbdo, "/check_qr", "false");
    delay(2000);
    digitalWrite(RELAY_PIN, LOW); //도어락 잠금 해제
    digitalWrite(LED_PIN, LOW);
   }  

   if(input_password == secretCode) {
    if(!timeClient.update()) { //현재 시간 가져오기 위해 필요한 코드
         timeClient.forceUpdate();
    }
    digitalWrite(RELAY_PIN, HIGH); //도어락 잠금 해제
    digitalWrite(LED_PIN, HIGH);
    String count_str = String(count);
    count = count + 1;
    formattedDate = timeClient.getFormattedDate(); //현재 날짜, 시간 가져옴
    Firebase.set(fbdo, "/lock_log/" + count_str,formattedDate); //현재 시간 파이어베이스로 전송 
    Firebase.set(fbdo, "/input_password", "0000"); //무한 루프 제한
    delay(2000);
    digitalWrite(RELAY_PIN, LOW); //도어락 잠금 해제
    digitalWrite(LED_PIN, LOW);
   }    
} //loop
