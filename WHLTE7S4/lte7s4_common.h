#ifndef __LTE7S4_COMMON_H__
#define __LTE7S4_COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"
#include "malloc.h"	 	 	 	 	 
#include "string.h"    	
#include "usart3.h" 
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//void wh_lte_7s4_at_response(u8 mode);
u8* wh_lte_7s4_check_cmd(u8 *str);
void wh_lte_7s4_send_data(u8 *data, u8 dataNum);
u8 wh_lte_7s4_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 wh_lte_7s4_enter_config(void);

void wh_lte_7s4_config(void);
void wh_lte_7s4_udp_config(void);
void wh_lte_7s4_data_process(void);

//用户配置参数
extern const u8* NTP_server_addr;	//NTP 服务器地址
extern const u8* NTP_portnum;		//连接端口
extern u8 NTPDataSend[48];
extern u32 T2_second;
extern u32 T2_millisecond; 
#endif





