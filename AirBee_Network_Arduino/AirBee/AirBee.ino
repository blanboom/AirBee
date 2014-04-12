#include <LeweiTcpClientMod.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define RELAY A3 // 继电器接在 A3 上

#define LW_USERKEY "MyKey"
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

LeweiTcpClientMod *node1;
LeweiTcpClientMod *node2;

SoftwareSerial SoftSerial(A0, A1);  // 软串口

void setup() 
{
  // 设置串口
  Serial.begin(9600);
  SoftSerial.begin(9600);

  // 初始化继电器接口
  digitalWrite(RELAY, HIGH);

  // 以太网初始化
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);

  // 初始化乐为网关
  node1 = new LeweiTcpClientMod(LW_USERKEY, "01");
  node2 = new LeweiTcpClientMod(LW_USERKEY, "02");

  // 添加被控制函数
  UserFunction uf1 (switchOn,"turnSwitchOn");
  node1->addUserFunction(uf1);
  UserFunction uf2 (switchOff,"turnSwitchOff");
  node1->addUserFunction(uf2);
}

void loop() 
{
  char buf[9];      // 串口数据暂存
  unsigned char node;        // 节点
  unsigned char temperature; // 温度
  unsigned char humidity;    // 湿度
  int pm;          // PM2.5

  node1->keepOnline();
  node2->keepOnline();

  if(Serial.available() > 0)
  {
    // 读取串口数据
    Serial.readBytes(buf, 9);

    // 把数据通过软串口发出去
    SoftSerial.println(buf);

    // 处理数据
    node = buf[0] - '0';
    temperature = (buf[1] - '0') * 10 + (buf[2] - '0');
    humidity = (buf[3] - '0') * 10 + (buf[4] - '0');
    pm = (buf[5] - '0') * 100 + (buf[6] - '0') * 10 + (buf[7] - '0');

    // 发送数据
    switch (node)
    {
      case 1:
      if(node1)
      {
        node1->appendSensorValue("WD", (String)temperature);
        node1->appendSensorValue("SD", (String)humidity);
        node1->sendSensorValue("PM", (String)pm);
      }
      break;

      case 2:
      if(node2)
      {
        node2->appendSensorValue("WD", (String)temperature);
        node2->appendSensorValue("SD", (String)humidity);
        node2->sendSensorValue("PM", (String)pm);
      }
      break;

      default:
      Serial.println("Invalid node!");
    }
  }
}

void switchOn()
{
  node1->setRevCtrlMsg("true","on");
  digitalWrite(RELAY, LOW);
}

void switchOff()
{
  node1->setRevCtrlMsg("true","off");
  digitalWrite(RELAY, HIGH);
}
