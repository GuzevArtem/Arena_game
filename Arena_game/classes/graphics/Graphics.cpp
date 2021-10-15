#include "Graphics.h"

void DrawGrid(float step , float linewidth = 1.0 , float ofsetX = 0 , float ofsetY = 0 ,
	float StartPosX = -2048 , float StartPosY = -2048 , float EndPosX = 2048 , float EndPosY = 2048);

void DrawGrid(float step , float linewidth , float ofsetX , float ofsetY , float StartPosX , float StartPosY , float EndPosX , float EndPosY)
{
	glLineWidth(linewidth <= 0 ? (linewidth == 0 ? 1 : -linewidth) : linewidth); //analog if linewidth == 0 -> 1, else abs(linewidth)

	glBegin(GL_LINES);
	for (float j = StartPosY; j < EndPosY; j += step)
	{
		glVertex2f(StartPosX + ofsetX , j - ofsetX);
		glVertex2f(EndPosX + ofsetX , j - ofsetX);
	}
	for (float j = StartPosX; j < EndPosX; j += step)
	{
		glVertex2f(j + ofsetY , StartPosY - ofsetY);
		glVertex2f(j + ofsetY , EndPosY - ofsetY);
	}
	glEnd();
}

Graphics::Graphics()
{
	if (GetMapType())
	{
		map = new Map(GetMapRadius());
	}
	else
	{
		map = new Map( 0 , GetMapWidth(), GetMapHeight());
	}

	if (map->GetWidth() > GetWndWidth()/2 || map->GetHeight() > GetWndHeight()/2)
	{
		TRACKING = true;
	}

	all = map->SpawnActors(GetPlayers());
	fps = 0;
}
Graphics::~Graphics()
{
	map->~Map();
	delete map;
}

int Graphics::GetTimer()
{
	return timer / 60;
}

void Graphics::WIN()
{
	Pause();
	winner = WinCondition();
	win = true;
}

void Graphics::OnStart()
{
	if (!started)
	{
		started = true;
		Pause();
		new_game = true;
		Keyboard('y' , 0 , 0);
	}
}

void Graphics::Init()
{
	glutInit(&argv , argc);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ACCUM);
	glutInitWindowSize(w_width , w_height);
	glutInitWindowPosition((GetSystemMetrics(SM_CXSCREEN) - w_width) / 2 , (GetSystemMetrics(SM_CYSCREEN) - w_height) / 2 - 20);

	glutCreateWindow("Arena");
}

Pawn* Graphics::WinCondition()
{
	std::vector<Pawn*> candidats;

	for (auto p : map->GetObjectList())
	{
		if (auto q = dynamic_cast<Pawn*>(p))
			if (q->GetState() != DIED)
				candidats.push_back(q);
	}

	if (candidats.size() > 1)
	{
		win = true;
		for (auto p : candidats)
		{
			if (p->Team() != candidats[0]->Team())
			{
				win = false;
				break;
			}
			if (candidats[0]->Team() == 0)
			{
				win = false;
				break;
			}
		}
		if (GetTimer() < 0)
			win = true;
	}
	else
	{
		win = true;
		return candidats[0];
	}

	std::vector<Pawn*> min;

	min.push_back(candidats[0]);

	for (auto p : candidats)
	{
		if (p->GetKillCount() > min[0]->GetKillCount())
		{
			for (int i = 0; i < min.size(); i++)
				min.pop_back();
			min.push_back(p);
		}
		if (p->GetKillCount() == min[0]->GetKillCount())
			min.push_back(p);
	}

	if (min.size() > 0)
	{
		auto *p = new AI;
		std::string s = min[0]->GetName();
		for (int i = 1; i < min.size(); i++)
		{
			s.insert(s.length() , " ");
			s = min[i]->GetName();
		}
		p->SetName(s);
		map->GetObjectList().push_back(p);
		return p;
	}
	else
		return min[0];


	if (candidats.size() == 0 && win == true)
	{
		auto *p = new AI;
		p->SetName("FRIENDSHIP");
		map->GetObjectList().push_back(p);
		return p;
	}
	else return NULL;
}

