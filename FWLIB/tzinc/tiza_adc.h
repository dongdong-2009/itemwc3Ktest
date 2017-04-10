#ifndef __TIZA_ADC_H
#define __TIZA_ADC_H	

#include "tiza_include.h"

	#ifdef ADC_GLOBAL
		#define EXTERN_ADC
	#else
		#define EXTERN_ADC extern
	#endif

 							   
EXTERN_ADC void AdcInit(void); 						//ADCͨ����ʼ��
EXTERN_ADC uint16  ReadBatAdc(void); 				//��õ��ͨ����ADCֵ 
EXTERN_ADC float  ReadBatVoltage(void); 			//��õ�ص�ѹ ��λ:V

#endif
