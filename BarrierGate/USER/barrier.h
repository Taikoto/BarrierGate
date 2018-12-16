#ifndef __BARRIER_H
#define __BARRIER_H
#include <stdio.h>

#include "stm32f10x.h"
#include "global.h"
#include "voice.h"


u8 Flow_Init(BARRIER_CHECKING_FLOW *p_flow);

u8 Open_Barrier(BARRIER_CHECKING_FLOW *pflow);


#endif