//dosen't work
void Graphics::Reshape(int w , int h)
{
	if (h == 0)
		h = 1;
	ratio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (TRACKING)
	{
		gluOrtho2D( -w_width  / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(X) ,
					 w_width  / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(X) ,
					-w_height / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(Y) ,
					 w_height / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(Y) );
	}
	else
	{
		gluOrtho2D(-w_width / 2 , w_width / 2 , -w_height / 2 , w_height / 2 );
	}
	glViewport(0 , 0 , w , h);
	//glViewport(0 , 0 , w_width , w_height);
	glMatrixMode(GL_MODELVIEW);
}
void Graphics::Display(void)
{
	if (TRACKING)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (dynamic_cast<Pawn*>(map->GetObjectListElem(0))->GetState() != DIED)
		{
			glOrtho(-w_width / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(X) ,
				w_width / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(X) ,
				-w_height / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(Y) ,
				w_height / 2 + (map->GetObjectListElem(0))->GetCurrentPosition(Y) , -1.0 , 1.0);
		}
		else
		{
			glOrtho(-w_width / 2 , w_width / 2 , -w_height / 2 , w_height / 2 , -1.0 , 1.0);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);

	DrawBackground();

	Draw();

	DrawStats();

	if (State() == UNPAUSE)
	{
		Collide();
		TickFunctions();
		winner = WinCondition();
	}
	else
	{
		if (map->GetObjectListElem(0)->GetCurrentPosition(X) != DEATH_PLACE)
		{
			auto player = dynamic_cast<Character*>(map->GetObjectListElem(0));
			int lcoord = 0 , tcoord = 0 , rcoord = 0 , bcoord = 0;;
			if (TRACKING)
			{
				rcoord = 130 + player->GetCurrentPosition(X);
				lcoord = -130 + player->GetCurrentPosition(X);
				tcoord = 60 + player->GetCurrentPosition(Y);
				bcoord = -10 + player->GetCurrentPosition(Y);
			}
			else
			{
				rcoord = 130;
				lcoord = -130;
				tcoord = 60;
				bcoord = -10;
			}
			glColor3f(0.7 , 0.7 , 0.7);
			DrawSqure(lcoord , bcoord , rcoord , tcoord);
			glColor3f(0 , 0 , 0);
			DrawSqure(lcoord , bcoord , rcoord , tcoord , GL_LINE_LOOP);
			DrawColorText(this , 0 , 0 , 0 , "Game on pause" , map->GetObjectListElem(0)->GetCurrentPosition(X) + 1 , map->GetObjectListElem(0)->GetCurrentPosition(Y));
			DrawColorText(this , 0 , 0 , 0 , "press P to resume" , map->GetObjectListElem(0)->GetCurrentPosition(X) + 1 , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 20);
			DrawColorText(this , 0 , 0 , 0 , "Game on pause" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 1);
			DrawColorText(this , 0 , 0 , 0 , "press P to resume" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 20 - 1);
			DrawColorText(this , 0 , 0 , 0 , "Game on pause" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y));
			DrawColorText(this , 0 , 0 , 0 , "press P to resume" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 20);
		}
	}

	if (map->GetObjectListElem(0)->GetCurrentPosition(X) == DEATH_PLACE || win || restart )
	{

		map->GetObjectListElem(0)->SetCurrentPositionX(0);
		map->GetObjectListElem(0)->SetCurrentPositionY(0);

		glColor3f(0.7 , 0.7 , 0.7);
		DrawSqure(-200 , -40 , 200 , 80);
		glColor3f(0 , 0 , 0);
		DrawSqure(-200 , -40 , 200 , 80 , GL_LINE_LOOP);

		if (!restart)
		{
			if (win)
			{
				//winner = WinCondition();

				std::string s = " WIN!";
				s.insert(0 , winner->GetName());
				DrawColorText(this , 1 , 0.2 , 0.2 , s , 0 + 1 , 20);
				DrawColorText(this , 1 , 0.2 , 0.2 , s , 0 , 20 - 1);
				DrawColorText(this , 1 , 0.2 , 0.2 , s , 0 , 20);

				s = "With  kills.";
				s.insert(6 , Global::ToString(winner->GetKillCount()));
				DrawColorText(this , 1 , 0.2 , 0.2 , s , 0 , 0);
			}
			if (map->GetObjectListElem(0)->GetCurrentPosition(X) == DEATH_PLACE)
			{
				DrawColorText(this , 1 , 0.2 , 0.2 , "YOU DIED!" , 0 + 1 , 0);
				DrawColorText(this , 1 , 0.2 , 0.2 , "YOU DIED!" , 0 , 0 - 1);
				DrawColorText(this , 1 , 0.2 , 0.2 , "YOU DIED!" , 0 , 0);
			}
		}
		if (restart)
		{

			DrawColorText(this , 0 , 0 , 0 , "Want to restart?" , map->GetObjectListElem(0)->GetCurrentPosition(X) + 1 , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 20);
			DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , map->GetObjectListElem(0)->GetCurrentPosition(X) + 1 , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 40);

			DrawColorText(this , 0 , 0 , 0 , "Want to restart?" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 20 - 1);
			DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 40 - 1);

			DrawColorText(this , 0 , 0 , 0 , "Want to restart?" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 20);
			DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , map->GetObjectListElem(0)->GetCurrentPosition(X) , map->GetObjectListElem(0)->GetCurrentPosition(Y) - 40);
		}
		else
		{
			DrawColorText(this , 0 , 0 , 0 , "Want to restart?" , 1 , -20);
			DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , 1 , -40);

			DrawColorText(this , 0 , 0 , 0 , "Want to restart?" , 0 , -20 - 1);
			DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , 0 , -40 - 1);

			DrawColorText(this , 0 , 0 , 0 , "Want to restart?" , 0 , -20);
			DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , 0 , -40);
			Pause();
		}

		restart = true;
	}

	if (new_game)
	{
		map->GetObjectListElem(0)->SetCurrentPositionX(0);
		map->GetObjectListElem(0)->SetCurrentPositionY(0);

		glColor3f(0.7 , 0.7 , 0.7);
		DrawSqure(-200 , -60 , 200 , 60);
		glColor3f(0 , 0 , 0);
		DrawSqure(-200 , -60 , 200 , 60 , GL_LINE_LOOP);
		DrawColorText(this , 1 , 0.2 , 0.2 , "Want create a new game?" , 0 , 0);
		DrawColorText(this , 0 , 0 , 0 , "[Y/N]" , 0 , -40);
	}
	if (options)
	{
		glColor3f(0.7 , 0.7 , 0.7);
		DrawSqure(-200 , -100 , 200 , 60);
		glColor3f(0 , 0 , 0);
		DrawSqure(-200 , -100 , 200 , 60 , GL_LINE_LOOP);
		DrawColorText(this , 1 , 0.2 , 0.2 , "Slide using V" , 0 , 0);
		std::string str = "Players: ";
		str.insert(str.length() , Global::ToString(GetPlayers()));
		DrawColorText(this , 0 , 0 , 0 , str , 0 , -20);


		str = "Map Type: ";
		if (GetMapType())
			str.insert(str.length() , "Round");
		else
			str.insert(str.length() , "Rect");

		DrawColorText(this , 0 , 0 , 0 , str , 0 , -40);

		if (GetMapType())
		{
			str = "Map Radius: ";
			str.insert(str.length() , Global::ToString(GetMapRadius()));
			DrawColorText(this , 0 , 0 , 0 , str , 0 , -60);
		}
		else
		{
			str = "Map Width: ";
			str.insert(str.length() , Global::ToString(GetMapWidth()));
			DrawColorText(this , 0 , 0 , 0 , str , 0 , -60);
			str = "Map Height: ";
			str.insert(str.length() , Global::ToString(GetMapHeight()));
			DrawColorText(this , 0 , 0 , 0 , str , 0 , -80);
		}
		DrawColorText(this , 0 , 0 , 0 , "less [" , -150 , pos);
		DrawColorText(this , 0 , 0 , 0 , "] more" , +150 , pos);


		DrawColorText(this , 0 , 0 , 0 , "Press B to start" , 0 , -100);
	}
	DrawFPS();

	glutSwapBuffers();
}

void Graphics::Collide()
{
	for(int i = 0; i < map->GetObjectList().size() - 1; i++)
	{
		for (int j = i; j <  map->GetObjectList().size(); j++)
		{
			Collision(map->GetObjectList().at(i) , map->GetObjectList().at(j));
			Collision(map->GetObjectList().at(j) , map->GetObjectList().at(i));
		}
		Collision(map->GetObjectList().at(i));
	}
}

