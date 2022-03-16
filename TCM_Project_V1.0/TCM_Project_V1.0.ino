//定时器库
#include <TimerOne.h>
bool isPrintPulse=false;

//三个脉搏压力传感器

#define sensorPulse1 A0
#define sensorPulse2 A1
#define sensorPulse3 A2
int valPulse1 = 0;int valPulse2 = 0;int valPulse3 = 0; //脉搏值

//三个气压传感器
const int numReadings = 5;
//1.用于Air1做10个点取平均
int readingsAir1[numReadings];      // 存储数组
int readIndexAir1 = 0;              // 当前读到的位置（数组下标）
int totalAir1 = 0;                  // 和
int averageAir1 = 0;                // 平均值
//2.用于Air2做10个点取平均
int readingsAir2[numReadings];      // 存储数组
int readIndexAir2 = 0;              // 当前读到的位置（数组下标）
int totalAir2 = 0;                  // 和
int averageAir2 = 0;                // 平均值
//3.用于Air3做10个点取平均
int readingsAir3[numReadings];      // 存储数组
int readIndexAir3 = 0;              // 当前读到的位置（数组下标）
int totalAir3 = 0;                  // 和
int averageAir3 = 0;                // 平均值

#define sensorAir1 A3
#define sensorAir2 A4
#define sensorAir3 A5  
int valAir1 = 0;int valAir2 = 0;int valAir3 = 0;
float V_pressure1 = 0;float mmhg_pressureP2 = 0;
float V_pressure2 = 0;float mmhg_pressureP3 = 0;
float V_pressure3 = 0;float mmhg_pressureP1 = 0; 

//泵和阀的引脚
#define pump1  5
#define valve1 4
#define pump2  7
#define valve2 6
#define pump3  2
#define valve3 3

//泵和阀压力阈值
int CunFlag = 0,GuanFlag = 0,ChiFlag = 0;
int Fu = 200,Zhong = 300,Chen = 400;
int airThreshold_of_Cun =5000;
int airThreshold_of_Guan=5000;
int airThreshold_of_Chi =5000;

//加压开始和停止标志位
bool isJiaya = false;
bool isXieya = false;

//串口接收命令
String inputString = "";     //存放接收的指令
bool stringComplete = false; //接收完成标志位
int Position;   //确定命令分隔符位置
int flag;       //加压还是泄压标志位

/*=============================================*/
void setup() 
{
    //定时器设置
    Timer1.initialize(5000);  //设置中断速度 5000us=5ms
    Timer1.attachInterrupt(ReadPulseADC);  //关联中断函数

    interrupts(); //开启中断
    
    //软件串口初始化
    Serial.begin(9600); 
    
    //预留200字节存放接收的命令
    inputString.reserve(200); 

    //初始化ADC口
    pinMode(sensorPulse1, INPUT);pinMode(sensorPulse2, INPUT);pinMode(sensorPulse3, INPUT);
    pinMode(sensorAir1, INPUT);pinMode(sensorAir2, INPUT);pinMode(sensorAir3, INPUT);

    //初始化平滑数组
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
        readingsAir1[thisReading] = 0;
        readingsAir2[thisReading] = 0;
        readingsAir3[thisReading] = 0;
    }
}

//回调函数；读取三个脉搏压力传感器的值，每隔5ms读取一次并串口发送
void ReadPulseADC()
{   
    
    
    valPulse1 = analogRead(sensorPulse1);
    valPulse2 = analogRead(sensorPulse2);
    valPulse3 = analogRead(sensorPulse3);

//    isPrintPulse = true; 
}

void ReadAirADC(){
    //1.读取Air1的10个值取平均
    // 总和扣除上次的值:
    totalAir1 = totalAir1 - readingsAir1[readIndexAir1];
    // 从传感器读新值:
    readingsAir1[readIndexAir1] = analogRead(sensorAir1);
    // 将新值加到总和:
    totalAir1 = totalAir1 + readingsAir1[readIndexAir1];
    // 下表加一:
    readIndexAir1 = readIndexAir1 + 1;
    // 如果在数组的最后
    if (readIndexAir1 >= numReadings) {
      // 从头开始:
      readIndexAir1 = 0;
    }
    // 计算平均值:
    averageAir1 = totalAir1 / numReadings;

    //2.读取Air2的10个值取平均
    // 总和扣除上次的值:
    totalAir2 = totalAir2 - readingsAir2[readIndexAir2];
    // 从传感器读新值:
    readingsAir2[readIndexAir2] = analogRead(sensorAir2);
    // 将新值加到总和:
    totalAir2 = totalAir2 + readingsAir2[readIndexAir2];
    // 下表加一:
    readIndexAir2 = readIndexAir2 + 1;
    // 如果在数组的最后
    if (readIndexAir2 >= numReadings) {
      // 从头开始:
      readIndexAir2 = 0;
    }
    // 计算平均值:
    averageAir2 = totalAir2 / numReadings;

    //3.读取Air3的10个值取平均
    // 总和扣除上次的值:
    totalAir3 = totalAir3 - readingsAir3[readIndexAir3];
    // 从传感器读新值:
    readingsAir3[readIndexAir3] = analogRead(sensorAir3);
    // 将新值加到总和:
    totalAir3 = totalAir3 + readingsAir3[readIndexAir3];
    // 下表加一:
    readIndexAir3 = readIndexAir3 + 1;
    // 如果在数组的最后
    if (readIndexAir3 >= numReadings) {
      // 从头开始:
      readIndexAir3 = 0;
    }
    // 计算平均值:
    averageAir3 = totalAir3 / numReadings;

  
  //读取气压传感器的数值，在循环中一直读取
  valAir1 = averageAir1;valAir2 = averageAir2;valAir3 = averageAir3;
  
  //气压值的计算
  V_pressure3 = (float)valAir3 * 5.0 / 1023.0;mmhg_pressureP3 = 106.57 * V_pressure3 - 19.72 + 19.72;
  V_pressure2 = (float)valAir2 * 5.0 / 1023.0;mmhg_pressureP2 = 106.57 * V_pressure2 - 19.72 + 9.30;
  V_pressure1 = (float)valAir1 * 5.0 / 1023.0;mmhg_pressureP1 = 106.57 * V_pressure1 - 19.72 + 14.03;
  
  Serial.print(mmhg_pressureP3); //Cun 寸
  Serial.print(',');
  Serial.print(mmhg_pressureP2); //Guan 关
  Serial.print(',');
  Serial.println(mmhg_pressureP1); //Chi 尺
}

