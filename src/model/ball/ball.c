
#include <lcom/lcf.h>
#include "ball.h"
#include "../../xpms/ball.xpm"
#include <math.h>



Ball *newBall(vector_t position){
  Ball* ball = (Ball*)malloc(sizeof(Ball));
  ball->position = position;
  ball->radius = 20;
  ball->state = STATIONARY;
  // Set image

  xpm_image_t img;
  xpm_load(ballXpm, XPM_8_8_8, &img);
  ball->img = img;

  Event transition = newInvalidEvent();
  ball->transition = (Event*)malloc(sizeof(Event));
  *ball->transition = transition;

  return ball;
}

void destroyBall(Ball* ball){
  free(ball->transition);
}


vector_t getBallPosition(Ball* ball){
  return ball->position;
}

vector_t getBallVelocity(Ball* ball){
  return ball->velocity;
}

vector3_t getBallAngVelocity(Ball* ball){
  return ball->ang_velocity;
}

BALL_STATE getBallState(Ball* ball){
  return ball->state;
}

xpm_image_t getBallImage(Ball* ball){
  return ball->img;
}

bool ballNotMoving(Ball* ball){
  return (ball->state == POCKETED || ball->state == STATIONARY || ball->state == SPINNING);
}
