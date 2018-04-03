#include "stm32f10x.h"
#include "usart1.h"
u8 RxData;

/***************************************************
* ��������    ��void RCC_Configuration()
* ��������    �� ��λ��ʱ�ӿ��� ����
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
***********************************************************/
void USART1_RCC_Configuration()
{
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);   //��GPIOA,AFIO,USART1����ʱ��
} 
 
/*********************************************
* ��������    �� NVIC_Configuration(void)
* ��������    �� NVIC(Ƕ���жϿ�����)����
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
***********************************************/ 
void USART1_NVIC_Configuration( )
{
      NVIC_InitTypeDef NVIC_InitStructure;    //����һ���жϽṹ��
 	   	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
       NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //ͨ������Ϊ����1�ж�
	   	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
       NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�ж���Ӧ���ȼ�0
       NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //���ж�
       NVIC_Init(&NVIC_InitStructure);   //��ʼ��
} 
 
/****************************************
* ��������    ��GPIO_Configuration()
* ��������    �� GPIO����
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
****************************************/
void USART1_GPIO_Configuration()
{
       GPIO_InitTypeDef GPIO_InitStructure;    //����GPIO��ʼ���ṹ��
 
       //--------��USART1 ��TX ����Ϊ����������� AF_PP---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;     //�ܽ�λ�ö��塣
       GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  //����ٶ�2MHz
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   //����������� AF_PP
       GPIO_Init(GPIOA,&GPIO_InitStructure);     //A��GPIO��ʼ��
 
       //--------��USART1 ��RX ����Ϊ�������� IN_FLOATING---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;   //�ܽ�λ�ö���
       GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;  //����ٶ�2MHz 
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //�������� IN_FLOATING                    
       GPIO_Init(GPIOA,&GPIO_InitStructure); //A��GPIO��ʼ��
}
 
/****************************************************
* ��������    ��USART1_Configuration( )
* ��������    �� ����USART1���ݸ�ʽ�������ʵȲ���
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
*******************************************************/
void USART1_Configuration( )
{
  		    
       USART_InitTypeDef USART_InitStructure; //�������ûָ�Ĭ�ϲ���

       USART1_RCC_Configuration();
       USART1_NVIC_Configuration( );
       USART1_GPIO_Configuration();
       
       USART_InitStructure.USART_BaudRate = 115200; //������9600
       USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�8λ
       USART_InitStructure.USART_StopBits = USART_StopBits_1; //1λֹͣ�ֽ�
       USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��
       USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //��������
       USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      //��Rx���պ�Tx���͹���
       USART_Init(USART1, &USART_InitStructure);  //��ʼ��
       USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // ���������ݼĴ�������������ж�
       USART_Cmd(USART1, ENABLE);                     //��������                   
                                                   
       //-----�����������1���ֽ��޷���ȷ���ͳ�ȥ������-----// 
       USART_ClearFlag(USART1, USART_FLAG_TC);     // �巢����ɱ�־
}



/********�����жϷ����ӳ���*************************/

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET) //�ж��Ƿ�Ϊ�����ж�
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//��������жϱ�־λ
		RxData=USART_ReceiveData(USART1);								//��ȡ��������
			
	}
	if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET){	//�ж��Ƿ�Ϊ�����ж�

		USART_ClearITPendingBit(USART1,USART_IT_TXE);//��������жϱ�־λ
		
	}
	if(USART_GetITStatus(USART1,USART_IT_TC)!=RESET){	//�ж��Ƿ�Ϊ��������ж�
		USART_ClearITPendingBit(USART1,USART_IT_TC);	//�����������жϱ�־λ
	}

 }	 

 
 



 
 

