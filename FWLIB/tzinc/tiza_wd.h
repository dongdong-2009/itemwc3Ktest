#ifndef __TIZA_WD_H
#define __TIZA_WD_H

#include "tiza_include.h"

	#ifdef WATCH_DOG_GLOBAL
		#define EXTERN_WATCH_DOG
	#else
		#define EXTERN_WATCH_DOG extern
	#endif

	//�ⲿӲ�����Ź�
	EXTERN_WATCH_DOG void EWdtInit(void);
	EXTERN_WATCH_DOG void ClearEWatchdog(void);

	//�ڲ��������Ź�
	EXTERN_WATCH_DOG void WdtInit(uint16 ReloadValue);		// ��λ���ʱ�� = ReloadValue * 1ms
	EXTERN_WATCH_DOG void ClearWatchdog(void);				// ����װ��ReloadValue

	/* ע��: ��Ϊ�ڲ��������Ź�ʹ�õ����ڲ�����ʱ�ӣ����Ƚϵͣ���΢��ЩƯ�ƣ������ڵ���
	         ClearIWatchDog��ʱ�����븴λ��ʱ�������Ĳ�Ҫ̫�࣬���縴λ���Ϊ2�룬
	         �忴�Ź���ʱ���������1.5������ */
	
#endif

