#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "malloc.h"
#include "usart2.h"
#include "esp8266_common.h"
#include "usart3.h"
#include "lte7s4_common.h"
#include "uart4.h"
#include "pms5003.h"
#include "timer.h"

//#define  DEBUG_USART1_RXTX
//#define  DEBUG_USART3_RXTX
//#define  DEBUG_UART4_RXTX

int main(void)
{

#if (defined DEBUG_USART1_RXTX) || (defined DEBUG_USART3_RXTX) || (defined DEBUG_UART4_RXTX)
	u8 t;
	u8 len;	 
#endif
	
	u8 keyManual, keyAuto, mode;
	u16 times=0;
	

	
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);		//串口初始化为115200
	USART2_Init(115200);	//初始化串口2波特率为115200
	USART3_Init(9600);		//初始化串口3波特率为9600
	UART4_Init(9600);		//初始化串口4波特率为9600
	LED_Init();				//初始化与LED连接的硬件接口
	KEY_Init();				//按键初始化
	mem_init();				//初始化内存池	
	TIM3_Int_Init(49999,7199);//10Khz的计数频率，计数到50000为5000ms  
	
#if (!defined DEBUG_USART1_RXTX) && (!defined DEBUG_USART3_RXTX) && (!defined DEBUG_UART4_RXTX)

	printf("\r\n开始配置PMS5003模块、4G模块、ESP8266模块...\r\n");
	while(pms5003_config());
	wh_lte_7s4_config();
	atk_8266_config();
	printf("PMS5003模块、4G模块、ESP8266模块配置完成！\r\n");
	printf("【手动模式】\r\n当前为手动模式，按下KEY0获取数据；按下KEY1进入自动模式；按下WK_UP退出终端系统。\r\n");	
	while(1)
	{
		delay_ms(10); 
		keyManual = KEY_Scan(0);
		if(keyManual == KEY0_PRES)
		{
			mode = 0;
			//先测AQI
			while(pms5003_data_process(mode));
			while(wh_lte_7s4_data_process());
			atk_8266_data_process(timestamp, PM25);
		}
		if(keyManual == KEY1_PRES)
		{
			printf("【自动模式】\r\n时间间隔：5秒；按下WK_UP退出自动模式。\r\n");
			//打开中断
			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
			TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
			while(1)
			{
				delay_ms(10); 
				keyAuto = KEY_Scan(0);
				if(keyAuto == WKUP_PRES)
				{
					//关闭中断
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					TIM_Cmd(TIM3, DISABLE);  //失能TIMx外设
					printf("自动模式退出成功。\r\n【手动模式】\r\n当前为手动模式，按下KEY0获取数据；按下KEY1进入自动模式；按下WK_UP退出终端系统。\r\n");
					break;
				}
				if((times%100)==0)
				{
					times = 0;
					LED0=!LED0;//1s闪烁 
				}
				times++;	
			}
		}
		if(keyManual == WKUP_PRES)
		{
			atk_8266_quit_trans();		//退出透传
			atk_8266_send_cmd("AT+CIPMODE=0", "OK", 20);
			break;
		}
		if((times%200)==0)
		{
			times = 0;
			LED0=!LED0;//2s闪烁 
		}
		times++;
	}
	
#endif
	 
#ifdef DEBUG_USART1_RXTX 
	//串口1收发测试
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n【串口1】您发送的消息为:\r\n");
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
				printf("\r\n【串口1】请输入数据,以回车键结束\r\n\r\n");
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(100);
	}
#endif

#ifdef DEBUG_USART3_RXTX 
	//串口3收发测试
	while(1)
	{
		if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			u3_printf("\r\n【串口3】您发送的消息为:\r\n");
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
			if(times%30==0)
			{
				u3_printf("\r\n【串口3】请输入数据,以回车键结束\r\n");  
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(100);
	}
#endif
	
#ifdef DEBUG_UART4_RXTX 
	//串口4收发测试
	while(1)
	{
		if(UART4_RX_STA&0x8000)
		{
			len=UART4_RX_STA&0x3fff;//得到此次接收到的数据长度
			u4_printf("\r\n【串口4】您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				UART4->DR=UART4_RX_BUF[t];
				while((UART4->SR&0X40)==0);//等待发送结束
			}
			u4_printf("\r\n");//插入换行
			UART4_RX_STA=0;
		}
		else
		{
			times++;
			if(times%30==0)
			{
				u4_printf("\r\n【串口4】请输入数据,以回车键结束\r\n");  
				LED0=!LED0;//闪烁LED,提示系统正在运行.
			}
			delay_ms(10);   
		}
		delay_ms(100);
	}
#endif	
	
}

