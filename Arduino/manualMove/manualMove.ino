#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  150
#define SERVOMAX  600
#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define MAXSERVONUM 16

int servo_values[MAXSERVONUM][2];

String inputString = "";
bool inputComplete = false;

void setup() {

    for(int i=0; i<MAXSERVONUM; i++){
      servo_values[i][0]=(SERVOMIN+SERVOMAX)/2;
      servo_values[i][1]=(SERVOMIN+SERVOMAX)/2;
    }

    Serial.begin(9600);
    Serial.println("8 channel Servo test!");

    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);

    delay(10);
}

void moveServoSmoothly(int setNumber, int* targetAngles, int speed) {
    int* currentAngle = servo_values[setNumber];
    int steps[2];
    for(int i=0; i<2; i++){
      steps[0]=(targetAngles[i] > currentAngle[i]) ? 1 : -1;
    }
    while((currentAngle[0]!=targetAngles[0])||(currentAngle[1]!=targetAngles[1])){
      for(int i=0; i<2; i++){
        if(currentAngle[i]!=targetAngles[i])
            currentAngle[i]+=steps[i];
        pwm.setPWM((setNumber<<1)+i, 0, currentAngle[i]);
      }
      delay(10*(101-speed));
    }
    for(int i=0; i<2; i++){
      currentAngle[i]=targetAngles[i];
      pwm.setPWM((setNumber<<1)+i, 0, currentAngle[i]);
    }
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
}