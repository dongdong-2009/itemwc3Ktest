#include "includes.h"

//-��ʼ��ϵͳʵ�ִ�ӡ����
void delay_num(uint8 Num,uint32 Len);




//RS232����
//��ʼ��IO ����1 
//bound:������
uint8 RS232OpenPort1(uint32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	//-USART_ClearFlag(USART1, USART_FLAG_TC);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{ //-??0?????????????????,???????????
	}
	
	//-USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	return 0;
}

static void delay_ms(uint16 time)
{ 
  uint32 tmp;// = time * 21;
  while(time--)
  {
    tmp = 21000;
    while(tmp--)
    {
      __NOP();
      __NOP();
      __NOP();
      __NOP();
    }
  }
}

#define SYSTEM_SUPPORT_OS 1
static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��os��,����ÿ�����ĵ�ms��
#define delay_ostickspersec	200
//��ʼ���ӳٺ���
//��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��Ƶ��
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	u32 reload;
#endif
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//SYSTICKʹ���ⲿʱ��Դ	 
	fac_us=SYSCLK/8;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	reload=SYSCLK/8;						//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/delay_ostickspersec;	//����delay_ostickspersec�趨���ʱ��
											//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/delay_ostickspersec;		//����OS������ʱ�����ٵ�λ	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//����SYSTICK�ж�
	SysTick->LOAD=reload; 					//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //����SYSTICK
#else
	fac_ms=(u16)fac_us*1000;				//��OS��,����ÿ��ms��Ҫ��systickʱ����   
#endif
}		

int main(void)
{
 
	//-delay_init(168);		  //��ʼ����ʱ����
	//-LED_Init();		        //��ʼ��LED�˿�
	InitSystem();
	//-delay_init(168);  	//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	
  /**���������CPUʱ�ӳ�ʼ�������濪ʼ�����ʼ��**/	
	RS232OpenPort(115200);
	
	IoInit();         //IO��ʼ��	
	
	delay_ms(100);
	printf("##### Device start... #####\r\n");
	
	while(1)
	{
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		printf("\r\nCANģ��������!\n");
		
		//-RS232SendData(0x23);
		//-delay_num(10,65530);
		//-RS232SendData(0x44);
		//-delay_num(10,65530);
		
		//-while((USART1->SR&0X40)==0);
		//-USART_SendData(USART1,0x23);
	//-GPIO_ResetBits(GPIOF,GPIO_Pin_9);  //LED0��Ӧ����GPIOF.9���ͣ���  ��ͬLED0=0;
	//-GPIO_SetBits(GPIOF,GPIO_Pin_10);   //LED1��Ӧ����GPIOF.10���ߣ��� ��ͬLED1=1;
	//-delay_ms(500);  		   //��ʱ300ms
	//-GPIO_SetBits(GPIOF,GPIO_Pin_9);	   //LED0��Ӧ����GPIOF.0���ߣ���  ��ͬLED0=1;
	//-GPIO_ResetBits(GPIOF,GPIO_Pin_10); //LED1��Ӧ����GPIOF.10���ͣ��� ��ͬLED1=0;
	//-delay_ms(500);                     //��ʱ300ms
	}
}


void delay_num(uint8 Num,uint32 Len)
{
	while(Num--)
		while(Len)
			Len--;
}

