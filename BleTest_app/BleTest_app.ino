#include <SoftwareSerial.h>
#define Software_TX 3
#define Software_RX 2
#define sensorP1 A0
#define sensorP2 A1
#define sensorP3 A2     //三个脉搏传感器
#define sensorA1 A3
#define sensorA2 A4
#define sensorA3 A5    //三个气压传感器
#define pump1 4
#define valve1 5
#define pump2 6
#define valve2 7
#define pump3 8
#define valve3 9

char read_dat, jdy_dat;
/*byte lowa = 0;
byte higha = 0;
byte lowb = 0;
byte highb = 0;
byte lowc = 0;
byte highc = 0;*/
int valP1 = 0;
int valP2 = 0;
int valP3 = 0;    //三个脉搏传感器的采样值
int valA1 = 0;
int valA2 = 0;
int valA3 = 0;    //三个气压传感器的采样值
float V_pressure1 = 0;
float mmhg_pressure1 = 0;
float V_pressure2 = 0;
float mmhg_pressure2 = 0;
float V_pressure3 = 0;
float mmhg_pressure3 = 0;    //三个气压传感器的采样值换算成电压和mmHg
char airMode;     //加压模式
bool state = 0;
bool sendData = 0;
SoftwareSerial BLE_JDY_16(Software_RX, Software_TX);//

/*byte int2bytelow (int x, unsigned char channel){
  byte lowx = ((byte)x) & 0x3f;
  switch (channel) {
    case 'a': lowx = lowx + 0; break;
    case 'b': lowx = lowx + 64; break;
    case 'c': lowx = lowx + 128; break;
  }
  return lowx;
}

byte int2bytehigh (int x, unsigned char channel){
  byte highx = (byte)(x >> 6);
  switch (channel) {
    case 'a': highx = highx + 0; break;
    case 'b': highx = highx + 16; break;
    case 'c': highx = highx + 32; break;
  }
  return highx;
}*/

void setup()
 {
     Serial.begin(9600);
     BLE_JDY_16.begin(9600);
     Serial.println("Ble Test");

     pinMode(sensorP1, INPUT);pinMode(sensorP2, INPUT);pinMode(sensorP3, INPUT);
     pinMode(sensorA1, INPUT);pinMode(sensorA2, INPUT);pinMode(sensorA3, INPUT);
     for (int i = 4; i <= 9; i ++){
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
     }
 }

void jiaya(int pos, float mmhg_pressure, float pessureValue){
  if (pos == 1) {digitalWrite(valve1, HIGH);}
  else if (pos == 2) {digitalWrite(valve2, HIGH);}
  else if (pos == 3) {digitalWrite(valve3, HIGH);}
  if (mmhg_pressure > pessureValue){
     if (pos == 1) {digitalWrite(pump1, LOW);}
     else if (pos == 2) {digitalWrite(pump2, LOW);}
     else if (pos == 3) {digitalWrite(pump3, LOW);}
  }
}

void jiayabaochi(int pos, float mmhg_pressure, float pessureValue){
  if (pos == 1) {digitalWrite(valve1, HIGH);}
  else if (pos == 2) {digitalWrite(valve2, HIGH);}
  else if (pos == 3) {digitalWrite(valve3, HIGH);}
  if (mmhg_pressure > pessureValue){
     if (pos == 1) {digitalWrite(pump1, LOW);}
     else if (pos == 2) {digitalWrite(pump2, LOW);}
     else if (pos == 3) {digitalWrite(pump3, LOW);}
  }  else if(mmhg_pressure <= 0.8 * pessureValue){
     if (pos == 1) {digitalWrite(pump1, HIGH);}
     else if (pos == 2) {digitalWrite(pump2, HIGH);}
     else if (pos == 3) {digitalWrite(pump3, HIGH);}
  }
}

void loop()
 {     
     unsigned long t = micros();
     while (true) {
        valA1 = analogRead(sensorA1);
        valA2 = analogRead(sensorA2);
        valA3 = analogRead(sensorA3);

        //气压值的计算
        V_pressure1 = (float)valA1 * 5.0 / 1023.0;
        mmhg_pressure1 = 106.57 * V_pressure1 - 19.72;
        V_pressure2 = (float)valA2 * 5.0 / 1023.0;
        mmhg_pressure2 = 106.57 * V_pressure2 - 19.72;
        V_pressure3 = (float)valA3 * 5.0 / 1023.0;
        mmhg_pressure3 = 106.57 * V_pressure3 - 19.72;
      
        if (BLE_JDY_16.available()) {
          jdy_dat = (char)BLE_JDY_16.read();
          //delay(2);
          Serial.write(jdy_dat);
          if ((jdy_dat >=  'a' && jdy_dat <=  'z') || jdy_dat ==  '9'){
             airMode = jdy_dat;
             digitalWrite(pump1, HIGH); digitalWrite(pump2, HIGH); digitalWrite(pump3, HIGH);
          }
       }       

       Serial.print(mmhg_pressure1);Serial.print("  ");Serial.print(mmhg_pressure2);Serial.print("  ");
       Serial.println(mmhg_pressure3);

       switch (airMode){
         case 'a': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'b': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'c': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'd': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'e': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'f': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'g': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'h': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'i': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 50.0); break;
         case 'j': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'k': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'l': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'm': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'n': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'o': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'p': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'q': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 'r': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 100.0); break;
         case 's': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 't': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 'u': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 50.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 'v': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 'w': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 'x': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 100.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 'y': jiaya(1, mmhg_pressure1, 50.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case 'z': jiaya(1, mmhg_pressure1, 100.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 150.0); break;
         case '9': jiaya(1, mmhg_pressure1, 150.0); jiaya(2, mmhg_pressure2, 150.0); jiaya(3, mmhg_pressure3, 150.0); break;
       }

       // 泄压
        if (jdy_dat == '8'){digitalWrite(valve1, LOW); digitalWrite(valve2, LOW); digitalWrite(valve3, LOW);}

        // 通过手机app上的接收和暂停按钮，确定是否发送蓝牙数据
        if (jdy_dat == '1'){sendData = 1;}
        else if(jdy_dat == '0'){sendData = 0;}

        if (sendData){
          unsigned long t1 = micros();
          if (t1 - t >= 25000) {
            state = ! state;
            t = t1;
            if (state) {BLE_JDY_16.write(105);BLE_JDY_16.write(155);BLE_JDY_16.write(205);}
            else {BLE_JDY_16.write(125);BLE_JDY_16.write(175);BLE_JDY_16.write(225);}
          }
        }
        
     }
 }

 
