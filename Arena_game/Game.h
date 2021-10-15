#ifndef GAME_H
#define GAME_H

#define PAUSE true
#define UNPAUSE false

class Game
{
	private:
		bool run;
		bool pause;

		// true - round, false - rectangl
		bool map_type;
		unsigned default_players = 2;
		unsigned default_radius = 400;
		unsigned default_width = 400;
		unsigned default_height = 500;
	public:
		Game();
		virtual ~Game();

		void SetPlayers(unsigned new_players);
		unsigned GetPlayers();

		void SetMapType(bool isRound);
		void SetMapRadius(unsigned r);
		void SetMapSizes(unsigned w, unsigned h);
		// true - round, false - rectangl
		bool GetMapType();
		unsigned GetMapRadius();
		unsigned GetMapWidth();
		unsigned GetMapHeight();

		int Execute();
		bool State();
		void Pause();
		void UnPause();

		void Exit();	
};

#endif /* GAME_H */
//Create, Save, Upload, Delete, Pause...
