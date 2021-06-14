# AHRS_USB_to_Can
GitHub -> [ntrexlab/AHRS_Can](https://github.com/ntrexlab/AHRS_Can)
***
## **Manual**
***
* ### 1.HardWare
    * #### 사용모델
         - AHRS - [MW_AHRSv1](http://www.devicemart.co.kr/goods/view?no=1310790)
         - USB2CAN(FIFO) - [USB2CAN(FIFO) v2](http://www.devicemart.co.kr/goods/view?no=1323536)
         - DSub 커넥터 - [DS1033_09M(MALE)](http://www.devicemart.co.kr/goods/view?no=286)
         - ESK SUB-D9-Connector - [ESK SUB-D9-Connector](http://www.eskorea.net/html/product/?prodNum=1&menuNum=1&pid=010701)

       ![회로도](https://user-images.githubusercontent.com/85467544/121839915-0e6e0d00-cd16-11eb-9a7f-5b6b590b6b12.png)

* ### 2.Tool
    * #### 사용버전
        - Visual Studio -Visual Studio 2019[설치경로](http://visualstudio.microsoft.com/ko/downloads/)
        - USB2CAN_UI_v2 [설치경로-관련자료](http://www.devicemart.co.kr/goods/view?no=1323536#goods_file)
***

* ### 3. 개발 순서
    #### 1. AHRS_USB_to_CAN을 제작한다. 
    #### 2. USB2CAN_UI_v2을 설치한다.
    #### 3. AHRS_USB_to_CAN를 PC에 연결 후 USB2CAN_UI_v2을 실행한다.
    #### 4. Connect 버튼(1)을 클릭한다.
    #### 5. USB_to_CAN(FIFO) 확인 후 Ok 버튼(2)
    ![ui11](https://user-images.githubusercontent.com/85467544/121843979-33667e00-cd1e-11eb-95e3-0cf4194ff5c2.png)
    #### 6. [예제](https://github.com/ntrexlab/AHRS_Can/tree/main/CAN_ExampleFIFO)를 다운 받는다.
    #### 7. Visual Studio로 CAN_ExampleFIFO 폴더를 연다.
    #### 8. Source Files의 CAN_Console.cpp 파일을 실행한다.
     ```c

    void RecvCanMessage(CAN_HANDLE h){
	    char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	    long rid;
	    int rlen, ext, rtr;

        // 데이터를 수신한다.
	    int ret = CAN_Recv(h, &rid, &rlen, rdata, &ext, &rtr);
	    if (ret) {
		    // 수신된 메시지가 있다면 리턴 값은 1이다. 
		    // 수신된 메시지가 없다면 리턴 값은 0이다.
		    // 또한, 잘못된 핸들에 대해 수신을 시도할 경우에도 리턴 값은 0이다.

            printf ("Recv: [%d] %02X %02X %02X %02X %02X %02X %02X %02X (%d)\n",
			    rid, (int)(unsigned char)rdata[0], (int)(unsigned char)rdata[1],
			    (int)(unsigned char)rdata[2], (int)(unsigned char)rdata[3],
			    (int)(unsigned char)rdata[4], (int)(unsigned char)rdata[5],
			    (int)(unsigned char)rdata[6], (int)(unsigned char)rdata[7],
			    rlen);

    ```
    ![can_Test](https://user-images.githubusercontent.com/85467544/121844600-3150ef00-cd1f-11eb-9403-b10988ccb184.gif)
   
    #### 9. 코드를 수정한다. (RecvCanMessage)
    ```c
    //추가
    #define ACC 0x33 
    #define GYR 0x34 
    #define ANG 0x35 
    // 4. Datasheet 참고 설정

    void RecvCanMessage(CAN_HANDLE h){
	    char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	    long rid;
	    int rlen, ext, rtr;

        //추가
        int16_t acc_x = 0, acc_y = 0, acc_z = 0, gyr_x = 0, gyr_y = 0, gyr_z = 0, ang_x = 0, ang_y = 0, ang_z = 0; 
        //배열 값을 int16_t 형식으로 받기 위해 선언.
	    float accx = 0, accy = 0, accz = 0, gyrx =0 , gyry=0,gyrz=0,angx=0, angy=0, angz=0;
        // 최종 값을 받기 위해 선언.

        // 데이터를 수신한다.
	    int ret = CAN_Recv(h, &rid, &rlen, rdata, &ext, &rtr);
	    if (ret) {
		    // 수신된 메시지가 있다면 리턴 값은 1이다. 
		    // 수신된 메시지가 없다면 리턴 값은 0이다.
		    // 또한, 잘못된 핸들에 대해 수신을 시도할 경우에도 리턴 값은 0이다.

        //추가
        switch (rdata[1]){
            // rdata[1] -> index (4. DataSheet 참고 설정)
		    case ACC:

			    acc_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			    acc_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			    acc_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);
                //rdata 배열에 있는 값을 int 형식으로 저장.

			    accx = acc_x / 1000.0;
			    accy = acc_y / 1000.0;
			    accz = acc_z / 1000.0;
                // int16_t 형식을 /1000.0으로 저장.(4.DataSheet 참고 설정)

			    printf("ACC는 : %+.2f %+.2f %+.2f \n", accx, accy, accz);
                // 값이 +일땐 +로, -일땐 -로 소수점 2번째 자리까지 출력.
			    break;

		    case GYR:

                gyr_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			    gyr_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			    gyr_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);
                //rdata 배열에 있는 값을 int 형식으로 받아옴.

			    gyrx = gyr_x / 10.0;
			    gyry = gyr_y / 10.0;
			    gyrz = gyr_z / 10.0;
                // int16_t 형식을 /10.0으로 저장.(4.DataSheet 참고 설정)

                printf("GYR는  : %+.2f %+.2f %+.2f \n", gyrx, gyry, gyrz);
                // 값이 +일땐 +로, -일땐 -로 소수점 2번째 자리까지 출력.
			    break;

		    case ANG:
                
                ang_x = ((int)(unsigned char)rdata[2] | (int)(unsigned char)rdata[3] << 8);
			    ang_y = ((int)(unsigned char)rdata[4] | (int)(unsigned char)rdata[5] << 8);
			    ang_z = ((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8);
                //rdata 배열에 있는 값을 int 형식으로 받아옴.

                angx = ang_x / 100.0;
			    angy = ang_y / 100.0;
			    angz = ang_z / 100.0;
                //int16_t 형식을 /100.0으로 저장.(4.DataSheet 참고 설정)

			    printf("ANG는 : %+.2f %+.2f %+.2f  \n", angx, angy, angz);
                // 값이 +일땐 +로, -일땐 -로 소수점 2번째 자리까지 출력.
                break;
		    }

		printf("\n");
        
        }
    }
    ```

    ##### 1. ACC -> 가속도 X축, Y축, Z축 값.
    ##### 2. GYR -> 각속도 X축, Y축, Z축 값.
    ##### 3. ANG -> 오일러각(roll, pitch, yaw) X축, Y축, Z축 값
   




***
* ### 4. DataSheet

    ![동](https://user-images.githubusercontent.com/85467544/121861144-89deb700-cd34-11eb-88f1-e67ee52015f6.PNG)
    ![변](https://user-images.githubusercontent.com/85467544/121861146-8b0fe400-cd34-11eb-9276-7bef175a0077.PNG)
***


