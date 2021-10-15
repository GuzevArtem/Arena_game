#ifndef GRAPICS_H
#define GRAPICS_H

#include <GL/glut.h>
#include "../general/map.h"
#include "../Object.h"
#include "../../Game.h"
#include <iostream>

class Graphics: public Map, public Object, public Game
{
private:
	int argv; char** argc;
	float w_width = 800 , w_height = 800;
	float ratio = w_width / w_height;

	bool started = false;
	bool restart = false , new_game = false , options = false , win = false;
	int pos = -20;
	int timer = 36000;

	Pawn* winner = NULL;

	bool TRACKING = false;
	int fps , frame = 0;

	Map* map;
	unsigned all = 1;

public:
	Graphics();
	~Graphics();

	int GetTimer();
	void WIN();

	void OnStart();
	void DrawOutsideMap();
	void Draw();
	void DrawBackground();
	void Reshape(int w, int h);
	void Display();
	void Collide();
	bool Collision(Object *a , Object *b = NULL);

	int AILOGIC(AI *a , Character *target);
	int AILOGIC(AI *a , AI *target);
	int AILOGIC(AI *a , Weapon *target);
	int AILOGIC(AI* a , Object* target);
	void AILOGIC(AI *a);

	void DrawSqure(int x1 , int y1 , int x2 , int y2 , int mode = GL_POLYGON);
	void DrawColoredTextOnScreen(float _R , float _G , float _B , std::string text , int width , int x_offset , int y_offset);
	void DrawStats();

	float* GetTeamColor(unsigned team);
	void TickFunctions();
	void DrawFPS();
	void DrawColorText(Object *t , float _R , float _G , float _B , std::string text , int x_offset , int y_offset);

	// 'P' = 'C'||'A', 'C' - Character, 'A' - AI, ''W' - weapon; 0 - nearest, number - not of this team
	Object* NearestOfType(char type, Object* caller = NULL, int notteam = 0);
	std::vector<Object*> GetListOfType(char type , Object* caller = NULL , int notteam = 0);
	Object* NearestFromList(std::vector<Object*> targets , Object* caller = NULL);

	void Keyboard(unsigned char cc , int x , int y);
	void Special(int cc , int x , int y);
	void MousePos(int x , int y);
	void Initialize();
	void Init();
	Pawn* WinCondition();

	float GetWndWidth()
	{
		return w_width;
	}
	float GetWndHeight()
	{
		return w_height;
	}

	void CoordsToReal(int &x , int &y);
};

#endif /* GRAPHICS_H */