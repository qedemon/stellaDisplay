#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  150
#define SERVOMAX  600
#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define MAXSERVONUM 16

int servo_values[MAXSERVONUM][2];
int servo_speed=50;

void setup() {

    for(int i=0; i<MAXSERVONUM; i++){
      servo_values[i][0]=(SERVOMIN+SERVOMAX)/2;
      servo_values[i][1]=(SERVOMIN+SERVOMAX)/2;
      servo_speed=50;
    }

    Serial.begin(9600);
    Serial.println("8 channel Servo test!");

    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);

    delay(10);
}

void moveServoSmoothly(int setNumber, int* targetAngles) {
    int* currentAngle = servo_values[setNumber];
    int steps[2];
    for(int i=0; i<2; i++){
      steps[0]=(targetAngles[i] > currentAngle[i]) ? 1 : -1;
    }
    while((currentAngle[0]!=targetAngles[0])||(currentAngle[1]!=targetAngles[1])){
      for(int i=0; i<2; i++){
        currentAngle[i]+=steps[i];
        pwm.setPWM((setNumber<<1)+i, 0, currentAngle[i]);
      }
      delay(10*(101-servo_speed));
    }
    for(int i=0; i<2; i++){
      currentAngle[i]=targetAngles[i];
      pwm.setPWM((setNumber<<1)+i, 0, currentAngle[i]);
    }
}

void loop() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        char type;
        int setNumber, val1, val2;

        if (sscanf(data.c_str(), "%c,%d,%d,%d", &type, &setNumber, &val1, &val2) == 4) {

            if (type == 'A') {  // 각도 변경
                int servoValues[2] = {val1, val2};
                moveServoSmoothly(setNumber, servoValues);
                Serial.print("Set ");
                Serial.print(setNumber);
                Serial.print(": Servo1 Angle=");
                Serial.print(servoValues[0]);
                Serial.print(", Servo2 Angle=");
                Serial.println(servoValues[1]);
            } 
            else if (type == 'S') {  // 속도 변경
                servo_speed = val1;
                Serial.print("Set ");
                Serial.print(setNumber);
                Serial.print(": Servo Speed=");
                Serial.print(servo_speed);
            }
        }
    }
}
