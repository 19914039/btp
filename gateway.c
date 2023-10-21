#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <modbus.h>
#include <sys/socket.h>
#include <error.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/ioctls.h>
#include <syslog.h>

int main(int argc, char *argv[])
{

modbus_t *ctxTcp=NULL;
modbus_t *ctxRtu=NULL;
modbus_mapping_t *mb_mapping=NULL;
int rc;
uint8_t *query;
char *ip;
char *device;
int ret;
int s=-1;
int header_length;
uint8_t fc;
uint16_t addr;
uint16_t nb;
int i;

struct timeval response_timeout;
response_timeout.tv_sec=10;
response_timeout.tv_usec=0;

ip="0.0.0.0";
device="/dev/ttyUSB0";
query=malloc(MODBUS_RTU_MAX_ADU_LENGTH);

ctxTcp=modbus_new_tcp(ip,502);
modbus_set_debug(ctxTcp,TRUE);
header_length = modbus_get_header_length(ctxTcp);
mb_mapping=modbus_mapping_new_start_address(0,100,0,100,0,100,0,100);
ctxRtu=modbus_new_rtu(device,19200,'E',8,1);
modbus_set_debug(ctxRtu,TRUE);
modbus_set_response_timeout(ctxRtu,&response_timeout.tv_sec,&response_timeout.tv_usec);
ret = modbus_set_slave(ctxRtu,1);

s=modbus_tcp_listen(ctxTcp,1);
next:
	modbus_tcp_accept(ctxTcp, &s);
	ret=modbus_connect(ctxRtu);

	for(;;)
	{
	do{
	rc=modbus_receive(ctxTcp, query);
	}while(rc==0);
	
	fc=query[header_length];
	addr=MODBUS_GET_INT16_FROM_INT8(query,header_length+1);
	
	if(fc==1 || fc==2 || fc==3 || fc==4)
	{
		
		nb=MODBUS_GET_INT16_FROM_INT8(query,header_length+3);
		
		switch(fc)
		{
			case 1:
				modbus_read_bits (ctxRtu,addr,nb,mb_mapping->tab_bits);
				
				break;
			case 2:
				modbus_read_input_bits (ctxRtu,addr,nb,mb_mapping->tab_input_bits);
				
				break;
			case 3:
				modbus_read_registers(ctxRtu,addr,nb,mb_mapping->tab_registers);
				
				break;
			case 4:
				modbus_read_input_registers (ctxRtu,addr,nb,mb_mapping->tab_input_registers);
				
				break;
		}
	} else if(fc==5 || fc==6 || fc==15 || fc==16)
	{
		switch(fc)
		{
			case 5:
				
				break;
			case 6:
				mb_mapping->tab_registers[addr]=MODBUS_GET_INT16_FROM_INT8(query,header_length+3);
				modbus_write_register(ctxRtu,addr,mb_mapping->tab_registers[addr]);
				
				break;
			case 15:
				
				break;
			case 16:
				nb=MODBUS_GET_INT16_FROM_INT8(query,header_length+3);
				
				for (i=0;i<nb;i++)
				{
					mb_mapping->tab_registers[addr+i]=MODBUS_GET_INT16_FROM_INT8(query,header_length+6+(2*i));
					modbus_write_registers(ctxRtu,addr, nb, mb_mapping->tab_registers);
				}
				
				break;
		}
		
	}
			
	if(rc>0)
	{
	ret=modbus_reply(ctxTcp,query,rc,mb_mapping);
	}else if (rc == -1)
	{
		goto next;
	}
	sleep(1);
	}

modbus_mapping_free(mb_mapping);

modbus_close(ctxTcp);
modbus_free(ctxTcp);
modbus_close(ctxRtu);
modbus_free(ctxRtu);
return 0;
}