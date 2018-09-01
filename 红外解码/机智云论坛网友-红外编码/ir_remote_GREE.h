#ifndef __IR_REMOTE_GREE_H__
#define __IR_REMOTE_GREE_H__

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include "gizwits_product.h"

void coding_convert_dataPoint(void);
void dataPoint_convert_coding(void);
void IR_send_YBOF(bool GREE_buf[]);
void IR_receive_callback(void);
void IR_remote_init(void);

#endif
