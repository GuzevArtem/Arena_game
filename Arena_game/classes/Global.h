#ifndef GLOBAL_H
#define GLOBAL_H
#include <string>

class Global
{
private:
	int max_armor;
	int max_health;
	int max_damage;
	int max_regen;

	unsigned max_pickDistance;

	double max_speed;

	unsigned PawnSize = 30;
	unsigned WeaponSize = 20;
public:

	Global();
	virtual ~Global()=0;

	int Max_armor();
	int Max_health();
	int Max_damage();
	int Max_regen();

	unsigned Max_pickDistane();

	double Max_speed();

	unsigned Pawn_Size();
	unsigned Weapon_Size();
	
	void SetMax_armor(int new_max_armor);
	void SetMax_health(int new_max_health);
	void SetMax_damage(int new_max_damage);
	void SetMax_regen(int new_max_regen);

	void SetMax_pickDistance(unsigned new_max_pickDistance);

	void SetMax_speed(double new_max_speed);

	/*************************************************************/
	/************** Additional functions *************************/
	/*************************************************************/

	double Distance(double x1 , double y1 , double x2 , double y2);
	double Distance(double* a , double *b);
	double AngleOX(double x1 , double y1);
	double Angle(double x1 , double y1 , double x2 , double y2);
	double Angle(double *a , double *b);

	std::string ToString(unsigned num)
	{
		return std::to_string(num);
	}
	/*************************************************************/
	/*********** End of Additional functions *********************/
	/*************************************************************/
};

#endif /* GLOBAL_H */
//Class of max and min global variables to work with character and ai