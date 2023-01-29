// Nhập mã token của project
#define BLYNK_TEMPLATE_ID "TMPLUbt56hbv"
#define BLYNK_DEVICE_NAME "IoT"
#define BLYNK_AUTH_TOKEN "kpVcMcZno3KVJWgdkElc2fyNd0u__7Jk"

#define Motor 13
#define Yellow_LED 12
#define Red_LED 27
#define Green_LED 14
#define bt1 18  // Start
#define bt2 19 // Stop
// Cài đặt wifi
char ssid[] = "wifichua";
char pass[] = "uacv29319";
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
//#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Preferences.h>



Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer; // Khai báo timer
int i =0;
sensors_event_t a, g, temp; // Khai báo biến cho MPU6051
double Ax_T = 20, Ay_T = 20 , Az_T =20,T = 45; // Khai báo biến giao động ngưỡng và khai báo biến nhiệt độ ngưỡng
boolean Button = HIGH,wififlag; //Khai báo biến giao động ngưỡng, biến nút bấm và biến Flag Wifi
boolean bt1State = HIGH;
boolean bt2State = HIGH;
void sendSensor(){ // Chương trình gửi giữ liệu lên Blynk
  Blynk.virtualWrite(V4,temp.temperature);
  Blynk.virtualWrite(V5,a.acceleration.x);
  Blynk.virtualWrite(V6,a.acceleration.y);
  Blynk.virtualWrite(V7,a.acceleration.z);
  Blynk.virtualWrite(V8,g.gyro.x);
  Blynk.virtualWrite(V9,g.gyro.y);
  Blynk.virtualWrite(V10,g.gyro.z);
}
void checkButton(){
  if(digitalRead(bt1)==LOW){
    if(bt1State==HIGH){
      Button= LOW;
      delay(200);
      bt1State=LOW;
    }
  }else{
    bt1State=HIGH;
  }
  if(digitalRead(bt2)==LOW){
    if(bt2State==HIGH){
      Button= HIGH;
      delay(200);
      bt2State=LOW;
    }
  }else{
    bt2State=HIGH;
  }
}
void checkBlynkStatus() { // Chương trình kiểm tra kết nối internet

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wififlag = 1;
  }
  if (isconnected == true) {
    wififlag = 0;

  }
}

void setup() {
  pinMode(bt1,INPUT_PULLUP);
  pinMode(bt2,INPUT_PULLUP);
  pinMode (Motor, OUTPUT);
  pinMode (Yellow_LED, OUTPUT);
  pinMode (Red_LED,OUTPUT);
  pinMode (Green_LED, OUTPUT);
  digitalWrite(Motor,1);
  digitalWrite(Red_LED,1);
  digitalWrite(Green_LED,1);
  digitalWrite(Yellow_LED,0);
  Serial.begin(115200);
  // while (!Serial);
  Serial.println("MPU6050 OLED demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU-6050 sensor");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(0.5);
  display.setTextColor(WHITE);
  display.setRotation(0);
   
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // Kiểm tra kết nối internet mỗi 2 giây
  timer.setInterval(500L, sendSensor); // Gửi dữ liệu lên server mỗi 0.5 giây
  Blynk.config(auth);
  delay(1000);
}

void loop() {
  
  // Đọc cảm biến MPU6051
  mpu.getEvent(&a, &g, &temp);
  i = i +1;
  if (i >20){  
  display.clearDisplay();
  display.setCursor(0, 0);
  //Hiển thị acceleration m/s^2
  display.print("A= ");
  display.print(a.acceleration.x, 1);
  display.print(", ");
  display.print(a.acceleration.y, 1);
  display.print(", ");
  display.print(a.acceleration.z, 1);
  display.println("");

 
  //Hiển thị  Gyroscope - rps
  display.print("G= ");
  display.print(g.gyro.x, 1);
  display.print(", ");
  display.print(g.gyro.y, 1);
  display.print(", ");
  display.print(g.gyro.z, 1);
  display.print("");
  display.println("");
  //Hiển thị nhiệt độ
  display.print("temp :");
  display.print(temp.temperature);
  display.println("deg C")  ;
  // Hiển thị trạng thái Wifi
  display.print("Status:");
  if (wififlag ==1){  
  display.print("Offline");
  }
  else{
    display.print("Online");
  }
  
  display.display();  
  i=0;
  }

  Blynk.run();
  timer.run();
  // Cảnh báo khi các thông số vượt quá ngưỡng
  if(a.acceleration.x > Ax_T||a.acceleration.y >Ay_T|| a.acceleration.z >Az_T || temp.temperature > T)
      {
      digitalWrite (Red_LED,LOW);       
      }
      else digitalWrite (Red_LED,HIGH); 
  checkButton(); // kiểm tra nút nhấn
   digitalWrite(Motor,Button);
  Blynk.virtualWrite(V0, Button);  	 
  // Hiển thị trạng thái của động cơ 
  digitalWrite(Green_LED, Button);
  digitalWrite(Yellow_LED, !Button);
}
// Chương trình ngắt của nút bấm trên Blynk
BLYNK_WRITE(V0)
{
  Button = param.asInt();
 digitalWrite(Motor,Button);
}
// Đọc nhiệt độ Ngưỡng từ Blynk
BLYNK_WRITE(V12)
{
  T = param.asDouble();
}
// Đọc giao động ngưỡng từ blynk
BLYNK_WRITE(V11)
{
  Ax_T = param.asDouble();
}
BLYNK_WRITE(V1)
{
  Ay_T = param.asDouble();
}
BLYNK_WRITE(V2)
{
  Az_T = param.asDouble();
}