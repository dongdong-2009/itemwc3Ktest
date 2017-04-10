#ifndef __TIZA_SPI_FLASH_H
#define __TIZA_SPI_FLASH_H

#include "tiza_include.h"

	#ifdef SPI_FLASH_GLOBAL
		#define EXTERN_SPI_FLASH
	#else
		#define EXTERN_SPI_FLASH extern
	#endif

// һ��SECTOR��СΪ4096
#define FLASH_SECTOR_SIZE	4096


EXTERN_SPI_FLASH uint8 FlashInit(void);
EXTERN_SPI_FLASH uint8 FlashWrite(uint32 WriteAddr,uint8* pBuffer,uint16 NumByteToWrite);	//��������д�����Ƿ����
EXTERN_SPI_FLASH uint8 FlashRead(uint32 ReadAddr, uint8* pBuffer, uint16 NumByteToRead);   //��ȡflash
EXTERN_SPI_FLASH uint8 FlashEraseChip(void);    	  		//��Ƭ�������ȴ�ʱ�䳤
EXTERN_SPI_FLASH uint8 FlashEraseSector(uint16 SectorNum);	//��������������һ������������ʱ��:150ms

EXTERN_SPI_FLASH uint8 FlashPowerDown(void);        	//�������ģʽ
EXTERN_SPI_FLASH uint8 FlashWakeUp(void);				//����


#endif
