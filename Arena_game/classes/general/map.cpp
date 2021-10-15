#include "Map.h"

Map::Map(unsigned new_radius ,
		unsigned new_map_width ,
		unsigned new_map_height)
{
	if (new_radius != 0)
		SetRadius(new_radius);
	else
		SetSize(new_map_width, new_map_height);
}

Map::~Map()
{
	for (auto *p : Object_list)
		p->~Object();
	Object_list.clear();
}

bool Map::isRound()
{
	if (radius <= 0)
		return false;
	else return true;
}

void Map::SetRadius(unsigned new_radius)
{
	if (radius < Pawn_Size() * 5)
		radius = Pawn_Size() * 5;
	radius = new_radius;
	map_width = map_height = new_radius * 2;
}
unsigned Map::GetRadius()
{
	return radius;
}
void Map::SetSize(unsigned new_width , unsigned new_height)
{
	map_width = new_width;
	map_height = new_height;
}
unsigned Map::GetWidth()
{
	return map_width;
}
unsigned Map::GetHeight()
{
	return map_height;
}

unsigned Map::SpawnActors(unsigned players)
{
	srand(time(NULL));
	short unsigned perPlayer = rand() % 2 + 1;
	Object_list.reserve(players*(perPlayer + 1));

	std::string name = "Player_";

	name.insert(name.length(),ToString(0));

	Object_list.push_back(new Character);
	auto *pc = Object_list.at(0);
	if (isRound())
	{
		pc->SetCurrentPosition(labs(radius - 2 * Pawn_Size()) , 0 , 180);
	}
	else
	{
		pc->SetCurrentPosition(labs(map_width / 2 - 2 * Pawn_Size()) , 0 , 180);
	}
	(dynamic_cast<Character*>(pc))->Spawn(name , Max_health() , rand() % Max_regen() + 1 , rand() % Max_armor() + 1);

	double fi;
	for (int i = 1; i < players; i++)
	{
		fi = 3.1416f*((float)i * 2 / players);

		Object_list.push_back(new AI);

		auto *pa = Object_list.at(i);

		name = "AI_";
		name.insert(name.length() , ToString(i));
		if (isRound())
		{
			pa->SetCurrentPosition(
				labs(radius - 2 * Pawn_Size())*cos(fi) ,
				labs(radius - 2 * Pawn_Size())*sin(fi) ,
				180 - fi);
		}
		else
		{
			pa->SetCurrentPosition(
				labs(map_width / 2 - 2 * Pawn_Size())*cos(fi) ,
				labs(map_height / 2 - 2 * Pawn_Size())*sin(fi) ,
				180 - fi);
		}
		(dynamic_cast<AI*>(pa))->Spawn(name , Max_health() , rand() % Max_regen() + 1 , rand() % Max_armor() + 1);
	}


	for (int i = 0; i < players*perPlayer; i++)
	{
		name = "Weapon_";
		name.insert(name.length() , ToString(i));
		fi = 3.1416f*((float)i * 2 / (players*perPlayer));

		Object_list.push_back(new Weapon);

		auto *pw = Object_list.at(i + players);
		pw->SetName(name);
		if (isRound())
		{
			pw->SetCurrentPosition(
				labs(radius - 2 * Pawn_Size())*cos(fi) ,
				labs(radius - 2 * Pawn_Size())*sin(fi) ,
				rand() % 360);
		}
		else
		{
			pw->SetCurrentPosition(
				labs(map_width / 2 - 2 * Pawn_Size())*cos(fi) ,
				labs(map_height / 2 - 2 * Pawn_Size())*sin(fi) ,
				rand() % 360);
		}
		(dynamic_cast<Weapon*>(pw))->Spawn(rand() % Max_damage() , rand() % Max_health() / 5 , rand() % Max_pickDistane() , rand() % Max_armor() / 5 , rand() % Max_regen() / 5);
	}
	return Object_list.size();					//count of all objects on map
}

std::vector<Object*> Map::GetObjectList()
{
	return Object_list;
}

Object* Map::GetObjectListElem(unsigned id)
{
	if (id < Object_list.size() )
		return (Object_list.at(id));
	else return NULL;
}