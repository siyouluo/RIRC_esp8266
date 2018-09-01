/*
这是最初用51单片机实现的红外遥控，使用了两个定时器，
Timer1用于控制高低电平的时间长度，Timer0用于添加38KHz载波
后来因为串口通信还需要一个定时器，就改用空循环来做延时函数
但如果不需要使用串口，本代码可以产生一个正确的红外遥控波形 
*/ 
#include<reg52.h>
#include<intrins.h>
#define MAIN_Fosc 11059200	//主时钟频率
sbit IR_LED	= P2^4;			//定义红外发射引脚
unsigned char IR_CodeBuf[68]; //存储红外数据:IR_CodeBuf[1]~	IR_CodeBuf[67]
//模式0~模式4
void change_mode(unsigned char x)
{
	switch(x)
	{
	case 0:IR_CodeBuf[1]=0;IR_CodeBuf[2]=0;IR_CodeBuf[3]=0;break;//自动
	case 1:IR_CodeBuf[1]=1;IR_CodeBuf[2]=0;IR_CodeBuf[3]=0;break;//制冷
	case 2:IR_CodeBuf[1]=0;IR_CodeBuf[2]=1;IR_CodeBuf[3]=0;break;//加湿
	case 3:IR_CodeBuf[1]=1;IR_CodeBuf[2]=1;IR_CodeBuf[3]=0;break;//送风
	case 4:IR_CodeBuf[1]=0;IR_CodeBuf[2]=0;IR_CodeBuf[3]=1;break;//制热
	default: IR_CodeBuf[1]=0;IR_CodeBuf[2]=0;IR_CodeBuf[3]=0;//默认：自动
	}
}
// 1：开机，0：关机
void On_Off(bit x)
{
	IR_CodeBuf[4]=x;
}
//风速
void change_speed(unsigned char x)
{
	switch(x)
	{
		case 0:IR_CodeBuf[5]=0;IR_CodeBuf[6]=0;break;//自动
		case 1:IR_CodeBuf[5]=1;IR_CodeBuf[6]=0;break;//一级
		case 2:IR_CodeBuf[5]=0;IR_CodeBuf[6]=1;break;//二级
		case 3:IR_CodeBuf[5]=1;IR_CodeBuf[6]=1;break;//三级
		default: IR_CodeBuf[5]=0;IR_CodeBuf[6]=0;//默认：自动
	}
}
//扫风
void swing_flap(bit x)
{
	IR_CodeBuf[7]=x;
}
//睡眠，每隔一段时间，自动调整温度
void sleep(bit x)
{
 	IR_CodeBuf[8]=x;
}

//调温，16~30度
//将(t-16)转换成2进制，IR_CodeBuf[9]为LSB，IR_CodeBuf[12]为MSB
void temperature(unsigned char t)
{
	IR_CodeBuf[9]=(t-16)&1;
	IR_CodeBuf[10]=((t-16)>>1)&1;
	IR_CodeBuf[11]=((t-16)>>2)&1;
	IR_CodeBuf[12]=((t-16)>>3)&1;
}

//定时
//暂不清粗如何使用定时，默认全0
void timing(unsigned char t)
{
	IR_CodeBuf[13]=(t>>0)&1;
	IR_CodeBuf[14]=(t>>1)&1;
	IR_CodeBuf[15]=(t>>2)&1;
	IR_CodeBuf[16]=(t>>3)&1;

	IR_CodeBuf[17]=(t>>4)&1;
	IR_CodeBuf[18]=(t>>5)&1;
	IR_CodeBuf[19]=(t>>6)&1;
	IR_CodeBuf[20]=(t>>7)&1;
}
//加湿
void moistening(bit x)
{
	IR_CodeBuf[21]=x;
}
//灯光		   
void light(bit x)
{
	IR_CodeBuf[22]=x;
}
//负离子
void anion(bit x)
{
	IR_CodeBuf[23]=x;
}
//节电
void dimout(bit x)
{
	IR_CodeBuf[24]=x;
}
//换气
void aeration(bit x)
{
	IR_CodeBuf[25]=x;
}

//一段固定的数据码作为前35位的结束
void end1()
{
	IR_CodeBuf[26]=0;
	IR_CodeBuf[27]=0;
	IR_CodeBuf[28]=0;
	IR_CodeBuf[29]=1;
	IR_CodeBuf[30]=0;
	IR_CodeBuf[31]=1;
	IR_CodeBuf[32]=0;

	IR_CodeBuf[33]=0;
	IR_CodeBuf[34]=1;
	IR_CodeBuf[35]=0;
}


