/**
************************************************************
* @file         gizwits_product.c
* @brief        Control protocol processing, and platform-related hardware initialization
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
#include <stdio.h>
#include <string.h>
#include "gizwits_product.h"
#include "driver/hal_key.h"

#include "driver/ir_remote_GREE.h"

extern bool run_coding[134];

extern gizwitsProtocol_t gizwitsProtocol;

/** User area The current device state structure */
dataPoint_t currentDataPoint;	//机智云当前下传的数据点
dataPoint_t runDataPoint;		//设备当前运行的数据点



/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t ICACHE_FLASH_ATTR gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
    uint8_t i = 0;
    dataPoint_t * dataPointPtr = (dataPoint_t *)data;
    moduleStatusInfo_t * wifiData = (moduleStatusInfo_t *)data;

    if((NULL == info) || (NULL == data))
    {
        GIZWITS_LOG("!!! gizwitsEventProcess Error \n");
        return -1;
    }

    for(i = 0; i < info->num; i++)
    {
        switch(info->event[i])
        {
        case EVENT_onoff :
            currentDataPoint.valueonoff = dataPointPtr->valueonoff;
            GIZWITS_LOG("Evt: EVENT_onoff %d \n", currentDataPoint.valueonoff);

            break;
        
        case EVENT_mode:
            currentDataPoint.valuemode = dataPointPtr->valuemode;
            GIZWITS_LOG("Evt: EVENT_mode %d\n", currentDataPoint.valuemode);

            break;
        case EVENT_wind_speed:
            currentDataPoint.valuewind_speed = dataPointPtr->valuewind_speed;
            GIZWITS_LOG("Evt: EVENT_wind_speed %d\n", currentDataPoint.valuewind_speed);

            break;
        case EVENT_up_down_scan:
            currentDataPoint.valueup_down_scan = dataPointPtr->valueup_down_scan;
            GIZWITS_LOG("Evt: EVENT_up_down_scan %d\n", currentDataPoint.valueup_down_scan);

            break;
        case EVENT_left_right_scan:
            currentDataPoint.valueleft_right_scan = dataPointPtr->valueleft_right_scan;
            GIZWITS_LOG("Evt: EVENT_left_right_scan %d\n", currentDataPoint.valueleft_right_scan);

            break;
        case EVENT_temp:
            currentDataPoint.valuetemp= dataPointPtr->valuetemp;
            GIZWITS_LOG("Evt:EVENT_temp %d\n",currentDataPoint.valuetemp);
            //user handle
            break;


        case WIFI_SOFTAP:
            break;
        case WIFI_AIRLINK:
            break;
        case WIFI_STATION:
            break;
        case WIFI_CON_ROUTER:
            GIZWITS_LOG("@@@@ connected router\n");
 
            break;
        case WIFI_DISCON_ROUTER:
            GIZWITS_LOG("@@@@ disconnected router\n");
 
            break;
        case WIFI_CON_M2M:
            GIZWITS_LOG("@@@@ connected m2m\n");
			setConnectM2MStatus(0x01);
 
            break;
        case WIFI_DISCON_M2M:
            GIZWITS_LOG("@@@@ disconnected m2m\n");
			setConnectM2MStatus(0x00);
 
            break;
        case WIFI_RSSI:
            GIZWITS_LOG("@@@@ RSSI %d\n", wifiData->rssi);
            break;
        case TRANSPARENT_DATA:
            GIZWITS_LOG("TRANSPARENT_DATA \n");
            //user handle , Fetch data from [data] , size is [len]
            break;
        case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
            break;
            
        default:
            break;
        }
    }

    gizMemcpy((uint8_t *)&gizwitsProtocol.gizLastDataPoint, (uint8_t *)&currentDataPoint, sizeof(currentDataPoint));

	//控制部分：如果云下传数据与运行数据不同才发送给空调，没有则空调每10分钟会响一次
	if(0 != memcmp(&currentDataPoint, &runDataPoint, sizeof(currentDataPoint)))
	{
		runDataPoint = currentDataPoint;	//把云下传数据点数据传给运行数据点
		dataPoint_convert_coding();
		IR_send_YBOF(run_coding);
	}


    system_os_post(USER_TASK_PRIO_2, SIG_UPGRADE_DATA, 0);
    
    return 0; 
}


/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void ICACHE_FLASH_ATTR userHandle(void)
{
    /*

    */

	system_os_post(USER_TASK_PRIO_2, SIG_UPGRADE_DATA, 0);
	
}


/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void ICACHE_FLASH_ATTR userInit(void)
{
    gizMemset((uint8_t *)&currentDataPoint, 0, sizeof(dataPoint_t));

 	/** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
		currentDataPoint.valueonoff = 0;
		currentDataPoint.valuemode = 1;
		currentDataPoint.valuewind_speed = 0;
		currentDataPoint.valueup_down_scan = 0;
		currentDataPoint.valueleft_right_scan = 0;
		currentDataPoint.valuetemp = 25;
		runDataPoint = currentDataPoint;
		
  	    IR_remote_init();
		dataPoint_convert_coding();
		IR_send_YBOF(run_coding);
}


