#include "Global.h"
#include <iostream>
#include <exception>
#define _USE_MATH_DEFINES
#include <math.h>

Global::Global()
{
	max_armor = 50;
	max_health = 100;
	max_damage = 30;
	max_regen = 3;
	max_pickDistance = 70;
	max_speed = 5;					// Random value
}

Global::~Global() {}

int Global::Max_armor()
{
	return max_armor;
}
int Global::Max_health()
{
	return max_health;
}
int Global::Max_damage()
{
	return max_damage;
}
int Global::Max_regen()
{
	return max_regen;
}

unsigned Global::Max_pickDistane()
{
	return max_pickDistance;
}

double Global::Max_speed()
{
	return max_speed;
}

unsigned Global::Pawn_Size()
{
	return PawnSize;
}
unsigned Global::Weapon_Size()
{
	return WeaponSize;
}

void Global::SetMax_armor(int new_max_armor)
{
	if (new_max_armor > 0)
		max_armor = new_max_armor;
}
void Global::SetMax_health(int new_max_health)
{
	if (new_max_health > 0)
		max_health = new_max_health;
}
void Global::SetMax_damage(int new_max_damage)
{
	if (new_max_damage > 0)
		max_damage = new_max_damage;
}
void Global::SetMax_regen(int new_max_regen)
{
	if (new_max_regen <= max_health / 10)
		max_regen = new_max_regen;
}

void Global::SetMax_pickDistance(unsigned new_max_pickDistance)
{
	max_pickDistance = new_max_pickDistance;
}

void Global::SetMax_speed(double new_max_speed)
{
	if (new_max_speed >= 0)
		max_speed = new_max_speed;
}

/*************************************************************/
/************** Additional functions *************************/
/*************************************************************/

double Global::Distance(double x1, double y1 , double x2 , double y2 )
{
	return std::sqrt(std::abs((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));
}
double Global::Distance(double* a , double *b)
{
	if (a == nullptr || b == nullptr)
		return INFINITY;

	return Distance(a[0] , a[1] , b[0] , b[1]);
}
double Global::AngleOX(double x1 , double y1)
{
	long double grad;
	if (x1 == 0 && y1 == 0)
		x1 = 1;
	grad = x1 / (sqrt((long double)x1*x1 + y1*y1));

	if (grad < -1)
		grad = -1.L;
	else if (grad > 1)
		grad = 1.L;

	grad = std::acos(grad) / M_PI * 180;
	if (y1 < 0)
		grad = -grad;

	return grad + 360;
}
double Global::Angle(double x1 , double y1 , double x2 , double y2)
{
	return AngleOX(x2 , y2) - AngleOX(x1 , y1);
}
double Global::Angle(double *a , double *b)
{
	if (a == nullptr || b == nullptr)
		return -1;

	return Angle(a[1] , a[2] , b[1] , b[2]);
}

/*************************************************************/
/*********** End of Additional functions *********************/
/*************************************************************/