#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#define MAXDRIVERNUM 6
#define MAXSERVONUM 8
#define TOTALSERVONUM 46
#define TATALSCENENUM 3

#define SERVOS_PER_SCENE 16
#define ETC_NUM 16
#define METEOR_NUM 10

typedef struct SCENE_SERVO {
  int index;
  int X, Y;
}SCENE_SERVO;

typedef struct SCENE{
  SCENE_SERVO servo[SERVOS_PER_SCENE];
  int etc[ETC_NUM];
  int meteor[METEOR_NUM];
}SCENE;

extern SCENE scenes[TATALSCENENUM];

#endif