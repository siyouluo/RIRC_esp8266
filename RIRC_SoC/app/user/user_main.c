/**
************************************************************
* @file         user_main.c
* @brief        The program entry file
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
*
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gagent_soc.h"
#include "user_devicefind.h"
#include "user_webserver.h"
#include "gizwits_product.h"
#include "driver/hal_key.h"

#include "driver/hal_rirc.h"

#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#else
#ifdef CLIENT_SSL_ENABLE
unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;
#endif
#endif

/**@} */ 

/**@name Key related definitions 
* @{
*/
#define GPIO_KEY_NUM                            1                           ///< Defines the total number of key members
#define KEY_0_IO_MUX                            PERIPHS_IO_MUX_GPIO4_U      ///< ESP8266 GPIO function
#define KEY_0_IO_NUM                            4                           ///< ESP8266 GPIO number
#define KEY_0_IO_FUNC                           FUNC_GPIO4                  ///< ESP8266 GPIO name
LOCAL key_typedef_t * singleKey[GPIO_KEY_NUM];                              ///< Defines a single key member array pointer
LOCAL keys_typedef_t keys;                                                  ///< Defines the overall key module structure pointer    
/**@} */

/**
* Key key to short press processing
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR keyShortPress(void)
{
    GIZWITS_LOG("#### key short press, soft ap mode \n");

    gizwitsSetMode(WIFI_SOFTAP_MODE);
}

/**
* Key button long press
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR keyLongPress(void)
{
    GIZWITS_LOG("#### key long press, airlink mode\n");
    
    gizwitsSetMode(WIFI_AIRLINK_MODE);
}

/**
* Key to initialize
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR keyInit(void)
{
    singleKey[0] = keyInitOne(KEY_0_IO_NUM, KEY_0_IO_MUX, KEY_0_IO_FUNC,
                                keyLongPress, keyShortPress);
    keys.singleKey = singleKey;
    keyParaInit(&keys);
}

/**
* @brief user_rf_cal_sector_set

* Use the 636 sector (2544k ~ 2548k) in flash to store the RF_CAL parameter
* @param none
* @return none
*/
uint32_t ICACHE_FLASH_ATTR user_rf_cal_sector_set()
{
    return 636;
}

/**
* @brief program entry function

* In the function to complete the user-related initialization
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR user_init(void)
{
    uint32_t system_free_size = 0;

    wifi_station_set_auto_connect(1);
    wifi_set_sleep_type(NONE_SLEEP_T);//set none sleep mode
    espconn_tcp_set_max_con(10);
    uart_init_3(9600,115200);
    UART_SetPrintPort(1);
    GIZWITS_LOG( "---------------SDK version:%s--------------\n", system_get_sdk_version());
    GIZWITS_LOG( "system_get_free_heap_size=%d\n",system_get_free_heap_size());

    struct rst_info *rtc_info = system_get_rst_info();
    GIZWITS_LOG( "reset reason: %x\n", rtc_info->reason);
    if (rtc_info->reason == REASON_WDT_RST ||
        rtc_info->reason == REASON_EXCEPTION_RST ||
        rtc_info->reason == REASON_SOFT_WDT_RST)
    {
        if (rtc_info->reason == REASON_EXCEPTION_RST)
        {
            GIZWITS_LOG("Fatal exception (%d):\n", rtc_info->exccause);
        }
        GIZWITS_LOG( "epc1=0x%08x, epc2=0x%08x, epc3=0x%08x, excvaddr=0x%08x, depc=0x%08x\n",
                rtc_info->epc1, rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr, rtc_info->depc);
    }

    if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1)
    {
        GIZWITS_LOG( "---UPGRADE_FW_BIN1---\n");
    }
    else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2)
    {
        GIZWITS_LOG( "---UPGRADE_FW_BIN2---\n");
    }


    keyInit();

    gizwitsInit();  

    GIZWITS_LOG("--- system_free_size = %d ---\n", system_get_free_heap_size());
}
