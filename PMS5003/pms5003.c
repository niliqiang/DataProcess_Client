#include "pms5003.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//全局变量定义区
//被动式指令及Return
u8 passiveCode[] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
u8 PCReturn[] = {0x42, 0x4D, 0x00, 0x04, 0xE1, 0x00, 0x01, 0x74};
//主动式指令
u8 activeCode[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
//被动式读数指令
u8 passiveReadCode[] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};

u16 PM25 = 0;

 
//向pms5003发送指定数据
//data:发送的数据(不需要添加回车了)
//dataNum:数据长度
void pms5003_send_data(u8 *data, u8 dataNum)
{
	u8 i; 
	for(i = 0; i<dataNum; i++) 
	{ 
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  
		USART_SendData(UART4,*data++); 
	}
}

//pms5003初始化函数
//工作模式转为被动式(passive mode)
u8 pms5003_config()
{
	u8 j;
	UART4_RX_STA = 0;	//清空接收标志，准备接收数据
	pms5003_send_data(passiveCode, 7);
	delay_ms(20);
	//配置结果检验
	while(1)
	{
		delay_ms(10); 
		if(UART4_RX_STA == 8)	//接收到一次PMS5003返回数据
		{
			UART4_RX_STA=0;		//清空接收标志
			for(j=0; j<8; j++)
			{
				if(UART4_RX_BUF[j] != PCReturn[j])
				{
					printf("PMS5003模块配置失败！\r\n");
					return 0;
				}
			}
			printf("PMS5003模块配置成功！\r\n");
			return 1;
		}
	}
}

u8 pms5003_data_process()
{
	u8 times;
	UART4_RX_STA = 0;	//清空接收标志，准备接收数据
	pms5003_send_data(passiveReadCode, 7);;	//发送PM2.5请求数据
	while(1)
	{
		delay_ms(20); 
		if(UART4_RX_STA == 32)	//接收到一次PMS5003返回数据
		{
			times = 0;			//清零计数
			UART4_RX_STA=0;		//清空接收标志
			if((UART4_RX_BUF[0]==0x42)&&(UART4_RX_BUF[1]==0x4d))
			{
				//先清零
				PM25 = 0;
				PM25 = ((u16)UART4_RX_BUF[12] << 8) + UART4_RX_BUF[13];
				printf("PM2.5: %d\r\n", PM25);	//发送到串口
				return 0;
				
			}
			else 
			{
				printf("PMS5003返回数据有误！1秒后重新发送请求...\r\n");
				delay_ms(1000);
				return 1;
			}
		}
		else
		{
			times++;
			if(times%20==0)
			{
				printf("PMS5003数据请求失败！1秒后重新发送请求...\r\n");
				delay_ms(1000);
				return 1;
			}
		}
	}
}



