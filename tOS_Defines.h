#pragma once

#include "tOS_Types.h"

const tOS_Size tOS_DEF_VEC_STD_CAP = 6;
const uint16_t tOS_DEF_MIN_STACK_ADR = 0x0060;
const uint16_t tOS_DEF_MAX_STACK_ADR = 0x085F;
const uint16_t tOS_DEF_LOCAL_STACK_SIZE = 0x100;

typedef enum {
	tOS_Prio_REALTIME = 0,
	tOS_Prio_UPPERNORMAL = UINT32_MAX/5,
	tOS_Prio_NORMAL = UINT32_MAX/4,
	tOS_Prio_LOWERNORMAL = UINT32_MAX/3,
	tOS_Prio_IDLE = UINT32_MAX-1,
	tOS_Prio_NONE = UINT32_MAX,
} tOS_Task_Priority;

typedef enum {
	tOS_State_TERMINATED = 0,
	tOS_State_SUSPENDED,
	tOS_State_BLOCKED,
	tOS_State_UNBLOCKED,
	tOS_State_READY,
	tOS_State_RUNNING,
} tOS_Task_State;
