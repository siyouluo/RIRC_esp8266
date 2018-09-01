#include "driver/ir_remote_GREE.h"

#define GREE_HDR_MARK			9000
#define GREE_HDR_SPACE			4500
#define GREE_BIT_MARK			579
#define GREE_ZERO_SPACE			579		//0
#define GREE_ONE_SPACE			1737	//1
#define GREE_LINK_1_SPACE		20000	//连接码1
#define GREE_LINK_2_SPACE		40000	//连接码2

/**@红外接收输入脚相关定义 */
#define IR_receive_IO_MUX        PERIPHS_IO_MUX_MTCK_U      	///< ESP8266 GPIO 功能
#define IR_receive_IO_NUM        13                           	///< ESP8266 GPIO 编号
#define IR_receive_IO_FUNC       FUNC_GPIO13                  	///< ESP8266 GPIO 名称

/**@红外接收输出脚相关定义 */
#define IR_send_IO_MUX        	PERIPHS_IO_MUX_MTDI_U    	  	///< ESP8266 GPIO 功能
#define IR_send_IO_NUM        	12                           	///< ESP8266 GPIO 编号
#define IR_send_IO_FUNC       	FUNC_GPIO12                		///< ESP8266 GPIO 名称

/** 格力空调编码数据YBOF*/
bool run_coding[134]={
	1,0,0,0,0,0,1,0,	//0-7
	1,0,0,1,0,0,0,0,	//8-15
	0,0,0,0,0,0,0,0,	//16-23
	0,0,0,0,1,0,1,0,	//24-31
	0,1,0,				//32-34
	0,0,0,0,0,0,0,0,	//35-42
	0,0,0,0,0,0,1,0,	//43-50
	0,0,0,0,0,0,0,0,	//51-58
	0,0,0,0,1,0,0,1,	//59-66

	0,0,0,0,0,0,1,0,	//0-7
	0,0,0,0,0,0,0,0,	//8-15
	0,0,0,0,0,1,0,0,	//16-23
	0,0,0,0,1,0,1,0,	//24-31
	0,1,0,				//32-34
	0,0,0,0,0,0,0,0,	//35-42
	1,0,0,0,0,1,0,0,	//43-50
	0,0,0,0,0,0,0,0,	//51-58
	0,0,0,0,0,0,0,0,	//59-66
};

//解码判断用的时间
uint32 ZERO_SPACE = 800;		//编码0高电平时间，us
uint32 ONE_SPACE = 2000;		//编码1高电平时间，us
uint32 LINK_2_SPACE = 42000;	//连接码2高电平时间，us

extern dataPoint_t currentDataPoint;
extern dataPoint_t runDataPoint;	


/**
* @空调编码转上传数据处理函数
* @param run_coding[]
* @return currentDataPoint
*/
void ICACHE_FLASH_ATTR coding_convert_dataPoint(void)
{
	//开关
	runDataPoint.valueonoff = run_coding[3];
	//模式
	runDataPoint.valuemode = run_coding[2]*4 + run_coding[1]*2 + run_coding[0];
	//风速
	runDataPoint.valuewind_speed = run_coding[5]*2 + run_coding[4];
	//上下扫风
	uint8_t up_down_scan_value = run_coding[38]*8 +run_coding[37]*4 + run_coding[36]*2 + run_coding[35];
	switch(up_down_scan_value)
	{
	  case 0://不扫风
		runDataPoint.valueup_down_scan = 0;
		break;
	  case 1://上下扫风
		runDataPoint.valueup_down_scan = 1;
		break;
	  case 11://上扫风
		runDataPoint.valueup_down_scan = 2;
		break;
	  case 9://中扫风
		runDataPoint.valueup_down_scan = 3;
		break;
	  case 7://下扫风
		runDataPoint.valueup_down_scan = 4;
		break;
	  case 2://上吹风
		runDataPoint.valueup_down_scan = 5;
		break;
	  case 3://上中吹风
		runDataPoint.valueup_down_scan = 6;
		break;
	  case 4://中吹风
		runDataPoint.valueup_down_scan = 7;
		break;
	  case 5://中下吹风
		runDataPoint.valueup_down_scan = 8;
		break;
	  case 6://下吹风
		runDataPoint.valueup_down_scan = 9;
		break;
	  default:
		break;
	}	
	//左右扫风
	runDataPoint.valueleft_right_scan = run_coding[42]*8 + run_coding[41]*4 + run_coding[40]*2 + run_coding[39];
	//温度
	runDataPoint.valuetemp = run_coding[11]*8 + run_coding[10]*4 + run_coding[9]*2 + run_coding[8] + 16;

	currentDataPoint = runDataPoint;	//运行数据传给上报数据
}

