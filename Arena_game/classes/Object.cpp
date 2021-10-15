#include "Object.h"
#include <iostream>

Object::Object() {}

Object::~Object() {}

/***************************************************************************************/
/******************** Start of drawing-texturing functions *****************************/
/***************************************************************************************/

//NO COLOR, NO LINE WIDTH, LOOP
void Object::Draw()
{
	glBegin(GL_LINE_LOOP);
	int n = 36, r = Pawn_Size();
	for (int i = 0; i < n; i++)
	{
		glVertex2f(GetCurrentPosition(X) + r*cos(3.1416f*((float)i * 2 / n) + GetCurrentPosition(R)) ,
					GetCurrentPosition(Y) + r*sin(3.1416f*((float)i * 2 / n) + GetCurrentPosition(R)));
	}
	glEnd();
}

/***************************************************************************************/
/******************** End of drawing-texturing functions *******************************/
/***************************************************************************************/

bool IsValid(std::string s , std::string list = "")
{
	char c;
	if (list == "")
	{
		for (int i = 0; i < s.length(); i++)
		{
			c = s[i];
			c = tolower(c);
			if (c >= '\33' && c <= '\132')
				continue;
			else s[i] = 'x';
		}
	}
	else
	{
		for (int i = 0; i < s.length(); i++)
			for (int j = 0; j < list.length(); j++)
				if (s[i] != list[j]) return false;
	}
	return true;
}

void Object::SetName(std::string new_name)
{
	if (&new_name == nullptr)
		return;
	if (IsValid(new_name))
		name = new_name;
}
std::string Object::GetName()
{
	return name;
}
void Object::SetSpeed(double new_speed )
{
	speed = std::abs(new_speed) <= Max_speed() ? std::abs(new_speed) : Max_speed();
}
double Object::GetSpeed()
{
	return speed;
}

void Object::SetCurrentPosition(double *new_coord)
{
	if (new_coord == nullptr)
	{
		*coord = NULL;
		return;
	}

	SetCurrentPosition(new_coord[X] , new_coord[Y] , new_coord[R]);
}

void Object::SetCurrentPositionX(double position_x)
{
	SetSpeed(position_x - coord[X]);
	coord[X] = position_x;
}

void Object::SetCurrentPositionY(double position_y)
{
	SetSpeed(position_y - coord[Y]);
	coord[Y] = position_y;
}

void Object::SetCurrentPositionR(double rotation)
{
	if (rotation < 0)
	{
		rotation = (rotation / 360 - (int)(rotation / 360)) * 360 + 360;
	}
	else
	{
		if (rotation > 360)
		{
			rotation = (rotation / 360 - (int)(rotation / 360)) * 360;
		}
	}
	coord[R] = rotation;
}

double Object::GetCurrentPosition(unsigned i)
{
	if (i < 3)
		return coord[i];
	else return coord[X];
}

void Object::SetCurrentPosition(double position_x , double position_y , double rotation)
{

	SetSpeed(std::sqrt((position_x - coord[X])*(position_x - coord[X]) + (position_y - coord[Y])*(position_y - coord[Y])));

	coord[X] = position_x;
	coord[Y] = position_y;
	SetCurrentPositionR(rotation);
}

double* Object::GetCurrentPosition()
{
	return coord;
}

bool Object::Move(double addspeed , double grad)
{
	if (grad == -1)
		return false;

	if (grad < 0)
	{
		grad = (grad / 360 - (int)(grad / 360)) * 360 + 360;
	}
	else
	{
		if (grad > 360)
		{
			grad = (grad / 360 - (int)(grad / 360)) * 360;
		}
	}
	grad = grad / 180 * 3.14159;

	coord[X] += addspeed*std::cos(grad);
	coord[Y] += addspeed*std::sin(grad);
	coord[R] += grad;
	SetSpeed(addspeed);
	return true;
}


/*************************************************************/
/************** Additional functions *************************/
/*************************************************************/

double Object::Distance(Object* a , Object* b)
{
	if (a == nullptr || b == nullptr)
		return INFINITE;

	return Global::Distance(a->GetCurrentPosition() , b->GetCurrentPosition());
}
double Object::Distance(Object* a , double *b)
{
	if (a == nullptr || b == nullptr)
		return INFINITE;

	return Global::Distance(a->GetCurrentPosition() , b);
}
double Object::Distance(Object* a , double x2 , double y2)
{
	if (a != nullptr)
		return Global::Distance(a->GetCurrentPosition(X), a->GetCurrentPosition(Y) , x2 , y2);
	else
		return INFINITE;
}
double Object::AngleOX(Object *a)
{
	if (a != nullptr)
		Global::AngleOX(a->GetCurrentPosition(X), a->GetCurrentPosition(Y));
	else
		return -1;
}
double Object::Angle(Object *a , double x2 , double y2)
{
	if (a != NULL)
		return Global::Angle(a->GetCurrentPosition(X) , a->GetCurrentPosition(Y) , x2 , y2);
}
double Object::Angle(Object *a , double *b)
{
	if (a == nullptr || b == nullptr)
		return -1;
	return Global::Angle(a->GetCurrentPosition() , b);
}
double Object::Angle(Object *a , Object *b)
{
	if (a != NULL && b != NULL)
		return Global::Angle(a->GetCurrentPosition() , b->GetCurrentPosition());
	else return -1;
}

void Object::OpositeMoves(Object* a , Object* b , double distance)
{
	if (a == nullptr || b == nullptr)
		return;

	if (distance == -1)
		distance = Distance(a , b) / 2;

	b->Move(distance , Angle(a , b));
	a->Move(distance , Angle(b , a));
}

/*************************************************************/
/*********** End of Additional functions *********************/
/*************************************************************/