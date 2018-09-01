#include "driver/hal_rirc.h"
#include "osapi.h"
#include "driver/uart.h"


bool Pre_On_Off;
bool Pre_Cold_Warm;
uint8_t Pre_t;

void UART_Transmit_IR_Code(uint8_t ch)
{
	uart_tx_one_char_no_wait(UART0, ch);
}
void ICACHE_FLASH_ATTR Emit_IR(bool OnOff, bool ColdWarm, uint8 t)
{
	if(!((OnOff==Pre_On_Off)&&(ColdWarm==Pre_Cold_Warm)&&(t==Pre_t)))
	{
//		IR_Data_Change(OnOff, ColdWarm, t);
//		Transmit_IR_Code();
		UART_Transmit_IR_Code(OnOff*64+ColdWarm*32+t);
		Pre_On_Off=OnOff;
		Pre_Cold_Warm=ColdWarm;
		Pre_t=t;
	}
	
}
