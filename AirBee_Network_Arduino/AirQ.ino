#include <LeweiClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

#define LW_USERKEY "75d79e68f03d4801a7d47f283d97e4a2"
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

LeWeiClient *node1;
LeWeiClient *node2;

SoftwareSerial SoftSerial(A0, A1);  // 软串口

void setup() 
{
  // start serial port:
  Serial.begin(9600);
  SoftSerial.begin(9600);

  while (Ethernet.begin(mac) == 0);
  node1 = new LeWeiClient(LW_USERKEY, "01");
  node2 = new LeWeiClient(LW_USERKEY, "02");
}

void loop() 
{
  char buf[9];      // 串口数据暂存
  unsigned char node;        // 节点
  unsigned char temperature; // 温度
  unsigned char humidity;    // 湿度
  int pm;          // PM2.5


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
        node1->append("WD", temperature);
        node1->append("SD", humidity);
        node1->append("PM", pm);
        node1->send();
      }
      break;

      case 2:
      if(node2)
      {
        node2->append("WD", temperature);
        node2->append("SD", humidity);
        node2->append("PM", pm);
        node2->send();
      }
      break;

      default:
      Serial.println("Invalid node!");
    }
  }
}

