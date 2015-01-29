# AirBee

## 简介

多组传感器通过 ZigBee 组网监测环境质量，并上传至物联网公众平台(lewei50.com)


## 文件说明

**AirBee_Sensor_PCB**: 

各个传感器节点的 PCB 底板，用于供电和连接传感器。需要配合该 [CC2530 核心板](http://item.taobao.com/item.htm?spm=a230r.1.14.1.y1gohq&id=19051924425&ns=1&abbucket=3#detail)使用。

**AirBee_Network_PCB**: 网络部分的 PCB. 网络部分由 ZigBee 协调器、Arduino 和 Arduino 以太网扩展板组成，ZigBee 协调器将接收到的数据发送至 Arduino，再由 Arduino 通过以太网扩展板上传至服务器。

**AirBee_Network_Arduino**: 网络部分的 Ardunio 程序。

CC2530 的程序暂时不便公开，如有需要请给我发邮件。
