#ifndef __TIZA_SYS_H
#define __TIZA_SYS_H

#include "tiza_include.h"

	#ifdef SYS_GLOBAL
		#define EXTERN_SYS
	#else
		#define EXTERN_SYS extern
	#endif
	
	EXTERN_SYS void CPU_IntDis(void);
	EXTERN_SYS void CPU_IntEn(void);
	
	#define OS_ENTER_CRITICAL() CPU_IntDis()
	#define OS_EXIT_CRITICAL()  CPU_IntEn()

	EXTERN_SYS void InitSystem(void);
	EXTERN_SYS void SysTickConfig(uint32 ticks);	// ticks����1����ϣ����tick��������1ms����һ��tick������Ϊ1000
	EXTERN_SYS void SysTick_Handler(void);
	
#endif

