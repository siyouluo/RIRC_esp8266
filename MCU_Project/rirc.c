					 #include<reg52.h>
#include<intrins.h>
#define MAIN_Fosc 11059200
sbit IR_LED	= P2^4;			//定义红外发射引脚
bit flag=0;
unsigned char temp=0xfe;
unsigned char my_IR_CodeBuf[68]; //存储红外数据
//模式0~模式4
void change_mode(unsigned char x)
{
	switch(x)
	{
	case 0:my_IR_CodeBuf[1]=0;my_IR_CodeBuf[2]=0;my_IR_CodeBuf[3]=0;break;//自动
	case 1:my_IR_CodeBuf[1]=1;my_IR_CodeBuf[2]=0;my_IR_CodeBuf[3]=0;break;//制冷
	case 2:my_IR_CodeBuf[1]=0;my_IR_CodeBuf[2]=1;my_IR_CodeBuf[3]=0;break;//加湿
	case 3:my_IR_CodeBuf[1]=1;my_IR_CodeBuf[2]=1;my_IR_CodeBuf[3]=0;break;//送风
	case 4:my_IR_CodeBuf[1]=0;my_IR_CodeBuf[2]=0;my_IR_CodeBuf[3]=1;break;//制热
	default: my_IR_CodeBuf[1]=0;my_IR_CodeBuf[2]=0;my_IR_CodeBuf[3]=0;//默认：自动
	}
}
// 1：开机，0：关机
void On_Off(bit x)
{
	my_IR_CodeBuf[4]=x;
}
//风速
void change_speed(unsigned char x)
{
	switch(x)
	{
		case 0:my_IR_CodeBuf[5]=0;my_IR_CodeBuf[6]=0;break;//自动
		case 1:my_IR_CodeBuf[5]=1;my_IR_CodeBuf[6]=0;break;//一级
		case 2:my_IR_CodeBuf[5]=0;my_IR_CodeBuf[6]=1;break;//二级
		case 3:my_IR_CodeBuf[5]=1;my_IR_CodeBuf[6]=1;break;//三级
		default: my_IR_CodeBuf[5]=0;my_IR_CodeBuf[6]=0;//默认：自动
	}
}
//扫风
void swing_flap(bit x)
{
	my_IR_CodeBuf[7]=x;
}
//睡眠，每隔一段时间，自动调整温度
void sleep(bit x)
{
 	my_IR_CodeBuf[8]=x;
}
//调温，16~30度
void temperature(unsigned char t)
{
	my_IR_CodeBuf[9]=(t-16)&1;
	my_IR_CodeBuf[10]=((t-16)>>1)&1;
	my_IR_CodeBuf[11]=((t-16)>>2)&1;
	my_IR_CodeBuf[12]=((t-16)>>3)&1;
}
//定时
void timing(unsigned char t)
{
	my_IR_CodeBuf[13]=(t>>0)&1;
	my_IR_CodeBuf[14]=(t>>1)&1;
	my_IR_CodeBuf[15]=(t>>2)&1;
	my_IR_CodeBuf[16]=(t>>3)&1;

	my_IR_CodeBuf[17]=(t>>4)&1;
	my_IR_CodeBuf[18]=(t>>5)&1;
	my_IR_CodeBuf[19]=(t>>6)&1;
	my_IR_CodeBuf[20]=(t>>7)&1;
}
//加湿
void moistening(bit x)
{
	my_IR_CodeBuf[21]=x;
}
//灯光		   
void light(bit x)
{
	my_IR_CodeBuf[22]=x;
}
//负离子
void anion(bit x)
{
	my_IR_CodeBuf[23]=x;
}
//节电
void dimout(bit x)
{
	my_IR_CodeBuf[24]=x;
}
//换气
void aeration(bit x)
{
	my_IR_CodeBuf[25]=x;
}

void end1()
{
	my_IR_CodeBuf[26]=0;
	my_IR_CodeBuf[27]=0;
	my_IR_CodeBuf[28]=0;
	my_IR_CodeBuf[29]=1;
	my_IR_CodeBuf[30]=0;
	my_IR_CodeBuf[31]=1;
	my_IR_CodeBuf[32]=0;

	my_IR_CodeBuf[33]=0;
	my_IR_CodeBuf[34]=1;
	my_IR_CodeBuf[35]=0;
}



