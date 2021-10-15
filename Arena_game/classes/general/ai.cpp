#include "AI.h"

AI::AI( double* new_coord,
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

AI::~AI() {}

void AI::Info()
{
	std::cout << "AI       " << ": " << GetName() << " pos: " \
		<< GetCurrentPosition(X) << " "\
		<< GetCurrentPosition(Y) << " "\
		<< GetCurrentPosition(R) << " "\
		<< "hp: " << GetHealth() << " reg: " << GetRegen() << " arm: " << GetArmor() << "\n";
	if (GetWeapon() != NULL)
		GetWeapon()->Info();
	else
		std::cout << "Weapon: NO_WEAPON_PICKED damage " << GetDamage() << "\n";
}

void AI::SetAIM(int target)
{
	if (target == NULL)
	{
		SetTargetCoord(0 , 0);
		AIM = NULL;
		AIMtype = NO_TYPE;
	}
}
void AI::SetAIM(Object* target)
{
	SetTargetCoord(target->GetCurrentPosition(X) , target->GetCurrentPosition(Y));
	AIM = target;
	AIMtype = Object_TYPE;
}
void AI::SetAIM(Pawn* target)
{
	SetTargetCoord(target->GetCurrentPosition(X), target->GetCurrentPosition(Y));
	AIM = target;
	AIMtype = PAWN_TYPE;
}
void AI::SetAIM(Character* target)
{
	SetTargetCoord(target->GetCurrentPosition(X) , target->GetCurrentPosition(Y));
	AIM = target;
	AIMtype = CHARACTER_TYPE;
}
void AI::SetAIM(AI* target)
{
	SetTargetCoord(target->GetCurrentPosition(X) , target->GetCurrentPosition(Y));
	AIM = target;
	AIMtype = AI_TYPE;
}
void AI::SetAIM(Weapon* target)
{
	SetTargetCoord(target->GetCurrentPosition(X) , target->GetCurrentPosition(Y));
	AIM = target;
	AIMtype = WEAPON_TYPE;
}
Object* AI::GetAIM()
{
	return AIM;
}
int AI::GetAIMtype()
{
	return AIMtype;
}

void AI::Interact_with(AI *ai)
{
	if (Team() != ai->Team() || Team() == 0)
	{
		Attack(ai);
	}
}
void AI::Interact_with(Character *character)
{
	if (this->Team() != character->Team() || this->Team() == 0)
	{
		Attack(character);
	}
}
void AI::Interact_with(Weapon *weapon)
{
	if (this->GetWeapon() == NULL)
	{
		SetWeapon(weapon);
		return;
	}
	if (this->GetWeapon()->GetWeaponStat(Damage) < weapon->GetWeaponStat(Damage) &&
			this->GetWeapon()->GetWeaponStat(Health_b) <= weapon->GetWeaponStat(Health_b))
	{
		DropWeapon();
		SetWeapon(weapon);
	}
}

void AI::Draw()
{
	if (GetWeapon() != NULL && 0.5 * GetWeapon()->GetWeaponStat(Distance_b) > Pawn_Size() * 1 / 3 + 5)
	{
		GetWeapon()->SetCurrentPosition(GetCurrentPosition(X) , GetCurrentPosition(Y) , 0);
		GetWeapon()->Draw();
	}

	glColor3f(0.7 * (float)GetHealth() / Global::Max_health() , 0.3 * (float)GetHealth() / Global::Max_health() , 0.3 * (float)GetHealth() / Global::Max_health());
	int n = 36 , r = Pawn_Size();
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(GetCurrentPosition(X) + r*cos(3.1416f*((float)i * 2 / n) + GetCurrentPosition(R)) ,
			GetCurrentPosition(Y) + r*sin(3.1416f*((float)i * 2 / n) + GetCurrentPosition(R)));
	}
	glEnd();

	if (GetWeapon() != NULL && 0.5 * GetWeapon()->GetWeaponStat(Distance_b) < Pawn_Size() * 1 / 3 + 5)
	{
		GetWeapon()->SetCurrentPosition(GetCurrentPosition(X) , GetCurrentPosition(Y) , 0);
		GetWeapon()->Draw();
	}
}
