#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "scene.h"
#define SERVOMIN  120
#define SERVOMAX  600
#define SERVO_FREQ 50

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
    {359, 250},
    {383, 358},
    {369, 350},
    {369, 376},
    {369, 376},
    {392, 360},
    {350, 350},
    {369, 350}
  },
  {
    {345, 500},
    {364, 339},
    {384, 351},
    {396, 337},
    {402, 372},
    {421, 346},
    {340, 360},
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
/*
double data[46][2] = {
    {-2.881509338, 1.821797004},
    {11.19475232, 18.68557647},
    {2.974174118, -35.01751625},
    {1.121005444, -28.48026187},
    {1.643803881, 32.4050504},
    {8.377008978, 1.622516339},
    {-2.690039835, -25.88551722},
    {-2.785689123, -20.18921014},
    {1.493646578, -41.83797302},
    {12.99495862, -55.0537255},
    {-5.166752584, 11.36641858},
    {-3.855553988, 16.3704542},
    {-9.208653832, 13.86404759},
    {-0.593597839, -42.257639},
    {2.723927423, 13.8414631},
    {-9.642726997, 11.20306098},
    {-16.69383405, -44.49541035},
    {-30, 0},
    {4.62607688, -42.35776538},
    {-12.295282, -39.02852445},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {6.472092556, 15.27003133},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {13.03605234, -31.89424437},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-30, 0},
    {-1.987384202, 0}
};*/


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

void move(int setIndex, int X, int Y){
  int driverIndex = setIndex/MAXSERVONUM;
  int servoIndex = setIndex%MAXSERVONUM;
  pwm[driverIndex].setPWM(servoIndex*2, 0, X);
  pwm[driverIndex].setPWM(servoIndex*2+1, 0, Y);
}

void display(int servoValues[][2]){
  for(int j=0; j<TOTALSERVONUM; j++){
    move(j, servoValues[j][0], servoValues[j][1]);
  }
}

void playScene(int sceneIndex){
  int servoValues[TOTALSERVONUM][2];
  for(int i=0; i<TOTALSERVONUM; i++){
    int driverIndex = i/MAXSERVONUM;
    int servoIndex = i%MAXSERVONUM;
    servoValues[i][0]=servoOffset[driverIndex][servoIndex][0]-65;
    servoValues[i][1]=servoOffset[driverIndex][servoIndex][1];
  }

  for(int i=0; i<SERVOS_PER_SCENE; i++){
    int servoNum = scenes[sceneIndex].servo[i].index;
    int X=scenes[sceneIndex].servo[i].X;
    int Y=scenes[sceneIndex].servo[i].Y;
    if(servoNum>=0){
      servoValues[servoNum][0]=X;
      servoValues[servoNum][1]=Y;
    }
  }
  for(int i=0; i<ETC_NUM; i++){
    int etcNum = scenes[sceneIndex].etc[i];
    if(etcNum>=0){
      int driverIndex = etcNum/MAXSERVONUM;
      int servoIndex = etcNum%MAXSERVONUM;
      servoValues[etcNum][0]=servoOffset[driverIndex][servoIndex][0];
      servoValues[etcNum][1]=servoOffset[driverIndex][servoIndex][1];
    }
  }

  for(int i=0; i<METEOR_NUM; i++){
    int meteorNum = scenes[sceneIndex].meteor[i];
    if(meteorNum>=0){
      int driverIndex = meteorNum/MAXSERVONUM;
      int servoIndex = meteorNum%MAXSERVONUM;

      int yStart = random(-150, 151);
      int yEnd = random(-150, 151); 

      for(int frameIndex=0; frameIndex<22; frameIndex++){
        servoValues[meteorNum][0]=servoOffset[driverIndex][servoIndex][0]-70+frameIndex*5;
        servoValues[meteorNum][1]=servoOffset[driverIndex][servoIndex][1]+(yStart+(yEnd-yStart)*frameIndex/22);

        display(servoValues);
      }
    }
    display(servoValues);
    delay(50);
  }


}

int sceneIndex = 0;

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(sceneIndex);
  playScene(sceneIndex);
  sceneIndex++;
  if(sceneIndex>=TATALSCENENUM){
    sceneIndex=0;
  }
  delay(1000);

}
