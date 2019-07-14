#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "malloc.h"
#include "usart2.h"
#include "common.h"


int main(void)
{

//	u8 t;
//	u8 len;	
//	u16 times=0; 

	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);	 //串口初始化为115200
	USART2_Init(115200);  //初始化串口2波特率为115200
	LED_Init();		  	 //初始化与LED连接的硬件接口
	KEY_Init();				//按键初始化
	mem_init();				//初始化内存池	
	
	printf("\r\nESP8266 WiFi测试开始\r\n");
	atk_8266_test();
	 
	 
/*
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nALIENTEK MiniSTM32开发板 串口实验\r\n");
				printf("正点原子@ALIENTEK\r\n\r\n\r\n");
			}
			if(times%200==0)printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}
*/	
}


