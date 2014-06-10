/**
  ******************************************************************************
  * @file    AirBee_Network_Arduino.ino
  * @author  Blanboom
  * @date    15-Mar-2011
  * @brief   Arduino 网关，将各个节点的数据汇总并发送至云平台
  ******************************************************************************
  * @note
  * 
  * 该程序实现下列功能：
  *     1. 通过串口接收各个节点、各个传感器的数据
  *     2. 对接收到的数据进行处理
  *     3. 将处理好的数据通过 HTTP 协议上传至云平台
  *     4. 将处理好的数据通过软件串口发送，方便调试和功能拓展
  *
  * 硬件平台：
  *     ITEAD IBOARD(ATMega328p + W5100)
  *
  * 接口定义：
  *     串口:        rx: UART_RX, tx: UART_TA
  *     软件串口:    rx: A0, tx: A1
  *     控制输出口:  A3(预留)
  *
  * 编译环境：
  *     Arduino IDE for OS X 1.5.6-r2
  *
  * 库：
  *     官方库：SPI, Ethernet, SoftSerial
  *     其他：LeweiClient
  *
  ******************************************************************************
  * @attention
  * 
  * 本程序需要实现多个节点同时发送，所以不能使用官方的 LeweiClient 库，
  * 而需要使用程序目录下的 LeweiClient
  *
  ******************************************************************************
  */
#include <LeweiClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

#define LW_USERKEY "MyUserKey"  /* 需要替换为用户私有的 UserKey */
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  /* 网卡 MAC 地址，不能与其他设备相同 */

/* 节点对象 */
LeWeiClient *node1;
LeWeiClient *node2;

SoftwareSerial SoftSerial(A0, A1);  /* 软串口 */

/**
 * @brief 硬件初始化
 * @param None
 * @retval None
 */
void setup() 
{
  /* 打开串口 */
  Serial.begin(9600);
  SoftSerial.begin(9600);

  /* 初始化网卡 */
  while (Ethernet.begin(mac) == 0);

  /* 初始化节点 */
  node1 = new LeWeiClient(LW_USERKEY, "01");
  node2 = new LeWeiClient(LW_USERKEY, "02");
}

/**
 * @brief 主程序
 * @param None
 * @retval None
 */
void loop() 
{
  /* 用来储存各个传感器数值的变量 */
  char buf[9];                   /* 串口数据暂存 */
  unsigned char node = 0;        /* 节点  */
  unsigned char temperature = 0; /* 温度  */
  unsigned char humidity = 0;    /* 湿度  */
  int pm = 0;                    /* PM2.5 */
  int mq = 0;                    /* MQ135 */


  if(Serial.available() > 0)
  {
    /* 读取串口数据 */
    Serial.readBytes(buf, 13);

    /* 处理数据 */
    node = buf[0] - '0';
    temperature = (buf[1] - '0') * 10 + (buf[2] - '0');
    humidity = (buf[3] - '0') * 10 + (buf[4] - '0');
    mq = (buf[6] - '0') * 100 + (buf[7] - '0') * 10 + (buf[8] - '0');
    pm = (buf[9] - '0') * 100 + (buf[10] - '0') * 10 + (buf[11] - '0');

    /* 通过软串口发送数据 */
    SoftSerial.println(buf);

    /* 通过 HTTP 方式发送数据到服务器 */
    switch (node)
    {
      /* 节点 1 */
      case 1:
      if(node1)
      {
        node1->append("WD", temperature);
        node1->append("SD", humidity);
        node1->append("PM", pm); 
        node1->append("MQ", mq);
        node1->send();
      }
      break;

      /* 节点 2 */
      case 2:
      if(node2)
      {
        node2->append("WD", temperature);
        node2->append("SD", humidity);
        node2->append("PM", pm);
        node2->append("MQ", mq);
        node2->send();
      }
      break;

      /* 无效节点 */
      default:
      SoftSerial.println("Invalid node!");
    }
  }
}
