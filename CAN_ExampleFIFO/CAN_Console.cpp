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

	// CAN ��Ʈ��ũ�� �����͸� �����Ѵ�.
	int ret1 = CAN_Send (h, sid, 8,  (char *)sdata, 0, 0);
	if (!ret1) {
		// ���� ���� 0�̸� ������ ������ ����.
		// ���⼭ ������ ����, USB2CAN �����Ͱ� CAN ��Ʈ��ũ�� ����Ǿ� ���� �ʰų�
		// CAN ��Ŷ�� �������� �������� �ʾƵ� ����(1)�� �����Ѵٴ� ���̴�.
		// ���� ���а� ���ϵǴ� ���� �߸��� �ڵ鿡 ���� ������ �õ��ϴ� ����̴�.
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


	// �����͸� �����Ѵ�.
	int ret = CAN_Recv(h, &rid, &rlen, rdata, &ext, &rtr);
	if (ret) {
		// ���ŵ� �޽����� �ִٸ� ���� ���� 1�̴�. 
		// ���ŵ� �޽����� ���ٸ� ���� ���� 0�̴�.
		// ����, �߸��� �ڵ鿡 ���� ������ �õ��� ��쿡�� ���� ���� 0�̴�.


		switch (rdata[1])

		{
		case ACC:

			acc_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			acc_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			acc_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);

			accx = acc_x / 1000.0;
			accy = acc_y / 1000.0;
			accz = acc_z / 1000.0;

			printf("ACC�� : %+.2f %+.2f %+.2f \n", accx, accy, accz);

			break;

		case GYR:


			gyr_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			gyr_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			gyr_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);


			gyrx = gyr_x / 10.0;
			gyry = gyr_y / 10.0;
			gyrz = gyr_z / 10.0;


			printf("GYR��  : %+.2f %+.2f %+.2f \n", gyrx, gyry, gyrz);
			break;

		case ANG:


			ang_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			ang_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			ang_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);

			angx = ang_x / 100.0;
			angy = ang_y / 100.0;
			angz = ang_z / 100.0;

			printf("ANG�� : %+.2f %+.2f %+.2f  \n", angx, angy, angz);

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
	// PC�� ����� USB2CAN(FIFO) �����͸� ��� �˻��Ѵ�.
	// ���⼭ ��� ���� USB2CAN(FIFO) �����ʹ� �˻����� ���ܵȴ�.
	int noDevice = CAN_Fifo_ScanSerialNumber ();
	if (noDevice <= 0) return -1;

	// �˻��� �ø��� �ѹ��� ȭ�鿡 ǥ���Ѵ�.
	for (int i=0; i<noDevice; i++) {
		const char *serialNumber = CAN_Fifo_GetSerialNumber (i);
		printf ("%s\n", serialNumber);
	}

	// ù ��° �ø��� �ѹ��� USB2CAN(FIFO) �ڵ��� ���´�.
	CAN_HANDLE h = CAN_OpenFifo (CAN_Fifo_GetSerialNumber (0));

	if (h < 0) { 
		printf ("USB2CAN open failed.\n");
		return -1;
	}

	// �޽����� ���ŵǾ��� �� �̺�Ʈ�� �ޱ����� Event �ڵ��� �����Ѵ�.
	HANDLE hEvent = CreateEvent (NULL, false, false, ""); 
	
	// USB2CAN(FIFO) �ڵ鿡 �̺�Ʈ �ڵ��� ����Ѵ�.
	CAN_SetRxEventNotification (h, hEvent);

	// USB2CAN �����͸� �����ϸ� ���۸��� 0(���۾���)���� �����Ǿ� �ִ�.
	// �̸� 1(Binary) ���� �����Ѵ�.
	CAN_SetTransferMode (h, 1);
	
	Sleep (1000);

	for (int i=0; i<1000; i++) {
		SendCanMessage (h);

		int rx_count = CAN_CountRxQueue (h);
		if (rx_count > 0) {
			// Rx queue�� �޽����� ���� �ִٸ�, �޽����� ��� �д´�.
			RecvCanMessage (h);
		}
		else {
			// �޽����� ���ŵǾ��ٴ� �̺�Ʈ�� �߻��� ������ �Ǵ� 1�� ���� ����Ѵ�.
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

