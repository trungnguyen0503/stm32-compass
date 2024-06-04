#pragma once

#include <stdbool.h>

void StateCtrl_Init(void);
void StateCtrl_Update(void);
bool StateCtrl_IsPrevOn(void);
bool StateCtrl_IsOn(void);