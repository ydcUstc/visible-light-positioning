
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "bsp_spi_nrf.h"
#include "lcd.h"
#include "ov7670.h"
#include "usart1.h"
typedef struct LED{
	float x;
	float y;
}LED;

u16 color,rcolor,gcolor,bcolor,graycolor;
u32 coorx[10],coory[10],pointCount[10];
int coorxFirpre[10],coorxLatpre[10],coorxCount,coorypre[10],coorxTemp,cooryTemp;
int countnum,itemp,jtemp;
u8  logicFlagy,logicFlagx,utiltFlag[10],repFlag[10],maxlightFlag,utiltnum,repnum;
LED ledup,ledlf,ledrt;
u8 countReflect[10];
int uppointx,uppointy,lfpointx,lfpointy,rtpointx,rtpointy;
float xerr,yerr,xmsur,ymsur,pic2cm=0.25*30/23*50/52;
u16 xcm,ycm,charxpix;
u8 xnum,ynum;


void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


 



extern u8 ov_sta;	//在exit.c里 面定义	 
//更新LCD显示
void camera_refresh(void)
{
	u32 j;

	if(ov_sta==2)//有帧中断更新？
	{
		coorxCount=0;
		LCD_Scan_Dir(4);		//从上到下,从左到右  
//LCD_Set_Window((lcddev.width-160)/2,(lcddev.height-160)/2,160,200);//将显示区域设置到屏幕中央
//LCD_Set_Window((lcddev.width-200)/2,(lcddev.height-160)/2,200,160);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		for(countnum=0;countnum<10;countnum++){
			coorx[countnum]=0;
			coory[countnum]=0;
			coorxFirpre[countnum]=-1;
			coorxLatpre[countnum]=-1;
			coorypre[countnum]=-1;
			pointCount[countnum]=0;
			repFlag[countnum]=0;
			utiltFlag[countnum]=0;
		}
		countnum=0;
		utiltnum=0;
		xmsur=0;
		ymsur=0;
		ledlf.x=-15;
		ledrt.x=15;
		ledup.x=0;
		ledup.y=12;
		ledrt.y=-12;
		ledlf.y=-12;
		charxpix=228;
		for(j=0;j<240*320;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			color<<=8;  
			
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			LCD->LCD_RAM=color; 
			rcolor=color&0xf800;
			rcolor>>=11;
			gcolor=color&0x07e0;
			gcolor>>=5;
			bcolor=color&0x001f;
			graycolor=rcolor+gcolor+bcolor;
			if(graycolor>=123){
				coorxTemp=j%320;
				cooryTemp=(int)(j/320);
				if(coorypre[countnum]!=-1){
					repnum=0;
					for(itemp=0,logicFlagy=0,logicFlagx=0;itemp<10;itemp++){
							if(utiltFlag[itemp]==1){
								if((cooryTemp-coorypre[itemp])>5){
									logicFlagy++;
								}
								else if((coorxTemp-coorxLatpre[itemp])<10&&(coorxTemp-coorxFirpre[itemp])>-10){
									repFlag[repnum++]=itemp;
								}
							
								else 
									logicFlagx++;
								}				
						
					}
					if(repnum>0){
						countnum=repFlag[0];
						for(itemp=1;itemp<repnum;itemp++){
							coorx[countnum]+=coorx[repFlag[itemp]];
							coory[countnum]+=coory[repFlag[itemp]];
							pointCount[countnum]+=pointCount[repFlag[itemp]];
							if(coorxFirpre[countnum]>coorxFirpre[repFlag[itemp]])coorxFirpre[countnum]=coorxFirpre[repFlag[itemp]];
							if(coorxLatpre[countnum]<coorxLatpre[repFlag[itemp]])coorxLatpre[countnum]=coorxLatpre[repFlag[itemp]];
							utiltFlag[repFlag[itemp]]=0;
							utiltnum--;
							
						}
//						for(itemp=0;itemp<10;itemp++){
//							if(utiltFlag[itemp]==1)coorxCount=itemp;
//						}
					}
					
					else {
						for(itemp=0,jtemp=0;itemp<utiltnum&&itemp!=-1;jtemp++){
							if(utiltFlag[jtemp])itemp++;
							else {
								itemp=-1;
								countnum=jtemp;
							}
						}
							if(itemp==utiltnum){
//								coorxCount++;
								if(jtemp!=10)
								countnum=jtemp;
							}
						utiltnum++;
						utiltFlag[countnum]=1;
					}					
			
				}
				if((cooryTemp!=coorypre[countnum])){
					coorypre[countnum]=cooryTemp;
					coorxFirpre[countnum]=coorxTemp;
				}
				coorxLatpre[countnum]=coorxTemp;
				coorx[countnum]+=coorxTemp;
				coory[countnum]+=cooryTemp;
				pointCount[countnum]++;
			}
			   
		}
		for(itemp=0,utiltnum=0;itemp<10&&coorypre[0]!=-1;itemp++){
			if(utiltFlag[itemp]==1){
				if(pointCount[itemp]>150){
					utiltFlag[itemp]=2;
					utiltnum++;
				}
			}
		}
		
		for(itemp=0,jtemp=0;itemp<10&&coorypre[0]!=-1;itemp++){
			if(utiltFlag[itemp]==2){
				coorx[itemp]/=pointCount[itemp];
				coory[itemp]/=pointCount[itemp];
				LCD_Draw_Circle(coorx[itemp],coory[itemp],10);
				countReflect[jtemp++]=itemp;
			}

		}
		if(utiltnum==3){
			u32 temp=0;
			if(coory[countReflect[0]]>coory[countReflect[1]]){
				if(coory[countReflect[2]]<coory[countReflect[1]]){
					uppointx=coorx[countReflect[2]];
					uppointy=coory[countReflect[2]];
					if(coorx[countReflect[0]]>coorx[countReflect[1]]){
						lfpointx=coorx[countReflect[0]];
						rtpointx=coorx[countReflect[1]];
						lfpointy=coory[countReflect[0]];
						rtpointy=coory[countReflect[1]];
					}
					else{
						rtpointx=coorx[countReflect[0]];
						lfpointx=coorx[countReflect[1]];
						rtpointy=coory[countReflect[0]];
						lfpointy=coory[countReflect[1]];
					}
				}
				else{
					uppointx=coorx[countReflect[1]];
					uppointy=coory[countReflect[1]];
					if(coorx[countReflect[2]]>coorx[countReflect[0]]){
						lfpointx=coorx[countReflect[2]];
						rtpointx=coorx[countReflect[0]];
						lfpointy=coory[countReflect[2]];
						rtpointy=coory[countReflect[0]];
					}
					else{
						rtpointx=coorx[countReflect[2]];
						lfpointx=coorx[countReflect[0]];
						rtpointy=coory[countReflect[2]];
						lfpointy=coory[countReflect[0]];
					}
				}
			}
			else{
				if(coory[countReflect[2]]<coory[countReflect[0]]){
					uppointx=coorx[countReflect[2]];
					uppointy=coory[countReflect[2]];
					if(coorx[countReflect[0]]>coorx[countReflect[1]]){
						lfpointx=coorx[countReflect[0]];
						rtpointx=coorx[countReflect[1]];
						lfpointy=coory[countReflect[0]];
						rtpointy=coory[countReflect[1]];
					}
					else{
						rtpointx=coorx[countReflect[0]];
						lfpointx=coorx[countReflect[1]];
						rtpointy=coory[countReflect[0]];
						lfpointy=coory[countReflect[1]];
					}
				}
				else{
					uppointx=coorx[countReflect[0]];
					uppointy=coory[countReflect[0]];
					if(coorx[countReflect[2]]>coorx[countReflect[1]]){
						lfpointx=coorx[countReflect[2]];
						rtpointx=coorx[countReflect[1]];
						lfpointy=coory[countReflect[2]];
						rtpointy=coory[countReflect[1]];
					}
					else{
						rtpointx=coorx[countReflect[2]];
						lfpointx=coorx[countReflect[1]];
						rtpointy=coory[countReflect[2]];
						lfpointy=coory[countReflect[1]];
					}
				}
			}
			xerr=uppointx-160;
			yerr=120-uppointy;
			xerr*=pic2cm;
			yerr*=pic2cm;
			xmsur+=(xerr+ledup.x);
			ymsur+=(yerr+ledup.y);
			
			xerr=lfpointx-160;
			yerr=120-lfpointy;
			xerr*=pic2cm;
			yerr*=pic2cm;
			xmsur+=(xerr+ledlf.x);
			ymsur+=(yerr+ledlf.y);
			
			xerr=rtpointx-160;
			yerr=120-rtpointy;
			xerr*=pic2cm;
			yerr*=pic2cm;
			xmsur+=(xerr+ledrt.x);
			ymsur+=(yerr+ledrt.y);
			
			xmsur/=3;
			ymsur/=3;
			
		}
		else if(utiltnum==2){
			if((int)coory[countReflect[0]]-(int)coory[countReflect[1]]>10||(int)coory[countReflect[1]]-(int)coory[countReflect[0]]>10){
				if(coorx[countReflect[0]]>160&&coorx[countReflect[1]]>160){
					if(coory[countReflect[1]]>coory[countReflect[0]]){
						uppointx=coorx[countReflect[0]];
						rtpointx=coorx[countReflect[1]];
						uppointy=coory[countReflect[0]];
						rtpointy=coory[countReflect[1]];
					}
					else {
						uppointx=coorx[countReflect[1]];
						rtpointx=coorx[countReflect[0]];
						uppointy=coory[countReflect[1]];
						rtpointy=coory[countReflect[0]];
					}
					xerr=rtpointx-160;
					yerr=120-rtpointy;
					xerr*=pic2cm;
					yerr*=pic2cm;
					xmsur+=(xerr+ledrt.x);
					ymsur+=(yerr+ledrt.y);
					
					xerr=uppointx-160;
					yerr=120-uppointy;
					xerr*=pic2cm;
					yerr*=pic2cm;
					xmsur+=(xerr+ledup.x);
					ymsur+=(yerr+ledup.y);
				}
			
				else if(coorx[countReflect[0]]<160&&coorx[countReflect[1]]<160){
					if(coory[countReflect[1]]>coory[countReflect[0]]){
						uppointx=coorx[countReflect[0]];
						lfpointx=coorx[countReflect[1]];
						uppointy=coory[countReflect[0]];
						lfpointy=coory[countReflect[1]];
					}
					else {
						uppointx=coorx[countReflect[1]];
						lfpointx=coorx[countReflect[0]];
						uppointy=coory[countReflect[1]];
						lfpointy=coory[countReflect[0]];
					}
					xerr=uppointx-160;
					yerr=120-uppointy;
					xerr*=pic2cm;
					yerr*=pic2cm;
					xmsur+=(xerr+ledup.x);
					ymsur+=(yerr+ledup.y);
					
					xerr=lfpointx-160;
					yerr=120-lfpointy;
					xerr*=pic2cm;
					yerr*=pic2cm;
					xmsur+=(xerr+ledlf.x);
					ymsur+=(yerr+ledlf.y);
				}
			}
		
			else {
				if(coory[countReflect[1]]>120&&coory[countReflect[0]]>120){
						if(coorx[countReflect[1]]>coorx[countReflect[0]]){
							rtpointx=coorx[countReflect[0]];
							lfpointx=coorx[countReflect[1]];
							rtpointy=coory[countReflect[0]];
							lfpointy=coory[countReflect[1]];
						}
						else {
							rtpointx=coorx[countReflect[1]];
							lfpointx=coorx[countReflect[0]];
							rtpointy=coory[countReflect[1]];
							lfpointy=coory[countReflect[0]];
						}
						xerr=lfpointx-160;
						yerr=120-lfpointy;
						xerr*=pic2cm;
						yerr*=pic2cm;
						xmsur+=(xerr+ledlf.x);
						ymsur+=(yerr+ledlf.y);
					
						xerr=rtpointx-160;
						yerr=120-rtpointy;
						xerr*=pic2cm;
						yerr*=pic2cm;
						xmsur+=(xerr+ledrt.x);
						ymsur+=(yerr+ledrt.y);
					}
					
			}
			xmsur/=2;
			ymsur/=2;
		}
	
		else if(utiltnum==1){
			if(coory[countReflect[0]]<120){
				uppointx=coorx[countReflect[0]];
				uppointy=coory[countReflect[0]];
				xerr=uppointx-160;
				yerr=120-uppointy;
				xerr*=pic2cm;
				yerr*=pic2cm;
				xmsur+=(xerr+ledup.x);
				ymsur+=(yerr+ledup.y);
			}
			else if(coorx[countReflect[0]]>160){
				rtpointx=coorx[countReflect[0]];
				rtpointy=coory[countReflect[0]];
				xerr=rtpointx-160;
				yerr=120-rtpointy;
				xerr*=pic2cm;
				yerr*=pic2cm;
				xmsur+=(xerr+ledrt.x);
				ymsur+=(yerr+ledrt.y);
			}
			else{
				lfpointx=coorx[countReflect[0]];
				lfpointy=coory[countReflect[0]];
				xerr=lfpointx-160;
				yerr=120-lfpointy;
				xerr*=pic2cm;
				yerr*=pic2cm;
				xmsur+=(xerr+ledlf.x);
				ymsur+=(yerr+ledlf.y);
			}
				
			
		}
		else{
			
		}
		
		xmsur-=3;
		ymsur-=7;
		
//		xerr=(float)coorx[countReflect[0]]-160;
//		yerr=120-(float)coory[countReflect[0]];
//		xerr*=pic2cm;
//		yerr*=pic2cm;
//		xerr-=3;
//		yerr-=7;
//		xmsur=xerr;
//		ymsur=yerr;
//		xcm=abs((int)xerr);
//		ycm=abs((int)yerr);
		
		xcm=abs((int)(xmsur*10));
		ycm=abs((int)(ymsur*10));

		
		//delay_ms(1000);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);							  
 		ov_sta=0;					//清零帧中断标记
		
		
		
		
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
		LCD_ShowChar(charxpix,0,'m',12,1);
		charxpix-=12;
		LCD_ShowChar(charxpix,0,'m',12,1);
		charxpix-=12;
		if(ycm!=0){
			for(;ycm!=0;charxpix-=12){
				ynum=ycm%10;
				ycm/=10;
				LCD_ShowChar(charxpix,0,'0'+ynum,12,1);
			}
			if(ymsur<0){
				LCD_ShowChar(charxpix,0,'-',12,1);
				charxpix-=12;
			}
		}
		else{
			LCD_ShowChar(charxpix,0,'0',12,1);
			charxpix-=12;
		}
		
		LCD_ShowChar(charxpix,0,',',12,1);
		charxpix-=12;
		
		LCD_ShowChar(charxpix,0,'m',12,1);
		charxpix-=12;
		LCD_ShowChar(charxpix,0,'m',12,1);
		charxpix-=12;
		if(xcm!=0){
			for(;xcm!=0;charxpix-=12){
				xnum=xcm%10;
				xcm/=10;
				LCD_ShowChar(charxpix,0,'0'+xnum,12,1);
			}
			if(xmsur<0){
				LCD_ShowChar(charxpix,0,'-',12,1);
				charxpix-=12;
			}
		}
		else{
			LCD_ShowChar(charxpix,0,'0',12,1);
			charxpix-=12;
		}
		charxpix=228;
		LCD_ShowChar(charxpix,12,')',12,1);
		charxpix-=12;
		if(ymsur>20&&ymsur>=xmsur&&ymsur>=-xmsur){
			LCD_ShowChar(charxpix,12,'B',12,1);
			charxpix-=12;
		}
		else if(ymsur<-20&&ymsur<=xmsur&&ymsur<=-xmsur){
			LCD_ShowChar(charxpix,12,'D',12,1);
			charxpix-=12;
		}
		else if(xmsur>20&&ymsur<=xmsur&&ymsur>=-xmsur){
			LCD_ShowChar(charxpix,12,'C',12,1);
			charxpix-=12;
		}
		else if(xmsur<-20&&ymsur>=xmsur&&ymsur<=-xmsur){
			LCD_ShowChar(charxpix,12,'E',12,1);
			charxpix-=12;
		}
		else{
			LCD_ShowChar(charxpix,12,'A',12,1);
			charxpix-=12;
		}
		LCD_ShowChar(charxpix,12,'(',12,1);
		charxpix-=12;
		if(xmsur<0){
			LCD_ShowChar(charxpix,12,'t',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'f',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'e',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'l',12,1);
			charxpix-=12;
		}
		else{
			LCD_ShowChar(charxpix,12,'t',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'h',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'g',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'i',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'r',12,1);
			charxpix-=12;
		}
		LCD_ShowChar(charxpix,12,',',12,1);
		charxpix-=12;
		if(ymsur>0){
			LCD_ShowChar(charxpix,12,'p',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'u',12,1);
			charxpix-=12;
		}
		else{
			LCD_ShowChar(charxpix,12,'n',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'w',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'o',12,1);
			charxpix-=12;
			LCD_ShowChar(charxpix,12,'d',12,1);
			charxpix-=12;
		}
	
	}	
}	 


		


