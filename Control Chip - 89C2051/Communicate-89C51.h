#ifdef Communicate_H
void init_com(void);
void send(int cmd);
int  recv(void);
#endif

#include <reg51.h>
/*

   定时器初值=256-(fosc*(SMOD+1)/384*波特率)
			 =256-(24000000*(0+1)/384*9600)	 <- 我使24MHz个晶振
			 =256-6=250
			 =0xFA

*/

void init_com(void)	   // 初始化RXD,TXD
{
	 
 	PCON=0x00;		   // 不用双倍波特率
	SM0=0;			   // 方式一通信
	SM1=1;
	REN=1;			   // 可以接收
	TI=0;			   // 还可以发送
	TMOD=0x20;		   // 设置定时器
	TH1=0xfd;	       // 相应波特率设初值计算方法。初值X=(256－11059200/(12*32*9600)) 
	TL1=0xfd;
	ES=1;
	EA=1;			   // 开启串行中断
//	TH1=0xfa;	 /
///	TL1=0xfa;	
	TR1=1;			   // 开启定时器
}

void send(int cmd)	   // 发送数据,传入的是一个整型命令
{
	SBUF=cmd;		   // 写入到数据缓存
	while(TI) TI=0;	   // 等待TI置为1
}

int recv(void)	       // 发送数据,传入的是一个整型命令
{
	int RecvData=SBUF; // 从数据缓存中读取数据
	RI=0;
	
//	while(RI) RI=0;	   // 等待RI置为1
	return RecvData;
}