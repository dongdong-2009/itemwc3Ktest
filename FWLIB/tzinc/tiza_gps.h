#ifndef __TIZA_GPS_H
#define __TIZA_GPS_H
	
	#include "tiza_include.h"

	#include "tiza_rtc.h"
    
	#ifdef GPS_STRUCT_GLOBAL
		#define EXTERN_GPS
	#else
		#define EXTERN_GPS extern
	#endif


	// STATE 
	typedef enum{
		NSS_ANTOK=0,	// ��������
		NSS_ANTOPEN,	// ���߿�·
		NSS_ANTSHORT,	// ���߶�·
		NSS_FAULT		// ��������
	}EV_NSS_STS;
	
	typedef struct
	{
		uint8 Status;		// ��λ״̬  0: ��λ    1:����λ
		uint8 PlanetNum;	// ��������
		uint8 Longitude[5];	// ����(118��52.9276��)	byte0: 01  byte1: 18  byte2: 52  byte3: 92  byte4: 76 (BCD��)
		uint8 EWhemisphere;	// 0: ������  1:������
		uint8 Latitude[4];	// γ��(38��52.9276��)	byte0: 38  byte1: 52  byte2: 92  byte3: 76
		uint8 SNhemisphere;	// 0: �ϰ���  1:������
		RTC_ST Time;		// ת����Ķ�8��ʱ��
		uint8 Altitude[4];	// ���θ߶�((-)1238.3 ��)  byte0: 0x2d ��ֵ, 0x0 ��ֵ  byte1: 12  byte2: 38  byte3: 3(low 4bits)
		uint8 Speed[3];		// �ٶ�(110.5 km/h) byte0: 01  byte1: 10  byte2: 5(low 4bits)
		uint8 Direction[3];	// ����(270.1) 		byte0: 02  byte1: 70  byte2: 1(low 4bits)
	}NSS_INFO;


	EXTERN_GPS uint8 GetNssAntStatus(void);
	EXTERN_GPS uint8 NssInit(void);
	EXTERN_GPS uint8 GetCurrentPosition(NSS_INFO *NssInfo);
	EXTERN_GPS uint8 NssResetStart(void);
	EXTERN_GPS uint8 NssResetComplete(void);	// NssResetStart����500ms��ɵ���NssResetComplete

#endif
