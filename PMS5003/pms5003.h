#ifndef __PMS5003_H__
#define __PMS5003_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	 	 	 	 
#include "string.h"    	
#include "uart4.h" 
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

void pms5003_send_data(u8 *data, u8 dataNum);
u8 pms5003_config(void);
u8 pms5003_data_process(u8 mode);

extern u16 PM25;

#endif