int main(void)
{
	

	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;
	u8 effect=0;	 
 	u8 i=0;	    
	u8 msgbuf[15];				//消息缓存区
	u8 tm=0; 

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
//	USART1_Configuration( );
	
	LCD_Init();		 //初始化LCD 

	
		POINT_COLOR=RED;			//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"OV7670 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2015/1/18"); 
	LCD_ShowString(30,130,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(30,150,200,16,16,"KEY1:Saturation");
	LCD_ShowString(30,170,200,16,16,"KEY2:Brightness");
	LCD_ShowString(30,190,200,16,16,"KEY_UP:Contrast");
	LCD_ShowString(30,210,200,16,16,"TPAD:Effects");	 
  	LCD_ShowString(30,230,200,16,16,"NRF Init...");
	
//	while(NRF_Check()!=SUCCESS){
//		LCD_ShowString(30,230,200,16,16,"NRF Error!!");
//		delay_ms(200);
//	} 

	LCD_ShowString(30,250,200,16,16,"NRF Init...");
		
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(30,250,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(30,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(30,230,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 	   
	OV7670_Light_Mode(lightmode);
	OV7670_Color_Saturation(saturation);
	OV7670_Brightness(brightness);
	OV7670_Contrast(contrast);
 	OV7670_Special_Effects(0);	 						  
	EXTI8_Init();						//使能定时器捕获
	OV7670_Window_Set(10,174,240,320);	//设置窗口	  
  	OV7670_CS=0;					
	LCD_Clear(BLACK);
	
	
	
	
	
	


	while(1)
	
		
		
		

		
  

		
		camera_refresh();
		
		
		
		
		
		
		
		
//		switch(NRF_Tx_Dat(txbuf))
//    {
//      case MAX_RT:
//        GPIO_SetBits(GPIOC,GPIO_Pin_14); 
//      break;

//      case ERROR:
//				
//      break;

//      case TX_DS:
//        		GPIO_ResetBits(GPIOC,GPIO_Pin_14); 
//      break;  								
//    }	
		
	
 
		
  }		
	









