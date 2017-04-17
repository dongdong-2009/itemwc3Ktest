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

//-初始化系统实现打印函数
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


//RS232部分
//初始化IO 串口1 
//bound:波特率
uint8 RS232OpenPort1(uint32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	//-USART_ClearFlag(USART1, USART_FLAG_TC);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{ //-??0?????????????????,???????????
	}
	
	//-USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
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
static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在os下,代表每个节拍的ms数
#define delay_ostickspersec	200
//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
	u32 reload;
#endif
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//SYSTICK使用外部时钟源	 
	fac_us=SYSCLK/8;						//不论是否使用OS,fac_us都需要使用
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
	reload=SYSCLK/8;						//每秒钟的计数次数 单位为K	   
	reload*=1000000/delay_ostickspersec;	//根据delay_ostickspersec设定溢出时间
											//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/delay_ostickspersec;		//代表OS可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
	SysTick->LOAD=reload; 					//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
#else
	fac_ms=(u16)fac_us*1000;				//非OS下,代表每个ms需要的systick时钟数   
#endif
}		

int main(void)
{
	uint8 TestData[200],res;
	uint32 len;
	CAN_MSG TestCanmsg;
	NSS_INFO TestNssInfo;
 
	//-delay_init(168);		  //初始化延时函数
	//-LED_Init();		        //初始化LED端口
	InitSystem();
	//-delay_init(168);  	//时钟初始化
	//-NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	
  /**上面完成了CPU时钟初始化，下面开始外设初始化**/	
	RS232OpenPort(115200);
	
	IoInit();         //IO初始化	
	
	delay_ms(100);
	printf("##### Device start... #####\r\n");
	
	#if 0 > 0
	S_DEBUGF("\n测试CAN模块:");
	DealDebugSend();
	//-PowerSts.WorkSts = NORMAL_WORK;
	DealDebugSend();
	//-InitSTFIFO(&Can1RxFifo,CanRx1Msg,sizeof(CAN_MSG),32);
	//-InitSTFIFO(&Can2RxFifo,CanRx2Msg,sizeof(CAN_MSG),32);
	S_DEBUGF("\n------1M波特率,扩展帧:");
	if(CanOpenPort(1,1000)==0)
	{
		S_DEBUGF("\n----CAN端口1打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口1打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,1000)==0)
	{
		S_DEBUGF("\n----CAN端口2打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口2打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	TestData[0] = 0x02;
	TestData[1] = 0x10;
	TestData[2] = 0x61;
	if(CanSendMessage(1,0x18DA00F1,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道1数据发送成功,ID:0x%x",0x18DA00F1);
		S_DEBUGF("\n1----CAN通道1数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道1数据发送失败.");
	}
	DealDebugSend();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n1----CAN通道2数据条数：%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x06,10);
	if(CanSendMessage(2,0x18000024,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道2数据发送成功,ID:0x%x",0x18000024);
		S_DEBUGF("\n2----CAN通道2数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道2数据发送失败");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n2----CAN通道1数据条数：%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------1M波特率,标准帧:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,1000)==0)
	{
		S_DEBUGF("\n----CAN端口1打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口1打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,1000)==0)
	{
		S_DEBUGF("\n----CAN端口2打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口2打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x11,10);
	if(CanSendMessage(1,0x7e0,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道1数据发送成功,ID:0x%x",0x7e0);
		S_DEBUGF("\n3----CAN通道1数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道1数据发送失败.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n3----CAN通道2数据条数：%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x22,10);
	if(CanSendMessage(2,0x7e1,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道2数据发送成功,ID:0x%x",0x7e1);
		S_DEBUGF("\n4----CAN通道2数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道2数据发送失败");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n4----CAN通道1数据条数：%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------500K波特率,扩展帧:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,500)==0)
	{
		S_DEBUGF("\n----CAN端口1打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口1打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,500)==0)
	{
		S_DEBUGF("\n----CAN端口2打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口2打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x33,10);
	if(CanSendMessage(1,0x18DA00F2,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道1数据发送成功,ID:0x%x",0x18DA00F2);
		S_DEBUGF("\n5----CAN通道1数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道1数据发送失败.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n5----CAN通道2数据条数：%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x44,10);
	if(CanSendMessage(2,0x18000023,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道2数据发送成功,ID:0x%x",0x18000023);
		S_DEBUGF("\n6----CAN通道2数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道2数据发送失败");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n6----CAN通道1数据条数：%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------500K波特率,标准帧:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,500)==0)
	{
		S_DEBUGF("\n----CAN端口1打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口1打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,500)==0)
	{
		S_DEBUGF("\n----CAN端口2打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口2打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x55,10);
	if(CanSendMessage(1,0x7e2,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道1数据发送成功,ID:0x%x",0x7e2);
		S_DEBUGF("\n7----CAN通道1数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道1数据发送失败.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n7----CAN通道2数据条数：%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x66,10);
	if(CanSendMessage(2,0x7e3,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道2数据发送成功,ID:0x%x",0x7e3);
		S_DEBUGF("\n8----CAN通道2数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道2数据发送失败");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n8----CAN通道1数据条数：%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------250K波特率,扩展帧:");
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(1,250)==0)
	{
		S_DEBUGF("\n----CAN端口1打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口1打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,250)==0)
	{
		S_DEBUGF("\n----CAN端口2打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口2打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x77,10);
	if(CanSendMessage(1,0x18DA00F5,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道1数据发送成功,ID:0x%x",0x18DA00F5);
		S_DEBUGF("\n9----CAN通道1数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道1数据发送失败.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n9----CAN通道2数据条数：%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x88,10);
	if(CanSendMessage(2,0x18000022,EXPAND_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道2数据发送成功,ID:0x%x",0x18000022);
		S_DEBUGF("\n10----CAN通道2数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道2数据发送失败");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n10----CAN通道1数据条数：%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	S_DEBUGF("\n------250K波特率,标准帧:");
	if(CanOpenPort(1,250)==0)
	{
		S_DEBUGF("\n----CAN端口1打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口1打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	if(CanOpenPort(2,250)==0)
	{
		S_DEBUGF("\n----CAN端口2打开成功");
	}
	else
	{
		S_DEBUGF("\n----CAN端口2打开失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0x99,10);
	if(CanSendMessage(1,0x7e6,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道1数据发送成功,ID:0x%x",0x7e6);
		S_DEBUGF("\n11----CAN通道1数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道1数据发送失败.");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can2RxFifo);
	S_DEBUGF("\n11----CAN通道2数据条数：%d \n",res);
	if(STFIFOUsed(&Can2RxFifo))
	{
		ReadSTFIFO(&Can2RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN2 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN2接收数据失败");
	}
	DealDebugSend();
	ClearWatchdog();
	memset(TestData,0xaa,10);
	if(CanSendMessage(2,0x7e7,STANDARD_CAN,TestData,8) == 0)
	{
		//S_DEBUGF("\n----CAN通道2数据发送成功,ID:0x%x",0x7e7);
		S_DEBUGF("\n12----CAN通道2数据发送成功");
	}
	else
	{
		S_DEBUGF("\n----CAN通道2数据发送失败");
	}
	DealDebugSend();
	ClearWatchdog();
	res = STFIFOUsed(&Can1RxFifo);
	S_DEBUGF("\n12----CAN通道1数据条数：%d \n",res);
	if(STFIFOUsed(&Can1RxFifo))
	{
		ReadSTFIFO(&Can1RxFifo,(void*)&TestCanmsg);
		//DPrint("\n---CAN1 RECID:0x%x",TestCanmsg.id);
		//DPrint("---DATA: %h\n",TestCanmsg.Data,8);
	}
	else
	{
		S_DEBUGF("\n----CAN1接收数据失败");
	}
	S_DEBUGF("\nCAN模块测试完成!\n");
	DealDebugSend();
	ClearWatchdog();
#endif

#if 0 > 0
	S_DEBUGF("\n测试GPS北斗:");
	DealDebugSend();
	if(NssInit()==0)
	{
		S_DEBUGF("\n----GPS北斗初始化成功");
	}
	else
	{
		S_DEBUGF("\n----GPS北斗初始化失败");
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
				S_DEBUGF("\n----GPS北斗定位信息更新完成.");
				S_DEBUGF("\n经度:%o%o%o%o%o\n;",TestNssInfo.Longitude[0],TestNssInfo.Longitude[1],TestNssInfo.Longitude[2],TestNssInfo.Longitude[3],TestNssInfo.Longitude[4]);
				S_DEBUGF("\n纬度:%o%o%o%o\n",TestNssInfo.Latitude[0],TestNssInfo.Latitude[1],TestNssInfo.Latitude[2],TestNssInfo.Latitude[3]);
				S_DEBUGF("\n海拔:%o%o%o%o\n",TestNssInfo.Altitude[0],TestNssInfo.Altitude[1],TestNssInfo.Altitude[2],TestNssInfo.Altitude[3]);
				S_DEBUGF("\n速度:%o%o%o\n",TestNssInfo.Speed[0],TestNssInfo.Speed[1],TestNssInfo.Speed[2]);
				S_DEBUGF("\n方向:%o%o%o\n",TestNssInfo.Direction[0],TestNssInfo.Direction[1],TestNssInfo.Direction[2]);
				//-S_DEBUGF("\n时间:%d-%d-%d %d:%d:%d\n",TestNssInfo.time.year,TestNssInfo.time.month,TestNssInfo.time.day,TestNssInfo.time.hour,TestNssInfo.time.minute,TestNssInfo.time.second);
				break;
			}
			continue;
		}
		
	}
	S_DEBUGF("\n当前时间:");
	//-PrintRtc();
	DealDebugSend();
	res = GetNssAntStatus();
	if(res == 0)
	{
		S_DEBUGF("\n----GPS北斗天线无故障");
	}
	else if(res == 1)
	{
		S_DEBUGF("\n----GPS北斗天线断路");
	}
	else if(res == 2)
	{
		S_DEBUGF("\n----GPS北斗天线短路");
	}
	S_DEBUGF("\n-----断开GPS天线,输入OK\n");
	DealDebugSend();
//-	WaitInputOK();
	res = GetNssAntStatus();
	if(res == 0)
	{
		S_DEBUGF("\n----GPS北斗天线无故障");
	}
	else if(res == 1)
	{
		S_DEBUGF("\n----GPS北斗天线断路");
	}
	else if(res == 2)
	{
		S_DEBUGF("\n----GPS北斗天线短路");
	}
	S_DEBUGF("\nGPS北斗测试完成!\n");
	DealDebugSend();
#endif
	
#if 1
	S_DEBUGF("\n测试省电模式");
	res = GetExtPowerStatus();
	if(res == 0)
	{
		S_DEBUGF("\n---外部电源没电\n");
	}
	else
	{
		S_DEBUGF("\n---外部电源有电\n");
	}
	S_DEBUGF("\n---断开外部电源,按OK\n");
	DealDebugSend();
	for(len=150000;len>0;len--);
	//WaitInputOK();
	res = GetExtPowerStatus();
	if(res == 0)
	{
		S_DEBUGF("\n---外部电源没电\n");
	}
	else
	{
		S_DEBUGF("\n---外部电源有电\n");
	}
	DealDebugSend();
	S_DEBUGF("\n---断开外设电源.\n");
	ClearWatchdog();
	
	ClearWatchdog();
	len = 0;
	RtcInit();
	while(1)
	{
		
		//-memset(TestData,0x7E,10);
		//-CanSendMessage(1,0x18DA00F9,EXPAND_CAN,TestData,8);
		S_DEBUGF("\n---进入休眠\n");
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
		S_DEBUGF("\n---休眠唤醒\n");
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
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		printf("\r\nCAN模块测试完成!\n");
		ClearWatchdog();
		//-RS232SendData(0x23);
		//-delay_num(10,65530);
		//-RS232SendData(0x44);
		//-delay_num(10,65530);
		
		//-while((USART1->SR&0X40)==0);
		//-USART_SendData(USART1,0x23);
	//-GPIO_ResetBits(GPIOF,GPIO_Pin_9);  //LED0对应引脚GPIOF.9拉低，亮  等同LED0=0;
	//-GPIO_SetBits(GPIOF,GPIO_Pin_10);   //LED1对应引脚GPIOF.10拉高，灭 等同LED1=1;
	//-delay_ms(500);  		   //延时300ms
	//-GPIO_SetBits(GPIOF,GPIO_Pin_9);	   //LED0对应引脚GPIOF.0拉高，灭  等同LED0=1;
	//-GPIO_ResetBits(GPIOF,GPIO_Pin_10); //LED1对应引脚GPIOF.10拉低，亮 等同LED1=0;
	//-delay_ms(500);                     //延时300ms
	}
}


void delay_num(uint8 Num,uint32 Len)
{
	while(Num--)
		while(Len)
			Len--;
}

