//#include <AT892051.h>
#include <reg51.h>			   // 由于2K代码问题,无办法咯
#include "Communicate-89C51.h"
#define A1 0x01
#define A2 0x02
#define A3 0x03
#define B1 0x11
#define B2 0x12
#define B3 0x13
// 查询位置常量
#define CtrlA1 0x51
#define CtrlA2 0x52
#define CtrlA3 0x53
#define CtrlB1 0x61
#define CtrlB2 0x62
#define CtrlB3 0x63
// 控制位置常量
#define Busy      0	   // AT89C2051繁忙
#define Idle      1	   // AT89C2051空闲
#define OhNo	  4	   // 无料到都咯..
#define OhYeah	  5	   // 改华有料到
#define WorkStart 10   // 开始启动
#define WorkEnd	  11   // 已经出货
// AT89C2051回传给AT89C51的值
#define true    1
#define false   0
// Boolean常量
#define Working 1
#define Readly	0
// stat 状态变量取值
/* 原来是定义给AT89C2051的
sbit PortA1=P1^7;
sbit PortA2=P1^6;
sbit PortA3=P1^5;
sbit PortB1=P1^4;
sbit PortB2=P1^3;
sbit PortB3=P1^2;
// 爱控制的马达
sbit ChackPortA1=P1^1;
sbit ChackPortA2=P1^0;
sbit ChackPortA3=P3^2;
sbit ChackPortB1=P3^3;
sbit ChackPortB2=P3^4;
sbit ChackPortB3=P3^5;
// 检验古个位置系无系有也库存紧(通过检验输入电平)
*/
sbit PortA1=P2^1;
sbit PortA2=P2^2;
sbit PortA3=P2^3;
sbit PortB1=P2^4;
sbit PortB2=P2^5;
sbit PortB3=P2^6;

sbit ChackPortA1=P1^0;
sbit ChackPortA2=P1^1;
sbit ChackPortA3=P1^2;
sbit ChackPortB1=P1^3;
sbit ChackPortB2=P1^4;
sbit ChackPortB3=P1^5;

int  stat=Readly;

void   CtrlPort (int PortIndex);
int    ChackPort(int PortIndex);
void   init_com(void);
void   send(int cmd);
int    recv(void);

void Delay(int ms)
{
   int a,b;
   for(a=1;a<=2*ms;a++)
      for(b=1;b<=1000;b++);  
}

void RecvMessageInterrupt(void) interrupt 4
{
	 int rcv=recv();
          if(rcv==A1)		 // 据说用if 写出来的代码空间比switch 小
		  {
			       if (ChackPort(A1)) send(OhYeah);
				   else               send(OhNo);
          }
		  else if(rcv==A2)
		  {
			       if (ChackPort(A2)) send(OhYeah);
				   else               send(OhNo);
          }
		  else if(rcv==A3)
	      {
			       if (ChackPort(A3)) send(OhYeah);
				   else               send(OhNo);
          }
		  else if(rcv==B1)
		  {
			       if (ChackPort(B1)) send(OhYeah);
				   else               send(OhNo);
          }
		  else if(rcv==B2)
		  {
			       if (ChackPort(B2)) send(OhYeah);
				   else               send(OhNo);
          }
		  else if(rcv==B3)
		  {
			       if (ChackPort(B3)) send(OhYeah);
				   else               send(OhNo);
          }
		  else if(rcv==CtrlA1)
		  {
		  send(WorkStart);
		  Delay(1000);
		  CtrlPort(A1);
		  Delay(1000);
		  send(WorkEnd);
          }
		  else if(rcv==CtrlA2)
		  {
		  send(WorkStart);
		  Delay(1000);
		  CtrlPort(A2);
		  Delay(1000);
		  send(WorkEnd);
		  }
		  else if(rcv==CtrlA3)
		  {
		  send(WorkStart);
		  Delay(1000);
		  CtrlPort(A3);
		  Delay(1000);
		  send(WorkEnd);
		  }
		  else if(rcv==CtrlB1)
		  {
		  send(WorkStart);
		  Delay(1000);
		  CtrlPort(B1);
		  Delay(1000);
		  send(WorkEnd);
		  }
		  else if(rcv==CtrlB2)
		  {
		  send(WorkStart);
		  Delay(1000);
		  CtrlPort(B2);
		  Delay(1000);
		  send(WorkEnd);
		  }
		  else if(rcv==CtrlB3)
		  {
		  send(WorkStart);
		  Delay(1000);
		  CtrlPort(B3);
		  Delay(1000);
		  send(WorkEnd);
		  }
		  else{}
}

void main()
{
	init_com();				      	// 初始化RXD,TXD
	while(1);						// 死循环,主要是让串行中断来工作
}

void CtrlPort(int PortIndex)		// 控制端口
{
  switch(PortIndex)	                // 读取接收到的信息
  {
	  case A1: PortA1=0;Delay(3000);PortA1=1; 	// 判断信息,开始控制每一个端口 ,延迟3秒是假设值
	  case A2: PortA2=0;Delay(3000);PortA2=1;	
	  case A3: PortA3=0;Delay(3000);PortA3=1;
	  case B1: PortB1=0;Delay(3000);PortB1=1;	
	  case B2: PortB2=0;Delay(3000);PortB2=1;
	  case B3: PortB3=0;Delay(3000);PortB3=1;	  
  }
}

int ChackPort(int PortIndex)        // 判断这个端口是否有货物库存
{
  switch(PortIndex)	
  {
	  case A1:	return (ChackPortA1==0)?true:false;	
	  case A2:	return (ChackPortA2==0)?true:false;		
	  case A3:	return (ChackPortA3==0)?true:false;	
	  case B1:	return (ChackPortB1==0)?true:false;		
	  case B2: 	return (ChackPortB2==0)?true:false;	
	  case B3:	return (ChackPortB3==0)?true:false;	  
  }
  return false;
}