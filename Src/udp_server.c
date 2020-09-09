/*
 * udp_server.c
 *
 *  Created on: 14 џэт. 2020 у.
 *      Author: User
 */

#include "udp_server.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "crc.h"
#include <string.h>
#include "eeprom.h"
#include "modbus.h"

#define UDP_SERVER_PORT    12146
#define INCORRECT_PAGE_NUM	1
#define READ_ERROR		2
#define PAGE_BUSY		3
#define BAD_BLOCK		4


static char answer[1324];
static unsigned short reqID = 0;

extern uint8_t jump_flag;
extern uint16_t VirtAddVarTab[NB_OF_VAR];

extern uint8_t ip_addr[4];
extern uint8_t ip_mask[4];
extern uint8_t ip_gate[4];
extern uint16_t ai_type;
extern uint8_t net_address;
extern uint16_t rs485_conf1;
extern uint16_t rs485_conf2;

static void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
static void inline send_udp_data(struct udp_pcb *upcb,const ip_addr_t *addr,u16_t port,u16_t length);

void udp_server_init(void) {
	struct udp_pcb *upcb;
	err_t err;

	/* Create a new UDP control block  */
	upcb = udp_new();

	if (upcb)
	{
	 /* Bind the upcb to the UDP_PORT port */
	 /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
	  err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);

	  if(err == ERR_OK)
	  {

		/* Set a receive callback for the upcb */
		udp_recv(upcb, udp_server_receive_callback, NULL);
	  }
	  else
	  {
		udp_remove(upcb);
	  }
	}
}

