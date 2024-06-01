
#include "table.h"
#include "../labs/graphics.h"
#include "../physics/utilities.h"
#include "../viewer/cueViewer.h"
#include "../xpms/biggerTable.xpm"
#include "../xpms/table.xpm"
#include "../xpms/finalTable.xpm"
#include "math.h"

Table *newTable() {
  Table *table = (Table *) malloc(sizeof(Table));

  // Set balls
  table->ballNumber = 2;
  table->balls = (Ball **) malloc(sizeof(Ball *) * table->ballNumber);

  vector_t cueBallPosition = {300, 300};
  table->balls[0] = newBall(cueBallPosition);
  vector_t otherBallPosition = {400, 400};
  table->balls[1] = newBall(otherBallPosition);
  // otherBallPosition.x = 100;
  // otherBallPosition.y = 500;
  // table->balls[2] = newBall(otherBallPosition);
  // otherBallPosition.x = 500;
  // otherBallPosition.y = 200;
  // table->balls[3] = newBall(otherBallPosition);
  // otherBallPosition.x = 300;
  // otherBallPosition.y = 400;
  // table->balls[4] = newBall(otherBallPosition);
  // otherBallPosition.x = 700;
  // otherBallPosition.y = 400;
  // table->balls[5] = newBall(otherBallPosition);
  // otherBallPosition.x = 800;
  // otherBallPosition.y = 500;
  // table->balls[6] = newBall(otherBallPosition);
  // Set image
  xpm_image_t img;
  xpm_load(finalTableXpm, XPM_8_8_8, &img);
  table->img = img;

  // Set pockets
  vector_t p1, p2;
  p1.x = 514;
  p1.y = 192;
  table->pockets[0] = newPocket(p1, 24);
  p1.x = 514;
  p1.y = 698;
  table->pockets[1] = newPocket(p1, 24);
  p1.x = 987;
  p1.y = 201;
  table->pockets[2] = newPocket(p1, 30);
  p1.x = 987;
  p1.y = 687;
  table->pockets[3] = newPocket(p1, 30);
  p1.x = 41;
  p1.y = 201;
  table->pockets[4] = newPocket(p1, 30);
  p1.x = 41;
  p1.y = 687;
  table->pockets[5] = newPocket(p1, 30);

  // Set linear cushions
  double radius = 20;
  p1.x = 55;
  p1.y = 249;
  p2.x = 55;
  p2.y = 638;
  table->linearCushions[0] = newLinearCushion(p1, p2);
  table->circularCushions[0] = newCircularCushion(p1, radius);
  table->circularCushions[1] = newCircularCushion(p2, radius);

  p1.x = 89;
  p1.y = 672;
  p2.x = 487;
  p2.y = 672;
  table->linearCushions[1] = newLinearCushion(p1, p2);
  table->circularCushions[2] = newCircularCushion(p1, radius);
  table->circularCushions[3] = newCircularCushion(p2, radius);

  p1.x = 542;
  p1.y = 672;
  p2.x = 936;
  p2.y = 672;
  table->linearCushions[2] = newLinearCushion(p1, p2);
  table->circularCushions[4] = newCircularCushion(p1, radius);
  table->circularCushions[5] = newCircularCushion(p2, radius);

  p1.x = 971;
  p1.y = 638;
  p2.x = 971;
  p2.y = 249;
  table->linearCushions[3] = newLinearCushion(p1, p2);
  table->circularCushions[6] = newCircularCushion(p1, radius);
  table->circularCushions[7] = newCircularCushion(p2, radius);

  p1.x = 936;
  p1.y = 216;
  p2.x = 542;
  p2.y = 216;
  table->linearCushions[4] = newLinearCushion(p1, p2);
  table->circularCushions[8] = newCircularCushion(p1, radius);
  table->circularCushions[9] = newCircularCushion(p2, radius);

  p1.x = 486;
  p1.y = 216;
  p2.x = 89;
  p2.y = 216;
  table->linearCushions[5] = newLinearCushion(p1, p2);
  table->circularCushions[10] = newCircularCushion(p1, radius);
  table->circularCushions[11] = newCircularCushion(p2, radius);
 



  // Set mouse
  table->mouse = newMouse();

  // Set cue
  table->cue = newCue();
  updateCueState(table, false);
  table->maxSpeedShot = 900;

  // Set state
  table->state = AIMING;

  // Set physics attributes
  table->gravityAcceleration = 9.81;
  table->slidingFriction = 12;
  table->spinningFriction = 0.01;
  table->rollingFriction = 12;
  table->cushionRestitution = 0.5;
  table->cushionFriction = 0.5;

  return table;
}

void destroyTable(Table *table) {
  for (size_t i = 0; i < table->ballNumber; i++) {
    destroyBall(table->balls[i]);
    free(table->balls[i]);
  }
  for (size_t i = 0; i<6; i++){
    free(table->linearCushions[i]);
  }
  for (size_t i = 0; i<12; i++){
    free(table->circularCushions[i]);
  }
  for (size_t i = 0; i<6; i++){
    free(table->pockets[i]);
  }

  free(table->balls);
  free(table->mouse);
  free(table->cue);
}

