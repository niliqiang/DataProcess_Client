# DataProcess_Client
> STM32F103+ESP8266+4G透传模块+PM2.5传感器  
  
**一. 硬件平台**  
1. 正点原子MiniSTM32F103开发板V3版本  
2. ESP8266 WIFI模块
3. WHLTE7S4v2 4G模块
4. PMS5003 PM2.5传感器

**二. 软件平台**  
1. Keil 5
2. 库函数v3.5版本

**三. 使用说明**  
1. 开发板的串口1连接电脑串口，实时检测DataProcess_Client的运行状态
2. ESP8266 WIFI模块：  
- 供电电压：3.3V
- 使用串口2（TX:PA2; RX:PA3）
3. WHLTE7S4v2 4G模块：  
- 单独供电，注意与开发板共地
- 使用串口3（TX:PB10; RX:PB11）