/**
* @运行数据转换成格力空调编码函数
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR dataPoint_convert_coding(void)
{
	if(0 == runDataPoint.valuemode)
	{
		runDataPoint.valuetemp = 25;	//自动时，温度设为25度
	}
	if(2 == runDataPoint.valuemode)
	{
		runDataPoint.valuewind_speed = 1;	//除湿时，风速只能为低速
	}	
	
	//开关
	run_coding[3] = runDataPoint.valueonoff;
	
	//模式
	run_coding[0] = runDataPoint.valuemode & 0x01;
	run_coding[1] = runDataPoint.valuemode >> 1 & 0x01;
	run_coding[2] = runDataPoint.valuemode >> 2 & 0x01;
	
	//风速
	run_coding[4] = runDataPoint.valuewind_speed & 0x01;
	run_coding[5] = runDataPoint.valuewind_speed >> 1 & 0x01;
	
	//上下扫风
	switch(runDataPoint.valueup_down_scan)
	{
	  case 0://不扫风
		run_coding[35] = 0;
		run_coding[36] = 0;
		run_coding[37] = 0;
		run_coding[38] = 0;
		break;
	  case 1://上下扫风
		run_coding[35] = 1;
		run_coding[36] = 0;
		run_coding[37] = 0;
		run_coding[38] = 0;
		break;
	  case 2://上扫风
		run_coding[35] = 1;
		run_coding[36] = 1;
		run_coding[37] = 0;
		run_coding[38] = 1;
		break;
	  case 3://中扫风
		run_coding[35] = 1;
		run_coding[36] = 0;
		run_coding[37] = 0;
		run_coding[38] = 1;
		break;
	  case 4://下扫风
		run_coding[35] = 1;
		run_coding[36] = 1;
		run_coding[37] = 1;
		run_coding[38] = 0;
		break;
	  case 5://上吹风
		run_coding[35] = 0;
		run_coding[36] = 1;
		run_coding[37] = 0;
		run_coding[38] = 0;
		break;
	  case 6://上中吹风
		run_coding[35] = 1;
		run_coding[36] = 1;
		run_coding[37] = 0;
		run_coding[38] = 0;
		break;
	  case 7://中吹风
		run_coding[35] = 0;
		run_coding[36] = 0;
		run_coding[37] = 1;
		run_coding[38] = 0;
		break;
	  case 8://中下吹风
		run_coding[35] = 1;
		run_coding[36] = 0;
		run_coding[37] = 1;
		run_coding[38] = 0;
		break;
	  case 9://下吹风
		run_coding[35] = 0;
		run_coding[36] = 1;
		run_coding[37] = 1;
		run_coding[38] = 0;
		break;
	  default:
		break;
	}
	
	//左右扫风
	switch(runDataPoint.valueleft_right_scan)
	{
	  case 0://不扫风
		run_coding[39] = 0;
		run_coding[40] = 0;
		run_coding[41] = 0;
		run_coding[42] = 0;
		break;
	  case 1://左右扫风
		run_coding[39] = 1;
		run_coding[40] = 0;
		run_coding[41] = 0;
		run_coding[42] = 0;
		break;
	  case 2://左吹风
		run_coding[39] = 0;
		run_coding[40] = 1;
		run_coding[41] = 0;
		run_coding[42] = 0;
		break;
	  case 3://左中吹风
		run_coding[39] = 1;
		run_coding[40] = 1;
		run_coding[41] = 0;
		run_coding[42] = 0;
		break;
	  case 4://中吹风
		run_coding[39] = 0;
		run_coding[40] = 0;
		run_coding[41] = 1;
		run_coding[42] = 0;
		break;
	  case 5://中右吹风
		run_coding[39] = 1;
		run_coding[40] = 0;
		run_coding[41] = 1;
		run_coding[42] = 0;
		break;
	  case 6://右吹风
		run_coding[39] = 0;
		run_coding[40] = 1;
		run_coding[41] = 1;
		run_coding[42] = 0;
		break;
	  default:
		break;
	}
	if(1 == runDataPoint.valueup_down_scan || 2 == runDataPoint.valueup_down_scan || 3 == runDataPoint.valueup_down_scan 
				|| 4 == runDataPoint.valueup_down_scan || 1 == runDataPoint.valueleft_right_scan)
	{
		run_coding[6] = 1;		//扫风
	}
	else
	{
		run_coding[6] = 0;
	}
	
	//温度
	run_coding[8] = (runDataPoint.valuetemp-16) & 0x01;
	run_coding[9] = (runDataPoint.valuetemp-16) >> 1 & 0x01;
	run_coding[10] = (runDataPoint.valuetemp-16) >> 2 & 0x01;
	run_coding[11] = (runDataPoint.valuetemp-16) >> 3 & 0x01;
	
	//定时(清除定时数据)
	run_coding[12] = 0;	//1:0.5小时
	run_coding[13] = 0;	//1:10小时
	run_coding[14] = 0;	//1:20小时
	run_coding[15] = 0;	//1:定时
	run_coding[16] = 0;	//16-19:0-9小时
	run_coding[17] = 0;
	run_coding[18] = 0;
	run_coding[19] = 0;
	
	//YBOF 验证码1 = 模式+(温度-16)+定时个位+左右扫风+换气+12-开关*8
	//[24]=换气，[39]=左右扫风，不使用空调主机定时器
	int c1 = runDataPoint.valuemode + ( runDataPoint.valuetemp - 16) + run_coding[39]
				+ run_coding[24] + 12  - runDataPoint.valueonoff * 8;
	run_coding[63] = c1 & 0x01;
	run_coding[64] = c1 >> 1 & 0x01;
	run_coding[65] = c1 >> 2 & 0x01;
	run_coding[66] = c1 >> 3 & 0x01;

}


/*格力空调遥控器:YBOF */
void ICACHE_FLASH_ATTR IR_send_YBOF(bool GREE_buf[])
{
	ETS_GPIO_INTR_DISABLE();  //关闭中断
	uint8_t i;
	
	//起始码
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
	os_delay_us(GREE_HDR_MARK);
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
	os_delay_us(GREE_HDR_SPACE);
	
	for (i = 0; i < 35; i++)
	{
		if (1 == GREE_buf[i])	// 1
		{
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
			os_delay_us(GREE_BIT_MARK);
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
			os_delay_us(GREE_ONE_SPACE);
		}
		else if (0 == GREE_buf[i])	// 0
		{
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
			os_delay_us(GREE_BIT_MARK);
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
			os_delay_us(GREE_ZERO_SPACE);
		}
	}

	//连接码1
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
	os_delay_us(GREE_BIT_MARK);
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
	os_delay_us(GREE_LINK_1_SPACE);
	
	for (i = 35; i < 67; i++)
	{
		if (1 == GREE_buf[i])	// 1
		{
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
			os_delay_us(GREE_BIT_MARK);
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
			os_delay_us(GREE_ONE_SPACE);
		}
		else if (0 == GREE_buf[i])	// 0
		{
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
			os_delay_us(GREE_BIT_MARK);
			GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
			os_delay_us(GREE_ZERO_SPACE);
		}
	}
	
	//结束码
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 0);
	os_delay_us(GREE_BIT_MARK);
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
	
	os_printf("<---GREE YBOF--->\n");
	os_printf("ONOFF = %d \n",currentDataPoint.valueonoff);
	os_printf(" MODE = %d \n",currentDataPoint.valuemode);
	os_printf(" TEMP = %d \n",currentDataPoint.valuetemp);
	os_printf("SPEED = %d \n",currentDataPoint.valuewind_speed);
	os_printf("UP_DO = %d \n",currentDataPoint.valueup_down_scan);
	os_printf("  L_R = %d \n",currentDataPoint.valueleft_right_scan);

	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(IR_receive_IO_NUM));//清除中断
	ETS_GPIO_INTR_ENABLE();//打开中断

}