//上下扫风
void up_down_swing(bit x)
{
	IR_CodeBuf[36]=x;
}
//固定的连接码
void swing_connect1()
{
	IR_CodeBuf[37]=0;
	IR_CodeBuf[38]=0;
	IR_CodeBuf[39]=0;
}
//左右扫风
void left_right_swing(bit x)
{
	IR_CodeBuf[40]=x;
}
//固定连接码
void swing_connect2()
{
	IR_CodeBuf[41]=0;
	IR_CodeBuf[42]=0;
	IR_CodeBuf[43]=0;
}
//温度显示
void display_temp(unsigned char x)
{
	switch(x)
	{
		case 0:IR_CodeBuf[44]=0;IR_CodeBuf[45]=0;break;
		case 1:IR_CodeBuf[44]=1;IR_CodeBuf[45]=0;break;
		case 2:IR_CodeBuf[44]=0;IR_CodeBuf[45]=1;break;
		case 3:IR_CodeBuf[44]=1;IR_CodeBuf[45]=1;break;
		default:	IR_CodeBuf[44]=0;IR_CodeBuf[45]=1;
	}
}
//固定连接码
void connect2()
{
	int i=0;
	for(i=46;i<=61;i++)
		IR_CodeBuf[i]=0;
	IR_CodeBuf[49]=1;
	IR_CodeBuf[63]=0;
}
//节能
void energy_conservation(bit x)
{
	IR_CodeBuf[62]=x;
}
//校验码
void checkout()
{
	unsigned char check;
	check=IR_CodeBuf[1] + IR_CodeBuf[2]*2 + IR_CodeBuf[3]*4+IR_CodeBuf[4]*8;
	check+=IR_CodeBuf[9] + IR_CodeBuf[10]*2 + IR_CodeBuf[11]*4 + IR_CodeBuf[12]*8;	
	check+=IR_CodeBuf[25];
	check+=IR_CodeBuf[40];
	check+=IR_CodeBuf[62];
	check+=12;
	check&=0x0f;

	IR_CodeBuf[64]=check&1;
	IR_CodeBuf[65]=(check>>1)&1;
	IR_CodeBuf[66]=(check>>2)&1;
	IR_CodeBuf[67]=(check>>3)&1;
}
void IR_Data_Init()
{
	change_mode(1);//默认模式：制冷
	
	On_Off(1);//默认开关：开
	
	change_speed(0);//默认风速：自动
	swing_flap(1);//默认扫风
	sleep(0);//默认睡眠：关
	
	temperature(23);//默认温度：23度


	timing(0);//默认定时：不定时
	moistening(0);//加湿：关
	light(1);//灯光：开
	anion(0);//负离子：关
	dimout(0);//节电：关
	aeration(0);//换气：关
	end1();//第一段数据的结束码
	up_down_swing(1);//上下扫风：开
	swing_connect1();
	left_right_swing(1);//左右扫风：开
	swing_connect2();
	display_temp(2);//温度显示：不知道怎么回事，反正这样没错
	connect2();
	energy_conservation(0);//节能：关
	checkout();//每次修改数据之后都要更改一下校验码
}
//更改红外数据时仅更改开关，制冷或制热模式，以及温度，其余保持IR_Data_Init()函数中默认值不变
//On_Off：开：1，关：0
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
//使用定时器中断来进行红外发射的时序控制
void IR_TX_Init()
{
	TMOD = 0x12;// 0001(timer1) 0010(timer0) 
	TH0	= 0xf4;
	TL0	= 0xf4;//38KHz 
	EA	= 1;// Enable All Interrupt
	ET1	= 1;// Enable Timer1
	ET0	= 1;// Enable Timer0	
	IR_LED = 1;	// stop emit
}
//定时器1控制发射的时间长度，定时器0通过不断翻转电平给红外光添加38KHz载波
void Timer1_Delay(unsigned char th1,unsigned char tl1,bit mod)
{
	TH1 = th1;
	TL1 = tl1;
	TR1 = 1;//timer1 run
	TR0 = mod;//timer0 run
	while(!TF1);//wait timer1 flag
	TF1 = 0;
	TR0 = 0;//timer0 stop run
	TR1 = 0;//timer1 stop run
	IR_LED = 1;// stop emit
}

//发送红外数据
//依次发送：起始码+35位数据码+连接码+32位数据码
void Transmit_TR_CODE()
{
	unsigned char i;

	//起始码
	Timer1_Delay(0xdf, 0x9a, 1);//emit 9ms
	Timer1_Delay(0xef, 0xcd, 0);//delay 4.5ms
	//35位数据码
	for(i = 1; i < 36; i++)
	{		
		Timer1_Delay(0xfd, 0xfc, 1);//emit 0.56ms
		if (IR_CodeBuf[i])
			Timer1_Delay(0xf9, 0xef, 0);//delay 1.685ms
		else
			Timer1_Delay(0xfd, 0xfc, 0);//delay 0.56ms	
	}
	//连接码
	Timer1_Delay(0xfd, 0xd7, 1);//emit 600us
	Timer1_Delay(0xb7, 0xff, 0);//delay 20000us
	//32位数据码
	for(i = 36; i < 68; i++)
	{		
		Timer1_Delay(0xfd, 0xfc, 1);//emit 0.56ms
		if (IR_CodeBuf[i])
			Timer1_Delay(0xf9, 0xef, 0);//delay 1.685ms
		else
			Timer1_Delay(0xfd, 0xfc, 0);//delay 0.56ms		
	}
	Timer1_Delay(0xfd, 0xd7, 1);//emit 600us

}
//定时器0负责给发送的红外光添加38KHz载波
void Timer0() interrupt 1 //1 mean timer0
{
	IR_LED = ~IR_LED;
}
//定时器1只用到了定时标志位TF1，不用中断函数不用做任何操作
void Timer1() interrupt 3
{
	
}
void main()
{
	 unsigned char temp=0xfe;	 
	 IR_TX_Init();//IR发送、接收初始化
	 IR_Data_Init();//IR数据初始化
	 while(1)
	 {
		IR_Data_Change(1,1,23);//开，制冷，25摄氏度		
		Transmit_TR_CODE();		

		temp = _cror_(temp,1);
		P1 = temp;
		
		Delay_ms(2000);
	 }
	 	
}
