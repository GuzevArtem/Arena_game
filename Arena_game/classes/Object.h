#ifndef OBJECT_H
#define OBJECT_H

#include <GL/freeglut.h>
#include <stddef.h>
#include <iostream>
#include "Global.h"

#define X 0
#define Y 1
#define R 2

#define DEATH_PLACE 99999

class Object: public Global
{
private:
	std::string name = "";
	double speed = 0;
	double coord[3] = {0,0,0};			// X Y Rotare
public:

	Object();
	virtual ~Object();

	/**** Start of deawing-texturing functions ****/

	//NO COLOR NO LINE WIDTH
	void Draw();
	/***** End of deawing-texturing functions *****/

	void SetName(std::string new_name);
	std::string GetName();

	void SetSpeed(double new_speed);
	double GetSpeed();
	void SetCurrentPosition(double* new_coord);
	void SetCurrentPosition(double position_x , double position_y , double rotation = 0);
	void SetCurrentPositionX(double position_x);
	void SetCurrentPositionY(double position_y);
	void SetCurrentPositionR(double rotation);
	double GetCurrentPosition(unsigned i);
	double* GetCurrentPosition();
	bool Move(double speed , double grad);

	/*************************************************************/
	/************** Additional functions *************************/
	/*************************************************************/

	double Distance(Object* a , Object* b);
	double Distance(Object* a , double *b);
	double Distance(Object* a , double x2 , double y2);
	double AngleOX(Object *a);
	double Angle(Object *a , double x2 , double y2);
	double Angle(Object *a , double *b);
	double Angle(Object *a , Object *b);
	void OpositeMoves(Object* a , Object* b , double distance);

	/*************************************************************/
	/*********** End of Additional functions *********************/
	/*************************************************************/
};

#endif /* OBJECT_H */