/**
* @brief 红外接收中断处理回调函数
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR IR_receive_callback(void)
{
	uint32 pin_status = GPIO_REG_READ( GPIO_STATUS_ADDRESS );	//读取GPIO中断状态
    ETS_GPIO_INTR_DISABLE();									//关闭GPIO中断
	
	uint32 int_i = 0;
	uint32 int_j;
	bool IR_level_data[134] = {};	//编码暂存
	
	/** 检测是否是红外接收输入引脚中断 */
    if ( pin_status & BIT( IR_receive_IO_NUM ) )
    {
		uint32 time_t0 = 0;		//时长，us
		uint32 time_t1 = system_get_time();	//系统时间1，us
		uint32 time_t2 = 0;		//系统时间2，us

		bool IR_level_0 = 0;	//初始电平为 0
		
		//解码
		for(; ;) 
		{
			time_t2 = system_get_time();
			time_t0 = time_t2 - time_t1;
			
			bool IR_level = GPIO_INPUT_GET(GPIO_ID_PIN(IR_receive_IO_NUM));	//获取引脚电平
			GPIO_OUTPUT_SET(IR_send_IO_NUM, IR_level);	//输出
			
			if(time_t0 > LINK_2_SPACE)	break;				
			
			if(IR_level_0 != IR_level)
			{
				time_t1 = time_t2;
				
				if(0 == IR_level)
				{
					if(time_t0 <= ZERO_SPACE)
					{
						IR_level_data[int_i] = 0;		//数据：0
						int_i++;
					}
					else if(time_t0 <= ONE_SPACE)
					{
						IR_level_data[int_i] = 1;		//数据：1
						int_i++;
					}
				}
			}
			IR_level_0 = IR_level;
		}
    }
	
	//YBOF验证码1=模式+(温度-16)+定时个位+左右扫风+换气+12-开关*8
	int verification_code_1_value = (IR_level_data[0]+IR_level_data[1]*2+IR_level_data[2]*4)
									+ (IR_level_data[8]+IR_level_data[9]*2+IR_level_data[10]*4+IR_level_data[11]*8)
									+ (IR_level_data[16]+IR_level_data[17]*2+IR_level_data[18]*4+IR_level_data[19]*8)
									+ IR_level_data[39]+IR_level_data[40]*2+IR_level_data[41]*4+IR_level_data[42]*8
									+ IR_level_data[24] + 12 - IR_level_data[3] * 8;
						
	int verification_code_1_A = verification_code_1_value & 0x0F; 
	int verification_code_1_B = IR_level_data[63] + IR_level_data[64]*2	+ IR_level_data[65]*4 + IR_level_data[66]*8;
	
	if((67 == int_i || 134 == int_i) && verification_code_1_A == verification_code_1_B) 
	{
		for(int_j=0; int_j<int_i; int_j++)
		{
			run_coding[int_j] = IR_level_data[int_j];	//转存编码
		}
		
		coding_convert_dataPoint();	//把编码转成数据点数据上报
		os_printf("ONOFF = %d \n",currentDataPoint.valueonoff);
		os_printf(" MODE = %d \n",currentDataPoint.valuemode);
		os_printf(" TEMP = %d \n",currentDataPoint.valuetemp);
		os_printf("SPEED = %d \n",currentDataPoint.valuewind_speed);
		os_printf("UP_DO = %d \n",currentDataPoint.valueup_down_scan);
		os_printf("  L_R = %d \n",currentDataPoint.valueleft_right_scan);

	}

	/*可打印空调编码数据
	os_printf("####\n");
	for(int_j = 0; int_j < int_i; int_j++)
	{
		os_printf("%d\n",IR_level_data[int_j]);
	}
	os_printf("####\n");
	*/

	GPIO_REG_WRITE( GPIO_STATUS_W1TC_ADDRESS, BIT(IR_receive_IO_NUM) );		//清除GPIO中断标志
    ETS_GPIO_INTR_ENABLE();		//开启GPIO中断
}

void ICACHE_FLASH_ATTR IR_remote_init(void)
{
    //发送
	PIN_FUNC_SELECT(IR_send_IO_MUX, IR_send_IO_FUNC);
	GPIO_OUTPUT_SET(IR_send_IO_NUM, 1);
	
	//接收
	PIN_FUNC_SELECT(IR_receive_IO_MUX,IR_receive_IO_FUNC);								//pin脚选择
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(IR_receive_IO_NUM)); 									//设置成输入
    PIN_PULLUP_EN(IR_receive_IO_MUX);													//引脚启动上拉电阻
    ETS_GPIO_INTR_DISABLE();															//关闭GPIO中断
	gpio_pin_intr_state_set(GPIO_ID_PIN(IR_receive_IO_NUM),GPIO_PIN_INTR_NEGEDGE);		//设置中断触发方式:下降沿
	ETS_GPIO_INTR_ATTACH(IR_receive_callback,NULL);										//设置中断回调函数
    GPIO_REG_WRITE( GPIO_STATUS_W1TC_ADDRESS, BIT(IR_receive_IO_NUM) );					//清除该引脚的GPIO中断标志
	ETS_GPIO_INTR_ENABLE();																//开启GPIO中断

	os_printf("IR_remote_init ....ok \n");
}
