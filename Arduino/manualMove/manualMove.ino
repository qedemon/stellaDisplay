#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "servoNum.h"
#include "offset.h"

Adafruit_PWMServoDriver pwm[MAXDRIVERNUM] = {
  Adafruit_PWMServoDriver(0x41),
  Adafruit_PWMServoDriver(0x42),
  Adafruit_PWMServoDriver(0x43),
  Adafruit_PWMServoDriver(0x44),
  Adafruit_PWMServoDriver(0x45),
  Adafruit_PWMServoDriver(0x46)
};

String inputString = "";
bool inputComplete = false;

void setup() {
    //Wire.setSCL(PB_6);
    //Wire.setSDA(PB_7);
    //Serial.setRx(PA10);
    //Serial.setTx(PA9);


    Serial.begin(115200);
    Serial.println("8 channel Servo test!");

    for(int i=0; i<MAXDRIVERNUM; i++){
      pwm[i].begin();
      pwm[i].setOscillatorFrequency(25000000);
      pwm[i].setPWMFreq(SERVO_FREQ);
    }

    delay(10);


    /*for(int i=0; i<TOTAL_SERVO_NUM; i++){
      int driverIndex = i/MAXSERVONUM;
      int servoIndex = i%MAXSERVONUM;
      move(i, servoOffset[driverIndex][servoIndex][0]-65, servoOffset[driverIndex][servoIndex][1]);
    }*/

    /*for(int i=0; i<MAXSERVONUM; i++){
      double zero[2]={0, 0};
      moveToPoint(i, zero, 100);
    }*/
}

void move(int setIndex, int X, int Y){
  int driverIndex = setIndex/MAXSERVONUM;
  int servoIndex = setIndex%MAXSERVONUM;
  pwm[driverIndex].setPWM(servoIndex*2, 0, X);
  pwm[driverIndex].setPWM(servoIndex*2+1, 0, Y);
}

/*void moveServo(int setNumber, int* targetAngles=NULL){
  int* target = (targetAngles==NULL)?servo_values[setNumber]:targetAngles;
  servo_values[setNumber][0]=target[0];
  servo_values[setNumber][1]=target[1]+servo2_offset;
  for(int i=0; i<2; i++){
    pwm.setPWM((setNumber<<1)+1, 0, servo_values[setNumber][i]);
  }
}

void moveServoSmoothly(int setNumber, int* targetAngles, int speed) {
    int* currentAngle = servo_values[setNumber];
    int targetAnglesWithOffset[2];
    targetAnglesWithOffset[0]=targetAngles[0];
    targetAnglesWithOffset[1]=targetAngles[1]+servo2_offset;

    int steps[2];
    for(int i=0; i<2; i++){
      steps[i]=(targetAnglesWithOffset[i] > currentAngle[i]) ? 1 : -1;
    }
    while((currentAngle[0]!=targetAnglesWithOffset[0])||(currentAngle[1]!=targetAnglesWithOffset[1])){
      for(int i=0; i<2; i++){
        if(currentAngle[i]!=targetAnglesWithOffset[i])
            currentAngle[i]+=steps[i];
        pwm.setPWM((setNumber<<1)+i, 0, currentAngle[i]);
      }
      delay(4);
    }
    for(int i=0; i<2; i++){
      currentAngle[i]=targetAnglesWithOffset[i];
      pwm.setPWM((setNumber<<1)+i, 0, currentAngle[i]);
    }
}

void mapPointToAngle(int setNumber, double* point, int* result){
    for(int i=0; i<2; i++){
        result[i] = round(
            servo_parameters[setNumber][i][0]
            + servo_parameters[setNumber][i][1] * point[0]
            + servo_parameters[setNumber][i][2] * point[1]
            + servo_parameters[setNumber][i][3] * point[0] * point[0]
            + servo_parameters[setNumber][i][4] * point[1] * point[1]
            + servo_parameters[setNumber][i][5] * point[0] * point[1]
        );
    }
}
void moveToPoint(int setNumber, double* targetPosition, int speed){
    int targetAngle[2];
    mapPointToAngle(setNumber, targetPosition, targetAngle);
    moveServoSmoothly(setNumber, targetAngle, speed);
}*/

void loop() {
    // 시리얼 입력이 있을 경우 처리
    if (Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '\n') {
            inputComplete = true;
        } else {
            inputString += inChar;
        }
    }

    // 데이터가 완성되면 처리
    if (inputComplete) {
        processCommand(inputString);
        inputString = ""; // 입력 문자열 초기화
        inputComplete = false;
    }
}

// 명령어 처리 함수
void processCommand(String command) {
    if (command.startsWith("SET:")) {
        int setNumber, speed, v1, v2;
        sscanf(command.c_str(), "SET:%d, S:%d, V1:%d, V2:%d", 
               &setNumber, &speed, &v1, &v2);
        
        if (setNumber < 0) {
            Serial.println("Error: Invalid Set Number");
            return;
        }

        // 서보 이동
        int X=v1;
        int Y=v2;
        move(setNumber, X, Y);

        Serial.print("Move to Position X:");
        Serial.print(X);
        Serial.print(", ");
        Serial.println(Y);
        

        Serial.print("Servo 1 moved to: ");
        Serial.print(v1);

        Serial.print("Servo 2 moved to: ");
        Serial.println(v2);


        Serial.print(" at speed ");
        Serial.println(speed);
    }
}