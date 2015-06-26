#include <reg51.h>		             // AT89C51器件引脚库
#include <string.h>	              	 // 字符操作函数库,用采投币器中断
#include "delay.h"					 // 延时库,根据12MHz晶振,1ms 运行1000条循环语句
#include "Key.h"	  				 // 键盘库,读取按下的键盘位置
#include "LCD.h"					 // LCD显示库,有清除,打印等函数
//#include "Communicate-89C51.h"		 // 89C51通信库,集合了初始化,发送接收函数
//两行三列的测试版本不需要这个
//自定义库	
#define A1 0x01
#define A2 0x02
#define A3 0x03
#define B1 0x11
#define B2 0x12
#define B3 0x13
//AT89C2051查询位置常量
#define CtrlA1 0x51
#define CtrlA2 0x52
#define CtrlA3 0x53
#define CtrlB1 0x61
#define CtrlB2 0x62
#define CtrlB3 0x63
//AT89C2051控制位置常量(在AT89C51时也可用)
/*#define Busy      0	   // AT89C2051繁忙
#define Idle      1	   // AT89C2051空闲
#define OhNo	  4	   // AT89C2051回传常量,意思是没有货物
#define OhYeah	  5	   // AT89C2051回传常量,意思是有货物
#define WorkStart 10   // 开始启动
#define WorkEnd	  11   // 已经出货
// AT89C2051回传给AT89C51的值
*/ // 暂未用到的常量
#define ChipInit    0
//#define ChipRun     1
//#define ShutDown   -1
#define ReadySelect 1
#define ReadyPay	2 
//#define SelectLine  10
//#define SelectList  11
//stat 变量的状态
#define NoSelectLine -1	
#define SelectLineA 1
#define SelectLineB 2	
//selline 变量的状态
#define NoSelectList NoSelectLine	
#define SelectList1 1
#define SelectList2 2
#define SelectList3 3
#define SelectList4 4	 // 未有用到,古阵使个系1-3排
//sellist 变量的状态
#define true  1
#define false 0
//boolean 类型,C51 没有定义啊..
/*
 void sleep(int ms);// 延时函数
//delay.h
 int scankey(void); // 读取按下的键的位置,取值0-15,错误返回-1
//Key.h
 void init_lcd(void);// 初始化1602LCD
 void printlcd(char *str);// 输出字符到LCD
 void clear(void);// 清空LCD上的字符
//LCD.h
 void init_com(void);// 初始化单片机通信接口
 void send(int cmd);// 发送数据
 int  recv(void);// 接收数据
//Communicate-89C51.h
//自定义函数   */
sbit key1=P3^2;		 
sbit key2=P3^3;
sbit bep =P3^4;
sbit INT =P3^2;
//调试键位
sbit PortA1=P1^0;
sbit PortA2=P1^1;
sbit PortA3=P1^2;
sbit PortB1=P1^3;
sbit PortB2=P1^4;
sbit PortB3=P1^5;
// 出货端口
sbit ChackPort1=P1^6;
sbit ChackPort2=P1^7;
sbit ChackPort3=P3^0;
sbit ChackPort4=P3^1;
sbit ChackPort5=P3^2;
sbit ChackPort6=P3^3;
// 检测端口
int stat=ChipInit;			// 状态变量
int selline=NoSelectLine;	// 选择的行  (默认为-1,即没有选择)
int sellist=NoSelectList;	// 选择的列
int money=0;				// 投币数	 (默认为 0,即没有投币)
//要使用的变量

void CtrlPort(int PortIndex)	  	 // 控制端口
{
  switch(PortIndex)	                 // 读取接收到的信息
  {
	  case CtrlA1: PortA1=0;sleep(2000);PortA1=1; 	// 判断信息,开始控制每一个端口 ,延迟3秒是假设值
	  case CtrlA2: PortA2=0;sleep(2000);PortA2=1;	
	  case CtrlA3: PortA3=0;sleep(2000);PortA3=1;
	  case CtrlB1: PortB1=0;sleep(2000);PortB1=1;	
	  case CtrlB2: PortB2=0;sleep(2000);PortB2=1;
	  case CtrlB3: PortB3=0;sleep(2000);PortB3=1;	  
  }
}

int ChackPoint(int Point)			  // 检测这个位置有没有货(注意,这是一个阻塞函数)
{
//	 int rcv;						  // AT89C51接收数据变量
	 EA=0;							  // 关全局中断,这样的话在等待的时候投币就不会卡死	 
/*	 send(Point);					  // 发送位置代码到控制芯片	 
	 while(1)						  // 循环接收
	 {
		 rcv=recv();
		 if (rcv==OhNo)  return false;// 没有了
		 if (rcv==OhYeah)return true; // 有呀
		 sleep(100);				  // 让程序延迟一下,这样会好点
	 }*/  //  这段代码是AT89C51和AT89C2051之间的通信阻塞代码

	 //全部输出,便于接收高电平,假如不输出低电平的话,IO端口已经是低电平也不会判断成功

	 switch(Point)
	 {
		 case A1:EA=1;return (ChackPort1==0)?1:0;
         case A2:EA=1;return (ChackPort2==0)?1:0;
		 case A3:EA=1;return (ChackPort3==0)?1:0;
		 case B1:EA=1;return (ChackPort4==0)?1:0;
		 case B2:EA=1;return (ChackPort5==0)?1:0;
		 case B3:EA=1;return (ChackPort6==0)?1:0;
	 }
	 EA=1;							  // 中断继续开
	 return -1;
}

