#ifndef CHARACTER_H
#define CHARACTER_H

#include "Pawn.h"
#include "AI.h"

class Character : public Pawn
{
private:

public:
	friend class AI;
	Character() {};
	Character(  double *new_coord ,
				int new_health = 0 ,
				int new_regen = 0 ,
				int new_armor = 0,
				Weapon *new_weapon = NULL);
	Character(  double x ,
				double y ,
				double rotation = 0,
				int new_health = 0 ,
				int new_regen = 0 ,
				int new_armor = 0 ,
				Weapon *new_weapon = NULL);

	~Character();

	void Info();

	void Interact_with(AI *ai);
	void Interact_with(Character *character);
	void Interact_with(Weapon *weapon);

	void Draw();
};

#endif /* CHARACTER_H */