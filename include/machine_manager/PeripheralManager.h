#ifndef _included_peripheral_manager
#define _included_peripheral_manager
#include "machine_peripheral/Buzzer.h"
#include "machine_peripheral/RotationCounter.h"
#include "config.h"

// BUZZER
#include "machine_peripheral/Buzzer.h"
Buzzer buzzer = Buzzer(GPIO_NUM_13, false, 500);

// ROTATION
#include "machine_peripheral/RotationCounter.h"
RotationCounter rotation_counter = RotationCounter(GPIO_NUM_14, GPIO_NUM_27, MODE_ACTIVE_HIGH, 4);

#endif