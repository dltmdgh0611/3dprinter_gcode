#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E_STEP_PIN         26
#define E_DIR_PIN          28
#define E_ENABLE_PIN       24

#define SDPOWER            -1
#define SDSS               53
#define LED_PIN            13

#define FAN_PIN            9

#define PS_ON_PIN          12
#define KILL_PIN           -1

#define HEATER_0_PIN       10
#define HEATER_1_PIN       8
#define TEMP_0_PIN          13   // ANALOG NUMBERING
#define TEMP_1_PIN          14   // ANALOG NUMBERING

#include "gcode_sample_480.h"
#include "math.h"

double radius;
//radius//


int gcode_count;
int count_x, count_y;
////counts////

bool toggle_init_x = true, toggle_init_y = true;
bool toggle_pos_x = false, toggle_pos_y = false;
bool toggle_x, toggle_y;
bool toggle_stop_x, toggle_stop_y, toggle_draw_start;
////toggles////


unsigned long position_x, position_y;
int moving_x, moving_y;
int angle_tan = (int)((5000 * radius));
////positions////

unsigned long per, per2;
int X_PWM = 200;
int Y_PWM = 200;
//int Y_PWM = (int)(100 * (5000 / (float)angle_tan));
////PWM////



void choice_dir(int stepx, int stepy) {
	if (stepx < 0) digitalWrite(X_DIR_PIN, 1);
	else digitalWrite(X_DIR_PIN, 0);
	if (stepy < 0) digitalWrite(Y_DIR_PIN, 1);
	else digitalWrite(Y_DIR_PIN, 0);
}
void Moving(int stepx, int stepy)
{
	if (stepx == 0)
	{
		toggle_stop_y = 0;
		moving_y = stepy;
	}
	else if (stepy == 0)
	{ 
		toggle_stop_x = 0;
		moving_x = stepx;
	}
	else
	{
		toggle_stop_x = 0;
		toggle_stop_y = 0;
		moving_x = stepx;
		moving_y = stepy;
	}
	choice_dir(stepx, stepy);
}

void firstSetting()
{
	Serial.begin(115200);
	pinMode(X_STEP_PIN, OUTPUT);
	pinMode(X_DIR_PIN, OUTPUT);
	pinMode(X_ENABLE_PIN, OUTPUT);
	pinMode(Y_STEP_PIN, OUTPUT);
	pinMode(Y_DIR_PIN, OUTPUT);
	pinMode(Y_ENABLE_PIN, OUTPUT);
	pinMode(X_MIN_PIN, INPUT);
	pinMode(Y_MIN_PIN, INPUT);
	////////////// PINMODE //////////////
	digitalWrite(X_MIN_PIN, 1);
	digitalWrite(Y_MIN_PIN, 1);
	digitalWrite(X_ENABLE_PIN, 0);
	digitalWrite(Y_ENABLE_PIN, 0);
	digitalWrite(X_DIR_PIN, 1);
	digitalWrite(Y_DIR_PIN, 1);
	toggle_pos_x = 0;
	toggle_init_x = true, toggle_init_y = true;
	for (int i = 0; i < 480; i++) {
		renew_xy_pos[i][0] = (int((xy_pos[i][0] - xy_pos[i - 1][0]) / 0.01 + 0.5));
		renew_xy_pos[i][1] = (int((xy_pos[i][1] - xy_pos[i - 1][1]) / 0.01 + 0.5));
	}
	Serial.println("angle(radian) : " + String(radius));
	Serial.println("angle(degree) : " + String((int)((radius/PI)*180)));
	Serial.println("X_PWM : " + String(X_PWM));
	Serial.println("Y_PWM : " + String(Y_PWM));
	Serial.println("abcd");
	Serial.println("movestep : " + String(angle_tan));
}

void draw_gcode() {
	if (toggle_draw_start)
	{
		radius = abs(atan(renew_xy_pos[gcode_count+1][1] / renew_xy_pos[gcode_count+1][0]));
		X_PWM = abs((int)(200.0 / cos(radius)));
		Y_PWM = abs((int)(200.0 / sin(radius)));

		/*Serial.println("x : " + String(renew_xy_pos[gcode_count][0]) + " y : " + String(renew_xy_pos[gcode_count][1]));
		Serial.println("angle(degree) : " + String((int)((radius / PI) * 180)));
		Serial.println("X_PWM : " + String(X_PWM));
		Serial.println("Y_PWM : " + String(Y_PWM));
		Serial.println();*/
		Moving((renew_xy_pos[gcode_count + 1][0]), (renew_xy_pos[gcode_count + 1][1]));
		gcode_count++;
		if (gcode_count > 480)
		{
			toggle_draw_start = 0;
			gcode_count = 0;
		}
	}
}
void Input_Key()
{
	if (Serial.available())
	{
		char key = Serial.read();
		if (key == 'a') {
			X_PWM = Y_PWM = 200;
			Moving(5000, 0);
		}
		if (key == 'd') {
			X_PWM = Y_PWM = 200;
			Moving(-5000, 0);
		}
		if (key == 'w') {
			X_PWM = Y_PWM = 200;
			Moving(0, 5000);
		}
		if (key == 's') {
			X_PWM = Y_PWM = 200;
			Moving(0, -5000);
		}
		if (key == 'r') {
			toggle_stop_x = 0;
			toggle_stop_y = 0;
			digitalWrite(X_DIR_PIN, 1);
			digitalWrite(Y_DIR_PIN, 1);
			toggle_init_x = true;
			toggle_init_y = true;
		}
		if (key == 'o')
		{
			if (position_y > 11500) digitalWrite(Y_DIR_PIN, 1);
			else digitalWrite(Y_DIR_PIN, 0);
			if (position_x > 12500) digitalWrite(X_DIR_PIN, 1);
			else digitalWrite(X_DIR_PIN, 0);
			toggle_stop_x = 0;
			toggle_stop_y = 0;
			toggle_pos_x = true;
			toggle_pos_y = true;
		}
		
		if (key == 'm') {
			toggle_draw_start = 1;
		}
		if (key == 'g') {
			X_PWM = (int)(200 / cos(radius));
			Y_PWM = (int)(200 / sin(radius));
			Moving(5000, angle_tan);
		}
		//26
		if (key == 't') {
			Moving(5000, -angle_tan);
		}
		if (key == 'h') {
			Moving(5000, 5000);
		}
		//45
		if (key == 'j') {
			Moving(5000, 2885);
		}
	}
}

