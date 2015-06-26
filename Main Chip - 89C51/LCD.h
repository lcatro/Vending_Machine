#ifdef LCD_H
#define LCD_H

void init_lcd(void);
void printlcd(char *str);
void clear(void);
void crlf(void);
#endif

/*

古个库默认系以 
P2口来输出数据
P1^0 
P1^1 读写数据端
P1^2 使能端

*/
#include <reg51.h>
		 
sbit lcdrs=P1^0;
sbit lcdrw=P1^1;
sbit lcden=P1^2; 
void clear(void);
void crlf(void);

void write_com(char com)		//写命令函数
{
	lcdrs=0;
	P2=com;	
	lcden=1;
	sleep(5);						 			  
	lcden=0;
}

void write_date(char date)		//写数据函数
{
	lcdrs=1;
	P2=date;
	lcden=1;
	sleep(5);
	lcden=0;
}

void init_lcd()					//初始化函数
{	
	lcdrw=0;
	write_com(0x0f);
	write_com(0x38);			//显示模式设置，默认为0x38,不用变。
	clear();		        	//显示清屏，将上次的内容清除，默认为0x01. 
	write_com(0x80);			//设置初始化数据指针，是在读指令的操作里进行的
}

void printlcd(char *str)					//显示函数
{
  int i;
	for(i=0;i<16;i++)
	{
	   	if (str[i]=='\0') break;		
		write_date(str[i]);
	}
	crlf();	
}

void clear(void)
{
    write_com(0x01);	
}

void crlf(void)
{
	write_com(0x80+0x40); 		//更改数据指针，让字符换行
	sleep(5);
}


