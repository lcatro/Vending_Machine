#ifndef __TIME_H__
#define __TIME_H__
void  init_time0(void);
void  init_time1(void);
void  init_time2(void);
#endif

//  中断格式  void time0(void) interrupt 1
void init_time0(void)
{
	TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 	TH0=0x00;	      //给定初值，这里使用定时器最大值从0开始计数一直到65535溢出。
 	TL0=0x00;
 	EA=1;             //总中断打开
 	ET0=1;            //定时器T0允许中断
 	TR0=1;            //定时器T0开始工作
}
//  中断格式  void time1(void) interrupt 3
void init_time1(void)
{
	TMOD |= 0x10;		     
 	TH1=0x00;	      
 	TL1=0x00;
 	EA=1;          
 	ET1=1;            //定时器T1允许中断
 	TR1=1;            //定时器T1开始工作
}
//  中断格式  void time2(void) interrupt 5
void init_time2(void)
{
	TMOD |= 0x10;		     
 	TH2=0x00;	      
 	TL2=0x00;
 	EA=1;          
 	ET2=1;            //定时器T1允许中断
 	TR2=1;            //定时器T1开始工作
}