void ShowLine(void)				      // 还没有购买东西时的界面	(桌面)
{
	 char optmey=money+48;			  // 由于合成ASCII,所以要加上48(ASCII48=数字0)
	 char str[10]="Yon coin:";        // 准备合成一个新的字符串,// 输出字符,告诉用户你投币多少		 	 
	 clear();
	 sleep(5);
	 printlcd("Plaese select!");	 			
	 strcat(str,&optmey);
	 printlcd(str);
	 selline=NoSelectLine;			  // 回到了桌面,要清理数据了
	 sellist=NoSelectList;
	 stat=ReadySelect;				  // 回到选择列的状态
}

void ChackPay(void)					  // 这个函数是检测当前的money 还有多少,用于HasMoney和Determine
{
			     if(money==0) printlcd("Please coin:0/3");
			else if(money==1) printlcd("Please coin:1/3");
			else if(money==2) printlcd("Please coin:2/3");
			else
			{
				money-=3;		         // money-=3;
		        clear();										 
		        sleep(5);
		        printlcd("Pay success");
				sleep(1000);			     // 暂停1秒
				clear();
				sleep(5);
				printlcd("Control Ready!");	 // 控制端的芯片已经准备好

				if(selline==SelectLineA)	 // 分开控制
				{
					if(sellist==SelectList1) CtrlPort(CtrlA1);
					if(sellist==SelectList2) CtrlPort(CtrlA2);
				    if(sellist==SelectList3) CtrlPort(CtrlA3);
				}
				else if(selline==SelectLineB)
				{
					if(sellist==SelectList1) CtrlPort(CtrlB1);
					if(sellist==SelectList2) CtrlPort(CtrlB2);
				    if(sellist==SelectList3) CtrlPort(CtrlB3);
				}			
//				while(recv()==WorkEnd) break; // 循环判断AT89C2051系无系已经出都货
				printlcd("Control End!!");    // 控制端的芯片已经运行结束
				sleep(1500);
				selline=NoSelectLine;
				sellist=NoSelectList;
				stat=ReadySelect;
				ShowLine();
	        }
}

void Determine(void)				  // 确定交易
{
	   clear();
	   sleep(5);
	   // 判断选中的行
	   if(selline==NoSelectLine)	  // 无选中
	   {
		        printlcd("Error!");
		    	printlcd("No Select!");
				selline=NoSelectLine;
	            sellist=NoSelectList;  
			    ShowLine();
				return;				  // 有错误的话立刻退出交易流程 有错误住跟手退出流程
	   }
	   if(selline==SelectLineA)       // 选中A行	  
	   {
			if(sellist==NoSelectList) // 没有选中的话也点确定 无选中其也点确定
			{ 
			    printlcd("Error");	  // 提示错误
				printlcd("No Select List!");
				selline=NoSelectLine;
	            sellist=NoSelectList; // 清空选中个(的)数据
			    ShowLine();
				return;				  // 交易出错住(就)退出交易流程
			}
			if(sellist==SelectList1)  // 选中行邓(和)列,再点确定
			{	
			    printlcd("Select Point:A1");
				if(!ChackPoint(A1))	  // 假如无都(没有了)货..
				{
					 clear();
					 sleep(5);
					 printlcd("Point:A1 No Goos");
					 selline=NoSelectLine;
	                 sellist=NoSelectList;     // 清空选中个数据
					 sleep(1500);
					 ShowLine(); 			   // 返回到桌面
					 return;
				}
			}
			if(sellist==SelectList2)
			{	
			    printlcd("Select Point:A2");
				if(!ChackPoint(A2))	  
				{
					 clear();
					 sleep(5);
					 printlcd("Point:A2 No Goos");
					 selline=NoSelectLine;
	                 sellist=NoSelectList;     // 清空选中个数据
					 sleep(1500);
					 ShowLine(); 			   // 返回到桌面
					 return;
				}
			}
			if(sellist==SelectList3)
			{	
			    printlcd("Select Point:A3");
				if(!ChackPoint(A3))	  
				{
					 clear();
					 sleep(5);
					 printlcd("Point:A3 No Goos");
					 selline=NoSelectLine;
	                 sellist=NoSelectList; // 清空选中个数据
					 sleep(1500);
					 ShowLine(); 		   // 返回到桌面
					 return;
				}
	  		}
	   }
	   if(selline==SelectLineB)	      // 选中B行
	   {
			if(sellist==NoSelectList)
			{ 
			    printlcd("Error");
				printlcd("No Select List!");
				selline=NoSelectLine;
	            sellist=NoSelectList; // 清空选中个数据
				ShowLine();
				return;
			}
			if(sellist==SelectList1)
			{ 	
		     	printlcd("Select Point:B1");
				if(!ChackPoint(B1))	  
				{
					 clear();
					 sleep(5);
					 printlcd("Point:B1 No Goos");
					 selline=NoSelectLine;
	                 sellist=NoSelectList; // 清空选中个数据
					 sleep(1500);
					 ShowLine(); 			   // 返回到桌面
					 return;
				}
			}
			if(sellist==SelectList2)
			{ 
				printlcd("Select Point:B2");
				if(!ChackPoint(B2))	  
				{
					 clear();
					 sleep(5);
					 printlcd("Point:B2 No Goos");
					 selline=NoSelectLine;
	                 sellist=NoSelectList; // 清空选中个数据
					 sleep(1500);
					 ShowLine(); 			   // 返回到桌面
					 return;
				}
			}
			if(sellist==SelectList3)
			{ 	
				printlcd("Select Point:B3");
			    if(!ChackPoint(B3))	  
				{
					 clear();
					 sleep(5);
					 printlcd("Point:B3 No Goos");
					 selline=NoSelectLine;
	                 sellist=NoSelectList; // 清空选中个数据
					 sleep(1500);
					 ShowLine(); 			   // 返回到桌面
					 return;
				}
			}
	   }
	   
	   stat=ReadyPay;				  // 选好了货,状态变量也要变,然后呢,其住可以控制紧无比用户投币那阵又选择其它也..哈....
	   ChackPay();					  // 要检测一次..
	   EA=1;							  // 中断继续开
}

