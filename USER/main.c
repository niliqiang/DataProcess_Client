#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "malloc.h"
#include "usart2.h"
#include "common.h"
#include "usart3.h"
#include "lte7s4_common.h"

//#define  DEBUG_USART1_RXTX
//#define  DEBUG_USART3_RXTX

int main(void)
{

#if (defined DEBUG_USART1_RXTX) || (defined DEBUG_USART3_RXTX)
	u8 t;
	u8 len;	
	u16 times=0; 
#endif
	
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);		//串口初始化为115200
	USART2_Init(115200);	//初始化串口2波特率为115200
	USART3_Init(9600);		//初始化串口3波特率为9600
	LED_Init();				//初始化与LED连接的硬件接口
	KEY_Init();				//按键初始化
	mem_init();				//初始化内存池	

//	atk_8266_test();
	wh_lte_7s4_test();
	
	
	 
#ifdef DEBUG_USART1_RXTX 
	//串口1收发测试
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
			printf("\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%30==0)
			{
				printf("请输入数据,以回车键结束\r\n\r\n");
				USART_SendData(USART1,0x1B);
				printf("\r\n\r\n");
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(50);
	}
#endif

#ifdef DEBUG_USART3_RXTX 
	//串口3收发测试
	while(1)
	{
		u3_printf("\r\n串口3测试\r\n");
		if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			u3_printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART3->DR=USART3_RX_BUF[t];
				while((USART3->SR&0X40)==0);//等待发送结束
			}
			u3_printf("\r\n");//插入换行
			USART3_RX_STA=0;
		}
		else
		{
			times++;
			if(times%50==0)
			{
				u3_printf("请输入数据,以回车键结束\r\n");  
				USART_SendData(USART3,0x1B) ;
				u3_printf("\r\n\r\n");
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(1000);
	}
#endif
}


