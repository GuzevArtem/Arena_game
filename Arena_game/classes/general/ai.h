#ifndef AI_H
#define AI_H

#include "Character.h"
#include "Pawn.h"
#include "Weapon.h"

#define NO_TYPE			0
#define AI_TYPE			1
#define CHARACTER_TYPE	2
#define PAWN_TYPE		3	// == AI_TYPE & CHARACTER_TYPE
#define WEAPON_TYPE		4
#define Object_TYPE		7	// == PAWN_TYPE & WEAPON_TYPE

class AI: public Pawn
{
private:
	Object* AIM = NULL;
	int AIMtype = NO_TYPE;

public:
	friend class Character;
	AI( double* new_coord = NULL,
		int new_health = 0 ,
		int new_regen = 0 ,
		int new_armor = 0 ,
		Weapon *new_weapon = NULL);

	~AI();

	void Info();

	void SetAIM(int target = NULL);
	void SetAIM(Object* target);
	void SetAIM(Pawn* target);
	void SetAIM(Character* target);
	void SetAIM(AI* target);
	void SetAIM(Weapon* target);
	Object* GetAIM();
	int GetAIMtype();

	void InteractWith(Object *obj) {};
	void Interact_with(AI *ai);
	void Interact_with(Character *character);
	void Interact_with(Weapon *weapon);

	void Draw();
};

#endif /* AI_H  */