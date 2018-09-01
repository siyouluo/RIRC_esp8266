#ifndef _HAL_RIRC_H
#define _HAL_RIRC_H

#include <c_types.h>
#include <eagle_soc.h>
#include <ets_sys.h>
#include <gpio.h>
#include "gizwits_product.h"
#include <osapi.h>
#include <os_type.h>
#include <stdio.h>

void UART_Transmit_IR_Code(uint8_t ch);
void Emit_IR(bool On_Off, bool Cold_Warm, uint8 t);




#endif
