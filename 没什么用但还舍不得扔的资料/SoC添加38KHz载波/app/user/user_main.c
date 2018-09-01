/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include"driver/ringbuf.h"
#include "driver/ir_tx_rx.h"





os_timer_t ir_timer;

void ICACHE_FLASH_ATTR
	test_ir_nec_tx()
{
    uint8 ir_data;
    os_printf("-----------------------\r\n");
    os_printf("ir rx \r\n");
    while(IR_RX_BUFF.fill_cnt){
        RINGBUF_Get(&IR_RX_BUFF, &ir_data,1);
        os_printf("IR buf pop : %02xh \r\n",ir_data);
    }
	os_printf("------------------------\r\n");
    uint8 addr=0x55;
    uint8 cmd = 0x28;
    uint8 repeat = 10;
    set_tx_data(addr,cmd,repeat);
    os_printf("==================\r\n");
    os_printf("ir tx..\n\r");
    os_printf("addr:%02xh;cmd:%02xh;repeat:%d;\r\n",addr,cmd,repeat);
    if(IR_TX_IDLE == get_ir_tx_status()){
		os_printf("ir tx start...\r\n");
        ir_tx_handler();
    }else{
        os_printf("ir tx busy...\r\n");
    }
}

void ICACHE_FLASH_ATTR
	user_ir_test_func()
{
    os_printf("SDK version:%s\n", system_get_sdk_version());
    uart_div_modify(0, 80000000 / 115200);

    os_printf("ir tx/rx test \r\n");
	//ir tx code
    ir_tx_init();
	//ir rx code
    ir_rx_init();

	//demo: send IR command every 5 seconds.
    os_timer_disarm(&ir_timer);
    os_timer_setfn(&ir_timer, test_ir_nec_tx , NULL);
    os_timer_arm(&ir_timer,5000,1);

	
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{

	system_init_done_cb(user_ir_test_func);

	
}