bool Graphics::Collision(Object *a , Object *b)
{
	double move = 0;
	if (a->GetCurrentPosition(X) == DEATH_PLACE)
		return false;

	if (b == NULL || a == b)
	{
		if (map->isRound())
		{
			move = Object::Distance(a , 0 , 0) - map->GetRadius() + Global::Pawn_Size();
			if (move > 0)
			{
				a->Move(move , Object::AngleOX(a) + 180);
				return true;
			}
		}
		else
		{
			if (a->GetCurrentPosition(X) >= map->GetWidth() - Global::Pawn_Size())
			{
				a->SetCurrentPositionX(a->GetCurrentPosition(X) - Global::Max_speed());
				return true;
			}
			if (a->GetCurrentPosition(Y) >= map->GetHeight() - Global::Pawn_Size())
			{
				a->SetCurrentPositionY(a->GetCurrentPosition(Y) - Global::Max_speed());
				return true;
			}
			if (a->GetCurrentPosition(X) * (-1) >= map->GetWidth() - Global::Pawn_Size())
			{
				a->SetCurrentPositionX(a->GetCurrentPosition(X) + Global::Max_speed());
				return true;
			}
			if (a->GetCurrentPosition(Y) * (-1) >= map->GetHeight() - Global::Pawn_Size())
			{
				a->SetCurrentPositionY(a->GetCurrentPosition(Y) + Global::Max_speed());
				return true;
			}
		}
	}
	else
	{
		move += Global::Pawn_Size();
		if (auto p = dynamic_cast<Pawn*>(a))
		{
			if(p->GetWeapon() != NULL)
				move += p->GetWeapon()->GetWeaponStat(Distance_b)/2;
			move += Global::Pawn_Size();
		}

		move -= (Object::Distance(a , b) == INFINITY ? 2 * Global::Pawn_Size() : Object::Distance(a , b));

		if (move > 0)
		{
			if (auto pc = dynamic_cast<Character*>(a))
			{
				if (auto qc = dynamic_cast<Character*>(b))
				{
					pc->Interact_with(qc);
					return true;
				}
				else if (auto qa = dynamic_cast<AI*>(b))
				{
					pc->Interact_with(qa);
					return true;
				}
				else if (auto qw = dynamic_cast<Weapon*>(b))
				{
					if (pc->isPickable(qw))
					{
						pc->Interact_with(qw);
						return true;
					}
				}
			}
			if (auto pa = dynamic_cast<AI*>(a))
			{
				if (auto qc = dynamic_cast<Character*>(b))
				{
					pa->Interact_with(qc);
					return true;
				}
				else if (auto qa = dynamic_cast<AI*>(b))
				{
					pa->Interact_with(qa);
					return true;
				}
				else if (auto qw = dynamic_cast<Weapon*>(b))
				{
					if (!(qw->IsPicked()))
					{
						pa->Interact_with(qw);
						return true;
					}
				}
			}
		}
	}
	return false;
}

float* Graphics::GetTeamColor(unsigned team)
{
	float* color = new float[3];
	/*if (team == 0)
	{
		color[0] = (float)(1 % 100) / 100;
		color[1] = (float)(2 % 200) / 200;
		color[2] = (float)(3 % 300) / 300;
	}
	else
	{*/
		color[0] = color[1] = color[2] = 0;
		color[0] = (team & 0x00001) * (team & 0x000008) / 0x000008;
		color[1] = (team & 0x00002) * (team & 0x000010) / 0x000010;
		color[2] = (team & 0x00004) * (team & 0x000020) / 0x000020;
	//}
	return color;
}

int Graphics::AILOGIC(AI *a , Character *target)
{
	if (a->GetState() == DIED || target->GetState() == DIED)
		return -100;

	int priority = 0;

	srand(time(NULL));

	if (a->GetState() == IDLE)
	{
		if (GetPlayers() < 5)
		{
			a->SetState(ATTACK);
			a->SetAIM(NearestOfType('P',a,a->Team()));
			return 100;
		}
		else if (Object::Distance(a , a->GetAIM()) < 2 * Global::Pawn_Size())
		{
			a->SetState(ATTACK);
			a->SetAIM(target);
			return 100;
		}
	}
	else if (a->GetState() == ATTACK)
	{
		if (a->GetAIM() == NULL)
		{
			a->SetState(IDLE);
			return priority;
		}
		if (Object::Distance(a , a->GetAIM()) > 6 * Global::Pawn_Size() && GetPlayers() > 5)
		{
			a->SetAIM(NULL);
		}
		if (Object::Distance(a , target) < 2 * Global::Pawn_Size())
		{
			a->SetAIM(target);
			if (a->GetHealth() < target->GetHealth() / 2)
				priority -= 2;
			if(a->GetHealth()/2 > target->GetHealth())
				priority += 2;

			if (a->GetDamage() < target->GetDamage() / 2)
				priority -= 2;
			if (a->GetDamage() / 2 > target->GetDamage())
				priority += 2;

			if (a->GetArmor() < target->GetArmor() / 2)
				priority -= 1;
			if (a->GetArmor() / 2 > target->GetArmor())
				priority += 1;

			if (a->GetWeapon() != NULL)
			{
				if (target->GetWeapon() != NULL)
				{
					if (a->GetWeapon()->GetWeaponStat(Distance_b) < target->GetWeapon()->GetWeaponStat(Distance_b) / 2)
						priority -= 1;
					if (a->GetWeapon()->GetWeaponStat(Distance_b) / 2 < target->GetWeapon()->GetWeaponStat(Distance_b))
						priority += 1;
				}
				else priority += 2;
			}
			return priority += Object::Distance(a , target)/ Global::Pawn_Size();
		}
	}

}
int Graphics::AILOGIC(AI *a , AI *target)
{
	if (a->GetState() == DIED || target->GetState() == DIED)
		return -100;

	int priority = 0;

	srand(time(NULL));

	if (a->GetState() == IDLE)
	{
		if (GetPlayers() < 5)
		{
			a->SetState(ATTACK);
			a->SetAIM(NearestOfType('P' , a , a->Team()));
			return 100;
		}
		else if (Object::Distance(a , a->GetAIM()) < 2 * Global::Pawn_Size())
		{
			a->SetState(ATTACK);
			return 100;
		}
	}
	else if (a->GetState() == ATTACK)
	{
		if (a->GetAIM() == NULL)
		{
			a->SetState(IDLE);
			return priority;
		}
		if (Object::Distance(a , a->GetAIM()) > 6 * Global::Pawn_Size() && GetPlayers() > 5)
		{
			a->SetAIM(NULL);
		}
		if (Object::Distance(a , target) < 2 * Global::Pawn_Size())
		{
			a->SetAIM(target);
			if (a->GetHealth() < target->GetHealth() / 2)
				priority -= 2;
			if (a->GetHealth() / 2 > target->GetHealth())
				priority += 2;

			if (a->GetDamage() < target->GetDamage() / 2)
				priority -= 2;
			if (a->GetDamage() / 2 > target->GetDamage())
				priority += 2;

			if (a->GetArmor() < target->GetArmor() / 2)
				priority -= 1;
			if (a->GetArmor() / 2 > target->GetArmor())
				priority += 1;

			if (a->GetWeapon() != NULL)
			{
				if (target->GetWeapon() != NULL)
				{
					if (a->GetWeapon()->GetWeaponStat(Distance_b) < target->GetWeapon()->GetWeaponStat(Distance_b) / 2)
						priority -= 1;
					if (a->GetWeapon()->GetWeaponStat(Distance_b) / 2 < target->GetWeapon()->GetWeaponStat(Distance_b))
						priority += 1;
				}
				else priority += 2;
			}
			return priority += Object::Distance(a , target) / Global::Pawn_Size();
		}
	}
}
int Graphics::AILOGIC(AI *a , Weapon *target)
{
	if (a->GetState() == DIED || target->IsPicked())
		return -100;

	double priority = 0;

	if (a->GetWeapon() == NULL)
	{
		if (target->GetWeaponStat(Damage)/ 1.0 * Global::Max_damage() * 4.0 > 1)
			priority += target->GetWeaponStat(Damage) / 1.0 * Global::Max_damage() * 4.0;
		else priority -= 4.0 * Global::Max_damage() / 1.0 *target->GetWeaponStat(Damage);

		if (target->GetWeaponStat(Armor_b) / 1.0 * Global::Max_armor() * 5.0 > 1)
			priority += target->GetWeaponStat(Armor_b) / 1.0 * Global::Max_armor() * 5.0;
		else priority -= 4.0 * Global::Max_armor() / 1.0 *target->GetWeaponStat(Armor_b);

		if (target->GetWeaponStat(Distance_b) / 1.0 * Global::Pawn_Size() * 5.0 > 1)
			priority += target->GetWeaponStat(Distance_b) / 1.0 * Global::Pawn_Size() * 5.0;
		else priority -= 5.0 * Global::Pawn_Size() / 1.0 *target->GetWeaponStat(Distance_b);

		return (int)priority/2;
	}
	else
	{
		auto p = a->GetWeapon();

		if (target->GetWeaponStat(Damage) / 1.0 * p->GetWeaponStat(Damage) > 1)
			priority += target->GetWeaponStat(Damage) / 1.0 * p->GetWeaponStat(Damage);
		else priority -= p->GetWeaponStat(Damage) / 1.0 *target->GetWeaponStat(Damage);

		if (target->GetWeaponStat(Armor_b) / 1.0 * p->GetWeaponStat(Armor_b) > 1)
			priority += target->GetWeaponStat(Armor_b) / p->GetWeaponStat(Armor_b);
		else priority -= p->GetWeaponStat(Armor_b) / 1.0 *target->GetWeaponStat(Armor_b);

		if (target->GetWeaponStat(Distance_b) / 1.0 * p->GetWeaponStat(Distance_b) > 1)
			priority += target->GetWeaponStat(Distance_b) / p->GetWeaponStat(Distance_b);
		else priority -= p->GetWeaponStat(Distance_b) / 1.0 *target->GetWeaponStat(Distance_b);

		return (int)(priority/4);
	}
}

