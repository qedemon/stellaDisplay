#include <Servo.h>

Servo servo1, servo2;
int setNumber = 1;
int servo1_value = 300;
int servo2_value = 300;
int servo1_speed = 50;
int servo2_speed = 50;

void setup() {
    Serial.begin(9600);
    servo1.attach(9);
    servo2.attach(10);
}

void moveServoSmoothly(Servo &servo, int targetAngle, int speed) {
    int currentAngle = servo.read();
    int step = (targetAngle > currentAngle) ? 1 : -1;

    for (int angle = currentAngle; angle != targetAngle; angle += step) {
        servo.write(angle);
        delay(10 * (101 - speed));  // 속도 조절
    }
    servo.write(targetAngle);
}

void loop() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        char type;
        int newSet, val1, val2;

        if (sscanf(data.c_str(), "%c,%d,%d,%d", &type, &newSet, &val1, &val2) == 4) {
            setNumber = newSet;

            if (type == 'A') {  // 각도 변경
                servo1_value = val1;
                servo2_value = val2;
                moveServoSmoothly(servo1, servo1_value, servo1_speed);
                moveServoSmoothly(servo2, servo2_value, servo2_speed);
                Serial.print("Set ");
                Serial.print(setNumber);
                Serial.print(": Servo1 Angle=");
                Serial.print(servo1_value);
                Serial.print(", Servo2 Angle=");
                Serial.println(servo2_value);
            } 
            else if (type == 'S') {  // 속도 변경
                servo1_speed = val1;
                servo2_speed = val2;
                Serial.print("Set ");
                Serial.print(setNumber);
                Serial.print(": Servo1 Speed=");
                Serial.print(servo1_speed);
                Serial.print(", Servo2 Speed=");
                Serial.println(servo2_speed);
            }
        }
    }
}
