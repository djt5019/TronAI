#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <time.h>

#define MAX_X 80

#define MAX_Y 30

using namespace std;

struct _playerData
{
	int x;
	int y;
	int playerNum;
};

 
void _sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

class Player2
{
	public:
		bool Attack1(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
		{
			if (me_x > them_x)
			{
				if (board[me_y][me_x-1] == ' ')
				{
					me_x--;
					return true;
				}
			}
			else if (me_x < them_x)
			{
				if (board[me_y][me_x+1] == ' ')
				{
					me_x++;
					return true;
				}
			}
			else if (me_y > them_y)
			{
				if (board[me_y-1][me_x] == ' ')
				{
					me_y--;
					return true;
				}
			}
			else if (me_y < them_y)
			{
				if (board[me_y+1][me_x] == ' ')
				{
					me_y++;
					return true;
				}
			}

			return false; // none of these worked
		}
		bool Attack2(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
		{
			if (me_y > them_y)
			{
				if (board[me_y-1][me_x] == ' ')
				{
					me_y--;
					return true;
				}
			}
			else if (me_y < them_y)
			{
				if (board[me_y+1][me_x] == ' ')
				{
					me_y++;
					return true;
				}
			}
			else if (me_x > them_x)
			{
				if (board[me_y][me_x-1] == ' ')
				{
					me_x--;
					return true;
				}
			}
			else if (me_x < them_x)
			{
				if (board[me_y][me_x+1] == ' ')
				{
					me_x++;
					return true;
				}
			}

			return false; // none of these worked
		}
		void Initialize(_playerData *data)
		{
			printf("I am player %d, starting at %d,%d\n", 
				data->playerNum, data->x, data->y);
		}

		bool Move(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y);
};

class Player1
{
	public:
		bool RandomWalk(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
		{
			static int counter = 0;
			static int choice = 0;

			if (counter++ > 7)
			{
				counter = 0;
				choice = rand() % 4;
			}

			switch(choice)
			{
				case 0:
					if (board[me_y][me_x+1] == ' ')
					{
						me_x++;
						return true;
					}
				case 1:
					if (board[me_y+1][me_x] == ' ')
					{
						me_y++;
						return true;
					}
				case 2:
					if (board[me_y][me_x-1] == ' ')
					{
						me_x--;
						return true;
					}
				case 3:
					if (board[me_y-1][me_x] == ' ')
					{
						me_y--;
						return true;
					}
			};

			return false;
		}

		void Initialize(_playerData *data)
		{
			printf("I am player %d, starting at %d,%d\n", 
				data->playerNum, data->x, data->y);
		}


		bool Move(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
		{
			for (int q = 0; q < 8; q++)
			{
				if (RandomWalk(board, me_x, me_y, them_x, them_y))
					return true;
			}

			// ok, none of that worked, just find a place to turn!
			if (board[me_y][me_x+1] == ' ')
				me_x++;
			else if (board[me_y+1][me_x] == ' ')
				me_y++;
			else if (board[me_y][me_x-1] == ' ')
				me_x--;
			else if (board[me_y-1][me_x] == ' ')
				me_y--;
			else
				return false;

			return true;
		}
};

void displayBoard(char board[MAX_Y][MAX_X])
{
	system("clear");
	for (int y = 0; y < MAX_Y; y++)
	{
		for (int x = 0; x < MAX_X; x++)
		{
			putchar(board[y][x]);
		}
		putchar('\n');
	}
}

void initBoard(char board[MAX_Y][MAX_X], int p1_x, int p1_y, int p2_x, int p2_y)
{
	for (int y = 0; y < MAX_Y; y++)
		for (int x = 0; x < MAX_X; x++)
			board[y][x] = ' ';
	for (int y = 0; y < MAX_Y; y++)
		board[y][0] = '|';
	for (int y = 0; y < MAX_Y; y++)
		board[y][MAX_X-1] = '|';
	for (int x = 0; x < MAX_X; x++)
		board[0][x] = '-';
	for (int x = 0; x < MAX_X; x++)
		board[MAX_Y-1][x] = '-';
	board[p1_y][p1_x] = '1';
	board[p2_y][p2_x] = '2';

}

void Pause()
{
	//char scs[5]; 
	//gets(scs);
	_sleep(600);
}

int main()
{
	Player1 P1;
	Player2 P2;
	char board[MAX_Y][MAX_X];
      
	_playerData data1, data2;
	data1.playerNum = 1;
	data1.x = 5;
	data1.y = 5;
	data2.playerNum = 2;
	data2.x = 40;
	data2.y = 15;
	P1.Initialize(&data1);
	P2.Initialize(&data2);

	srand((unsigned int)(time(NULL)));
	initBoard(board, data1.x, data1.y, data2.x, data2.y);
	displayBoard(board);

	bool goOn = true;
	while (goOn)
	{
		bool p1 = P1.Move(board, data1.x, data1.y, data2.x, data2.y);
		bool p2 = P2.Move(board, data2.x, data2.y, data1.x, data1.y);
		goOn = false; // assume
		if ((data2.x == data1.x)&&(data2.y == data1.y))
		{
			board[data1.y][data1.x] = '*';
			board[data2.y][data2.x] = '*';
			displayBoard(board);
			printf("\n\a COLLISION!\n");
		}
		else if ((!p1) && (!p2))
		{
			board[data1.y][data1.x] = '*';
			board[data2.y][data2.x] = '*';
			displayBoard(board);
			printf("\n\a ENDS IN A TIE\n");
		}
		else if (!p1)
		{
			board[data1.y][data1.x] = '*';
			displayBoard(board);
			printf("\n\a P2 wins\n");
		}
		else if (!p2)
		{
			board[data2.y][data2.x] = '*';
			displayBoard(board);
			printf("\n\a P1 wins\n");
		}
		else
		{
			board[data1.y][data1.x] = '1';
			board[data2.y][data2.x] = '2';
			displayBoard(board);
			goOn = true;
		}
		Pause();
	}
}

