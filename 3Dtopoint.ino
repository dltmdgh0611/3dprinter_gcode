#include "3dmotor.h"

void setup() {
	firstSetting();
}

void loop() {
	unsigned long cur = micros();
	Input_Key();
	MOVE_XY_Relative(cur);
}
