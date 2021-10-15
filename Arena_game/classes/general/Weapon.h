#ifndef WEAPON_H
#define WEAPON_H

/* stats id defining */
#define Damage		0
#define Health_b	1
#define Distance_b	2
#define Armor_b		3
#define Regen_b		4

#include <stddef.h>
#include "../Object.h"	

class Weapon: public Object
{
private:
	bool isPicked = false;
	int stats[5] = {0,0,0,0,0}; //0 - damage, 1 - health_b , 2 - distance_b , 3 - armor_b , 4 - regen_b

public:
		Weapon(int *new_stats);
		Weapon(int damage = 0, int health_b = 0 , int distance_b = 0 , int armor_b = 0 , int regen_b = 0);
		~Weapon();

		void IsPicked(bool val);
		bool IsPicked();

		void Spawn();
		void Spawn(int damage, int health_b, int distance_b, int armor_b, int regen_b);
		void Info();

		//0 - damage, 1 - healt_b, 2 - distance_b, 3 - armor_b, 4 - regen_b
		int* GetAllWeaponStats();
		int GetWeaponStat(unsigned num);
	

		void Draw(int x, int y, int r = 0);
		void Draw();
};

#endif /* WEAPON_H */