//void Graphics::AILOGIC(AI *a)
//{
//	if (a->GetState() == DIED)
//		return;
//
//	double angle = 9999;
//
//	int priority = 0;
//
//	std::vector<Object*> targets = GetListOfType('P' , a , a->Team());
//
//	std::vector<Object*> targets_w;
//
//	for (int i = 0; i < targets.size(); i++)
//	{
//		priority = 0;
//		auto tp = dynamic_cast<Pawn*>(NearestFromList(targets , a));
//		if (tp == NULL)
//			continue;
//
//		if (targets.size() == 3)
//		{
//			a->SetState(ATTACK);
//			a->SetTargetCoord(tp->GetCurrentPosition(X) , tp->GetCurrentPosition(Y));
//			angle = Object::Angle(a , tp) + (rand() % 20 - 10);
//			break;
//		}
//
//		if (a->GetState() == IDLE)
//		{
//			if ((tp->GetDamage() >= a->GetDamage() * 2 && Object::Distance(a , tp) < 4 * Global::Pawn_Size()) || tp->GetHealth() <= Global::Max_health() / 2)
//			{
//				angle = Object::Angle(a , tp) + 180 + (rand() % 20 - 10);
//				break;
//			}
//			else
//			{
//				targets_w = GetListOfType('W' , a , a->Team());
//				for (int j = 0; j < targets.size(); j++)
//				{
//					auto tw = dynamic_cast<Weapon*>(NearestFromList(targets_w));
//					if (a->GetWeapon() != NULL)
//					{
//						if (a->GetWeapon()->GetWeaponStat(Damage) < tw->GetWeaponStat(Damage) / 2)
//							priority += 1;
//						if (a->GetWeapon()->GetWeaponStat(Armor_b) < tw->GetWeaponStat(Armor_b) * 0.9)
//							priority += 0.5;
//						if (a->GetWeapon()->GetWeaponStat(Regen_b) < tw->GetWeaponStat(Regen_b) / 2)
//							priority += 0.5;
//						if (a->GetWeapon()->GetWeaponStat(Distance_b) < tw->GetWeaponStat(Distance_b) / 2)
//							priority += 0.5;
//					}
//					else priority += 2.5;
//
//					if (Object::Distance(NearestOfType('P' , a , a->Team()) , tw) <= 4 * Global::Pawn_Size())
//						priority -= 1;
//					if (Object::Distance(NearestOfType('P' , a , a->Team()) , a) <= 2 * Global::Pawn_Size())
//						priority -= 1;
//
//					if (priority >= 1.5 && tw == dynamic_cast<Weapon*>(NearestFromList(targets_w)))
//					{
//						angle = Object::Angle(a , tw);
//						if (a->isPickable(tw))
//						{
//							//std::cout << "state - Attac\n";
//							a->DropWeapon();
//							a->SetWeapon(tw);
//							a->SetState(ATTACK);
//						}
//						break;
//					}
//					if (priority >= 1.5)
//					{
//						angle = Object::Angle(tw , a);
//						if (a->isPickable(tw))
//						{
//							a->DropWeapon();
//							a->SetWeapon(tw);
//							a->SetState(ATTACK);
//						}
//						break;
//					}
//				}
//			}
//		}
//		else
//		{
//			if (a->GetWeapon() != NULL && tp->GetWeapon() != NULL)
//			{
//				if (a->GetWeapon()->GetWeaponStat(Damage) < tp->GetWeapon()->GetWeaponStat(Damage) / 2)
//					priority += 1;
//				if (a->GetWeapon()->GetWeaponStat(Armor_b) < tp->GetWeapon()->GetWeaponStat(Armor_b) * 0.9)
//					priority += 0.5;
//				if (a->GetWeapon()->GetWeaponStat(Regen_b) < tp->GetWeapon()->GetWeaponStat(Regen_b) / 2)
//					priority += 0.5;
//				if (a->GetWeapon()->GetWeaponStat(Distance_b) < tp->GetWeapon()->GetWeaponStat(Distance_b) / 2)
//					priority += 0.5;
//			}
//			else priority += 1;
//
//			if (Object::Distance(NearestOfType('P' , a , a->Team()) , tp) >= 10 * Global::Pawn_Size())
//				priority -= 1;
//			if (dynamic_cast<Pawn*>(NearestOfType('P' , a , a->Team()) , a)->GetHealth() > a->GetHealth())
//				priority -= 1;
//			else priority += 1;
//
//			if (priority >= 1.5)
//			{
//				a->SetTargetCoord(tp->GetCurrentPosition(X) , tp->GetCurrentPosition(Y));
//				angle = Object::Angle(a , tp);
//				break;
//			}
//		}
//		if (angle == 9999)
//		{//not prioritiesed
//			if (Object::Distance(a , 0 , 0) <= map->GetRadius()*0.75)
//			{
//				angle = Object::Angle(a , 1 , 0) + (rand() % 201 - 100);
//			}
//			else
//			{
//				angle = Object::Angle(a , 1 , 0) + (rand() % 161 - 80);
//			}
//			a->SetState(IDLE);
//
//		}
//
//		//a->Move(Global::Max_speed() / 3 , Object::Angle(a , 1 , 0));
//		angle += (rand() % 3 - 1) * 10;
//		a->Move(Global::Max_speed() / 3 , angle);
//	}
//}
//

