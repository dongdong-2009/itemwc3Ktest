#include "includes.h"

//-初始化系统实现打印函数
void delay_num(uint8 Num,uint32 Len);




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
 
	//-delay_init(168);		  //初始化延时函数
	//-LED_Init();		        //初始化LED端口
	InitSystem();
	//-delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	
  /**上面完成了CPU时钟初始化，下面开始外设初始化**/	
	RS232OpenPort(115200);
	
	IoInit();         //IO初始化	
	
	delay_ms(100);
	printf("##### Device start... #####\r\n");
	
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

