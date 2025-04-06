#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  120
#define SERVOMAX  600
#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

#define MAXSERVONUM 16

int servo_values[MAXSERVONUM][2];
double servo_parameters[MAXSERVONUM][2][6];
int servo2_offset = 0;

#define PARAMETERS_READ 1
double servo_parameters_read[PARAMETERS_READ][2][6] = {
    /*{
        {416.5377, -1.5357, -12.2100, 25.1623, 12.6698, 0.6176},
        {373.8580, 83.6429, 5.8291, -8.2693, -2.2524, -35.9412},
    }*/
    {
      {360, 0, 0, 0, 0, 0},
      {360, 0, 0, 0, 0, 0}
    }
};

String inputString = "";
bool inputComplete = false;

void setup() {
    //Wire.setSCL(PB_6);
    //Wire.setSDA(PB_7);
    //Serial.setRx(PA10);
    //Serial.setTx(PA9);

    for(int i=0; i<MAXSERVONUM; i++){
      servo_values[i][0]=(SERVOMIN+SERVOMAX)/2;
      servo_values[i][1]=(SERVOMIN+SERVOMAX)/2;
    }

    for(int i=0; i<MAXSERVONUM; i++){
        if(i<PARAMETERS_READ){
            for(int j=0; j<2; j++){
                for(int k=0; k<6; k++){
                    servo_parameters[i][j][k]=servo_parameters_read[i][j][k];
                }
            }
        }
        else{
            for(int j=0; j<2; j++){
                for(int k=0; k<6; k++){
                    servo_parameters[i][j][k]=0;
                }
                servo_parameters[i][j][0]=(SERVOMIN+SERVOMAX)/2;
            }
        }
    }

    Serial.begin(115200);
    Serial.println("8 channel Servo test!");

    pwm.begin();
    pwm.setOscillatorFrequency(25000000);
    pwm.setPWMFreq(SERVO_FREQ);

    delay(10);

    /*for(int i=0; i<MAXSERVONUM; i++){
      double zero[2]={0, 0};
      moveToPoint(i, zero, 100);
    }*/
}

void moveServo(int setNumber, int* targetAngles=NULL){
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
}

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
        
        if (setNumber < 0 || setNumber >= MAXSERVONUM) {
            Serial.println("Error: Invalid Set Number");
            return;
        }

        // 서보 이동
        {
            int targetValues[2] = {v1, v2};
            moveServoSmoothly(setNumber, targetValues, speed);
        }
        

        Serial.print("Servo 1 moved to: ");
        Serial.print(v1);

        Serial.print("Servo 2 moved to: ");
        Serial.println(v2);


        Serial.print(" at speed ");
        Serial.println(speed);
    }
    else if(command.startsWith(("OFFSET:"))){
      int o2;
      sscanf(command.c_str(), "OFFSET: O2:%d", &o2);
      servo2_offset=o2;
      for(int i=0; i<MAXSERVONUM; i++){
        moveServo(i);
      }
    }
    else if(command.startsWith("MOVE:")){
        int setNumber, speed, X, Y;
        sscanf(command.c_str(), "MOVE:%d, S:%d, X:%d, Y:%d", 
               &setNumber, &speed, &X, &Y);
        
        {
            double targetPosition[2] = {static_cast<double>(X), static_cast<double>(Y)};
            moveToPoint(setNumber, targetPosition, speed);
        }

        Serial.print("Move to Position X:");
        Serial.print(X);
        Serial.print(", ");
        Serial.println(Y);
    }
}