void up_down_swing(bit x)
{
	if(x)	my_IR_CodeBuf[36]=1;
	else	my_IR_CodeBuf[36]=0;
}
void swing_connect1()
{
	my_IR_CodeBuf[37]=0;
	my_IR_CodeBuf[38]=0;
	my_IR_CodeBuf[39]=0;
}
void left_right_swing(bit x)
{
	if(x)	my_IR_CodeBuf[40]=1;
	else	my_IR_CodeBuf[40]=0;
}
void swing_connect2()
{
	my_IR_CodeBuf[41]=0;
	my_IR_CodeBuf[42]=0;
	my_IR_CodeBuf[43]=0;
}
void display_temp(unsigned char x)
{
	switch(x)
	{
		case 0:my_IR_CodeBuf[44]=1;my_IR_CodeBuf[45]=0;break;
		case 1:my_IR_CodeBuf[44]=0;my_IR_CodeBuf[45]=1;break;
		case 2:my_IR_CodeBuf[44]=1;my_IR_CodeBuf[45]=0;break;
		case 3:my_IR_CodeBuf[44]=1;my_IR_CodeBuf[45]=0;break;
		default:	my_IR_CodeBuf[44]=1;my_IR_CodeBuf[45]=0;
	}
}
void connect2()
{
	int i=0;
	for(i=46;i<=61;i++)
		my_IR_CodeBuf[i]=0;
	my_IR_CodeBuf[49]=1;
	my_IR_CodeBuf[63]=0;
}
void energy_conservation(bit x)
{
	my_IR_CodeBuf[62]=x;
}
void checkout()
{
	unsigned char check;
	check=my_IR_CodeBuf[1] + my_IR_CodeBuf[2]*2 + my_IR_CodeBuf[3]*4+my_IR_CodeBuf[4]*8;
	check+=my_IR_CodeBuf[9] + my_IR_CodeBuf[10]*2 + my_IR_CodeBuf[11]*4 + my_IR_CodeBuf[12]*8;
	check+=12;
	check+=my_IR_CodeBuf[40];
	check+=my_IR_CodeBuf[25];
	check+=my_IR_CodeBuf[62];
	check&=0x0f;

	my_IR_CodeBuf[64]=check&1;
	my_IR_CodeBuf[65]=(check>>1)&1;
	my_IR_CodeBuf[66]=(check>>2)&1;
	my_IR_CodeBuf[67]=(check>>3)&1;
}
void IR_Data_Init()
{
	change_mode(1);//
	
	On_Off(1);//默认开关：关
	
	change_speed(0);//默认风速：自动
	swing_flap(1);//默认扫风
	sleep(0);//默认睡眠
	
	temperature(23);//默认温度：23度


	timing(0);//默认定时：不定时
	moistening(0);//
	light(1);//
	anion(0);//
	dimout(0);//
	aeration(0);//
	end1();
	up_down_swing(1);
	swing_connect1();
	left_right_swing(1);
	swing_connect2();
	display_temp(1);//
	connect2();
	energy_conservation(0);//
	checkout();
}
//更改红外数据时仅更改制冷或制热模式，以及温度，其余保持IR_Data_Init()函数中默认值不变
//mode：制冷（1），制热（0）
//t：	26~30
void IR_Data_Change(bit Turn_On_off, bit mode, unsigned char t)
{
	On_Off(Turn_On_off);
	if(mode)	change_mode(1);
	else		change_mode(4);
	temperature(t);
	checkout();
}
void Delay_ms(unsigned int ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 96000;
		  while(--i)	;   //96T per loop
     }while(--ms);
}

void IR_TX_RX_Init()
{
	TMOD = 0x22;// 0000(timer1) 0010(timer0) 
	TH0	= 0xf4;
	TL0	= 0xf4;//38KHz 
	EA	= 1;// Enable All Interrupt
	
	ET0	= 1;// Enable Timer0	
	IR_LED = 1;	// stop emit
}
void UART_init()
{
	TMOD = 0x22;  	//T1工作模式2  8位自动重装
	TH1 = 0xfd;
	TL1 = 0xfd; 	//比特率9600
	TR1 = 1;		//启动T1定时器
	SM0 = 0;
	SM1 = 1; 		//串口工作方式1 10位异步
	REN = 1;		//串口允许接收
	EA  = 1;		//开总中断
	ES  = 1;		//串口中断打开
}
void Timer1_Delay(unsigned int us,bit mod)
{

	TR0 = mod;//timer0 run
	while(us--);
	TR0 = 0;//timer0 stop run
	IR_LED = 1;// stop emit
}


void Transmit_TR_CODE()
{
	unsigned char num;
	Timer1_Delay(390, 1);//emit 9ms
	IR_LED = 1;// stop emit
	Timer1_Delay(450, 0);//delay 4.5ms
	for(num = 1; num < 36; num++)
	{
		Timer1_Delay(25, 1);//emit 0.56ms
		IR_LED = 1;// stop emit
		if (my_IR_CodeBuf[num])
			Timer1_Delay(160, 0);//delay 1.685ms
		else
			Timer1_Delay(55, 0);//delay 0.56ms		
	}
	Timer1_Delay(25, 1);//emit 600us
	IR_LED = 1;// stop emit
	Timer1_Delay(2001, 0);//delay 20000us
	for(num = 36; num < 68; num++)
	{	
		Timer1_Delay(25, 1);//emit 0.56ms
		IR_LED = 1;// stop emit
		if (my_IR_CodeBuf[num])
			Timer1_Delay(160, 0);//delay 1.685ms
		else
			Timer1_Delay(55, 0);//delay 0.56ms		
	}
	Timer1_Delay(25, 1);//emit 600us
	IR_LED = 1;// stop emit
}

void Timer0() interrupt 1 //1 mean timer0
{
	IR_LED = ~IR_LED;
}
void UART() interrupt 4
{
	if(RI)	//检测是否接收完成
	{
	
		P1 = SBUF;
		flag=1;
		temp=SBUF;
		RI = 0;
	}
}
void main()
{
	 
	 	 
	 while(1)
	 {
	 	
		IR_Data_Init();
		IR_TX_RX_Init();//IR发送、接收初始化
		UART_init();
		
		if(flag)
		{
			flag=0;
			 IR_Data_Change((temp>>6)&0x01,(temp>>5)&0x01,temp&0x1f);
		
			Transmit_TR_CODE();
	
		} 
		
		
		}	 	
}