void limit_switch(char xy)
{
	if (xy == 'x')
	{
		if (!(digitalRead(X_MIN_PIN)))
		{
			if (digitalRead(X_DIR_PIN)) toggle_stop_x = 1;
			else toggle_stop_x = 0;
			position_x = 0;
			toggle_init_x = false;
		}
	}
	else
	{
		if (!(digitalRead(Y_MIN_PIN)))
		{
			if (digitalRead(Y_DIR_PIN)) toggle_stop_y = 1;
			else toggle_stop_y = 0;
			position_y = 0;
			toggle_init_y = false;
		}
	}
}
void limit_position(char xy)
{
	if (xy == 'x')
	{
		if (position_x >= 19000)
		{
			if (!(digitalRead(X_DIR_PIN))) toggle_stop_x = 1;
			else toggle_stop_x = 0;
		}
	}
	else
	{
		if (position_y >= 18000)
		{
			if (!(digitalRead(Y_DIR_PIN))) toggle_stop_y = 1;
			else toggle_stop_y = 0;
		}

	}
}
void center_position(char xy)
{
	if (xy == 'x')
	{
		if (position_x == 12500&&toggle_pos_x)
		{
			toggle_stop_x = 1;
			toggle_pos_x = false;
			
		}
	}
	else
	{
		if (position_y == 11500&&toggle_pos_y)
		{
			toggle_stop_y = 1;
			toggle_pos_y = false;
			
		}
		
	}
}
void MOVE_XY_Absolute(char xy, int pos) {
	if (xy == 'x')
	{

		if (position_x == pos)
		{
			if (position_x > 12500 + moving_x) digitalWrite(X_DIR_PIN, 1);
			else digitalWrite(X_DIR_PIN, 0);
			toggle_stop_x = 1;
			toggle_pos_x = false;
		}
	}
	else
	{
		if (position_y == pos)
		{
			if (position_y > 11500 + moving_y) digitalWrite(Y_DIR_PIN, 1);
			else digitalWrite(Y_DIR_PIN, 0);
			toggle_stop_y = 1;
			toggle_pos_y = false;
		}
	}
}
void PWM(char xy)
{
	if (xy == 'x')
	{
		if (toggle_x == true)
		{
			count_x++;
			if (digitalRead(X_DIR_PIN)) position_x--;
			else position_x++;

			if (toggle_init_x) count_x = 0;
			else if (toggle_pos_x) count_x = 0;
			else if (count_x >= abs(moving_x))
			{
				toggle_stop_x = 1;
				count_x = 0;
				
			}
			digitalWrite(X_STEP_PIN, 1);
			toggle_x = false;
		}
		else
		{
			digitalWrite(X_STEP_PIN, 0);
			toggle_x = true;
		}
	}
	else
	{
		if (toggle_y == true)
		{
			count_y++;
			if (digitalRead(Y_DIR_PIN)) position_y--;
			else position_y++;

			if (toggle_init_y) count_y = 0;
			else if (toggle_pos_y) count_y = 0;
			else if (count_y >= abs(moving_y))
			{
				toggle_stop_y = 1;
				count_y = 0;
			}
			digitalWrite(Y_STEP_PIN, 1);
			toggle_y = false;
		}
		else
		{
			digitalWrite(Y_STEP_PIN, 0);
			toggle_y = true;
		}
	}
}


void MOVE_XY_Relative(unsigned long cur) {
	if (cur - per >= X_PWM && toggle_stop_x == 0)
	{
		per = cur;
		limit_switch('x');
		limit_position('x');
		center_position('x');
		PWM('x');
	}
	if (cur - per2 >= Y_PWM && toggle_stop_y == 0)
	{
		per2 = cur;
		limit_switch('y');
		limit_position('y');
		center_position('y');
		PWM('y');
	}
	if (toggle_stop_x&&toggle_stop_y) draw_gcode();
}
