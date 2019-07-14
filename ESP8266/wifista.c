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
//返回值:0,正常
//    其他,错误代码
u8 netpro=0;	//网络模式
u8 atk_8266_wifista_test(void)
{
	//u8 netpro=0;	//网络模式
	u8 key;
	u8 ipbuf[16] = "10.157.167.190"; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	p=mymalloc(32);							//申请32字节内存
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
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
//		atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
//		sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //开启Server模式(0，关闭；1，打开)，端口号为portnum
//		atk_8266_send_cmd(p,"OK",50);
	}	
	else if(netpro&0x01)	//TCP client  透传模式
	{
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接		
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
		while(atk_8266_send_cmd(p,"OK",200))
		{
			printf("ESP8266 连接 TCP 失败, IP:%s, Port:%s", ipbuf, (u8*)portnum);
		}		
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传	
	
	}	
	else if(netpro&0x02)	//UDP(待完善)
	{
//		sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
//	    delay_ms(200);
//		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //单链接模式
//		delay_ms(200);
//		while(atk_8266_send_cmd(p,"OK",500));
	}
	
	USART2_RX_STA = 0;
	while(1)
	{
		key = KEY_Scan(0);
		if(key == WKUP_PRES)
		{
			res = 0;
			atk_8266_quit_trans();		//退出透传
			atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20);
			break;
		}
		else if(key == KEY0_PRES)
		{
			if(netpro&0x00)		//TCP server(待完善)
			{
//				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
//				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
//				delay_ms(200);
//				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
			}
			else if(netpro&0x01)		//TCP client  透传模式
			{
				atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				u2_printf("%s",p);
			}
			else if((netpro==3)||(netpro==2))   //UDP(待完善)
			{
//				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
//				atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //发送指定长度的数据
//				delay_ms(200);
//				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
			}
		}
		t++;
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//接收到一次数据了
		{ 
			rlen=USART2_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART2_RX_BUF[rlen]=0;		//添加结束符 
			printf("%s",USART2_RX_BUF);	//发送到串口   
			USART2_RX_STA=0;
			if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
			else t=0;                   //状态为已经连接了,10秒后再检查
		}  
		
		if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
		{
			constate=atk_8266_consta_check();//得到连接状态
			if(constate=='+')
				printf("已连接\r\n");  //连接状态
			else
				printf("已断开连接\r\n");
			t=0;
		}
		if((t%200)==0)LED0=!LED0;
		atk_8266_at_response(1);
	}

	myfree(p);		//释放内存 
	return res;		
} 



