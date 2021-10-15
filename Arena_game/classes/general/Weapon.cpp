#include "Weapon.h"
#include <string>
#include <GL/freeglut.h>
#include <math.h>

Weapon::Weapon(int* new_stats)
{
	if(sizeof(new_stats)/sizeof(int)>5)
		for (int i = 0; i < 5; i++)
			stats[i] = new_stats[i];
	else std::cout << "Wrong stats assigment\nargument new_stats hasn't enough values\n";
}
Weapon::Weapon( int new_damage,
				int new_health_b,
				int new_distance_b,
				int new_armor_b,
				int new_regen_b)
{
	stats[0] = new_damage;
	stats[1] = new_health_b;
	stats[2] = new_distance_b;
	stats[3] = new_armor_b;
	stats[4] = new_regen_b;
}

Weapon::~Weapon() {}

void Weapon::IsPicked(bool val)
{
	isPicked = val;
}
bool Weapon::IsPicked()
{
	return isPicked;
}

void Weapon::Spawn()
{
	Spawn(stats[0] , stats[1] , stats[2] , stats[3] , stats[4] );
}
void Weapon::Spawn(int damage , int health_b , int distance_b , int armor_b , int regen_b)
{
	stats[0] = damage;
	stats[1] = health_b;
	stats[2] = distance_b;
	stats[3] = armor_b;
	stats[4] = regen_b;
	SetSpeed(0);
	Info();
}
void Weapon::Info()
{
	std::cout << "Weapon: " << GetName();
	if(!IsPicked())
		std::cout << " pos: " << GetCurrentPosition(X) << " "\
		<< GetCurrentPosition(Y) << " "\
		<< GetCurrentPosition(R);
	std::cout << "\n damage: " << stats[Damage]\
				<< " hp_b: " << stats[Health_b]\
				<< " reg_b: " << stats[Regen_b]\
				<< " arm_b: " << stats[Armor_b]\
				<< " dist_b: " << stats[Distance_b] << "\n";
}
int Weapon::GetWeaponStat(unsigned num)
{
	if (num < sizeof(stats)/sizeof(stats[0]))
		return stats[num];
	else return 0;
}

int* Weapon::GetAllWeaponStats()
{
	return stats;
}

void Weapon::Draw(int x , int y, int r )
{
	float _R = 0 , _G = 0 , _B = 0;

	_R = ( 5.0 * GetWeaponStat(Health_b)) / Max_health();
	if (Max_regen() > 0)
		_G = ( 5.0 * GetWeaponStat(Regen_b)) / Max_regen();
	if (Max_damage() > 0)
		_B = GetWeaponStat(Damage) * 1.0 / Max_damage();

	glColor3f(_R , _G , _B);

	float radius = 0.5 * GetWeaponStat(Distance_b) + Pawn_Size() * 2 / 3;
	short unsigned n = (5 * GetWeaponStat(Armor_b)) / (float)Max_armor() * 4 + 3;

	glBegin(GL_POLYGON);
	glRasterPos2f(0 , 0);
	for (int i = 0; i < n; i++)
	{
		glVertex2f( x + radius*cos(3.1416f*((float)i * 2 / n) + r) ,
					y + radius*sin(3.1416f*((float)i * 2 / n) + r));
	}
	glEnd();
}
void Weapon::Draw()
{
	Draw(this->GetCurrentPosition(X), this->GetCurrentPosition(Y), this->GetCurrentPosition(R));
}
