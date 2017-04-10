#ifndef __SPI_H
#define __SPI_H

#include "tiza_include.h"

	#ifdef SPI_GLOBAL
		#define EXTERN_SPI
	#else
		#define EXTERN_SPI extern
	#endif

#define SPI_CS_ENABLE 	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_CS_DISABLE 	GPIO_SetBits(GPIOA, GPIO_Pin_4)


EXTERN_SPI void SPI1_Init(void);			 //��ʼ��SPI1��
EXTERN_SPI void SPI1_SetSpeed(uint8 SpeedSet); //����SPI1�ٶ�   
EXTERN_SPI uint8 SPI1_ReadWriteByte(uint8 TxData);//SPI1���߶�дһ���ֽ�

#endif

