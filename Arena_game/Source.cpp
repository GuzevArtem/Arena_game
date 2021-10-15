#include "main.h"
#include <iostream>
Graphics Api;
void WIN(int value);
void DisplayOnPause();
void MouseMove(int x , int y);
void Reshape(int w, int h);
void Display();
void Keyboard(unsigned char cc , int x, int y);
void Special(int cc , int x , int y);
void MousePos(int x , int y);

int main()
{
	Api.Init();
	Api.Initialize();

	glutReshapeFunc(Reshape);

	if (Api.State())
	{
		glutIdleFunc(DisplayOnPause);
		glutDisplayFunc(DisplayOnPause);
		glutPassiveMotionFunc(MouseMove);
	}
	else
	{
		glutIdleFunc(Display);

		glutDisplayFunc(Display);
		glutKeyboardFunc(Keyboard);
		//glutKeyboardUpFunc();
		glutSpecialFunc(Special);
		//glutSpecialUpFunc();
		glutPassiveMotionFunc(MousePos);
		glutMotionFunc(MousePos);
	}

	Api.OnStart();

	if (Api.GetTimer() <= 0)
		WIN(0);
	glutMainLoop();

	return 0;
}

int onexit()
{
	Api.~Graphics();
	return 0;
}

void WIN(int value)
{
	Api.WIN();
}

void DisplayOnPause()
{
	Display();

	//glutCreateMenu();
}
void MouseMove(int x , int y)
{

}

void Display()
{
	Api.Display();
}

void Reshape(int w , int h)
{
	Api.Reshape( w, h);
}

void Keyboard(unsigned char cc , int x , int y)
{
	Api.Keyboard(cc , x , y);
}
void Special(int cc , int x , int y)
{
	Api.Special(cc , x , y);
}

void MousePos(int x , int y)
{
	Api.MousePos(x , y);
}
