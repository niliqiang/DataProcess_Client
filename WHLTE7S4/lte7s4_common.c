#include "lte7s4_common.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//用户配置区

//NTP服务器地址以及端口号:123
const u8* NTP_server_addr = "ntp1.aliyun.com";
const u8* NTP_portnum = "123";
u8 NTPDataSend[48] = {	0x1B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0xD0,0xAF,0x5F,0xF5,0x23,0xD7,0x08,0x00};

//为了调试方便，暂时使用T2来粗略表示本地时间
uint64_t T_Integer[4] = {0};
uint64_t T_Fraction[4] = {0}; 
uint64_t timestamp = 0;

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
//void wh_lte_7s4_at_response(u8 mode)
//{
//	if(USART3_RX_STA&0X8000)		//接收到一次数据了
//	{ 
//		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
//		printf("%s",USART3_RX_BUF);	//发送到串口
//		if(mode)USART3_RX_STA=0;
//	} 
//}
//wh_lte_7s4发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* wh_lte_7s4_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向wh_lte_7s4发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 wh_lte_7s4_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(wh_lte_7s4_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//向wh_lte_7s4发送指定数据
//data:发送的数据(不需要添加回车了)
//dataNum:数据长度
void wh_lte_7s4_send_data(u8 *data, u8 dataNum)
{
	u8 i; 
	for(i = 0; i<dataNum; i++) 
	{ 
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  
		USART_SendData(USART3,*data++); 
	}
}

//wh_lte_7s4进入指令配置状态
//返回值:0, 进入成功;
//       1, 进入失败
u8 wh_lte_7s4_enter_config(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';
	delay_ms(50);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='a';
	delay_ms(500);					//等待500ms
	return wh_lte_7s4_send_cmd("AT","OK",50);//进入指令配置状态判断	
}

//wh_lte_7s4模块配置主函数
void wh_lte_7s4_config(void)
{
	while(wh_lte_7s4_send_cmd("AT","OK",50))//检查4G模块是否处于指令配置模式
	{
		delay_ms(1000);
		wh_lte_7s4_enter_config();//退出透传，进入指令配置模式
		printf("未检测到4G模块！2秒后尝试重新连接模块...\r\n\r\n");
		delay_ms(1000);
	} 
	while(wh_lte_7s4_send_cmd("AT+E=OFF","OK",50));//关闭回显
	printf("4G模块连接成功，开始配置并测试UDP模式...\r\n");
	delay_ms(50); 
	wh_lte_7s4_udp_config();	//网络透传UDP测试
}



