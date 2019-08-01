#include "lte7s4_common.h"
#include "stdlib.h"

//whlte7s4 UDP测试函数
//用于测试网络透传UDP模式
void wh_lte_7s4_udp_test(void)
{
	u8 *p;	
	u8 key;
	u16 timex=0;
	u8 i, j;
	u8 NTPDataSend[48] = {	0x1B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0xD0,0xAF,0x5F,0xF5,0x23,0xD7,0x08,0x00};

	p=mymalloc(32);							//申请32字节内存
	wh_lte_7s4_send_cmd("AT+WKMOD=NET","OK",50);	//网络透传模式
	delay_ms(20);
	wh_lte_7s4_send_cmd("AT+SOCKAEN=ON","OK",50);	//socketA 使能
	delay_ms(20);
	sprintf((char*)p,"AT+SOCKA=UDP,%s,%s",NTP_server_addr,NTP_portnum);//设置 SocketA 的远程地址以及端口
	while(wh_lte_7s4_send_cmd(p,"OK",200))
	{
		printf("4G模块连接 NTP Server 失败，1S后重新配置，Addr:%s, Port:%s\r\n", NTP_server_addr, NTP_portnum);
		delay_ms(1000);
	}	
	wh_lte_7s4_send_cmd("AT+Z","OK",50);		//重启模块，参数生效
	printf("4G模块配置成功，等待重启完成...\r\n");
	for(i=0; i<20; i++)		//延时20S等待重启成功
	{
		delay_ms(1000);
	}
	myfree(p);		//释放内存 
	USART3_RX_STA = 0;	//清空接收标志，准备接收数据
	printf("4G模块重启完成，按下 KEY1 开始发送测试数据\r\n");
	while(1)
	{
		delay_ms(10); 
		key = KEY_Scan(0);
		if(key == KEY1_PRES)
		{
			wh_lte_7s4_send_data(NTPDataSend, 48);
			timex=0;
		}
		if(key == WKUP_PRES)
		{
			break;
		}
		if(USART3_RX_STA == 48)		//接收到一次NTP返回数据
		{
			for(j=0; j<48; j++)
				printf("%02X",USART3_RX_BUF[j]);	//发送到串口   
			USART3_RX_STA=0;		//清空接收标志
		}
		
		if((timex%100)==0)LED0=!LED0;//200ms闪烁 
		timex++;
		
	}	
} 



