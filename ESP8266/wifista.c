#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
u8 netpro=0;	//网络模式
void atk_8266_wifista_config(void)
{
	u8 *p;

	p=mymalloc(32);							//申请32字节内存
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时4S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
    
	delay_ms(200);		//直接设置netpro时，需要有一段延时
	netpro = 1;			//0:TCP server; 1:TCP client; 2:UDP
	
	if(netpro&0x00)		//TCP server(待完善)
	{

	}	
	else if(netpro&0x01)	//TCP client  透传模式
	{
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0:单连接,1:多连接		
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP Server
		while(atk_8266_send_cmd(p,"OK",300))
		{
			printf("ESP8266连接TCP失败, IP:%s, Port:%s\r\n", ipbuf, (u8*)portnum);
		}		
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
	
	}	
	else if(netpro&0x02)	//UDP(待完善)
	{

	}
	printf("ESP8266模块配置成功！\r\n");
	myfree(p);		//释放内存 	
} 

//ATK-ESP8266 Data Process
//用于向Server发送数据
void atk_8266_data_process(u32 T2_second, u32 T2_millisecond)
{
	u16 rlen=0;
	USART2_RX_STA = 0;
	
	if(netpro&0x00)		//TCP server(待完善)
	{

	}
	else if(netpro&0x01)		//TCP client  透传模式
	{
		atk_8266_quit_trans();
		atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传

		u2_printf("POST /dataInfo/store?clientId=%s&clientTime=2208988801&airPara=60 HTTP/1.1\r\n", clientId);
		delay_ms(10);//延时一段时间等待发送完成
		u2_printf("Content-Type: application/json;charset=utf-8\r\n");
		delay_ms(10);
		u2_printf("Host: %s:%s\r\n", ipbuf, (u8*)portnum);
		delay_ms(10);
		u2_printf("Connection: Keep Alive\r\n");
		delay_ms(10);
		u2_printf("\r\n");

		//u2_printf("%08X",T2_second);
		delay_ms(200);
		//u2_printf("%08X",T2_millisecond);
	}
	else if((netpro==3)||(netpro==2))   //UDP(待完善)
	{

	}
	
	while(1)
	{
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//接收到一次数据了
		{
			rlen=USART2_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART2_RX_BUF[rlen]=0;		//添加结束符 
			printf("%s",USART2_RX_BUF);	//发送到串口   
			USART2_RX_STA=0;
			break;
		}
	}
}

