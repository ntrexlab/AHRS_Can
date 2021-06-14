#include "stdafx.h"
#include "CAN_DLL/CAN_Access.h"
#include <windows.h>
#include <cstdint>

#define ACC 0x33
#define GYR 0x34
#define ANG 0x35

void SendCanMessage (CAN_HANDLE h)
{
	long sid = 0;
	unsigned char sdata[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11 };

	// CAN 네트워크로 데이터를 전송한다.
	int ret1 = CAN_Send (h, sid, 8,  (char *)sdata, 0, 0);
	if (!ret1) {
		// 리턴 값이 0이면 전송을 실패한 경우다.
		// 여기서 주의할 것은, USB2CAN 컨버터가 CAN 네트워크에 연결되어 있지 않거나
		// CAN 패킷이 목적지에 도달하지 않아도 성공(1)을 리턴한다는 것이다.
		// 전송 실패가 리턴되는 경우는 잘못된 핸들에 대해 전송을 시도하는 경우이다.
		printf ("Send failed.\n");
	}
}

void RecvCanMessage(CAN_HANDLE h)
{
	char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	long rid;
	int rlen, ext, rtr;
	int16_t acc_x = 0, acc_y = 0, acc_z = 0, gyr_x = 0, gyr_y = 0, gyr_z = 0, ang_x = 0, ang_y = 0, ang_z = 0;
	float accx = 0, accy = 0, accz = 0, gyrx =0 , gyry=0,gyrz=0,angx=0, angy=0, angz=0;


	// 데이터를 수신한다.
	int ret = CAN_Recv(h, &rid, &rlen, rdata, &ext, &rtr);
	if (ret) {
		// 수신된 메시지가 있다면 리턴 값은 1이다. 
		// 수신된 메시지가 없다면 리턴 값은 0이다.
		// 또한, 잘못된 핸들에 대해 수신을 시도할 경우에도 리턴 값은 0이다.


		switch (rdata[1])

		{
		case ACC:

			acc_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			acc_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			acc_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);

			accx = acc_x / 1000.0;
			accy = acc_y / 1000.0;
			accz = acc_z / 1000.0;

			printf("ACC는 : %+.2f %+.2f %+.2f \n", accx, accy, accz);

			break;

		case GYR:


			gyr_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			gyr_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			gyr_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);


			gyrx = gyr_x / 10.0;
			gyry = gyr_y / 10.0;
			gyrz = gyr_z / 10.0;


			printf("GYR는  : %+.2f %+.2f %+.2f \n", gyrx, gyry, gyrz);
			break;

		case ANG:


			ang_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			ang_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			ang_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);

			angx = ang_x / 100.0;
			angy = ang_y / 100.0;
			angz = ang_z / 100.0;

			printf("ANG는 : %+.2f %+.2f %+.2f  \n", angx, angy, angz);

			break;
		}

		printf("\n");


		/*
		printf ("Recv: [%d] %02X %02X %02X %02X %02X %02X %02X %02X (%d)\n",
			rid, (int)(unsigned char)rdata[0], (int)(unsigned char)rdata[1],
			(int)(unsigned char)rdata[2], (int)(unsigned char)rdata[3],
			(int)(unsigned char)rdata[4], (int)(unsigned char)rdata[5],
			(int)(unsigned char)rdata[6], (int)(unsigned char)rdata[7],
			rlen);
		*/
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	// PC에 연결된 USB2CAN(FIFO) 컨버터를 모두 검색한다.
	// 여기서 사용 중인 USB2CAN(FIFO) 컨버터는 검색에서 제외된다.
	int noDevice = CAN_Fifo_ScanSerialNumber ();
	if (noDevice <= 0) return -1;

	// 검색된 시리얼 넘버를 화면에 표시한다.
	for (int i=0; i<noDevice; i++) {
		const char *serialNumber = CAN_Fifo_GetSerialNumber (i);
		printf ("%s\n", serialNumber);
	}

	// 첫 번째 시리얼 넘버로 USB2CAN(FIFO) 핸들을 얻어온다.
	CAN_HANDLE h = CAN_OpenFifo (CAN_Fifo_GetSerialNumber (0));

	if (h < 0) { 
		printf ("USB2CAN open failed.\n");
		return -1;
	}

	// 메시지가 수신되었을 때 이벤트를 받기위한 Event 핸들을 생성한다.
	HANDLE hEvent = CreateEvent (NULL, false, false, ""); 
	
	// USB2CAN(FIFO) 핸들에 이벤트 핸들을 등록한다.
	CAN_SetRxEventNotification (h, hEvent);

	// USB2CAN 컨버터를 오픈하면 전송모드는 0(전송안함)으로 설정되어 있다.
	// 이를 1(Binary) 모드로 설정한다.
	CAN_SetTransferMode (h, 1);
	
	Sleep (1000);

	for (int i=0; i<1000; i++) {
		SendCanMessage (h);

		int rx_count = CAN_CountRxQueue (h);
		if (rx_count > 0) {
			// Rx queue에 메시지가 들어와 있다면, 메시지를 즉시 읽는다.
			RecvCanMessage (h);
		}
		else {
			// 메시지가 수신되었다는 이벤트가 발생할 때까지 또는 1초 동안 대기한다.
			DWORD dwWaitResult = WaitForSingleObject (hEvent, 1000); 
			if (dwWaitResult == WAIT_OBJECT_0) {
				RecvCanMessage (h);
			}
		}
	}

	CAN_Close (h);

	CloseHandle(hEvent);

	getchar ();

	return 0;
}

