//============================================================================
// Name        : Snake.cpp
// Author      : Filip Bjelonic
// Version     : 1.0
// Copyright   : /
// Description : Snake Game in c++
//============================================================================

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

const int width=60; /* Defines Mapsize in x-direction (horizontal) */
const int height=10; /* Defines Mapsize in y-direction (vertikal) */
int x=width/2; /* Snake Head position in x-direction */
int y=height/2; /* Snake Head position in y-direction */
int dx=0; /* Movement of Snake Head in x */
int dy=0; /* Movement of Snake Head in y */
int a; /* Food position in x */
int b; /* Food position in y */
int food=3; /* Defines the Size of the Snake (3 Circles in the beginning)
 	 	 	 * and final Score (food-3). */
char A[height][width]; /* Map variable */
int sbody[(width-2)*(height-2)][2];

int bytesWaiting=false;
char olddirection='a'; /* Help Variable for restriction of Movement */
int olddir[2]={-1,0}; /* Help Variable for defining if Movement is
					   * Restricted or not (No Movement in back direction allowed) */
int milliseconds=200; /* Defines the Frame Rate and Speed of the Snake Game. */
bool getfood=true; /* New Food location initializer. */
bool running=false; /* Game start and stop variable. */

void buildMap(char A[][width],const int width,const int height,int sbody[][2]);
void buildSnake();
void start();
void printMap(char A[][width]);
void snake(char A[][width],int food,int sbody[][2]);
void move(int dir[2]);
int _kbhit();
void sleepcp(int milliseconds);
void makeFood();
void touchFood();
void dead();
void calcSpeed();
bool checkFood();
void showControl();

int main() {
	for (int i=0;i<food;i++){	/* Initializing of Snake Body (sbody) */
		sbody[i][0]=width/2;
		sbody[i][1]=height/2;
	}
	buildMap(A,width,height,sbody);
	start();
	do {
		system("tput reset");
		snake(A,food,sbody);
		touchFood();
		if (getfood){
			makeFood();
			getfood=false;
		}
		buildSnake();
		printMap(A);
		cout << "Your Score: " << food-3 << endl;
		cout << "Keep Rolling!" << endl;
		cout << "Your Speed: " << (int) (400/milliseconds) << endl;
		sleepcp(milliseconds); /* Pauses the program until next Move of Snake */
		dead();
	}
	while (running);
	system("tput reset");
	printMap(A);
	cout << "You Lost! "<< endl;
	cout << "Good Luck next Time :)"<< endl;
	cout << "Your final Score: " << food-3 << endl;
	return 0;
}

void buildMap(char A[][width],const int width,const int height,int sbody[][2]){
	/* This function builds up the Map for printing. */

	/* Build Map here */
	for (int i=0;i<height;i++){
		for (int a=0;a<width;a++){
			if (i==0 || a==0 || i==height-1 || a==width-1){
				A[i][a]='#';
			} else {
				A[i][a]=' ';
			}
		}
	}
}
void buildSnake(){
	/* Build Snake here */
	for (int i=0;i<food;i++){
		A[sbody[i][1]][sbody[i][0]]='O';
	}
	A[0][0]='#';
}

void start(){
	system("tput reset");
	if (!running){ /* Start Command for User */
		touchFood();
		buildSnake();
		printMap(A);
		char k;
		cout<< "Press any Key to start!" << endl;
		showControl();
		cin >> k;
		running=true;
	}
}

void printMap(char A[][width]){
	/* After everything is initialized, prints the whole Map with Snake and
	 * food. */
	string welcome = "Welcome to Snake";
	int k=(width - (int) welcome.length())/2;
	string _space  = string(k,' ');
	cout << _space + welcome << endl;
	for (int i=0;i<height;i++){
		for (int a=0;a<width;a++){
			cout << A[i][a];
		}
		cout << endl;
	}
}

void snake(char A[][width],int food, int sbody[][2]){
	/* Builds the snake and the movement of it */
	int dir[2];
	move(dir);
	dx+=dir[0];
	dy+=dir[1];
	A[sbody[food-1][1]][sbody[food-1][0]]=' '; /* To erase old tail of Snake */
	for (int i=food-1;i>0;i--){
		sbody[i][0]=sbody[i-1][0];
		sbody[i][1]=sbody[i-1][1];
	}
	sbody[0][0]=x+dx;
	sbody[0][1]=y+dy;
}

void move(int dir[2]){
	/* Defines the movement of the Snake by key pressed. */
	char direction;
	if (_kbhit()){
		cin >> direction;
	}else {
		direction=olddirection;
	}

	switch (direction){
	case 'w' :	dir[0]=0;
				dir[1]=-1;
				break;
	case 'd' :	dir[0]=1;
				dir[1]=0;
				break;
	case 's' :	dir[0]=0;
				dir[1]=1;
				break;
	case 'a' :	dir[0]=-1;
				dir[1]=0;
				break;
	default: 	dir[0]=olddir[0];
				dir[1]=olddir[1];
				break;
	}
	if (olddir[0]+dir[0]==0 && olddir[1]+dir[1]==0){
		dir[0]=olddir[0];
		dir[1]=olddir[1];
	}
	olddirection=direction;
	olddir[0]=dir[0];
	olddir[1]=dir[1];
}

int _kbhit() {
	/* This function is similar to the kbhit() of conio.h library. This
	 * is the equivalent for Linux Systems. */
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    //int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void sleepcp(int milliseconds){
	/* Platform independent Sleep-Command. Defines the Frame-Rate */
    clock_t time_end;
    time_end = clock() +milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end)
    {
    }
}

void makeFood(){
	/* this function is called, if the Food is eaten by the Snake
	 * and a new peace should be generated at a random spot. */
	srand(time(NULL));
	A[b][a]=' '; /* Deletes old Food position */
	do {
		a=rand()%(width-4)+2;
		b=rand()%(height-4)+2;
	}
	while (checkFood());
	/* Build Food here */
	A[b][a]='X';

}

void touchFood(){
	/* This function determines, whether the Food is hit by the Snake or not. */
	if (a==x+dx && b==y+dy){
		getfood=true;
		food+=1;
		calcSpeed();
	}
}

void dead(){
	/* This function end the game, if the snake hits a wall or itself. */
	if (x+dx==0 || y+dy==0 || x+dx==width-1 || y+dy == height-1){
		running=false;
	}
	for (int i=1;i<food;i++){
		if (sbody[0][0]==sbody[i][0] && sbody[0][1]==sbody[i][1]){
			running=false;
			break;
		}
	}
}

void calcSpeed(){
	/* This function defines the frames of the game. The lower milliseconds, the
	 * faster the game gets. */
	if (food<=13){
		milliseconds-=10;
	}else if (food<=23){
		milliseconds-=2;
	}else if (food<=33){
		milliseconds-=1;
	}
}

bool checkFood(){
	/* This function checks, whether the food is in the snake's body
	 * or in a free space in the map. */
	bool bodyFood;
	for (int i=0;i<food;i++){
		if (sbody[i][0]==a && sbody[i][1]==b){
			bodyFood=true;
			break;
		} else {
			bodyFood=false;
		}
	}
	return bodyFood;
}

void showControl(){
	string _space=string((int) width/2-5,' ');
	cout << _space+"     w      " << endl;
	cout << _space+"     |      " << endl;
	cout << _space+"a <-- --> d " << endl;
	cout << _space+"     |      " << endl;
	cout << _space+"     s      " << endl;
}

