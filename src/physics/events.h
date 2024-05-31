#pragma once
#include <lcom/lcf.h>
#include "../model/table.h"
#include "../model/event.h"





Event getNextBallBallCollision(Table* table);

Event getNextBallCushionCollision(Table* table);

Event getNextBallPocketCollision(Table* table);

void updateBallNextTransition(Table* table, Ball* ball);

Event getNextTransition(Table* table);

Event getNextEvent(Table* table);