//Test
#include "../viewer/lineViewer.h"

int drawTable(Table *table) {

  drawXPMImage(table->img, 512, 443, 0);



  for (size_t i = 0; i < table->ballNumber; i++) {
    Ball *ball = table->balls[i];
    if (drawXPMImage(getBallImage(ball), getBallPosition(ball).x, getBallPosition(ball).y, 0))
      return 1;
  }

  // DEBUG 
  xpm_image_t ballImg = getBallImage(table->balls[0]);

  for (size_t i = 0; i < 12; i++) {
    CircularCushion* cushion = table->circularCushions[i];
    drawXPMImage(ballImg, cushion->position.x, cushion->position.y, 0);
  }

  for (size_t i = 0; i < 6; i++) {
    LinearCushion* cushion = table->linearCushions[i];
    drawThickLine(cushion->p1, cushion->p2, 3, 0xffffff);
  }




  switch (table->state) {
    case AIMING:
    case SHOOTING:
      drawCue(table);
      break;
    default:
      break;
  }
  if (vg_draw_rectangle(table->mouse->pos.x, table->mouse->pos.y, 9, 9, 0xff0000))
    return 1;

  return 0;
}

bool getColisionPoint(Table *table, vector_t *colisionPoint) {

  Cue *cue = table->cue;
  Ball *cueBall = table->balls[0];
  Ball *collisionBall = NULL;
  uint16_t collisionXDistance = UINT16_MAX;

  for (size_t i = 1; i < table->ballNumber; i++) {
    Ball *ball = table->balls[i];

    vector_t s = {cueBall->position.x - ball->position.x, cueBall->position.y - ball->position.y};
    double b = s.x * cue->directionVector.x + s.y * cue->directionVector.y;
    // TODO Fix ball radius hard code
    double c = s.x * s.x + s.y * s.y - 40 * 40;
    double h = b * b - c;
    if (h < 0) {
      continue;
    }
    else {
      h = sqrt(h);
      double t = 0 - b - h;
      if (t < 0)
        continue;
      else {
        uint16_t distance = abs(ball->position.x - cueBall->position.x);
        if (distance < collisionXDistance) {
          collisionXDistance = distance;
          collisionBall = ball;

          colisionPoint->x = cueBall->position.x + cue->directionVector.x * t;
          colisionPoint->y = cueBall->position.y + cue->directionVector.y * t;

          cue->targetBallCenter = ball->position;
        }
      }
    }
  }
  if (!collisionBall) {
    // TODO Colision with the sides
    colisionPoint->x = cueBall->position.x + cue->directionVector.x * 500;
    colisionPoint->y = cueBall->position.y + cue->directionVector.y * 500;
    return false;
  }
  return true;
}

int updateCueState(Table *table, bool power) {

  vector_t target;
  vector_t cueBall = table->balls[0]->position;
  Mouse *mouse = table->mouse;
  Cue *cue = table->cue;
  if (power) {

    target = table->mouse->savedPos;
    // Calculate the dot product
    cue->charge += (cue->directionVector.x * -mouse->delta.x + cue->directionVector.y * -mouse->delta.y) / cue->maximumDistance * cue->powerSensitivity;

    cue->charge = MAX(0, cue->charge);
    cue->charge = MIN(1, cue->charge);
  }
  else {
    target = table->mouse->pos;
    cue->directionVector = unitVector(cueBall, target);

    cue->angle = angle(cue->directionVector);

    if (getColisionPoint(table, &cue->colisionPoint)) {

      // Vector that connects target ball and colision point
      cue->targetBallVec = unitVector(cue->colisionPoint, cue->targetBallCenter);
      cue->guideScalar = dotProduct(cue->directionVector, cue->targetBallVec);

      double sinOfImpact = cue->directionVector.x * cue->targetBallVec.y - cue->directionVector.y * cue->targetBallVec.x;

      if (sinOfImpact < 0) {

        cue->whiteBallVec.x = -cue->targetBallVec.y;
        cue->whiteBallVec.y = cue->targetBallVec.x;
      }
      else {
        cue->whiteBallVec.x = cue->targetBallVec.y;
        cue->whiteBallVec.y = -cue->targetBallVec.x;
      }

      cue->colides = true;
    }
    else {
      cue->targetBallVec.x = 0;
      cue->targetBallVec.y = 0;
      cue->whiteBallVec.x = 0;
      cue->whiteBallVec.y = 0;

      cue->colides = false;
    }
  }

  float distance = cue->charge * cue->maximumDistance + cue->minimalDistance;
  cue->position.x = 212 + distance;
  cue->position.y = 0;
  cue->position = rotate2d(cue->position, cue->angle + M_PI);
  cue->position.x += cueBall.x;
  cue->position.y += cueBall.y;


  return 0;
}
