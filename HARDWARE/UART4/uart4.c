#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


//串口发送缓存区 	
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN];		//发送缓冲,最大UART4_MAX_SEND_LEN字节 
//串口接收缓存区 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN];		//接收缓冲,最大UART4_MAX_RECV_LEN个字节.

u16 UART4_RX_STA=0; 
/*******************************************************************************  
* 函数名      : UART4_Init  
* 函数功能    : IO端口及串口4,时钟初始化函数 C10,C11     
* 输  入      : 无 
* 输  出      : 无 
*******************************************************************************/    
void UART4_Init(u32 bound)    
{    
	USART_InitTypeDef USART_InitStructure;    
	NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure; 
   
	//使能串口的RCC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);    

	//串口使用的GPIO设置  
	// Configure UART4 Rx (PC.11) as input floating      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	// Configure UART4 Tx (PC.10) as alternate function push-pull    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	//配置串口  
	USART_InitStructure.USART_BaudRate = bound;    
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式   
	USART_InitStructure.USART_StopBits = USART_StopBits_1;    
	USART_InitStructure.USART_Parity = USART_Parity_No;    
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制    
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


	// Configure UART4     
	USART_Init(UART4, &USART_InitStructure);

	// Enable UART4 Receive interrupts 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);     

	// Enable the UART4     
	USART_Cmd(UART4, ENABLE);      

	// Enable the USART4 Interrupt     
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure);

	UART4_RX_STA=0;		//清零    
}


void UART4_IRQHandler(void)
{
	u8 res;             
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{         
		res =USART_ReceiveData(UART4);	//读取接收到的数据
		
		if((UART4_RX_STA&0x8000)==0)
		{
			if(UART4_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)UART4_RX_STA=0;	//接收错误, 重新开始接收
				else UART4_RX_STA|=0x8000;	//接收完成
			}else //还没接收到0X0D
			{        
				if(res==0x0d)UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=res;
					UART4_RX_STA++;
					if(UART4_RX_STA>(UART4_MAX_RECV_LEN-1))UART4_RX_STA=0;	//接收错误,重新开始接收          
				}                 
			}
		}                                                                                                
	}
}
  
//串口4, printf函数
//一次发送的数据不超过USART4_MAX_SEND_LEN字节
void u4_printf(char* fmt,...)  
{  
        u16 i,j; 
        va_list ap; 
        va_start(ap,fmt);
        vsprintf((char*)UART4_TX_BUF,fmt,ap);
        va_end(ap);
        i=strlen((const char*)UART4_TX_BUF);	//此次发送的数据长度
        for(j=0;j<i;j++)
        {
                while((UART4->SR&0X40)==0);	//循环发送，直到发送完毕	  
                UART4->DR=UART4_TX_BUF[j];  
        } 
}