int Graphics::AILOGIC(AI* a , Object* target)
{
 	if (auto p = dynamic_cast<Character*>(target))
		return AILOGIC(a , p);
	if (auto p = dynamic_cast<AI*>(target))
		return AILOGIC(a , p);
	if (auto p = dynamic_cast<Weapon*>(target))
		return AILOGIC(a , p);

	return 0;
}
void Graphics::AILOGIC(AI *a)
{
	if (a->GetState() == DIED)
		return;

	if(a->GetAIM() != NULL)

	srand(time(NULL));

	if (map->isRound())
	{
		if (Object::Distance(a , 0 , 0) > map->GetRadius()*0.75)
		{
			a->Object::Move(Global::Max_speed()*0.8 , Object::AngleOX(a) + 180 + (rand() % 201 - 100));
			return;
		}
	}
	else
	{
		int t = 0;

		if (abs(a->GetCurrentPosition(X)) > map->GetWidth()*0.75)
		{
			if (a->GetCurrentPosition(X) > 0)
				a->Object::Move(Global::Max_speed()*0.5 , Object::Angle(a , (double)map->GetWidth() , 0 ) + 180 + (rand() % 201 - 100));
			else a->Object::Move(Global::Max_speed()*0.5 , Object::Angle(a , -1*(map->GetWidth()) , 0) + 180 + (rand() % 201 - 100));
			t++;
		}
		if (abs(a->GetCurrentPosition(Y)) > map->GetHeight()*0.75)
		{
			if (a->GetCurrentPosition(Y) > 0)
				a->Object::Move(Global::Max_speed()*0.5 , Object::Angle(a , (double)map->GetHeight() , 0) + 180 + (rand() % 201 - 100));
			else a->Object::Move(Global::Max_speed()*0.5 , Object::Angle(a , -1*(map->GetHeight()) , 0) + 180 + (rand() % 201 - 100));
			t++;
		}
		if (t > 0)
			return;
	}

	if (a->GetHealth() < Global::Max_health() / 4)
	{
		if (auto p = NearestOfType('P' , a , a->Team()))
		{
			a->Object::Move(Global::Max_speed()*0.8 , Object::Angle(a , p));
			return;
		}
	}

	int size = map->GetObjectList().size();
	int* prior = new int[size];

	int id_max = 0;

	for (int i = 0; i < size; i++)
	{
		prior[i] = AILOGIC(a , map->GetObjectList().at(i));
		if (prior[i] > prior[id_max])
			id_max = i;
	}
	
	a->SetAIM(map->GetObjectList().at(id_max));
	if (a->GetAIM() != NULL)
		a->Move((Global::Max_speed()*prior[id_max]/100) , Object::Angle(a , a->GetAIM()));
	else a->Move((Global::Max_speed()*prior[id_max]/100) , rand()%360);

	if (a->GetAIMtype() == WEAPON_TYPE)
		if (a->isPickable(static_cast<Weapon*>(a->GetAIM())))
		{
			a->DropWeapon();
			a->SetWeapon(static_cast<Weapon*>(a->GetAIM()));
			static_cast<Weapon*>(a->GetAIM())->IsPicked(true);
		}
	
	delete[] prior;
}

void Graphics::DrawSqure(int x1, int y1, int x2 , int y2, int mode)
{
	glBegin(mode);
	glVertex2f(x1 , y1);
	glVertex2f(x1 , y2);
	glVertex2f(x2 , y2);
	glVertex2f(x2 , y1);
	glEnd();
}

void Graphics::DrawColoredTextOnScreen(float _R , float _G , float _B , std::string text , int width, int x_offset , int y_offset)
{
	glColor3f(_R,_G,_B);
	for (int i = 0; i < text.length(); i++)
	{
		if (TRACKING)
		{
			auto player = dynamic_cast<Character*>(map->GetObjectListElem(0));
			glRasterPos2f(GetWndWidth() / 2 + i * 10 + x_offset - width + player->GetCurrentPosition(X) , GetWndHeight() / 2 + y_offset + player->GetCurrentPosition(Y));
		}
		else
			glRasterPos2f(GetWndWidth() / 2 + i * 10 + x_offset - width , GetWndHeight() / 2 + y_offset);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12 , text[i]);
	}
}

void Graphics::DrawStats()
{
	//Draw Stats of player
	auto player = dynamic_cast<Character*>(map->GetObjectListElem(0));
	if (player != NULL)
	{				   //TODO put in special functions
		int lcoord = 0 , tcoord = 0;
		if (TRACKING)
		{
			lcoord = GetWndWidth() / 2 - 30 - 200 + player->GetCurrentPosition(X);
			tcoord = GetWndHeight() / 2 + player->GetCurrentPosition(Y);
		}
		else
		{
			lcoord = GetWndWidth() / -2 - 30 - 200;
			tcoord = GetWndHeight() / 2;
		}
		glColor3f(0.6 , 0.6 , 0.6);
		DrawSqure(lcoord , tcoord , GetWndWidth() , tcoord - 100);
		glColor3f(0 , 0 , 0);
		DrawSqure(lcoord , tcoord , GetWndWidth() , tcoord - 100 , GL_LINE_LOOP);

		std::string str = "Health: ";
		str.insert(str.length() , Global::ToString(player->GetHealth()));

		DrawColoredTextOnScreen(1 , 0.1 , 0.1 , str , 200 , -20 , -20);

		str = "Armor :";
		str.insert(str.length() , Global::ToString(player->GetArmor()));

		DrawColoredTextOnScreen(0.1 , 0.1 , 1 , str , 200 , -20 , -40);

		str = "Regen :";
		str.insert(str.length() , Global::ToString(player->GetRegen()));

		DrawColoredTextOnScreen(0.5 , 0.1 , 0.1 , str , 200 , -20 , -60);

		str = "Kills :";
		str.insert(str.length() , Global::ToString(player->GetKillCount()));

		DrawColoredTextOnScreen(0.5 , 0.2 , 0.4 , str , 200 , -20 , -80);
	}
}

