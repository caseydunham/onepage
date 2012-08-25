/*
	Zombie Version 1.1

	A One Page game, <= 80x50

	May or may not work correctly on different versions of windows. 

	Author:  Casey Dunham <casey.dunham@gmail.com> 

	Revisions:

	08/25/2012
		* Converted to work with VS 2010 Express
		* code formatting
		* added to github

	05/17/2008 
		* initial Creation
*/

#include <windows.h>
#include <time.h>
#include <vector>
using namespace std;

struct e
	{
	int x;
	int y;
	int dx;
	int dy;
	int state;
	};

// Output Handle
HANDLE wh;

// Input handle
HANDLE rh;

CHAR_INFO bb[4000];

COORD bs = {80, 50};
COORD pos = {0, 0};
SMALL_RECT a = {0, 0, 79, 49};

COORD arena = {25, 15};

e player;

int zombies = 10;
vector<e> z; // zombies

// Lost life flag, monster turn
bool ll,mt = false;

int score, lives, level, leaps, i, j;

void init() 
{
	wh = GetStdHandle(/*STD_OUTPUT_HANDLE*/ -11);
	rh = GetStdHandle(/*STD_INPUT_HANDLE*/ -10);
	SMALL_RECT  w = {0, 0, 79, 49};
	SetConsoleWindowInfo(wh, 1, &w);
	COORD b = {80, 50};
	SetConsoleScreenBufferSize(wh, b);
}

// Set the character at position x, y to c with attributes a
void setchar(int x, int y, char c, int a) 
{
	bb[(y*80) + x].Char.AsciiChar = c;
	bb[(y*80) + x].Attributes = a;
}

// Set the cursor position
void setpos(int x, int y) 
{
	COORD c1 = {x,y};
	SetConsoleCursorPosition(wh, c1);
}

void placeplayer()
{
	// Randomly place player, not on zombie cell!
	int ok = 0;
	while(!ok)
	{
		player.x = (rand() % 24) + 1;
		player.y = (rand() % 14) + 1;
		// Check against zombies
		for (i = 0; i < z.size(); ++i)
			if (z[i].state && player.x != z[i].x && player.y != z[i].y) 
				ok = 1;
	}
}

void input() 
{ 
	DWORD e = 0; // Number of events
	DWORD er = 0;
	GetNumberOfConsoleInputEvents(rh, &e);
	if (e != 0)
	{
		INPUT_RECORD *r = new INPUT_RECORD[e];
		ReadConsoleInput(rh, r, e, &er);
		for (i = 0; i < er; ++i)
		{
			if (r[i].EventType == KEY_EVENT)
			{
				WORD kc = r[i].Event.KeyEvent.wVirtualKeyCode;
				bool kd = r[i].Event.KeyEvent.bKeyDown;
				if (kd)
				{
					if (kc == /*VK_ESCAPE*/ 0x1B)
						exit(0);
				}
			}
			if (r[i].EventType == /*MOUSE_EVENT*/ 0x2)
			{
				// Check within bounds of cell				
				if (r[i].Event.MouseEvent.dwButtonState & /*FROM_LEFT_1ST_BUTTON_PRESSED*/ 0x1)
				{
					COORD pos = r[i].Event.MouseEvent.dwMousePosition;
					if ((pos.X >= 0 && pos.X < arena.X) && (pos.Y >= 0 && pos.Y < arena.Y))
					{
						// Check that the player is only clicking on a cell adjacent to player
						if (abs(player.x - pos.X) <= 1 && abs(player.y - pos.Y) <= 1)
						{
							player.x = pos.X;
							player.y = pos.Y;
							mt = true;
						}
					}
				}
				if (r[i].Event.MouseEvent.dwButtonState & /*RIGHTMOST_BUTTON_PRESSED*/ 0x2)
				{
					// Use a Leap
					if (leaps - 1 >= 0)
					{
						--leaps;
						placeplayer ();
					}
				}
			}
		}
		delete[] r;
	}
}

void reset() 
{
	// Randomly place player
	player.x = (rand() % 24) + 1;
	player.y = (rand() % 14) + 1;
	player.dx = player.dy = player.state = 1;

	// reset zombies
	z.clear();

	// Create some new zombies
	for (i = 0; i < zombies; ++i)
	{
		while (1) 
		{
			e t = {(rand() % 24) + 1, (rand() % 14) + 1, 1, 1, 1};
			if (t.x != player.x && t.y != player.y)
			{
				z.push_back(t);
				break;
			}
		}
	}
}

void draw() 
{
	// Draw arena
	for (i = 0; i < arena.Y; ++i)
		for (j = 0; j < arena.X; ++j)
			setchar(j, i, 219, 2);

	// Draw player
	setchar(player.x, player.y, 1, 14);

	// Draw Zombies - Make sure zombie appears above player on death!!
	for (i = 0; i < z.size(); ++i)
		if (z[i].state)
			setchar(z[i].x, z[i].y, 'Z', 4);

	WriteConsoleOutput(wh,bb,bs,pos, &a);

	setpos(30, 0);
	printf("Zombie - casey.dunham@gmail.com");
	setpos(30, 1);
	printf("Score: %d", score);
	setpos(30, 2);
	printf("Lives: %d", lives);
	setpos(30, 3);
	printf("Leaps: %d", leaps);
	setpos(30, 4);
	printf("Level: %d", level);
}

void update() 
{
	// Is it the monster turn to move?
	if (mt)
	{
		// Calculate distance to player and move accordingly
		// making sure not to go out of bounds
		// Also check if a zombie moves into a cell occupied
		// by another zombie.
		for (i = 0; i < z.size(); ++i)
		{
			if (z[i].state)
			{
				int x = player.x - z[i].x;
				int y = player.y - z[i].y;

				if (x < 0) 
					x = -1;
				else if (x > 0) 
					x = 1;
				if (y < 0) 
					y = -1;
				else if (y > 0)
					y = 1;

				z[i].x += x;
				z[i].y += y;

				// Check player collision
				if (z[i].x == player.x && z[i].y == player.y)
				{
					--zombies;
					ll = true;
					return;
				}

				// Check collisions with other zombies
				for (j = i + 1; j < z.size(); ++j)
				{
					if (z[j].state && (z[i].x == z[j].x) && (z[i].y == z[j].y))
					{
						// Kill both zombies and adjust player's score
						zombies -= 2;
						z[i].state = z[j].state = 0;
						score += 15;
					}
				}
			}
		}
		mt = false;
	}
}


void main() 
{
	// initialize console
	// Setup initial game
	// Randomly place zombies and player
	score = 0;
	level = 1;
	lives = 4;
	leaps = 3;
	
	srand(time(0));
	init();
	reset();

	while (1) 
	{
		memset(bb, 4000 * sizeof (CHAR_INFO), 0);
		input();
		update();
		draw();
	
		if(!lives)
		{
			// Game Over
			setpos(35,25);
			printf("GAME OVER");
			Sleep(5000);
			reset();
		}
		else if (!z.size()) 
		{
			// Zombies are dead
			++level;
			reset();
		}
		else if(ll) 
		{
			// Lost a life
			Sleep(1000);
			// Randomly place player??
			placeplayer();
			--lives;
			player.state = 1;
			reset();
			ll = false;
		}
		else
			Sleep(50);
	}
}
