#ifndef __UART4_H
#define __UART4_H	 
#include "sys.h"  

#define UART4_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define UART4_MAX_SEND_LEN		1024				//����ͻ����ֽ���
#define UART4_RX_EN 			1					//0,������;1,����.

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//���ջ���,���UART4_MAX_RECV_LEN�ֽ�
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�
extern u16 UART4_RX_STA;   						//��������״̬

void UART4_Init(u32 bound);				//����4��ʼ�� 
void u4_printf(char* fmt, ...);
#endif