void Graphics::TickFunctions()
{
	//Tick functions such Regen
	for (auto p : map->GetObjectList())
	{
		if (auto q = dynamic_cast<Pawn*>(p))
		{
			if (auto a = dynamic_cast<AI*>(p))
				AILOGIC(a);

			if (q->GetState() != DIED && frame + 1 >= fps)
			{
				if (q->GetHealth() <= 0)
					q->Death();
				q->Regen();
			}
			DrawColorText(q , 0.1 , 0.1 , 0.1 , q->GetName() , 1 , 1);
			DrawColorText(q , 0.1 , 0.1 , 0.1 , q->GetName() , -1 , -1);
			DrawColorText(q , 1.0 , 1.0 , 1.0 , q->GetName() , 0 , 0);
			DrawColorText(q , 1.0 , 0.1 , 0.1 , Global::ToString(q->GetHealth()) , 0 , 20);
			DrawColorText(q , 0.1 , 0.1 , 1.0 , Global::ToString(q->GetArmor()) , 0 , 10);
			DrawColorText(q , 0.1 , 0.1 , 1.0 , Global::ToString(q->GetDamage()) , 0 , -(int)Global::Pawn_Size() - 5);
		}
	}
}
void Graphics::DrawFPS()
{
	if(!State())
		timer--;

	static int time , timebase = 0;

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000)
	{
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
	}
	std::string str = "FPS :";
	str.insert(str.length() , Global::ToString(fps));

	//std::cout << str;

	glColor3f(0.1 , 1 , 0.1);
	for (int i = 0; i < str.length(); i++)
	{
		if(TRACKING)
			glRasterPos2f(GetWndWidth() / (-2) + i * 10 + 20 + (map->GetObjectListElem(0))->GetCurrentPosition(X) , GetWndHeight() / 2 - 20 + (map->GetObjectListElem(0))->GetCurrentPosition(Y));
		else
			glRasterPos2f(GetWndWidth()/(-2) + i*10 + 20 , GetWndHeight() / 2 - 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12 , str[i]);
	}

	str = "TIME:";
	str.insert(str.length() , Global::ToString(GetTimer()));
	glColor3f(0.3 , 0.3 , 1);
	for (int i = 0; i < str.length(); i++)
	{
		if (TRACKING)
			glRasterPos2f(GetWndWidth() / (2) + i * 10 - 100 + (map->GetObjectListElem(0))->GetCurrentPosition(X) , GetWndHeight() / (-2) + 20 + (map->GetObjectListElem(0))->GetCurrentPosition(Y));
		else
			glRasterPos2f(GetWndWidth() / (2) + i * 10 - 100 , GetWndHeight() / (-2) + 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12 , str[i]);
	}
}
void Graphics::DrawColorText(Object *t , float _R , float _G , float _B , std::string text , int x_offset , int y_offset)
{
	if (_R < 0)
		_R = -_R;
	if (_R > 1)
		_R -= (int)_R;
	if (_G < 0)
		_G = -_G;
	if (_G > 1)
		_G -= (int)_G;
	if (_B < 0)
		_B = -_B;
	if (_B > 1)
		_B -= (int)_B;
	glColor3f(_R , _G , _B);
	for (int i = 0; i < text.length(); i++)
	{
		glRasterPos2f(t->GetCurrentPosition(X) + 10 * i - text.length() * 5 + x_offset , t->GetCurrentPosition(Y) + Global::Pawn_Size() + 5 +  y_offset);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12 , text[i]);
	}
}

Object* Graphics::NearestOfType(char type, Object* caller, int notteam)
{
	if (caller == NULL)
		return map;

	return 	NearestFromList(GetListOfType(type , caller , notteam) , caller);
}
//Caller must == this
std::vector<Object*> Graphics::GetListOfType(char type , Object* caller , int notteam )
{
	std::vector<Object*> list;
	switch (type)
	{
		case 'P':
		case 'p':
			for (auto p : map->GetObjectList())
				if (auto q = dynamic_cast<Pawn*>(p))
				{
					if (q->GetState() != DIED)
					{
						if (notteam != 0)
						{
							if (q->Team() != dynamic_cast<Pawn*>(caller)->Team())
								list.push_back(q);
						}
						else
							list.push_back(q);
					}
				}
			break;
		case 'C':
		case 'c':
			for (auto p : map->GetObjectList())
				if (auto q = dynamic_cast<Character*>(p))
				{
					if (q->GetState() != DIED)
					{
						if (notteam != 0)
						{
							if (q->Team() != dynamic_cast<Pawn*>(caller)->Team())
								list.push_back(q);
						}
						else
							list.push_back(q);
					}
				}
			break;
		case 'A':
		case 'a':
			for (auto p : map->GetObjectList())
				if (auto q = dynamic_cast<AI*>(p))
				{
					if (q->GetState() != DIED)
					{
						if (notteam != 0)
						{
							if (q->Team() != dynamic_cast<Pawn*>(caller)->Team())
								list.push_back(q);
						}
						else
							list.push_back(q);
					}
				}
			break;
		case 'W':
		case 'w':
			for (auto p : map->GetObjectList())
				if (auto q = dynamic_cast<Weapon*>(p))
				{
					if (q->IsPicked() != true)
					{
						list.push_back(q);
					}
				}
			break;
	}

	return list;
}
Object* Graphics::NearestFromList(std::vector<Object*> targets, Object* caller)
{
	Object* min = NULL;
	double distance , min_distance = INFINITY;
	for (int i = 0; i < targets.size(); i++)
	{
		distance = Object::Distance(targets.at(i) , caller);
		if (distance < min_distance && distance > 0)
		{
			min_distance = distance;
			min = targets.at(i);
		}
	}
	targets.clear();
	return min;
}