void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
	unsigned char *data;

	uint16_t crc=0;
	uint16_t cnt=0;
	uint16_t value = 0;
	uint16_t mem_addr = 0;
	uint16_t tmp=0;

	data = (unsigned char*)(p->payload);
	crc = GetCRC16(data,p->len);


	if(crc==0)
	{
	  //udp_tmr = 0;
	  reqID = (unsigned short)data[0]<<8;
	  reqID |= data[1];
	  switch(data[2]){
		case 0xA0:
		  answer[0] = data[0];
		  answer[1] = data[1];
		  answer[2] = 0xA0;
		  answer[3] = 0x01;	// type of device identificator
		  answer[4] = 0xC4;
		  answer[5] = 0x00;
		  answer[6] = 0x01;
		  crc = GetCRC16((unsigned char*)answer,7);
		  answer[7]=crc>>8;
		  answer[8]=crc&0xFF;
		  send_udp_data(upcb, addr, port,9);
		  break;
		case 0xEC:// restart in boot mode
		  answer[0] = data[0];
		  answer[1] = data[1];
		  answer[2] = 0xEC;
		  crc = GetCRC16((unsigned char*)answer,3);
		  answer[3]=crc>>8;
		  answer[4]=crc&0xFF;
		  send_udp_data(upcb, addr, port,5);
		  EE_WriteVariable(VirtAddVarTab[1],0);
		  HAL_Delay(50);
		  NVIC_SystemReset();
		  break;
		case 0xED:// reset
		  answer[0] = data[0];
		  answer[1] = data[1];
		  answer[2] = 0xED;
		  crc = GetCRC16((unsigned char*)answer,3);
		  answer[3]=crc>>8;
		  answer[4]=crc&0xFF;
		  send_udp_data(upcb, addr, port,5);
		  HAL_Delay(50);
		  NVIC_SystemReset();
		  break;
		case 0x03:
		  answer[0] = data[0];
		  answer[1] = data[1];
		  answer[2] = 0x03;
		  mem_addr = ((unsigned short)data[3]<<8) | data[4];
		  cnt = ((unsigned short)data[5]<<8) | data[6];
		  if(cnt>HOLDR_COUNT || cnt==0) {
			  answer[3] = 0x02;
			  crc = GetCRC16((unsigned char*)answer,4);
			  answer[4]=crc>>8;
			  answer[5]=crc&0xFF;
			  send_udp_data(upcb, addr, port,6);
			  break;
		  }
		  if(mem_addr+cnt>HOLDR_COUNT) {
			  answer[3] = 0x03;
			  crc = GetCRC16((unsigned char*)answer,4);
			  answer[4]=crc>>8;
			  answer[5]=crc&0xFF;
			  send_udp_data(upcb, addr, port,6);
			  break;
		  }
		  answer[3] = 0x01;
		  for(tmp=0;tmp<cnt;tmp++) {
			switch(mem_addr+tmp) {
			case 0:
				answer[4+tmp*2] = ai_type>>8;
				answer[5+tmp*2] = ai_type&0xFF;
				break;
			case 1:
				answer[4+tmp*2] = 0;
				answer[5+tmp*2] = net_address;
				break;
			case 2:
				answer[4+tmp*2] = ip_addr[0];
				answer[5+tmp*2] = ip_addr[1];
				break;
			case 3:
				answer[4+tmp*2] = ip_addr[2];
				answer[5+tmp*2] = ip_addr[3];
				break;
			case 4:
				answer[4+tmp*2] = ip_mask[0];
				answer[5+tmp*2] = ip_mask[1];
				break;
			case 5:
				answer[4+tmp*2] = ip_mask[2];
				answer[5+tmp*2] = ip_mask[3];
				break;
			case 6:
				answer[4+tmp*2] = ip_gate[0];
				answer[5+tmp*2] = ip_gate[1];
				break;
			case 7:
				answer[4+tmp*2] = ip_gate[2];
				answer[5+tmp*2] = ip_gate[3];
				break;
			case 8:
				answer[4+tmp*2] = rs485_conf1>>8;
				answer[5+tmp*2] = rs485_conf1&0xFF;
				break;
			case 9:
				answer[4+tmp*2] = rs485_conf2>>8;
				answer[5+tmp*2] = rs485_conf2&0xFF;
				break;
			default:
				answer[4+tmp*2] = 0;
				answer[5+tmp*2] = 0;
				break;
			}
		  }
		  crc = GetCRC16((unsigned char*)answer,4+cnt*2);
	      answer[4+cnt*2]=crc>>8;
	      answer[5+cnt*2]=crc&0xFF;
	      send_udp_data(upcb, addr, port,6+cnt*2);
		  break;
		case 0x10:
		  answer[0] = data[0];
		  answer[1] = data[1];
		  answer[2] = 0x10;
		  mem_addr = ((unsigned short)data[3]<<8) | data[4];
		  cnt = ((unsigned short)data[5]<<8) | data[6];
		  if(cnt>=128 || cnt==0) {
			  answer[3] = 0x02;
			  crc = GetCRC16((unsigned char*)answer,4);
			  answer[4]=crc>>8;
			  answer[5]=crc&0xFF;
			  send_udp_data(upcb, addr, port,6);
			  break;
		  }
		  if(mem_addr+cnt>HOLDR_COUNT) {
			  answer[3] = 0x03;
			  crc = GetCRC16((unsigned char*)answer,4);
			  answer[4]=crc>>8;
			  answer[5]=crc&0xFF;
			  send_udp_data(upcb, addr, port,6);
			  break;
		  }
		  answer[3] = 0x01;
		  for(tmp=0;tmp<cnt;tmp++) {
			  value  = data[8+tmp*2] | ((unsigned short)data[7+tmp*2]<<8);
			  switch(mem_addr+tmp) {
				case 0:
					ai_type = value;
					EE_WriteVariable(VirtAddVarTab[9],ai_type);
					break;
				case 1:
					net_address = data[8+tmp*2] ;
					EE_WriteVariable(VirtAddVarTab[2],value);
					break;
				case 2:
					ip_addr[0] = data[7+tmp*2];
					ip_addr[1] = data[8+tmp*2];
					EE_WriteVariable(VirtAddVarTab[3],value);
					break;
				case 3:
					ip_addr[2] = data[7+tmp*2];
					ip_addr[3] = data[8+tmp*2];
					EE_WriteVariable(VirtAddVarTab[4],value);
					break;
				case 4:
					ip_mask[0] = data[7+tmp*2];
					ip_mask[1] = data[8+tmp*2];
					EE_WriteVariable(VirtAddVarTab[5],value);
					break;
				case 5:
					ip_mask[2] = data[7+tmp*2];
					ip_mask[3] = data[8+tmp*2];
					EE_WriteVariable(VirtAddVarTab[6],value);
					break;
				case 6:
					ip_gate[0] = data[7+tmp*2];
					ip_gate[1] = data[8+tmp*2];
					EE_WriteVariable(VirtAddVarTab[7],value);
					break;
				case 7:
					ip_gate[2] = data[7+tmp*2];
					ip_gate[3] = data[8+tmp*2];
					EE_WriteVariable(VirtAddVarTab[8],value);
					break;
				case 8:
					rs485_conf1 = value;
					EE_WriteVariable(VirtAddVarTab[10],rs485_conf1);
					break;
				case 9:
					rs485_conf2 = value;
					EE_WriteVariable(VirtAddVarTab[11],rs485_conf2);
					break;
			}
		  }
		  crc = GetCRC16((unsigned char*)answer,4);
		  answer[4]=crc>>8;
		  answer[5]=crc&0xFF;
		  send_udp_data(upcb, addr, port,6);
		  break;
	  }
	}

	pbuf_free(p);
}

void inline send_udp_data(struct udp_pcb *upcb,const ip_addr_t *addr,u16_t port,u16_t length) {
	struct pbuf *p_answer;
	udp_connect(upcb, addr, port);
	p_answer = pbuf_alloc(PBUF_TRANSPORT,length, PBUF_POOL);
	if (p_answer != NULL)
	{
	  pbuf_take(p_answer, answer, length);
	  udp_send(upcb, p_answer);
	  pbuf_free(p_answer);
	}
	udp_disconnect(upcb);
}
