#include "Character.h"
#include <math.h>

Character::Character(double *new_coord ,
	int new_health,
	int new_regen,
	int new_armor,
	Weapon *new_weapon)
{
	Pawn::SetCurrentPosition(new_coord);
	Pawn::SetHealth(new_health);
	Pawn::SetRegen(new_regen);
	Pawn::SetArmor(new_armor);
	Pawn::SetWeapon(new_weapon);
	SetState(IDLE);
}

Character::Character(  double x ,
	double y ,
	double rotation,
	int new_health,
	int new_regen,
	int new_armor,
	Weapon *new_weapon)
{
	Pawn::SetCurrentPosition(x,y,rotation);
	Pawn::SetHealth(new_health);
	Pawn::SetRegen(new_regen);
	Pawn::SetArmor(new_armor);
	Pawn::SetWeapon(new_weapon);
}

Character::~Character() {}

void Character::Info()
{
	std::cout << "Character" << ": " << GetName() << " pos: " \
		<< GetCurrentPosition(X) << " "\
		<< GetCurrentPosition(Y) << " "\
		<< GetCurrentPosition(R) << " "\
		<< "hp: " << GetHealth() << " reg: " << GetRegen() << " arm: " << GetArmor() << "\n";
	if (GetWeapon() != NULL)
		GetWeapon()->Info();
	else 
		std::cout << "Weapon: NO_WEAPON_PICKED damage "<< GetDamage() <<"\n";
}

void Character::Interact_with(AI *ai)
{
	if (this->Team() != ai->Team() || this->Team() == 0)
	{
		Attack(ai);
	}
}
void Character::Interact_with(Character *character)
{
	if (Team() != character->Team() || Team() == 0)
	{
		Attack(character);
	}
}
void Character::Interact_with(Weapon *weapon)
{
	if (this->GetWeapon() == NULL && !(weapon->IsPicked()))
	{
		SetWeapon(weapon);
		std::cout << "You picked up a ";
		this->GetWeapon()->Info();
	}
}

void Character::Draw()
{
	if (GetWeapon() != NULL && 0.5 * GetWeapon()->GetWeaponStat(Distance_b) >= Pawn_Size() * 1 / 3 + 5)
	{
		GetWeapon()->SetCurrentPosition(GetCurrentPosition(X) , GetCurrentPosition(Y) , 0);
		GetWeapon()->Draw();
	}

	glColor3f(0.7 * (float)GetHealth() / Global::Max_health() , 0.3 * (float)GetHealth() / Global::Max_health() , 0.3 * (float)GetHealth() / Global::Max_health());

	int n = 36;
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(GetCurrentPosition(X) + Pawn_Size()*cos(3.1416f*((float)i * 2 / n) + GetCurrentPosition(R)) ,
					GetCurrentPosition(Y) + Pawn_Size()*sin(3.1416f*((float)i * 2 / n) + GetCurrentPosition(R)));

	}
	glEnd();

	if (GetWeapon() != NULL && 0.5 * GetWeapon()->GetWeaponStat(Distance_b) < Pawn_Size() * 1 / 3 + 5)
	{
		GetWeapon()->SetCurrentPosition(GetCurrentPosition(X) , GetCurrentPosition(Y) , 0);
		GetWeapon()->Draw();
	}
}