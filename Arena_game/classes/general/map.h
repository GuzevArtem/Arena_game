#ifndef MAP_H
#define MAP_H

#include <vector>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "Character.h"
#include "AI.h"
#include "Weapon.h"
#include "../Object.h"

class Map: public Object
{
	private:
		unsigned radius = 0;				//if radius != 0 map is round
		unsigned map_width , map_height;	//else map is rectengular
		int playtime = 0;
		std::vector<Object*> Object_list;

	public:
		Map(unsigned new_radius = 0,
			unsigned new_map_width = 100,
			unsigned new_map_height = 100);
		~Map();

		bool isRound();

		void SetRadius(unsigned new_radius);
		unsigned GetRadius();
		void SetSize(unsigned new_width , unsigned new_height);
		unsigned GetWidth();
		unsigned GetHeight();

		unsigned SpawnActors(unsigned players);

		std::vector<Object*> GetObjectList();
		Object* GetObjectListElem(unsigned id);
};

#endif /* MAP_H */