void Graphics::Keyboard(unsigned char cc , int x , int y)
{
	switch (tolower(cc))
	{
		case '/': exit(0); break;
		case 'g':
		{
			auto p = dynamic_cast<Character*>(map->GetObjectListElem(0));
			if (p->GetCurrentPosition(X) == DEATH_PLACE)
				break;
			if (p->GetWeapon() != NULL)
			{
				p->DropWeapon();
				std::cout << "You drop weapon.\n";
			}
			break;
		}
		case 'f':
		{
			auto p = dynamic_cast<Character*>(map->GetObjectListElem(0));
			if (p->GetCurrentPosition(X) == DEATH_PLACE)
				break;
			if (p->GetWeapon() == NULL)
			{

				std::cout << "You tried to pick up a weapon.\n";
				auto w = dynamic_cast<Weapon*>(NearestOfType('W' , map->GetObjectListElem(0)));
				if (p->isPickable(w))
				{
					p->SetWeapon(w);
					std::cout << "You picked up a ";
					w->Info();
				}
			}
			break;
		}
		case 'r':
		{
			//std::cout << "You vant to restart? [Y/N]?\n";
			win = false;
			restart = true;
			Pause();
			break;
		}
		case 'p':
		{
			if (State())
				UnPause();
			else
				Pause();
			if (restart = true)
				restart = false;
			break;
		}
		case 'y':
		{
			if (new_game)
			{
				options = true;
			}
			if (restart && !new_game)
			{
				timer = 36000;
				//std::cout << "Restarting\n";
				map->~Map();
				map = new Map();

				if (GetMapType())
				{
					map->SetRadius(GetMapRadius());
				}
				else
				{
					map->SetSize(GetMapWidth(), GetMapHeight());
				}
				map->SpawnActors(GetPlayers());
				UnPause();
				restart = false;
			}
			break;
		}
		case 'n':
		{
			if (State() && restart)
			{
				UnPause();
				restart = false;
				if (static_cast<Character*>(map->GetObjectListElem(0))->GetState() == DIED || win)
					exit(0);
				break;
			}
			if (new_game)
			{
				new_game = false;
			}
			else
			{
				new_game = true;
				Pause();
			}
			break;
		}
		case 'b':
		{
			if (options)
			{
				win = false;
				timer = 36000;
				map->~Map();
				map = new Map();

				if (GetMapType())
				{
					map->SetRadius(GetMapRadius());
				}
				else
				{
					map->SetSize(GetMapWidth() , GetMapHeight());
				}
				map->SpawnActors(GetPlayers());
				UnPause();
				new_game = false;
				options = false;
				Pause();
			}
			break;
		}
		case 'v':
		{
			pos -= 20;

			if (GetMapType())
			{
				if (pos < -60)
					pos = -20;
			}
			else if (pos < -80)
				pos = -20;

			break;
		}
		case ']':
		{
			switch (pos)
			{
				case -20:
				{
					if(GetPlayers() < 10)
						SetPlayers(GetPlayers() + 1);
				}
				break;
				case -40:
				{
					SetMapType(!GetMapType());
				}
				break;
				case -60:
				{
					if (GetMapType())
					{
						if(GetMapRadius() < 1000)
							SetMapRadius(GetMapRadius() + 50);
					}
					else
					{
						if (GetMapWidth() < 1000)
							SetMapSizes(GetMapWidth() + 50, GetMapHeight());
					}
				}
				break;
				case -80:
				{
					if (GetMapHeight() < 1000)
						SetMapSizes(GetMapWidth() , GetMapHeight() + 50);
				}
				break;
			}
			break;
		}
		case '[':
		{
			switch (pos)
			{
				case -20:
				{
					if (GetPlayers() > 2)
						SetPlayers(GetPlayers() - 1);
				}
				break;
				case -40:
				{
					SetMapType(!GetMapType());
				}
				break;
				case -60:
				{
					if (GetMapType())
					{
						if (GetMapRadius() > 100)
							SetMapRadius(GetMapRadius() - 50);
					}
					else
					{
						if (GetMapWidth() > 100)
							SetMapSizes(GetMapWidth() - 50 , GetMapHeight());
					}
				}
				break;
				case -80:
				{
					if (GetMapHeight() > 100)
						SetMapSizes(GetMapWidth() , GetMapHeight() - 50);
				}
				break;
			}
			break;
		}
	}

	if (map->GetObjectListElem(0)->GetCurrentPosition(X) == DEATH_PLACE || State() == PAUSE)
		return;
	if (GetAsyncKeyState('W'))
	{
		if (GetAsyncKeyState('A'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 135);
			return;
		}
		if (GetAsyncKeyState('D'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 45);
			return;
		}

		map->GetObjectListElem(0)->Move(Global::Max_speed() , 90);
		return;
	}
	if (GetAsyncKeyState('S'))
	{
		if (GetAsyncKeyState('A'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 235);
			return;
		}
		if (GetAsyncKeyState('D'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 315);
			return;
		}
		map->GetObjectListElem(0)->Move(Global::Max_speed() , 270);
		return;
	}
	if (GetAsyncKeyState('A'))
	{
		if (GetAsyncKeyState('S'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 235);
			return;
		}
		if (GetAsyncKeyState('W'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 135);
			return;
		}
		map->GetObjectListElem(0)->Move(Global::Max_speed() , 180);
		return;
	}
	if (GetAsyncKeyState('D'))
	{
		if (GetAsyncKeyState('S'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 315);
			return;
		}
		if (GetAsyncKeyState('W'))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 45);
			return;
		}
		map->GetObjectListElem(0)->Move(Global::Max_speed() , 0);
		return;
	}
}
void Graphics::Special(int cc , int x , int y)
{
	if (map->GetObjectListElem(0)->GetCurrentPosition(X) == DEATH_PLACE)
		return;
	if (GetAsyncKeyState(VK_UP))
	{
		if (GetAsyncKeyState(VK_LEFT))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 135);
			return;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 45);
			return;
		}

		map->GetObjectListElem(0)->Move(Global::Max_speed() , 90);
		return;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (GetAsyncKeyState(VK_LEFT))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 235);
			return;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 315);
			return;
		}
		map->GetObjectListElem(0)->Move(Global::Max_speed() , 270);
		return;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		if (GetAsyncKeyState(VK_DOWN))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 235);
			return;
		}
		if (GetAsyncKeyState(VK_UP))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 135);
			return;
		}
		map->GetObjectListElem(0)->Move(Global::Max_speed() , 180);
		return;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (GetAsyncKeyState(VK_DOWN))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 315);
			return;
		}
		if (GetAsyncKeyState(VK_UP))
		{
			map->GetObjectListElem(0)->Move(Global::Max_speed() , 45);
			return;
		}
		map->GetObjectListElem(0)->Move(Global::Max_speed() , 0);
		return;
	}
	/*switch (cc)
	{
	case GLUT_KEY_UP:	map->GetObjectListElem(0)->Move(Global::Max_speed() , 90); break;
	case GLUT_KEY_LEFT:	map->GetObjectListElem(0)->Move(Global::Max_speed() , 180); break;
	case GLUT_KEY_DOWN:	map->GetObjectListElem(0)->Move(Global::Max_speed() , 270); break;
	case GLUT_KEY_RIGHT:map->GetObjectListElem(0)->Move(Global::Max_speed() , 0); break;
	}*/
}

