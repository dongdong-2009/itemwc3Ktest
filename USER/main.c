#include "includes.h"




typedef struct{
	uint32 id;
	uint8  data[8]; 
	uint8  flag;
}CAN_MSG;
#define STANDARD_CAN	1
#define EXPAND_CAN 		2
CAN_MSG Can1RxFifo;
CAN_MSG Can2RxFifo;

#define S_DEBUGF printf

//-��ʼ��ϵͳʵ�ִ�ӡ����
void delay_num(uint8 Num,uint32 Len);

/**weichai**/
void CanRecMessage(uint8 port,uint32 CanId,uint8* Dat)
{
	int i;
	
	printf("can port= %u can id = 0x%08X\r\n", port, CanId);
	printf("recv data: ");
	if(port == 1){//CAN1
		Can1RxFifo.id = 1;
		for(i=0; i < 8; i++){
			Can1RxFifo.data[i] = (uint8)*Dat;
			printf("%02X", (uint8)*Dat);
			Dat++;
		}
		printf("\r\n");
		Can1RxFifo.flag = 1;
	}
	else if(port == 2){//CAN2
		Can2RxFifo.id = 1;
		for(i=0; i < 8; i++){
			Can2RxFifo.data[i] = (uint8)*Dat;
			printf("%02X", (uint8)*Dat);
			Dat++;
		}
		printf("\r\n");
		Can2RxFifo.flag = 1;	
	}
}


 void DealDebugSend(){
	 
 }
