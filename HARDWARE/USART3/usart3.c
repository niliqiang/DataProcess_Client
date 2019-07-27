#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


//串口发送缓存区 	
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN];		//发送缓冲,最大USART3_MAX_SEND_LEN字节 
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];		//接收缓冲,最大USART3_MAX_RECV_LEN个字节.

u16 USART3_RX_STA=0; 
/*******************************************************************************  
* 函数名      : uart3_init  
* 函数功能    : IO端口及串口3,时钟初始化函数 B10,B11     
* 输  入      : 无 
* 输  出      : 无 
*******************************************************************************/    
void USART3_Init(u32 bound)    
{    
	USART_InitTypeDef USART_InitStructure;    
	NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure; 
   
	//使能串口的RCC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //??UART3??GPIOB???    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    

	//串口使用的GPIO设置  
	// Configure USART3 Rx (PB.11) as input floating      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);    

	// Configure USART3 Tx (PB.10) as alternate function push-pull    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);    

	//配置串口  
	USART_InitStructure.USART_BaudRate = bound;    
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式   
	USART_InitStructure.USART_StopBits = USART_StopBits_1;    
	USART_InitStructure.USART_Parity = USART_Parity_No;    
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制    
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


	// Configure USART3     
	USART_Init(USART3, &USART_InitStructure);

	// Enable USART1 Receive interrupts 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);     

	// Enable the USART3     
	USART_Cmd(USART3, ENABLE);   

	//串口中断配置 
	//Configure the NVIC Preemption Priority Bits       
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);    

	// Enable the USART3 Interrupt     
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure);

	USART3_RX_STA=0;		//清零    
}


void USART3_IRQHandler(void)
{
	u8 res;             
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{         
		res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART3_RX_STA&0x8000)==0)
		{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART3_RX_STA=0;	//接收错误, 重新开始接收
				else USART3_RX_STA|=0x8000;	//接收完成
			}else //还没接收到0X0D
			{        
				if(res==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=res;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_MAX_RECV_LEN-1))USART3_RX_STA=0;	//接收错误,重新开始接收          
				}                 
			}
		}                                                                                                
	}
}
  
//串口3, printf函数
//一次发送的数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
        u16 i,j; 
        va_list ap; 
        va_start(ap,fmt);
        vsprintf((char*)USART3_TX_BUF,fmt,ap);
        va_end(ap);
        i=strlen((const char*)USART3_TX_BUF);	//此次发送的数据长度
        for(j=0;j<i;j++)
        {
                while((USART3->SR&0X40)==0);	//循环发送，直到发送完毕	  
                USART3->DR=USART3_TX_BUF[j];  
        } 
}

