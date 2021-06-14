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
    #### 5. Connection(2) 창이 뜨면 COM PORT의 연결 USB 번호를 설정한다.
    #### 6. Baudrate를 115200으로 설정한 후 Connect 버튼을 클릭한다.
    #### 7. Configure 버튼을 클릭한다.
    ![ahrs연결](https://user-images.githubusercontent.com/85467544/121452116-d7c48980-c9d9-11eb-90ff-9787a9a066f1.png)
    #### 8. RS-232 Bandrate(3) 확인한다.
    #### 9. Transmission Mode(4)의 값을 Binary로 지정한다. 
    #### 10. Select RS232 Data Type(5) 확인한다. 
    ![ahrsui](https://user-images.githubusercontent.com/85467544/121452514-84067000-c9da-11eb-8d20-a0f80b553315.png)
    #### 11. Arduino_Hardware를 제작한다.(Hardware)
    #### 12. Arduino 프로그램을 설치한다.
    #### 13. 11번에서 제작한 Hardware를 연결 후 보드,프로세서,포트를 제작한 하드웨어에 맞게 설정한다.
     ![아두이노s](https://user-images.githubusercontent.com/85467544/121452666-c9c33880-c9da-11eb-95f6-901ed5563d18.png)
    #### 14. [코드](http://github.com/ntrexlab/AHRS_Binary/tree/main/ahrs_binary)를 작성한다.
    
    
    ```c
    #define DeviceID 0x01
    #define STX 0x02
    #define ETX 0x03
    #define Command 0xF0
    #define ACC 0x33
    #define GYR 0x34
    #define ANG 0x35

    //define은 AHRS [메뉴얼](http://www.devicemart.co.kr/goods/view?no=1310790#goods_file) datasheet 보고 설정. 아래 이미지로 첨부.


    unsigned char buf; //센서에서 들어오는 값을 저장.
    unsigned char buff_arrey[13] = {0,};  //buf를 통해 들어오는 값을 배열로 저장.
    char cs = 0; //checksum 
    int16_t acc_x = 0, acc_y = 0, acc_z = 0, gyr_x = 0, gyr_y = 0, gyr_z = 0, ang_x = 0, ang_y = 0, ang_z = 0; //16비트 크기의 부호있는 정수형 선언.

    void setup() {
  
    Serial.begin(115200); //AHRS_UI Baudrate 동일하게 설정.

    }

    void loop() {

        if (Serial.available()){  
    
            for(int i=0; i<13 ; i++){
        
                Serial.readBytes(&buf,1);
                buff_arrey[i] = buf;

                //버퍼에 들어오는 값을 buf를 통해 buff_arrey 배열에 1~13(0~12)까지 순서대로 저장.
            }          
      
            if (buff_arrey[0] == STX && buff_arrey[2] == DeviceID && buff_arrey[3] == Command && buff_arrey[12] == ETX){

                //buff_arrey 값이 위에 datasheet 대로 설정한 값과 같을경우.
          
                cs = 0; //CheckSumReset

                for (int i = 2; i < 11; ++i){
                    cs += buff_arrey[i];  
                    //buff_arrey 배열의 값을 cs에 추가.
                }
                
                if (cs == buff_arrey[11]){  //cs값이 buff_arrey[11] 과 같으면 정상 패킷.
                
                    switch (buff_arrey[4]){ //buff_arrey[4] = index
                
                        case ACC: //가속도
                            
                            acc_x = (buff_arrey[5] | buff_arrey[6] << 8);  // value1.
                            acc_y = (buff_arrey[7] | buff_arrey[8] << 8);  // value2.
                            acc_z = (buff_arrey[9] | buff_arrey[10] << 8); // value3.
                            Serial.print("ACC  ");
                            Serial.print(acc_x / 1000.0); //가속도 복원시 변환 식
                            Serial.print("  ");
                            Serial.print(acc_y / 1000.0); //가속도 복원시 변환 식
                            Serial.print("  ");
                            Serial.print(acc_z / 1000.0); //가속도 복원시 변환 식
                            break;
                    
                        case GYR: //각속도
                            
                            gyr_x = (buff_arrey[5] | buff_arrey[6] << 8); // value1.
                            gyr_y = (buff_arrey[7] | buff_arrey[8] << 8); // value2.
                            gyr_z = (buff_arrey[9] | buff_arrey[10] << 8); // value3.
                            Serial.print("GYR  ");
                            Serial.print(gyr_x / 10.0); //각속도 복원시 변환 식
                            Serial.print("  ");
                            Serial.print(gyr_y / 10.0); //각속도 복원시 변환 식
                            Serial.print("  ");
                            Serial.print(gyr_z / 10.0); //각속도 복원시 변환 식
                            break;
                    
                        case ANG: // 오일러 각도(roll, pitch, yaw )
                            
                            ang_x = (buff_arrey[5] | buff_arrey[6] << 8); // value1.
                            ang_y = (buff_arrey[7] | buff_arrey[8] << 8); // value2.
                            ang_z = (buff_arrey[9] | buff_arrey[10] << 8); // value3.
                            Serial.print("ANG  ");
                            Serial.print(ang_x/100.0); //오일러각 복원시 변환 식
                            Serial.print("  ");
                            Serial.print(ang_y/100.0); //오일러각 복원시 변환 식
                            Serial.print("  ");
                            Serial.print(ang_z/100.0); //오일러각 복원시 변환 식
                            break;
                    }
                }
            }
            Serial.println("");
        }
     }
    ```
***
* ### 4. DataSheet

    ![동기화](https://user-images.githubusercontent.com/85467544/121456121-823fab00-c9e0-11eb-919d-c036dfc42c2a.PNG)
    ![변환](https://user-images.githubusercontent.com/85467544/121456125-8370d800-c9e0-11eb-9afc-0be6999f7738.PNG)

***
  * ### 5. Test
    ![BI](https://user-images.githubusercontent.com/85467544/121464607-1e70ae80-c9ef-11eb-845e-2a0639b8e3d1.gif)
    
    #### 1. sp=100 -> 데이터 수신속도 100ms.
    #### 2. ACC -> 가속도 X축, Y축, Z축 값.
    #### 3. GYR -> 각속도 X축, Y축, Z축 값.
    #### 4. ANG -> 오일러각(roll, pitch, yaw) X축, Y축, Z축 값
    #### 5. 공백줄 -> CheckSum 결과 비정상 값. 


***