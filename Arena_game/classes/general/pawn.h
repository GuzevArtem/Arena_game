#ifndef PAWN_H
#define PAWN_H 

#include "../Object.h"
#include "Weapon.h"
#include <stddef.h>			/* for NULL */
#include <math.h>
#include <string>

/***** Pawn states *****/
#define PLAYING	0
#define DIED	1
#define LASTSTATE	4
/*** Only AI states ***/
#define ATTACK	2
#define MOVE	3
#define IDLE	4

class Pawn: public Object
{
private:
	int STATE = IDLE;
	int target_coord[2] = { 0 , 0 };	//coord where looking at
	short unsigned team = 0;			// 0 - no team
	int damage = 10, health, regen , armor;
	Weapon* weapon;
	unsigned kills = 0;
public:
	Pawn() {};
	virtual ~Pawn() = 0;
	void Spawn();
	void Spawn(std::string new_name , int new_health , int new_regen , int new_armor , Weapon *new_weapon = NULL);
	virtual void Info()=0;

	void SetState(int state);
	int GetState();

	int* GetTargetCoord();
	void SetTargetCoord(int x, int y);
	bool isPickable(Weapon* w);

	void SetTeam(short unsigned new_team);
	short unsigned Team();

	void Regen();

	void LoseHealth(int const damage);
	void Death();
	bool Attack(Pawn* attacted);
	unsigned GetKillCount();

	void SetDamage(int const new_damage);
	int GetDamage();

	void SetHealth(int const new_health);
	int GetHealth();

	void SetArmor(int const new_armor);
	int GetArmor();

	void SetRegen(int const new_regen);
	int GetRegen();

	void SetWeapon(Weapon* new_weapon);
	Weapon* GetWeapon();
	void DropWeapon();
};

#endif /* PAWN_H */
