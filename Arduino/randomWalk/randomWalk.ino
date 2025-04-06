#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  120
#define SERVOMAX  600
#define SERVO_FREQ 50

#define MAXDRIVERNUM 6
#define MAXSERVONUM 8
#define MAXSETNUM 46

#define MIN_POSITION -40
#define MAX_POSITION 40

#define MAX_FRAMES 5  // 줄임

Adafruit_PWMServoDriver pwm[MAXDRIVERNUM] = {
  Adafruit_PWMServoDriver(0x41),
  Adafruit_PWMServoDriver(0x42),
  Adafruit_PWMServoDriver(0x43),
  Adafruit_PWMServoDriver(0x44),
  Adafruit_PWMServoDriver(0x45),
  Adafruit_PWMServoDriver(0x46)
};

int8_t setValues[MAXSETNUM][2];  // int8_t로 변경
int8_t motionFrames[MAX_FRAMES][MAXSETNUM][2];  // memory 최적화
int frameCount = 0;

const int servoOffset[MAXDRIVERNUM][MAXSERVONUM][2] = {
  {
    {350, 250}, {358, 355}, {369, 350}, {369, 376},
    {369, 376}, {350, 400}, {350, 350}, {369, 350}
  },
  {
    {345, 500}, {364, 339}, {384, 351}, {396, 337},
    {402, 372}, {350, 400}, {400, 350}, {410, 367}
  },
  {
    {395, 406}, {414, 371}, {300, 500}, {350, 350},
    {382, 364}, {386, 373}, {370, 336}, {339, 331}
  },
  {
    {419, 370}, {381, 370}, {380, 404}, {442, 360},
    {505, 551}, {425, 415}, {450, 380}, {288, 367}
  },
  {
    {169, 430}, {389, 391}, {389, 391}, {314, 404},
    {360, 360}, {379, 460}, {367, 360}, {394, 552}
  },
  {
    {361, 334}, {371, 349}, {389, 339}, {376, 341},
    {300, 229}, {480, 280}, {360, 360}, {360, 360}
  }
};

void getPosition(int setNum, int* values){
  values[0] = setValues[setNum][0];
  values[1] = setValues[setNum][1];
}

void setPosition(int setNum, int X, int Y){
  setValues[setNum][0] = constrain(X, MIN_POSITION, MAX_POSITION);
  setValues[setNum][1] = constrain(Y, MIN_POSITION, MAX_POSITION);

  const int driveNum = setNum / MAXSERVONUM;
  const int servoNum = setNum % MAXSERVONUM;

  pwm[driveNum].setPWM(servoNum * 2, 0,
    servoOffset[driveNum][servoNum][0] + setValues[setNum][0]);
  pwm[driveNum].setPWM(servoNum * 2 + 1, 0,
    servoOffset[driveNum][servoNum][1] + setValues[setNum][1]);
}

void interpolatePosition(int setNum, int targetX, int targetY, int steps, int delayMs) {
  int current[2];
  getPosition(setNum, current);

  float stepX = (targetX - current[0]) / float(steps);
  float stepY = (targetY - current[1]) / float(steps);

  for (int i = 1; i <= steps; i++) {
    int interX = current[0] + stepX * i;
    int interY = current[1] + stepY * i;
    setPosition(setNum, interX, interY);
    delay(delayMs);
  }
}

void saveFrame() {
  if (frameCount >= MAX_FRAMES) return;
  for (int i = 0; i < MAXSETNUM; i++) {
    motionFrames[frameCount][i][0] = setValues[i][0];
    motionFrames[frameCount][i][1] = setValues[i][1];
  }
  Serial.print("Saved frame: ");
  Serial.println(frameCount);
  frameCount++;
}

void playMotion(int stepsPerMove, int delayMs) {
  for (int f = 0; f < frameCount; f++) {
    for (int i = 0; i < MAXSETNUM; i++) {
      interpolatePosition(i,
        motionFrames[f][i][0],
        motionFrames[f][i][1],
        stepsPerMove,
        delayMs
      );
    }
    delay(200);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Servo motion system start");

  for (int i = 0; i < MAXDRIVERNUM; i++) {
    pwm[i].begin();
    pwm[i].setOscillatorFrequency(25000000);
    pwm[i].setPWMFreq(SERVO_FREQ);
  }

  for (int i = 0; i < MAXSETNUM; i++) {
    setValues[i][0] = 0;
    setValues[i][1] = 0;
    const int driveNum = i / MAXSERVONUM;
    const int servoNum = i % MAXSERVONUM;
    pwm[driveNum].setPWM(servoNum * 2, 0, servoOffset[driveNum][servoNum][0]);
    pwm[driveNum].setPWM(servoNum * 2 + 1, 0, servoOffset[driveNum][servoNum][1]);
  }

  delay(10);
  Serial.println("System ready");
}

void loop() {
  // 랜덤 포즈 5개 저장 후 재생
  while (frameCount < MAX_FRAMES) {
    for (int i = 0; i < MAXSETNUM; i++) {
      int randX = random(-20, 21);
      int randY = random(-20, 21);
      setPosition(i, randX, randY);
    }
    saveFrame();
    delay(1000);
  }

  playMotion(10, 3);
  delay(500);
}
