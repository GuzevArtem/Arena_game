#include "Pawn.h"	   
#include "GL/freeglut.h"

Pawn::~Pawn() {}

void Pawn::Spawn()
{
	Spawn(GetName(),health,regen,armor);
}
void Pawn::Spawn(std::string new_name , int new_health , int new_regen , int new_armor, Weapon *new_weapon)
{
	SetName(new_name);
	SetHealth(new_health);
	SetRegen(new_regen);
	SetArmor(new_armor);
	if(new_weapon != NULL)
		SetWeapon(new_weapon);
	SetSpeed(0);
	SetState(IDLE);
	Info();
}

void Pawn::SetState(int state)
{
	if(state <= LASTSTATE)
		STATE = state;
	else STATE = IDLE;
}
int Pawn::GetState()
{
	return STATE;
}

int* Pawn::GetTargetCoord()
{
	return target_coord;
}
void Pawn::SetTargetCoord(int x, int y)
{
	target_coord[0] = x;
	target_coord[1] = y;
}
bool Pawn::isPickable(Weapon* w)
{
	if (w == NULL)
		return false;

	if (fabs(Distance(this,w)) <= Max_pickDistane() && (w->IsPicked() == false))
		return true;

	return false;
}

void Pawn::SetTeam(short unsigned new_team)
{
	team = new_team;
}
short unsigned Pawn::Team()
{
	return team;
}

void Pawn::Regen()
{
	if (this->GetState() != DIED);
	{
		if (GetWeapon() != NULL)
		{
			if (GetHealth() < Max_health() + GetWeapon()->GetWeaponStat(Health_b))
				SetHealth(GetHealth() + GetRegen());
		}
		else
		{
			if (GetHealth() < Max_health())
				SetHealth(GetHealth() + GetRegen());
		}
	}
}

void Pawn::LoseHealth(int damage)
{
	if (this->GetState() == DIED)
		return;

	if(this->GetWeapon() != NULL)
	{
		if (armor > Max_armor() + GetWeapon()->GetWeaponStat(Armor_b))
			armor = Max_armor() + GetWeapon()->GetWeaponStat(Armor_b);
	}
	else if (armor > Max_armor())
			armor = Max_armor();

	health -= damage * (1 - (armor / 101) * (armor / 101) *(armor / 101) + (armor / 2000) * (armor / 2000));
}
void Pawn::Death()
{
	DropWeapon();
	SetState(DIED);
	SetCurrentPosition(DEATH_PLACE , DEATH_PLACE);
}
bool Pawn::Attack(Pawn *attacted)
{
	if (attacted == NULL)
		return false;
	int tmp;
	static int tick = 0;

	if (tick >= 60)
	{
		tmp = attacted->GetHealth();

		attacted->LoseHealth(this->GetDamage());

		tick = 0;

		if (attacted->GetHealth() <= tmp)
		{
			std::cout << this->GetName() << " release " << tmp - attacted->GetHealth() << " damage to " << attacted->GetName() << "\n";
		}
		else return false;
	}
	if (attacted->GetHealth() <= 0 && attacted->GetState() != DIED)
	{
		kills++;
		std::cout << attacted->GetName() << " killed by " << this->GetName() << "\n";
		attacted->Death();
	}

	tick++;
	return true;
}
unsigned Pawn::GetKillCount()
{
	return kills;
}

void Pawn::SetDamage(int const new_damage)
{
	if (GetWeapon() != NULL)
	{
		damage = (new_damage < (10 + GetWeapon()->GetWeaponStat(Damage))) ? new_damage : (10 + GetWeapon()->GetWeaponStat(Damage));
	}
	else
	{
		damage = 10;
	}
}
int Pawn::GetDamage()
{
	return damage;
}

void Pawn::SetHealth(int new_health)
{
	if (new_health > 0)
	{
		if (GetWeapon() != NULL)
		{
			new_health = ((new_health < (Max_health() + GetWeapon()->GetWeaponStat(Health_b))) ? new_health : (Max_health() + GetWeapon()->GetWeaponStat(Health_b)));
		}
		else
		{
			new_health = (new_health < Max_health() ? new_health : Max_health());
		}
		if (health != new_health )
		{
			//std::cout << "Added " << new_health - health << " hp.\n";
			health = new_health;
		}
	}
	else health = 1;
}
int Pawn::GetHealth()
{
	return health;
}

void Pawn::SetArmor(int new_armor)
{
	if (new_armor > 0)
	{
		if (GetWeapon() != NULL)
		{
			armor = (new_armor <  (Max_armor() + GetWeapon()->GetWeaponStat(Armor_b)) ? new_armor : (Max_armor() + GetWeapon()->GetWeaponStat(Armor_b)));
		}
		else
		{
			armor = (new_armor < Max_armor() ? new_armor : Max_armor());
		}
	}
}
int Pawn::GetArmor()
{
	return armor;
}

void Pawn::SetRegen(int new_regen)
{
	if (new_regen >= 0 )
	{
		if (GetWeapon() != NULL)
		{
			regen = ((new_regen < Max_regen() +  GetWeapon()->GetWeaponStat(Regen_b)) ? new_regen : ( Max_regen() + GetWeapon()->GetWeaponStat(Regen_b)));
		}
		else
		{
			regen = ( new_regen < Max_regen() ? new_regen : Max_regen());
		}
	}
}
int Pawn::GetRegen()
{
	return regen;
}

void Pawn::SetWeapon(Weapon *new_weapon)
{
	if (weapon != NULL)
		return;

	if (new_weapon != NULL && new_weapon->IsPicked() == false )
	{
		new_weapon->SetCurrentPosition(this->GetCurrentPosition());
		new_weapon->IsPicked(true);
		weapon = new_weapon;
		SetDamage(GetDamage() + GetWeapon()->GetWeaponStat(Damage));
		SetHealth(GetHealth() + GetWeapon()->GetWeaponStat(Health_b));
		SetRegen(GetRegen() + GetWeapon()->GetWeaponStat(Regen_b));
		SetArmor(GetArmor() + GetWeapon()->GetWeaponStat(Armor_b));
		Regen();
	}
}
Weapon* Pawn::GetWeapon()
{
	return weapon;
}
void Pawn::DropWeapon()
{
	if (GetWeapon() == NULL)
		return;

	GetWeapon()->IsPicked(false);
	double pos[3] = { GetCurrentPosition(X) , GetCurrentPosition(Y) , GetCurrentPosition(R) };
	pos[X] += Pawn_Size() * 2.2 * cos(pos[R] / 180 * 3.14159);
	pos[Y] += Pawn_Size() * 2.2 *sin(pos[R] / 180 * 3.14159);
	GetWeapon()->SetCurrentPosition(pos);
	//SetDamage(GetDamage() - GetWeapon()->GetWeaponStat(Damage));
	//SetHealth(GetHealth() - GetWeapon()->GetWeaponStat(Health_b));
	SetRegen(GetRegen() - GetWeapon()->GetWeaponStat(Regen_b));
	SetArmor(GetArmor() - GetWeapon()->GetWeaponStat(Armor_b));
	weapon = NULL;
	SetDamage(GetDamage());
	SetHealth(GetHealth());
	SetRegen(GetRegen());
	SetArmor(GetArmor());
	Regen();
}