//主循环
void loop()
{
  if(isPrintPulse){
    Serial.print(valPulse1);
    Serial.print(',');
    Serial.print(valPulse2);
    Serial.print(',');
    Serial.println(valPulse3);

    isPrintPulse = false;
  }
  
  ReadAirADC();  //读取气压传感器
  
  serialEvent();  //读取命令
  if (stringComplete) {
    //读取命令，去掉尾部的#
    inputString = inputString.substring(0,inputString.length()-1);
    
    //获取寸的压力阈值；
    Position = inputString.indexOf(";");
     
    CunFlag = inputString.substring(0,Position).toInt();
    switch(CunFlag){
      case 0:airThreshold_of_Cun = 0;break;
      case 1:airThreshold_of_Cun = Fu;break;
      case 2:airThreshold_of_Cun = Zhong;break;
      case 3:airThreshold_of_Cun = Chen;break;
    }
    inputString = inputString.substring(Position+1,inputString.length());

    //获取关的压力阈值；
    Position = inputString.indexOf(";");
    GuanFlag = inputString.substring(0,Position).toInt();
    switch(GuanFlag){
      case 0:airThreshold_of_Guan = 0;break;
      case 1:airThreshold_of_Guan = Fu;break;
      case 2:airThreshold_of_Guan = Zhong;break;
      case 3:airThreshold_of_Guan = Chen;break;
    }
    inputString = inputString.substring(Position+1,inputString.length());

    //获取尺的压力阈值；
    Position = inputString.indexOf(";");
    ChiFlag = inputString.substring(0,Position).toInt();
    switch(ChiFlag){
      case 0:airThreshold_of_Chi = 0;break;
      case 1:airThreshold_of_Chi = Fu;break;
      case 2:airThreshold_of_Chi = Zhong;break;
      case 3:airThreshold_of_Chi = Chen;break;
    }
    inputString = inputString.substring(Position+1,inputString.length());

    //控制加压还是泄压
    Position = inputString.indexOf(";");
    flag = inputString.substring(0,Position).toInt();
    
    Serial.println(airThreshold_of_Cun);
    Serial.println(airThreshold_of_Guan);
    Serial.println(airThreshold_of_Chi);
    Serial.println(flag);
    
    if(flag == 1){
      isJiaya = true;
    }
    if(flag == 0){
      isXieya = true;
    }

    //结束一个循环，回到初始状态
    inputString = "";
    stringComplete = false;
  }
  
  //按下“加压”按钮后
  if(isJiaya){
    digitalWrite(pump1, HIGH);digitalWrite(pump2, HIGH);digitalWrite(pump3, HIGH);
    digitalWrite(valve1,HIGH);digitalWrite(valve2,HIGH);digitalWrite(valve3,HIGH);
    delay(5);
    isJiaya = false;
  }
  
  //按下“泄压”按钮后
  if(isXieya){
    digitalWrite(pump1, LOW);digitalWrite(pump2, LOW);digitalWrite(pump3, LOW);
    digitalWrite(valve1,LOW);digitalWrite(valve2,LOW);digitalWrite(valve3,LOW);
    delay(5);
    isXieya = false;
  }

  //达到各自的阈值之后停止加压
  if(mmhg_pressureP1 > airThreshold_of_Cun){ //假设P1达到阈值需要关闭
    digitalWrite(pump1, LOW);
  }
  if(mmhg_pressureP2 > airThreshold_of_Guan){
    digitalWrite(pump2, LOW);
  }
  if(mmhg_pressureP3 > airThreshold_of_Chi){ 
    digitalWrite(pump3, LOW);
  }
}

//读取命令函数
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '#') {
      stringComplete = true;
    }
  }
}
