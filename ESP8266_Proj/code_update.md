# 代码修改
本文档主要解释自动生成的代码包需要如何修改  
直接从机智云开发者中心下载的自动生成代码包位于[./code_package/GizwitsSoCESP8266_32M20190814124049489208a7c6.zip](./code_package/)  
经过修改之后的代码位于[./code_package/SoC_ESP8266_32M_source.zip](./code_package/)  


# 点亮RGB LED  
在 app/driver/ 文件夹下添加C语言源文件，在 app/include/driver/ 文件夹下添加头文件，在 app/user/user_main.c 中初始化，在 app/Gizwits/gizwits_product.c 文件中添加业务逻辑  
参考[小黑板ESP8266无线wifi SoC方案连接机智云最详细图文教程](http://club.gizwits.com/forum.php?mod=viewthread&tid=5070&highlight=%E5%B0%8F%E9%BB%91%E6%9D%BF)  
## 添加驱动头文件
```diff
+++ app/include/driver/hal_led.h

#ifndef _HAL_RGB_LED_H
#define _HAL_RGB_LED_H

#include <stdio.h>
#include <c_types.h>
#include <gpio.h>
#include <eagle_soc.h>

/* Define your drive pin, when the io is low, the led will be lighted! */
#define GPIO_RGB_R	15
#define GPIO_RGB_G	12
#define GPIO_RGB_B	13

/* Set GPIO Direction */
#define RedOn()			GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_R), 1)
#define RedOff()		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_R), 0)

#define GreenOn()		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_G), 1)
#define GreenOff()		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_G), 0)

#define BlueOn()		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_B), 1)
#define BlueOff()		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_B), 0)

/* Function declaration */
void rgbGpioInit(void);

#endif

```

## 添加驱动源文件

```diff
+++ app/driver/hal_led.c

/***********************************************
 * @file	hal_led.c
 * @author	Luo Siyou
 * @version	V1.0
 * @date	2019-07-26
 *
 * @brief	driver a gpio pin to simulate infrared protocol in order to control the airconditioner
 *
***********************************************/

#include "driver/hal_led.h"
#include "osapi.h"

void ICACHE_FLASH_ATTR rgbGpioInit(void)
{
	/* Migrate your driver code */
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);

	gpio_output_set(0, 0, GPIO_ID_PIN(GPIO_RGB_R) |  GPIO_ID_PIN(GPIO_RGB_G) |  GPIO_ID_PIN(GPIO_RGB_B), 0);
	os_printf("rgbGpioInit \r\n");
	
	RedOff();
	GreenOff();
	BlueOff();
}

```

 
## 修改主程序

```diff  
--- app/user/user_main.c
+++ app/user/user_main.c 

#include "driver/hal_key.h"
+#include "driver/hal_led.h"


/**@name Key related definitions 
* @{
*/
-#define GPIO_KEY_NUM                            2                           ///< Defines the total number of key members
+#define GPIO_KEY_NUM                            1                           ///< Defines the total number of key members
-#define KEY_0_IO_MUX                            PERIPHS_IO_MUX_GPIO0_U      ///< ESP8266 GPIO function
+#define KEY_0_IO_MUX                            PERIPHS_IO_MUX_GPIO4_U      ///< ESP8266 GPIO function
-#define KEY_0_IO_NUM                            0                           ///< ESP8266 GPIO number
+#define KEY_0_IO_NUM                            4                           ///< ESP8266 GPIO number
-#define KEY_0_IO_FUNC                           FUNC_GPIO0                  ///< ESP8266 GPIO name
+#define KEY_0_IO_FUNC                           FUNC_GPIO4                  ///< ESP8266 GPIO name
-#define KEY_1_IO_MUX                            PERIPHS_IO_MUX_MTMS_U       ///< ESP8266 GPIO function
-#define KEY_1_IO_NUM                            14                          ///< ESP8266 GPIO number
-#define KEY_1_IO_FUNC                           FUNC_GPIO14                 ///< ESP8266 GPIO name
LOCAL key_typedef_t * singleKey[GPIO_KEY_NUM];                              ///< Defines a single key member array pointer
LOCAL keys_typedef_t keys;                                                  ///< Defines the overall key module structure pointer    


-/**
-* Key1 key short press processing
-* @param none
-* @return none
-*/
-LOCAL void ICACHE_FLASH_ATTR key1ShortPress(void)
-{
-    GIZWITS_LOG("#### KEY1 short press ,Production Mode\n");
-    
-    gizwitsSetMode(WIFI_PRODUCTION_TEST);
-}
-
-/**
-* Key1 key presses a long press
-* @param none
-* @return none
-*/
-LOCAL void ICACHE_FLASH_ATTR key1LongPress(void)
-{
-    GIZWITS_LOG("#### key1 long press, default setup\n");
-    
-    gizwitsSetMode(WIFI_RESET_MODE);
-}
-

/**
-* Key2 key to short press processing
+* key button short press processing
* @param none
* @return none
*/
-LOCAL void ICACHE_FLASH_ATTR key2ShortPress(void)
+LOCAL void ICACHE_FLASH_ATTR keyShortPress(void)
{
-    GIZWITS_LOG("#### key2 short press, soft ap mode \n");
+    GIZWITS_LOG("#### key short press, soft ap mode \n");

    gizwitsSetMode(WIFI_SOFTAP_MODE);
}

/**
-* Key2 button long press
+* Key button long press
* @param none
* @return none
*/
-LOCAL void ICACHE_FLASH_ATTR key2LongPress(void)
+LOCAL void ICACHE_FLASH_ATTR keyLongPress(void)
{
-    GIZWITS_LOG("#### key2 long press, airlink mode\n");
+    GIZWITS_LOG("#### key long press, airlink mode\n");    

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
-                                key1LongPress, key1ShortPress);
+                                keyLongPress, keyShortPress);

-    singleKey[1] = keyInitOne(KEY_1_IO_NUM, KEY_1_IO_MUX, KEY_1_IO_FUNC,
-                                key2LongPress, key2ShortPress);
    keys.singleKey = singleKey;
    keyParaInit(&keys);
}

...

+   //rgb led Init
+   rgbGpioInit();
    keyInit();

    gizwitsInit();  

    GIZWITS_LOG("--- system_free_size = %d ---\n", system_get_free_heap_size());
}

```

## 填充业务逻辑
```diff
--- app/Gizwits/gizwits_product.c
+++ app/Gizwits/gizwits_product.c

#include "driver/hal_key.h"
+#include "driver/hal_led.h"

        switch(info->event[i])
        {
        case EVENT_On_Off :
            currentDataPoint.valueOn_Off = dataPointPtr->valueOn_Off;
            GIZWITS_LOG("Evt: EVENT_On_Off %d \n", currentDataPoint.valueOn_Off);
            if(0x01 == currentDataPoint.valueOn_Off)
            {
                //user handle
+		RedOn();
           }
            else
            {
                //user handle
+		RedOff();
           }
            break;
        case EVENT_Warm_Cold :
            currentDataPoint.valueWarm_Cold = dataPointPtr->valueWarm_Cold;
            GIZWITS_LOG("Evt: EVENT_Warm_Cold %d \n", currentDataPoint.valueWarm_Cold);
            if(0x01 == currentDataPoint.valueWarm_Cold)
            {
                //user handle
+		GreenOn();
            }
            else
            {
                //user handle
+		GreenOff();
            }
            break;


        case EVENT_Temperature:
            currentDataPoint.valueTemperature= dataPointPtr->valueTemperature;
            GIZWITS_LOG("Evt:EVENT_Temperature %d\n",currentDataPoint.valueTemperature);
            //user handle
+            if(currentDataPoint.valueTemperature>25)
+            {
+                BlueOn();
+            }
+            else
+            {
+                BlueOff();
+            }
            break;

...
            
        default:
            break;
        }


```


# 模拟NEC协议
ESP8266的管脚上只需要产生基带信号，然后接到555定时器的RST管脚进行调制，555定时器输出已调信号，因为RST无效时，555定时器输出恒定低电平，所以当三极管基极为低电平时需要截止，所以三极管选用NPN管，基本共射放大接法。

接下来我直接列出需要修改或添加的文件，你可以直接使用我的代码文件：  

将以下文件添加到指定路径：  
app/include/driver/hal_led.h  
app/driver/hal_led.c  
app/include/driver/hal_ir.h  
app/driver/hal_ir.c  

将以下文件用我的对应文件进行替换  
app/user/user_main.c  
app/Gizwits/gizwits_product.c  