uint8 STFIFOUsed(CAN_MSG* canmsg){
	if(canmsg->flag){
		canmsg->flag = 0;
		return 1;
	}
	else{
		return 0;
	}
}
void ReadSTFIFO(CAN_MSG* source, CAN_MSG* target){
	target = source;
}


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
	uint8 TestData[200],res;
	uint32 len;
	CAN_MSG TestCanmsg;
	NSS_INFO TestNssInfo;
 
	//-delay_init(168);		  //��ʼ����ʱ����
	//-LED_Init();		        //��ʼ��LED�˿�
	InitSystem();
	//-delay_init(168);  	//ʱ�ӳ�ʼ��
	//-NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	
  /**���������CPUʱ�ӳ�ʼ�������濪ʼ�����ʼ��**/	
	RS232OpenPort(115200);
	
	IoInit();         //IO��ʼ��	
	
	delay_ms(100);
	printf("##### Device start... #####\r\n");
	
	#if 0 > 0
	S_DEBUGF("\n����CANģ��:");
	DealDebugSend();
	//-PowerSts.WorkSts = NORMAL_WORK;
	DealDebugSend();
	//-InitSTFIFO(&Can1RxFifo,CanRx1Msg,sizeof(CAN_MSG),32);
	//-InitSTFIFO(&Can2RxFifo,CanRx2Msg,sizeof(CAN_MSG),32);
	S_DEBUGF("\n------1M������,��չ֡:");
	if(CanOpenPort(1,1000)==0)
	{
		S_DEBUGF("\n----CAN�˿�1�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�1��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,1000)==0)
	{
		S_DEBUGF("\n----CAN�˿�2�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�2��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	TestData[0] = 0x02;
	TestData[1] = 0x10;
	TestData[2] = 0x61;
	if(CanSendMessage(1,0x18DA00F1,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��1���ݷ��ͳɹ�,ID:0x%x",0x18DA00F1);
		S_DEBUGF("\n1----CANͨ��1���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��1���ݷ���ʧ��.");
	}
	DealDebugSend();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n1----CANͨ��2����������%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x06,10);
	if(CanSendMessage(2,0x18000024,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��2���ݷ��ͳɹ�,ID:0x%x",0x18000024);
		S_DEBUGF("\n2----CANͨ��2���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��2���ݷ���ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n2----CANͨ��1����������%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------1M������,��׼֡:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,1000)==0)
	{
		S_DEBUGF("\n----CAN�˿�1�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�1��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,1000)==0)
	{
		S_DEBUGF("\n----CAN�˿�2�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�2��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x11,10);
	if(CanSendMessage(1,0x7e0,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��1���ݷ��ͳɹ�,ID:0x%x",0x7e0);
		S_DEBUGF("\n3----CANͨ��1���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��1���ݷ���ʧ��.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n3----CANͨ��2����������%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x22,10);
	if(CanSendMessage(2,0x7e1,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��2���ݷ��ͳɹ�,ID:0x%x",0x7e1);
		S_DEBUGF("\n4----CANͨ��2���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��2���ݷ���ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n4----CANͨ��1����������%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------500K������,��չ֡:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,500)==0)
	{
		S_DEBUGF("\n----CAN�˿�1�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�1��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,500)==0)
	{
		S_DEBUGF("\n----CAN�˿�2�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�2��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x33,10);
	if(CanSendMessage(1,0x18DA00F2,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��1���ݷ��ͳɹ�,ID:0x%x",0x18DA00F2);
		S_DEBUGF("\n5----CANͨ��1���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��1���ݷ���ʧ��.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n5----CANͨ��2����������%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x44,10);
	if(CanSendMessage(2,0x18000023,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��2���ݷ��ͳɹ�,ID:0x%x",0x18000023);
		S_DEBUGF("\n6----CANͨ��2���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��2���ݷ���ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n6----CANͨ��1����������%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------500K������,��׼֡:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,500)==0)
	{
		S_DEBUGF("\n----CAN�˿�1�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�1��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,500)==0)
	{
		S_DEBUGF("\n----CAN�˿�2�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�2��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x55,10);
	if(CanSendMessage(1,0x7e2,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��1���ݷ��ͳɹ�,ID:0x%x",0x7e2);
		S_DEBUGF("\n7----CANͨ��1���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��1���ݷ���ʧ��.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n7----CANͨ��2����������%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x66,10);
	if(CanSendMessage(2,0x7e3,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��2���ݷ��ͳɹ�,ID:0x%x",0x7e3);
		S_DEBUGF("\n8----CANͨ��2���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��2���ݷ���ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n8----CANͨ��1����������%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------250K������,��չ֡:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,250)==0)
	{
		S_DEBUGF("\n----CAN�˿�1�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�1��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,250)==0)
	{
		S_DEBUGF("\n----CAN�˿�2�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�2��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x77,10);
	if(CanSendMessage(1,0x18DA00F5,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��1���ݷ��ͳɹ�,ID:0x%x",0x18DA00F5);
		S_DEBUGF("\n9----CANͨ��1���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��1���ݷ���ʧ��.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n9----CANͨ��2����������%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x88,10);
	if(CanSendMessage(2,0x18000022,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��2���ݷ��ͳɹ�,ID:0x%x",0x18000022);
		S_DEBUGF("\n10----CANͨ��2���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��2���ݷ���ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n10----CANͨ��1����������%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------250K������,��׼֡:");
	if(CanOpenPort(1,250)==0)
	{
		S_DEBUGF("\n----CAN�˿�1�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�1��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,250)==0)
	{
		S_DEBUGF("\n----CAN�˿�2�򿪳ɹ�");
	}
	else
	{
		S_DEBUGF("\n----CAN�˿�2��ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x99,10);
	if(CanSendMessage(1,0x7e6,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��1���ݷ��ͳɹ�,ID:0x%x",0x7e6);
		S_DEBUGF("\n11----CANͨ��1���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��1���ݷ���ʧ��.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n11----CANͨ��2����������%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2��������ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0xaa,10);
	if(CanSendMessage(2,0x7e7,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CANͨ��2���ݷ��ͳɹ�,ID:0x%x",0x7e7);
		S_DEBUGF("\n12----CANͨ��2���ݷ��ͳɹ�");
	}
	else
	{
		S_DEBUGF("\n----CANͨ��2���ݷ���ʧ��");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n12----CANͨ��1����������%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1��������ʧ��");
	}
	S_DEBUGF("\nCANģ��������!\n");
	DealDebugSend();
	ClearWatchdog();
#endif

#if 0 > 0
	S_DEBUGF("\n����GPS����:");
	DealDebugSend();
	if(NssInit()==0)
	{
		S_DEBUGF("\n----GPS������ʼ���ɹ�");
	}
	else
	{
		S_DEBUGF("\n----GPS������ʼ��ʧ��");
	}
	//-PrintRtc();
	DealDebugSend();
	while(1)
	{
		ClearWatchdog();
		//-if(TimeDelay == 0)
		{
			//-TimeDelay = 10;
			res = GetCurrentPosition(&TestNssInfo);
			S_DEBUGF("\n res =%d,TestNssInfo.Status = %d",res,TestNssInfo.Status);
			DealDebugSend();
			if((res == 0)&&(TestNssInfo.Status == 0))
			{
				S_DEBUGF("\n----GPS������λ��Ϣ�������.");
				S_DEBUGF("\n����:%o%o%o%o%o\n;",TestNssInfo.Longitude[0],TestNssInfo.Longitude[1],TestNssInfo.Longitude[2],TestNssInfo.Longitude[3],TestNssInfo.Longitude[4]);
				S_DEBUGF("\nγ��:%o%o%o%o\n",TestNssInfo.Latitude[0],TestNssInfo.Latitude[1],TestNssInfo.Latitude[2],TestNssInfo.Latitude[3]);
				S_DEBUGF("\n����:%o%o%o%o\n",TestNssInfo.Altitude[0],TestNssInfo.Altitude[1],TestNssInfo.Altitude[2],TestNssInfo.Altitude[3]);
				S_DEBUGF("\n�ٶ�:%o%o%o\n",TestNssInfo.Speed[0],TestNssInfo.Speed[1],TestNssInfo.Speed[2]);
				S_DEBUGF("\n����:%o%o%o\n",TestNssInfo.Direction[0],TestNssInfo.Direction[1],TestNssInfo.Direction[2]);
				//-S_DEBUGF("\nʱ��:%d-%d-%d %d:%d:%d\n",TestNssInfo.time.year,TestNssInfo.time.month,TestNssInfo.time.day,TestNssInfo.time.hour,TestNssInfo.time.minute,TestNssInfo.time.second);
				break;
			}
			continue;
		}
		
	}
	S_DEBUGF("\n��ǰʱ��:");
	//-PrintRtc();
	DealDebugSend();
	res = GetNssAntStatus();
	if(res == 0)
	{
		S_DEBUGF("\n----GPS���������޹���");
	}
	else if(res == 1)
	{
		S_DEBUGF("\n----GPS�������߶�·");
	}
	else if(res == 2)
	{
		S_DEBUGF("\n----GPS�������߶�·");
	}
	S_DEBUGF("\n-----�Ͽ�GPS����,����OK\n");
	DealDebugSend();
//-	WaitInputOK();
	res = GetNssAntStatus();
	if(res == 0)
	{
		S_DEBUGF("\n----GPS���������޹���");
	}
	else if(res == 1)
	{
		S_DEBUGF("\n----GPS�������߶�·");
	}
	else if(res == 2)
	{
		S_DEBUGF("\n----GPS�������߶�·");
	}
	S_DEBUGF("\nGPS�����������!\n");
	DealDebugSend();
#endif
	
#if 1
	S_DEBUGF("\n����ʡ��ģʽ");
	res = GetExtPowerStatus();
	if(res == 0)
	{
		S_DEBUGF("\n---�ⲿ��Դû��\n");
	}
	else
	{
		S_DEBUGF("\n---�ⲿ��Դ�е�\n");
	}
	S_DEBUGF("\n---�Ͽ��ⲿ��Դ,��OK\n");
	DealDebugSend();
	for(len=150000;len>0;len--);
	//WaitInputOK();
	res = GetExtPowerStatus();
	if(res == 0)
	{
		S_DEBUGF("\n---�ⲿ��Դû��\n");
	}
	else
	{
		S_DEBUGF("\n---�ⲿ��Դ�е�\n");
	}
	DealDebugSend();
	S_DEBUGF("\n---�Ͽ������Դ.\n");
	ClearWatchdog();
	
	ClearWatchdog();
	len = 0;
	RtcInit();
	while(1)
	{
		
		//-memset(TestData,0x7E,10);
		//-CanSendMessage(1,0x18DA00F9,EXPAND_CAN,TestData,8);
		S_DEBUGF("\n---��������\n");
		DealDebugSend();
		
		ClearWatchdog();
		RtcSetAlarm(1);
		SystemPowerDown();
		CpuPowerDown();
		//IoInit();
		RS232ClosePort();
		RS232OpenPort(115200);
		ClearWatchdog();
		//RS232Init();
		//RtcInit();
		//PrintRtc();
		S_DEBUGF("\n---���߻���\n");
		DealDebugSend();
		ClearWatchdog();
		AdcInit();
		res = BattCoulometry();
		printf("\r\nBattCoulometry = %d\n", res);
		if(len > 30)
		{
			SystemWakeup();
			break;;
		}
		len ++;
	}
	
	

	
#endif	
	
#if 1
	AdcInit();
	res = BattCoulometry();
	printf("\r\nBattCoulometry = %d\n", res);
	
#endif

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
		ClearWatchdog();
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

