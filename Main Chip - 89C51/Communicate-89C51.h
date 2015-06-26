#ifdef Communicate_H
void init_com(void);
void send(int cmd);
int recv(void);
#endif
#define No     	 -1
// 用与Recv 函数接收数据,意思为没有接收到数据
#define Busy      0	   // AT89C2051繁忙
#define Idle      1	   // AT89C2051空闲
#define OhNo	  4	   // AT89C2051回传常量,意思是没有货物
#define OhYeah	  5	   // AT89C2051回传常量,意思是有货物
#define WorkStart 10   // 开始启动
#define WorkEnd	  11   // 已经出货
// AT89C2051回传给AT89C51的值
// 接收数据的时候也许会有错误,用这个来判断
#include <reg51.h>
/*

   定时器初值=256-(fosc*(SMOD+1)/384*波特率)
			 =256-(24000000*(0+1)/384*9600)	 <- 我使24MHz个晶振
			 =256-6=250
			 =0xFA

*/

int RecvData=-1;	   // 存放从中断中接收过来的数据,-1为没有数据

void init_com(void)	   // 初始化RXD,TXD
{
	EA=1;
	ES=1; 			   // 利用中断来接收数据// 不使用中断来接收数据
 	PCON=0x00;		   // 不用双倍波特率
	SM0=0;			   // 方式一通信
	SM1=1;
	REN=1;			   // 可以接收
	TI=0;			   // 还可以发送
	TMOD=0x20;		   // 设置定时器
	TH1=0xfd;	
	TL1=0xfd;	
	TR1=1;			   // 开启定时器
}

void send(int cmd)	   // 发送数据,传入的是一个整型命令
{
	SBUF=cmd;		   // 写入到数据缓存
	while(!TI);		   // 等待TI置为1
	TI=0;	   
}
/* */
void RecvInterrupt(void) interrupt 4// 接收数据中断
{
	if (RI==0) RecvData=No; return; //没有接收到数据
	switch(SBUF)
	{		
		 case OhNo:		RecvData=OhNo;
		 case OhYeah:	RecvData=OhYeah;
		 case WorkStart:RecvData=WorkStart;
         case WorkEnd:	RecvData=WorkEnd;
	}
	if (RI==1) RI=0;   // 接收数据之后,RI为0
}
	
int recv(void)	       // 发送数据,传入的是一个整型命令
{
	int rtn=RecvData;
	RecvData=No;
	return rtn;
}