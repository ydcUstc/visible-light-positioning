#include "stm32f10x.h"
#include "usart1.h"
u8 RxData;

/***************************************************
* 函数名称    ：void RCC_Configuration()
* 功能描述    ： 复位和时钟控制 配置
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
***********************************************************/
void USART1_RCC_Configuration()
{
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);   //打开GPIOA,AFIO,USART1外设时钟
} 
 
/*********************************************
* 函数名称    ： NVIC_Configuration(void)
* 功能描述    ： NVIC(嵌套中断控制器)配置
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
***********************************************/ 
void USART1_NVIC_Configuration( )
{
      NVIC_InitTypeDef NVIC_InitStructure;    //定义一个中断结构体
 	   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
       NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //通道设置为串口1中断
	   	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
       NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //中断响应优先级0
       NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //打开中断
       NVIC_Init(&NVIC_InitStructure);   //初始化
} 
 
/****************************************
* 函数名称    ：GPIO_Configuration()
* 功能描述    ： GPIO配置
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
****************************************/
void USART1_GPIO_Configuration()
{
       GPIO_InitTypeDef GPIO_InitStructure;    //定义GPIO初始化结构体
 
       //--------将USART1 的TX 配置为复用推挽输出 AF_PP---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;     //管脚位置定义。
       GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  //输出速度2MHz
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   //复用推挽输出 AF_PP
       GPIO_Init(GPIOA,&GPIO_InitStructure);     //A组GPIO初始化
 
       //--------将USART1 的RX 配置为浮空输入 IN_FLOATING---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;   //管脚位置定义
       GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;  //输出速度2MHz 
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //浮空输入 IN_FLOATING                    
       GPIO_Init(GPIOA,&GPIO_InitStructure); //A组GPIO初始化
}
 
/****************************************************
* 函数名称    ：USART1_Configuration( )
* 功能描述    ： 配置USART1数据格式、波特率等参数
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
*******************************************************/
void USART1_Configuration( )
{
  		    
       USART_InitTypeDef USART_InitStructure; //串口设置恢复默认参数

       USART1_RCC_Configuration();
       USART1_NVIC_Configuration( );
       USART1_GPIO_Configuration();
       
       USART_InitStructure.USART_BaudRate = 115200; //波特率9600
       USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长8位
       USART_InitStructure.USART_StopBits = USART_StopBits_1; //1位停止字节
       USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验
       USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //无流控制
       USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      //打开Rx接收和Tx发送功能
       USART_Init(USART1, &USART_InitStructure);  //初始化
       USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // 若接收数据寄存器满，则产生中断
       USART_Cmd(USART1, ENABLE);                     //启动串口                   
                                                   
       //-----如下语句解决第1个字节无法正确发送出去的问题-----// 
       USART_ClearFlag(USART1, USART_FLAG_TC);     // 清发送完成标志
}



/********这是中断服务子程序*************************/

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET) //判断是否为接收中断
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//清除接受中断标志位
		RxData=USART_ReceiveData(USART1);								//读取接受数据
			
	}
	if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET){	//判断是否为发送中断

		USART_ClearITPendingBit(USART1,USART_IT_TXE);//清除发送中断标志位
		
	}
	if(USART_GetITStatus(USART1,USART_IT_TC)!=RESET){	//判断是否为发送完成中断
		USART_ClearITPendingBit(USART1,USART_IT_TC);	//清除发送完成中断标志位
	}

 }	 

 
 



 
 