void Cancel(void)					  // 取消交易的时候要清理数据
{
	   clear();
	   sleep(5);
	   selline=NoSelectLine;
	   sellist=NoSelectList;
	   stat=ReadySelect;			  // 还原到可以选择
	   printlcd("Cancel select!");
	   sleep(1500); 				  // 延迟3秒
	   ShowLine();
}

void HasMoney(void) interrupt 0		  // 投币器中断
{
	 sleep(20);						  // 软件消抖	 
	 money++;
	 if (money>=9) money=9;			  // 总要有一些东西来限制的,9块钱不算少吧...
	 if (stat==ReadyPay) ChackPay();EA=1;// 唉,程序员真正系(真的是)无容易个(不容易的)哇(呀) >_< ...
	 if (stat==ReadySelect)ShowLine();// 唉,程序员无容易(不容易)啊 -_- ...	
	 while(INT==0);				      // 等待INT(P3^2)变成高电平
} 

void main()
{
   stat=ChipInit;  // 初始化的运行状态
   init_lcd();
   sleep(10);
   printlcd("inited .."); // 输出到LCD上
   // LCD 初始化
   EA=1;		
   EX0=1;  		  
   IT0=0;          // 下降沿触发,投币器专用
   // 投币器中断初始化
//   init_com();   // 初始化串口接收数据
   // AT89C51通信初始化
   printlcd("Machine init OK!");
   sleep(1000);	   // 定时试神
   ShowLine();
   stat=ReadySelect;   // 转换状态到正常运行 
     
   while(1)
   {
	int k=scankey();                          // 假如无加上古个也个话呢,好容易伟导致键盘假失灵(看紧系通电都,事实上等都好久都无反应个)  
	if(stat==ReadySelect)					  // 假如投币紧个话呢,住系无得做其它个事个咯,不过取消交易华系可以个
	{  	    
	 if(k==3)	                              // 首先选择行数 (A键)
	 {
		if(selline!=NoSelectLine) continue;	  // 已经选择都列
	    clear();
		sleep(5);
		printlcd("Select Line:A"); 	          // 输出,你选择了A行
		selline=SelectLineA;
	 }	
	 if(k==7)	                              // 首先选择行数 (B键) 
	 {	 
		if(selline!=NoSelectLine) continue;
	    clear();
		sleep(5);
		printlcd("Select Line:B"); 
		selline=SelectLineB;
	 }
   	 if(k==0) // 1键
	 {
	    if(selline==NoSelectLine) continue;	  // 华未有选择行
	    sellist=SelectList1;
	    printlcd("Select List:1");            // 选择的列
	 }									      
     if(k==1) // 2键
	 {
		if(selline==NoSelectLine) continue;
		sellist=SelectList2;
		printlcd("Select List:2"); 
	 }
     if(k==2) // 3键
	 {
	    if(selline==NoSelectLine) continue;
	    sellist=SelectList3;
	    printlcd("Select List:3");
	 }
	 if(k==0xc)  Cancel();                    // 古个系(这个是)确定键(键盘上的*键)
	 if(k==0xe)  Determine();                 // 古个系取消键(键盘上的#键)  
    }
	else if(stat==ReadyPay)
	{
	 if(k==0xe)  Determine();                 // 古个系取消键(键盘上的#键) 
   	}
   } 	 
}
