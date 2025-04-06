#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  120
#define SERVOMAX  600
#define SERVO_FREQ 50

#define MAXDRIVERNUM 6
#define MAXSERVONUM 8

Adafruit_PWMServoDriver pwm[MAXDRIVERNUM] = {
  Adafruit_PWMServoDriver(0x41),
  Adafruit_PWMServoDriver(0x42),
  Adafruit_PWMServoDriver(0x43),
  Adafruit_PWMServoDriver(0x44),
  Adafruit_PWMServoDriver(0x45),
  Adafruit_PWMServoDriver(0x46)
};

const int servoOffset[MAXDRIVERNUM][MAXSERVONUM][2] = {
  {
    {350, 250},
    {358, 355},
    {369, 350},
    {369, 376},
    {369, 376},
    {350, 400},
    {350, 350},
    {369, 350}
  },
  {
    {345, 500},
    {364, 339},
    {384, 351},
    {396, 337},
    {402, 372},
    {350, 400},
    {400, 350},
    {410, 367}
  },
  {
    {395, 406},
    {414, 371},
    {300, 500},
    {350, 350},
    {382, 364},
    {386, 373},
    {370, 336},
    {339, 331}
  },
  {
    {419, 370},
    {381, 370},
    {380, 404},
    {442, 360},
    {505, 551},
    {425, 415},
    {450, 380},
    {288, 367}
  },
  {
    {169, 430},
    {389, 391},
    {389, 391},
    {314, 404},
    {360, 360},
    {379, 460},
    {367, 360},
    {394, 552}
  },
  {
    {361, 334},
    {371, 349},
    {389, 339},
    {376, 341},
    {300, 229},
    {480, 280},
    {360, 360},
    {360, 360}
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");

  for(int i=0; i<MAXDRIVERNUM; i++){
    pwm[i].begin();
    pwm[i].setOscillatorFrequency(25000000);
    pwm[i].setPWMFreq(SERVO_FREQ);
  }

  delay(10);
  Serial.println("start");
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<MAXSERVONUM; i++){
    for(int j=0; j<MAXDRIVERNUM; j++){
      pwm[j].setPWM(i*2, 0, servoOffset[j][i][0]+random(-20, 20)+10);
      pwm[j].setPWM(i*2+1, 0, servoOffset[j][i][1]+random(-20, 20));
    }
    delay(300);
  }
}