void Graphics::MousePos(int x , int y)
{
	CoordsToReal(x , y);
	dynamic_cast<Pawn*>(map->GetObjectListElem(0))->SetTargetCoord(x , y);
	
	map->GetObjectListElem(0)->SetCurrentPositionR(Global::AngleOX( x - map->GetObjectListElem(0)->GetCurrentPosition(X),
																	y - map->GetObjectListElem(0)->GetCurrentPosition(Y)));
	//std::cout << map->GetObjectListElem(0)->GetCurrentPosition(R) << "\n";
}

void Graphics::CoordsToReal(int &x, int &y)
{	//'cause Orto is between -w_width and w_width
	x -= w_width / 2;
	y -= w_height / 2;
	y = -y;
}

void Graphics::DrawBackground()
{	/*Checker*/

	float psize = 50.0;

	glClearColor(0.2 , 0.2 , 0.2 , 1.0);

	bool line = 1;
	glColor3f(0.2 , 0.2 , 0.6);
	for (int j = -w_height; j <= w_height; j += psize , line = !line)
	{
		for (int i = -w_width; i <= w_width; i += 2*psize)
		{
			glBegin(GL_POLYGON);
			glVertex2f(i		 + line*psize , j);
			glVertex2f(i + psize + line*psize , j);
			glVertex2f(i + psize + line*psize , j-psize);
			glVertex2f(i		 + line*psize , j-psize);
			glEnd();
		}
	}

	glColor4f(0.7 , 0.7 , 0.7 , 0.5);
	DrawGrid(psize , 2.2 , -1 , -1);

	glColor4f(0.1 , 0.1 , 0.1 , 0.5);
	DrawGrid(psize , 3.0 , 1 , 1);

	glColor4f(0.2 , 0.2 , 0.2 , 1.0);
	DrawGrid(psize);

	DrawOutsideMap();
}

void Graphics::DrawOutsideMap()
{
	if (map->isRound())
	{
		glColor4f(0.1 , 0.1 , 0.1 , 0.01);

		int n = 36 , r = map->GetRadius();

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(2 * map->GetWidth() , 2 * map->GetHeight());
		glVertex2f(2 * map->GetWidth() , 0);
		for (int i = 0; i <= n / 4; i++)
		{
			glVertex2f(r*cos(3.1416f*((float)i * 2 / n)) ,
				r*sin(3.1416f*((float)i * 2 / n)));
		}
		glVertex2f(0 , 2 * map->GetHeight());
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(-2 * (int)(map->GetWidth()) , 2 * map->GetHeight());
		glVertex2f(0 , 2 * map->GetHeight());
		for (int i = n / 4; i <= n / 2; i++)
		{
			glVertex2f(r*cos(3.1416f*((float)i * 2 / n)) ,
				r*sin(3.1416f*((float)i * 2 / n)));
		}
		glVertex2f(-2 * (int)(map->GetWidth()) , 0);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(-2 * (int)(map->GetWidth()) , -2 * (int)(map->GetHeight()));
		glVertex2f(-2 * (int)(map->GetWidth()) , 0);
		for (int i = n / 2; i <= 3 * n / 4; i++)
		{
			glVertex2f(r*cos(3.1416f*((float)i * 2 / n)) ,
				r*sin(3.1416f*((float)i * 2 / n)));
		}
		glVertex2f(0 , -2 * (int)(map->GetHeight()));
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(2 * map->GetWidth() , -2 * (int)(map->GetHeight()));
		glVertex2f(0 , -2 * (int)(map->GetHeight()));
		for (int i = 3 * n / 4; i <= n; i++)
		{
			glVertex2f(r*cos(3.1416f*((float)i * 2 / n)) ,
				r*sin(3.1416f*((float)i * 2 / n)));
		}
		glVertex2f(2 * map->GetWidth() , 0);
		glEnd();
	}
	else
	{
		glColor4f(0.1 , 0.1 , 0.1 , 0.4);

		glBegin(GL_POLYGON);
		glVertex2f(map->GetWidth() * 2 , map->GetHeight() * 2);
		glVertex2f(map->GetWidth() , map->GetHeight());
		glVertex2f(map->GetWidth() , -1 * (int)(map->GetHeight()));
		glVertex2f(map->GetWidth() * 2 , -2 * (int)(map->GetHeight()));
		glEnd();

		glBegin(GL_POLYGON);
		glVertex2f(map->GetWidth() * -2 , -2 * (int)(map->GetHeight()));
		glVertex2f(map->GetWidth() , -1 * (int)(map->GetHeight()));
		glVertex2f(-1 * (int)(map->GetWidth()) , -1 * (int)(map->GetHeight()));
		glVertex2f(-2 * (int)(map->GetWidth()) , -2 * (int)(map->GetHeight()));
		glEnd();

		glBegin(GL_POLYGON);
		glVertex2f(-2 * (int)(map->GetWidth()) , -2 * (int)(map->GetHeight()));
		glVertex2f(-1 * (int)(map->GetWidth()) , -1 * (int)(map->GetHeight()));
		glVertex2f(-1 * (int)(map->GetWidth()) , map->GetHeight());
		glVertex2f(-2 * (int)(map->GetWidth()) , map->GetHeight() * 2);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex2f(-2 * (int)(map->GetWidth()) , map->GetHeight() * 2);
		glVertex2f(-1 * (int)(map->GetWidth()) , map->GetHeight());
		glVertex2f(map->GetWidth() , map->GetHeight());
		glVertex2f(map->GetWidth() * 2 , map->GetHeight() * 2);
		glEnd();
	}
}

void Graphics::Draw()
{
	for (auto p : map->GetObjectList())
	{
		if (auto q = dynamic_cast<Character*>(p))
		{
			if (q->GetState() != DIED)
			{
				q->SetCurrentPositionR(Object::Angle(q , (double*)q->GetTargetCoord()));
				q->Draw();
			}
			else continue;
		}
		else if (auto q = dynamic_cast<AI*>(p))
		{
			if (q->GetState() != DIED)
			{
				q->SetCurrentPositionR(Object::Angle(q , (double*)q->GetTargetCoord()));
				q->Draw();
			}
			else continue;
		}
		else if (auto q = dynamic_cast<Weapon*>(p))
		{
			if(q->IsPicked() == false)
				q->Draw();
			else continue;
		}
		else
		{
			std::cout << "dynamic_cast of Object_list element failed\n";
			glColor3f(1 , 0.1 , 0.1);
			glLineWidth(10);
			p->Draw();
			return;
		}
		if (auto q = dynamic_cast<Pawn*>(p))
		{
			float* color = GetTeamColor(q->Team());
			glColor3f(color[0] , color[1] , color[2]);
			glLineWidth(5);
			delete[] color;
		}
		else
		{
			glColor3f(0.1 , 0.1 , 0.1);
			glLineWidth(2);
		}
		p->Draw();
	}
}

void Graphics::Initialize()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w_width/2 , w_width/2 , -w_height/2 , w_height/2 , -1.0 , 1.0);
}
