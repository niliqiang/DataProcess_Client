#include "lte7s4_common.h"
#include "stdlib.h"

//whlte7s4 UDP配置函数
//用于配置网络透传UDP模式
void wh_lte_7s4_udp_config()
{
	u8 *p;	
	u8 i;

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
	delay_ms(20);
	printf("4G模块重启完成！\r\n");
		
} 

//whlte7s4 UDP Data Process
//用于同步时间
u8 wh_lte_7s4_data_process(void)
{
	u8 j, times = 0;
	USART3_RX_STA = 0;	//清空接收标志，准备接收数据
	wh_lte_7s4_send_data(NTPDataSend, 48);	//发送NTP请求数据
	while(1)
	{
		delay_ms(50); 
		if(USART3_RX_STA == 48)	//接收到一次NTP返回数据
		{
			//先清零，以免溢出
			T_Integer[1] = 0;
			T_Fraction[1] = 0;
			timestamp = 0;
			for(j=0; j<4; j++)
			{
				T_Integer[1] |= (uint64_t)USART3_RX_BUF[32+j]<<(8*(3-j));
				T_Fraction[1] |= (uint64_t)USART3_RX_BUF[36+j]<<(8*(3-j));

			}
			timestamp = (T_Integer[1]-0x83AA7E80)*1000 + T_Fraction[1]*1000/4294967296;
			
			printf("Timestamp: %"PRIu64"\r\n",timestamp);	//发送到串口
			USART3_RX_STA=0;		//清空接收标志
			return 0;
		}
		else
		{
			times++;
			if(times%20==0)
			{
				printf("4G模块返回数据有误！5秒后重新发送请求...\r\n");
				times = 0;
				return 1;
			}
		}
	}
}


