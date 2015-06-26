
#ifdef DELAY_H
#define DELAY_H
void sleep(int ms);
#endif
void sleep(int ms)
{
 int a,b;
  for(a=1;a<=ms;a++) 
    for(b=1;b<=1000;b++);	  // ºÏ²¢ÁË
}